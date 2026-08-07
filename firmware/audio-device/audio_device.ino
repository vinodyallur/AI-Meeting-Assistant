#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <WiFiManager.h>
#include "driver/i2s.h"

// ===== OLED =====
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ================= PINS =================
#define BTN_START       15
#define BTN_STOP        14
#define WIFI_RESET_PIN  0

// ================= AUDIO =================
#define SAMPLE_RATE 16000
#define I2S_WS   25
#define I2S_SD   33
#define I2S_SCK  26

// ================= CLOUD =================
const char* CLOUD_HOST = "stt-premium-app.purplebay-3e569791.centralindia.azurecontainerapps.io";
const int   CLOUD_PORT = 443;
const char* CLOUD_PATH = "/api/upload?filename=live.wav&mac_address=MIC_DEVICE_01";

// ================= CAMERAS =================
const char* CAM1_HOST = "cam1.local";
const char* CAM2_HOST = "cam2.local";

// ================= GLOBALS =================
WebServer server(80);
WiFiClientSecure streamClient;
bool streaming = false;
bool cam1Online = false;
bool cam2Online = false;
bool lastStartState = HIGH;
bool lastStopState  = HIGH;
unsigned long bootTime;

// ======================================================
// OLED UPDATE
// ======================================================
void updateOLED(const char* statusText) {
    display.clearDisplay();
    
    // VINSHANKS title
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 0);
    display.println("VINSHANKS");
    
    display.drawLine(0, 20, 127, 20, SSD1306_WHITE);
    
    display.setTextSize(1);
    display.setCursor(0, 24);
    display.print("Cam 1: ");
    display.println(cam1Online ? "ONLINE" : "OFFLINE");
    
    display.setCursor(0, 34);
    display.print("Cam 2: ");
    display.println(cam2Online ? "ONLINE" : "OFFLINE");
    
    display.setCursor(0, 48);
    display.println(statusText);
    
    display.display();
}

// ======================================================
// CAMERA STATUS CHECK
// ======================================================
bool isCamOnline(const char* host) {
    HTTPClient http;
    http.setTimeout(2000);
    http.begin("http://" + String(host) + "/");
    int code = http.GET();
    http.end();
    return (code > 0);
}

// ======================================================
// SEND CAMERA COMMAND
// ======================================================
void sendCamCommand(const char* cmd) {
    HTTPClient http;
    Serial.println("[CAM CMD] " + String(cmd));
    
    http.begin("http://" + String(CAM1_HOST) + "/" + cmd);
    http.GET(); http.end();
    
    http.begin("http://" + String(CAM2_HOST) + "/" + cmd);
    http.GET(); http.end();
}

// ======================================================
// WAV HEADER
// ======================================================
void sendWavHeader(WiFiClientSecure &client) {
    uint32_t sampleRate = SAMPLE_RATE;
    uint16_t channels = 1;
    uint16_t bits = 16;
    uint32_t byteRate = sampleRate * channels * bits / 8;
    uint16_t blockAlign = channels * bits / 8;
    uint32_t dataSize = 0xFFFFFFFF;
    
    uint8_t header[44];
    memcpy(header, "RIFF", 4);
    uint32_t fileSize = 36 + dataSize;
    memcpy(header + 4, &fileSize, 4);
    memcpy(header + 8, "WAVEfmt ", 8);
    
    uint32_t subChunk1 = 16;
    uint16_t audioFmt = 1;
    memcpy(header + 16, &subChunk1, 4);
    memcpy(header + 20, &audioFmt, 2);
    memcpy(header + 22, &channels, 2);
    memcpy(header + 24, &sampleRate, 4);
    memcpy(header + 28, &byteRate, 4);
    memcpy(header + 32, &blockAlign, 2);
    memcpy(header + 34, &bits, 2);
    memcpy(header + 36, "data", 4);
    memcpy(header + 40, &dataSize, 4);
    
    client.print("2C\r\n");
    client.write(header, 44);
    client.print("\r\n");
}

// ======================================================
// I2S INIT
// ======================================================
void setupI2S() {
    i2s_config_t cfg = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 512,
        .use_apll = true
    };
    
    i2s_pin_config_t pins = {
        .bck_io_num = I2S_SCK,
        .ws_io_num  = I2S_WS,
        .data_out_num = -1,
        .data_in_num  = I2S_SD
    };
    
    i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pins);
    i2s_zero_dma_buffer(I2S_NUM_0);
    Serial.println("[I2S] Ready");
}

