# Setup Guide for AI Meeting Assistant

## Hardware Requirements

### Audio Device (Main Controller)
- ESP32-WROOM development board
- I2S microphone module (INMP441 or MAX9814)
- SSD1306 OLED display (128x64)
- 3x Tactile buttons
- Breadboard and jumper wires
- USB cable for programming

### Camera Devices (2 Units)
- ESP32-CAM module with OV2640 camera
- FTDI programmer (for ESP32-CAM programming)
- Power supply (5V, 2A)

### Additional Components
- WiFi network with internet access
- Azure subscription for cloud services
- 3D printed enclosures (optional)

## Step-by-Step Setup

### Step 1: Assemble Hardware

#### Audio Device Wiring:
```
ESP32 Pin    Component       Pin
GPIO25    → I2S Mic WS (LRCLK)
GPIO26    → I2S Mic SCK (BCLK)  
GPIO33    → I2S Mic SD (DATA)
GPIO22    → OLED SCL
GPIO21    → OLED SDA
GPIO15    → START Button (to GND)
GPIO14    → STOP Button (to GND)
GPIO0     → RESET Button (to GND)
3.3V      → All VCC pins
GND       → All GND pins
```

#### Camera Device:
Use standard ESP32-CAM pinout. No additional wiring needed.

### Step 2: Install Development Environment

1. **Install Arduino IDE** from [arduino.cc](https://www.arduino.cc/en/software)
2. **Add ESP32 Board Support**:
   - Open Arduino IDE → File → Preferences
   - Add to Additional Boards Manager URLs:
     ```
     https://espressif.github.io/arduino-esp32/package_esp32_index.json
     ```
   - Tools → Board → Boards Manager → Search "ESP32" → Install

3. **Install Required Libraries**:
   - Sketch → Include Library → Manage Libraries
   - Search and install:
     - WiFi
     - WebServer
     - WiFiClientSecure
     - HTTPClient
     - ESPmDNS
     - WiFiManager by tzapu
     - Adafruit SSD1306 by Adafruit
     - Adafruit GFX Library by Adafruit
     - ESP32 Camera (for camera device)

### Step 3: Configure Firmware

#### For Audio Device:
1. Open `firmware/audio-device/audio_device.ino`
2. Update cloud endpoints:
   ```cpp
   // Update these to your Azure endpoints
   const char* CLOUD_HOST = "your-app.purplebay.azurecontainerapps.io";
   const char* CLOUD_PATH = "/api/upload?filename=live.wav&mac_address=MIC_DEVICE_01";
   ```

#### For Camera Device:
1. Open `firmware/camera-device/camera_device.ino`
2. Update cloud endpoints:
   ```cpp
   const char* UPLOAD_HOST = "your-app.purplebay.azurecontainerapps.io";
   const char* UPLOAD_PATH = "/api/upload_image";
   ```

### Step 4: Upload Code

#### Audio Device:
1. Connect ESP32 via USB
2. Tools → Board → ESP32 Dev Module
3. Tools → Port → Select COM port
4. Tools → Upload Speed → 921600
5. Click Upload

#### Camera Device:
1. Connect ESP32-CAM via FTDI programmer
2. Tools → Board → AI Thinker ESP32-CAM
3. Tools → Port → Select COM port
4. Tools → Flash Mode → QIO
5. Tools → Partition Scheme → Huge App
6. Hold BOOT button, press RESET, release RESET, release BOOT
7. Click Upload

### Step 5: WiFi Configuration

1. Power on the device
2. Look for WiFi network:
   - Audio Device: "Audio-ESP32-Setup"
   - Camera Device: "CAM1_SETUP" or "CAM2_SETUP"
3. Connect to the network
4. Captive portal should open automatically
5. Select your WiFi network and enter password
6. Device will restart and connect

### Step 6: Cloud Setup (Azure)

1. **Create Azure Resources**:
   - Azure Container Apps for backend API
   - Azure Blob Storage for recordings
   - Azure Cognitive Services for STT
   - Azure Container Registry for Docker images

2. **Deploy Backend**:
   ```bash
   # Clone backend repository
   git clone <backend-repo>
   cd cloud-backend
   
   # Build and deploy
   az containerapp create \
     --name stt-premium-app \
     --resource-group your-rg \
     --image yourregistry.azurecr.io/meeting-backend:latest \
     --target-port 8080 \
     --ingress external
   ```

3. **Configure Environment Variables**:
   ```bash
   az containerapp update \
     --name stt-premium-app \
     --set-env-vars \
       STORAGE_CONNECTION_STRING="your-storage-connection" \
       SPEECH_KEY="your-speech-key" \
       OPENAI_API_KEY="your-openai-key"
   ```

### Step 7: Testing

#### Test Audio Device:
1. Open Serial Monitor (115200 baud)
2. Press START button
3. Verify "MEETING STARTED" on OLED
4. Check Azure logs for audio uploads
5. Press STOP button
6. Verify "MEETING ENDED" on OLED

#### Test Camera Device:
1. Open browser to `http://cam1.local/`
2. Should see "CAM-1 READY" message
3. Access `http://cam1.local/start`
4. Camera should capture and upload image
5. Check Azure Blob Storage for uploaded images

#### Test Complete System:
1. Start meeting with audio device
2. Verify all cameras receive start command
3. Check cloud logs for simultaneous audio and image uploads
4. Stop meeting
5. Verify AI processing trigger

### Step 8: Production Deployment

#### Hardware Final Assembly:
1. Solder components on PCB
2. 3D print enclosures
3. Assemble with proper cable management
4. Label devices with MAC addresses

#### Network Configuration:
1. Assign static IPs or DHCP reservations
2. Configure firewall rules for Azure endpoints
3. Set up network monitoring

#### Monitoring Setup:
1. Azure Application Insights
2. Device health monitoring
3. Alert rules for failures
4. Usage analytics

## Troubleshooting

### Common Issues:

1. **WiFi Connection Issues**:
   - Press reset button while holding GPIO0 low
   - Check WiFi signal strength
   - Verify credentials in captive portal

2. **Camera Not Working**:
   - Check PSRAM allocation
   - Verify camera module compatibility
   - Ensure proper power supply (5V, 2A)

3. **Audio Streaming Problems**:
   - Verify I2S microphone connections
   - Check sample rate compatibility
   - Test with different microphone

4. **Cloud Connection Failed**:
   - Verify Azure endpoint URL
   - Check network firewall settings
   - Test with Postman/curl

5. **OLED Display Issues**:
   - Check I2C address (0x3C)
   - Verify SCL/SDA connections
   - Test with Adafruit example sketches

### Debug Logs:

Enable verbose logging by modifying Serial.begin:
```cpp
Serial.begin(115200);
Serial.setDebugOutput(true);
```

Check Serial Monitor for detailed debug information.

## Maintenance

### Regular Tasks:
1. Update firmware for security patches
2. Monitor Azure costs and usage
3. Backup configuration settings
4. Test full system periodically

### Scaling:
1. Add more camera devices as needed
2. Scale Azure resources based on usage
3. Implement load balancing for multiple meetings

## Support

For additional help:
1. Check GitHub Issues for known problems
2. Review Azure documentation
3. Contact development team with logs and error details