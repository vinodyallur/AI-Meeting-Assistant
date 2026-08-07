#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <ESPmDNS.h>
#include <WiFiManager.h>

// ================= DEVICE =================
#define DEVICE_MAC "e08cfeb530b0"
#define CAMERA_ID  "CAM_1"

// ================= WIFI RESET PIN =================
#define WIFI_RESET_PIN 0   // Hold LOW at boot to reset WiFi

// ================= CLOUD =================
const char* UPLOAD_HOST = "stt-premium-app.purplebay-3e569791.centralindia.azurecontainerapps.io";
const char* UPLOAD_PATH = "/api/upload_image";
const int   UPLOAD_PORT = 443;

// ================= CAMERA PINS =================
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

WebServer server(80);

// ================= AUTO CAPTURE =================
bool autoCapture = false;
unsigned long lastCapture = 0;
const unsigned long CAPTURE_INTERVAL = 30000;

// ======================================================
// UPLOAD IMAGE
// ======================================================
void uploadImage(camera_fb_t *fb) {
    Serial.println("[UPLOAD] Connecting to cloud...");
    
    WiFiClientSecure client;
    client.setInsecure();
    
    if (!client.connect(UPLOAD_HOST, UPLOAD_PORT)) {
        Serial.println("[UPLOAD] Connection FAILED");
        return;
    }
    
    String boundary = "------------------------esp32boundary";
    String head = "--" + boundary + "\r\n" +
                  "Content-Disposition: form-data; name=\"mac_address\"\r\n\r\n" +
                  String(DEVICE_MAC) + "\r\n" +
                  "--" + boundary + "\r\n" +
                  "Content-Disposition: form-data; name=\"camera_id\"\r\n\r\n" +
                  String(CAMERA_ID) + "\r\n" +
                  "--" + boundary + "\r\n" +
                  "Content-Disposition: form-data; name=\"file\"; filename=\"capture.jpg\"\r\n" +
                  "Content-Type: image/jpeg\r\n\r\n";
    
    String tail = "\r\n--" + boundary + "--\r\n";
    
    uint32_t totalLen = head.length() + fb->len + tail.length();
    
    client.println("POST " + String(UPLOAD_PATH) + " HTTP/1.1");
    client.println("Host: " + String(UPLOAD_HOST));
    client.println("Content-Type: multipart/form-data; boundary=" + boundary);
    client.println("Content-Length: " + String(totalLen));
    client.println();
    client.print(head);
    
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    size_t bufferSize = 1024;
    
    for (size_t i = 0; i < fbLen; i += bufferSize) {
        size_t remaining = fbLen - i;
        client.write(fbBuf + i, (remaining < bufferSize) ? remaining : bufferSize);
    }
    
    client.print(tail);
    
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") break;
    }
    
    String response = client.readStringUntil('\n');
    Serial.println("[UPLOAD] Response: " + response);
    client.stop();
}

// ======================================================
// CAPTURE
// ======================================================
void captureAndUpload() {
    Serial.println("[CAM] Capturing...");
    camera_fb_t * fb = esp_camera_fb_get();
    if(!fb) {
        Serial.println("[ERROR] Capture Failed");
        return;
    }
    
    uploadImage(fb);
    esp_camera_fb_return(fb);
}

// ======================================================
// WEB HANDLERS
// ======================================================
void handleRoot() {
    server.send(200, "text/plain", "CAM-1 READY. Endpoints: /start, /stop, /resetwifi");
}

void handleStart() {
    Serial.println("[WEB] START received");
    autoCapture = true;
    captureAndUpload();
    lastCapture = millis();
    server.send(200, "text/plain", "Camera Started");
}

void handleStop() {
    Serial.println("[WEB] STOP received");
    autoCapture = false;
    server.send(200, "text/plain", "Camera Stopped");
}

void handleResetWiFi() {
    Serial.println("[WEB] WiFi reset requested");
    server.send(200, "text/plain", "Resetting WiFi...");
    delay(1000);
    WiFi.disconnect(true, true);
    ESP.restart();
}

// ======================================================
// SETUP
// ======================================================
void setup() {
    Serial.begin(115200);
    Serial.println("\n[BOOT] CAM-1 Initializing...");
    
    pinMode(WIFI_RESET_PIN, INPUT_PULLUP);
    
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    
    if(psramFound()){
        config.frame_size = FRAMESIZE_VGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_CIF;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }
    
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x\n", err);
        return;
    }
    
    sensor_t *s = esp_camera_sensor_get();
    s->set_vflip(s, 1);
    s->set_hmirror(s, 0);
    
    // ================= WIFI MANAGER =================
    WiFiManager wm;
    if (digitalRead(WIFI_RESET_PIN) == LOW) {
        Serial.println("[WiFi] Reset button pressed — clearing saved SSID");
        wm.resetSettings();
        delay(1000);
    }
    
    if (!wm.autoConnect("CAM1_SETUP")) {
        Serial.println("[WiFi] Failed to connect. Restarting...");
        delay(2000);
        ESP.restart();
    }
    
    Serial.print("[WiFi] Connected. IP: ");
    Serial.println(WiFi.localIP());
    
    // ================= mDNS =================
    if (MDNS.begin("cam1")) {
        MDNS.addService("http", "tcp", 80);
        Serial.println("[mDNS] cam1.local ready");
    }
    
    server.on("/", handleRoot);
    server.on("/start", handleStart);
    server.on("/stop", handleStop);
    server.on("/resetwifi", handleResetWiFi);
    server.begin();
    
    Serial.println("[SYSTEM] Ready.");
}

// ======================================================
// LOOP
// ======================================================
void loop() {
    server.handleClient();
    
    if (autoCapture && (millis() - lastCapture > CAPTURE_INTERVAL)) {
        lastCapture = millis();
        captureAndUpload();
    }
}