// ======================================================
// STREAM CONTROL
// ======================================================
void startStreaming() {
    Serial.println("[STREAM] START");
    updateOLED("MEETING STARTED");
    
    streamClient.setInsecure();
    if (!streamClient.connect(CLOUD_HOST, CLOUD_PORT)) {
        Serial.println("[ERROR] Cloud connect failed");
        return;
    }
    
    streamClient.print("POST " + String(CLOUD_PATH) + " HTTP/1.1\r\n"
                       "Host: " + String(CLOUD_HOST) + "\r\n"
                       "Content-Type: audio/wav\r\n"
                       "Transfer-Encoding: chunked\r\n\r\n");
    
    sendWavHeader(streamClient);
    streaming = true;
    sendCamCommand("start");
}

void stopStreaming() {
    Serial.println("[STREAM] STOP");
    updateOLED("MEETING ENDED");
    
    // 1. End chunked transfer
    streamClient.print("0\r\n\r\n");
    streamClient.stop();
    streaming = false;
    
    // 2. Stop Cameras
    sendCamCommand("stop");
    
    // 3. TRIGGER AI PROCESSING (Call new Cloud Endpoint)
    Serial.println("[CLOUD] Triggering AI Processing...");
    HTTPClient http;
    http.begin("https://" + String(CLOUD_HOST) + "/api/end_session_by_mac?mac_address=MIC_DEVICE_01");
    http.POST(""); // Empty POST request
    http.end();
    Serial.println("[CLOUD] AI Processing Triggered");
}

// ======================================================
// SETUP
// ======================================================
void setup() {
    Serial.begin(115200);
    delay(1000);
    
    pinMode(BTN_START, INPUT_PULLUP);
    pinMode(BTN_STOP, INPUT_PULLUP);
    pinMode(WIFI_RESET_PIN, INPUT_PULLUP);
    
    // OLED init
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("[OLED] FAILED");
    }
    updateOLED("BOOTING...");
    
    WiFiManager wm;
    if (digitalRead(WIFI_RESET_PIN) == LOW) {
        Serial.println("[WiFi] Reset requested");
        wm.resetSettings();
        delay(1000);
    }
    
    if (!wm.autoConnect("Audio-ESP32-Setup")) {
        ESP.restart();
    }
    
    Serial.println("[WiFi] Connected: " + WiFi.localIP().toString());
    
    if (MDNS.begin("audio")) {
        Serial.println("[mDNS] audio.local ready");
    }
    
    setupI2S();
    
    Serial.println("[DISCOVERY]");
    cam1Online = isCamOnline(CAM1_HOST);
    cam2Online = isCamOnline(CAM2_HOST);
    Serial.println(cam1Online ? "[CAM1] ONLINE" : "[CAM1] OFFLINE");
    Serial.println(cam2Online ? "[CAM2] ONLINE" : "[CAM2] OFFLINE");
    
    updateOLED("READY");
    bootTime = millis();
    Serial.println("[SYSTEM] READY — press D2 to start");
}

// ======================================================
// LOOP
// ======================================================
void loop() {
    if (millis() - bootTime < 2000) return;
    
    bool startState = digitalRead(BTN_START);
    bool stopState  = digitalRead(BTN_STOP);
    
    if (lastStartState == HIGH && startState == LOW && !streaming) {
        delay(30);
        if (digitalRead(BTN_START) == LOW) startStreaming();
    }
    
    if (lastStopState == HIGH && stopState == LOW && streaming) {
        delay(30);
        if (digitalRead(BTN_STOP) == LOW) stopStreaming();
    }
    
    lastStartState = startState;
    lastStopState  = stopState;
    
    if (!streaming) return;
    
    int32_t i2sBuf[512];
    int16_t pcmBuf[512];
    size_t bytesRead;
    
    i2s_read(I2S_NUM_0, i2sBuf, sizeof(i2sBuf), &bytesRead, portMAX_DELAY);
    int samples = bytesRead / 4;
    
    for (int i = 0; i < samples; i++) {
        int32_t s = i2sBuf[i] >> 16;
        s = constrain(s * 2, -32768, 32767);
        pcmBuf[i] = (int16_t)s;
    }
    
    int size = samples * 2;
    char hdr[16];
    sprintf(hdr, "%X\r\n", size);
    streamClient.print(hdr);
    streamClient.write((uint8_t*)pcmBuf, size);
    streamClient.print("\r\n");
}