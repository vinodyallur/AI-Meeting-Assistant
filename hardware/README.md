# Hardware Documentation - AI Meeting Assistant

This document provides detailed hardware specifications, schematics, and assembly instructions for the AI Meeting Assistant devices.

## Device Specifications

### 1. Audio Recording Device (Main Controller)

#### Core Components:
- **Microcontroller**: ESP32-WROOM-32 (Dual-core, 240MHz)
- **Flash Memory**: 4MB
- **PSRAM**: 4MB (optional for audio buffering)
- **WiFi**: 802.11 b/g/n (2.4GHz)
- **Bluetooth**: BLE 4.2

#### Audio Components:
- **Microphone**: INMP441 I2S Digital Microphone
  - Sampling Rate: 16kHz
  - Bit Depth: 24-bit (downsampled to 16-bit)
  - Sensitivity: -26dBFS
  - Signal-to-Noise Ratio: 61dB
- **Audio Interface**: I2S (Inter-IC Sound)
  - Master clock generation
  - Left-right clock synchronization
  - Serial data transmission

#### Display:
- **OLED**: SSD1306 128x64 I2C
  - Resolution: 128x64 pixels
  - Interface: I2C (0x3C address)
  - Contrast Ratio: 10,000:1
  - Viewing Angle: 160°

#### User Interface:
- **Buttons**: 3x Tactile switches
  - START: GPIO15 (Pull-up, active LOW)
  - STOP: GPIO14 (Pull-up, active LOW)
  - WiFi RESET: GPIO0 (Pull-up, active LOW)
- **LED Indicators**: 
  - Power LED: Red (3.3V)
  - Status LED: Blue (GPIO2)
  - WiFi LED: Green (GPIO13)

#### Power:
- **Input**: 5V DC via USB-C
- **Regulator**: AMS1117 3.3V (800mA)
- **Current Consumption**:
  - Idle: 80mA
  - Recording: 180mA
  - WiFi Active: 240mA
- **Battery Option**: 3.7V Li-Po with charging circuit

#### Dimensions:
- **PCB Size**: 60mm x 40mm
- **Enclosure Size**: 65mm x 45mm x 20mm
- **Weight**: 45g (without battery)

### 2. Camera Device (ESP32-CAM)

#### Core Components:
- **Microcontroller**: ESP32-S (Single-core, 240MHz)
- **Flash Memory**: 4MB
- **PSRAM**: 4MB (required for camera)
- **WiFi**: 802.11 b/g/n (2.4GHz)

#### Camera Module:
- **Sensor**: OV2640
  - Resolution: 2MP (1600x1200)
  - Pixel Size: 2.8μm x 2.8μm
  - Output Format: JPEG, RGB565, YUV422
  - Frame Rate: 15fps @ VGA
- **Lens**: Fixed focus, F2.0
  - Field of View: 65°
  - Focal Length: 3.6mm
  - IR Filter: Yes

#### Storage:
- **MicroSD Card Slot**: Support up to 16GB
- **File System**: FAT32
- **Storage Usage**: 30KB/image @ VGA quality 10

#### Power:
- **Input**: 5V DC via GPIO pins
- **Current Consumption**:
  - Idle: 120mA
  - Camera Active: 280mA
  - WiFi + Camera: 320mA
- **Power Options**: 
  - USB to TTL converter
  - External 5V power supply

#### Dimensions:
- **Module Size**: 27mm x 40.5mm
- **Enclosure Size**: 35mm x 50mm x 25mm
  - Camera opening: 8mm diameter
  - Mounting holes: M2.5

### 3. Network Specifications

#### WiFi Requirements:
- **Frequency**: 2.4GHz only (ESP32 limitation)
- **Security**: WPA2 Personal minimum
- **Signal Strength**: -70dBm minimum
- **Network Type**: Infrastructure mode
- **IP Assignment**: DHCP or static

#### mDNS Configuration:
- **Audio Device**: `audio.local`
- **Camera 1**: `cam1.local`
- **Camera 2**: `cam2.local`
- **Service Discovery**: HTTP on port 80

## Schematic Diagrams

### Audio Device Schematic:

```
                       ESP32-WROOM-32
                      ┌──────────────┐
    5V USB ──────────┤ VIN           │
                     │               │
    GND ─────────────┤ GND           │
                     │               │
    I2S_MIC_WS ─────┤ GPIO25        │
    I2S_MIC_SCK ────┤ GPIO26        │
    I2S_MIC_SD ─────┤ GPIO33        │
                     │               │
    OLED_SCL ───────┤ GPIO22        │
    OLED_SDA ───────┤ GPIO21        │
                     │               │
    BTN_START ──────┤ GPIO15 ─┬─10K─┤ 3.3V
    BTN_STOP ───────┤ GPIO14 ─┼─10K─┤ 3.3V
    BTN_RESET ──────┤ GPIO0 ──┴─10K─┤ 3.3V
                     │               │
    STATUS_LED ─────┤ GPIO2 ─┬─220Ω─┤ GND
    WIFI_LED ───────┤ GPIO13 ─┴─220Ω─┤ GND
                      └──────────────┘
```

### Power Regulation:
```
    5V USB ────┬─── AMS1117-3.3 ─── 3.3V
               │        │
             10µF     10µF
               │        │
              GND      GND
```

### Camera Device Schematic (Standard ESP32-CAM):

Refer to AI Thinker ESP32-CAM pinout diagram.

## Bill of Materials (BOM)

### Audio Device BOM:

| Quantity | Component | Part Number | Description |
|----------|-----------|-------------|-------------|
| 1 | ESP32-WROOM-32 | ESP32-WROOM-32 | Main microcontroller |
| 1 | I2S Microphone | INMP441 | Digital microphone |
| 1 | OLED Display | SSD1306 | 128x64 I2C display |
| 3 | Tactile Switch | TS-1187A | 6x6mm tactile switch |
| 1 | Voltage Regulator | AMS1117-3.3 | 3.3V 800mA regulator |
| 2 | LED | 5mm LED | Red (power), Blue (status) |
| 1 | USB-C Connector | USB-C-16P | USB Type-C port |
| 2 | Capacitor | 10µF 16V | Ceramic capacitor |
| 6 | Resistor | 10KΩ 0805 | Pull-up resistors |
| 2 | Resistor | 220Ω 0805 | LED current limiting |
| 1 | PCB | 60x40mm | 2-layer FR4 |
| 1 | Enclosure | Custom 3D print | ABS plastic |

### Camera Device BOM:

| Quantity | Component | Part Number | Description |
|----------|-----------|-------------|-------------|
| 1 | ESP32-CAM | AI-Thinker | Camera module |
| 1 | OV2640 Camera | OV2640 | 2MP camera sensor |
| 1 | MicroSD Slot | Push-push type | TF card slot |
| 1 | Antenna | PCB antenna | 2.4GHz antenna |
| 1 | 5V Regulator | RT9013-33GB | 3.3V 500mA LDO |
| 2 | Capacitor | 10µF 16V | Ceramic capacitor |
| 1 | LED | 0805 LED | Status indicator |
| 1 | Resistor | 1KΩ 0805 | LED resistor |
| 1 | Enclosure | Custom 3D print | ABS plastic |

## Assembly Instructions

### Audio Device Assembly:

1. **PCB Preparation**:
   - Clean PCB with isopropyl alcohol
   - Apply solder paste to pads
   - Place components using tweezers
   - Reflow with hot air station (230°C)

2. **Component Placement Order**:
   - Solder voltage regulator first
   - Solder ESP32 module
   - Solder passive components (resistors, capacitors)
   - Solder connectors (USB-C, headers)
   - Solder display and microphone

3. **Testing Sequence**:
   - Power test (check 3.3V output)
   - ESP32 boot test (serial output)
   - WiFi connection test
   - I2S microphone test
   - OLED display test
   - Button functionality test

### Camera Device Assembly:

1. **Module Preparation**:
   - Ensure PSRAM is properly soldered
   - Check camera connector alignment
   - Verify antenna connection

2. **Enclosure Assembly**:
   - 3D print enclosure parts
   - Install camera with lens aligned to opening
   - Secure ESP32-CAM module
   - Add mounting holes for wall/ceiling mount

3. **Initial Testing**:
   - Flash test firmware
   - Verify camera initialization
   - Test WiFi connectivity
   - Test image capture and upload

## Performance Specifications

### Audio Performance:
- **Frequency Response**: 100Hz - 8kHz (±3dB)
- **Total Harmonic Distortion**: <1% @ 1kHz
- **Dynamic Range**: >60dB
- **Signal-to-Noise Ratio**: >55dB
- **Latency**: 100ms (acquisition to cloud)

### Camera Performance:
- **Resolution**: VGA (640x480) default
- **Compression**: JPEG quality 10
- **Capture Interval**: 30 seconds
- **Image Size**: 20-40KB per image
- **Upload Latency**: <2 seconds

### Network Performance:
- **Connection Time**: <5 seconds
- **Reconnection Time**: <2 seconds
- **Audio Bitrate**: 256kbps
- **Image Upload Time**: <1 second/image
- **Connection Stability**: >99% uptime

## Environmental Specifications

### Operating Conditions:
- **Temperature**: 0°C to 40°C
- **Humidity**: 20% to 80% non-condensing
- **Altitude**: 0 to 2000 meters
- **Vibration**: 5-500Hz, 0.5g

### Storage Conditions:
- **Temperature**: -10°C to 60°C
- **Humidity**: 10% to 90% non-condensing

### Power Requirements:
- **Input Voltage**: 5V ±5%
- **Input Current**: 500mA minimum
- **Power Supply**: Regulated 5V DC
- **Protection**: Reverse polarity, over-current

## Compliance and Certifications

### Regulatory Compliance:
- **FCC Part 15**: Class B digital device
- **CE**: European conformity
- **RoHS**: Lead-free manufacturing
- **REACH**: Chemical compliance

### Safety Standards:
- **UL 60950-1**: IT equipment safety
- **IEC 62368-1**: Audio/video equipment

### Wireless Certifications:
- **FCC ID**: [To be obtained]
- **CE-RED**: Radio equipment directive
- **IC**: Industry Canada

## Maintenance and Troubleshooting

### Preventive Maintenance:
- Monthly: Clean microphone port
- Quarterly: Check WiFi connectivity
- Annually: Firmware update check
- As needed: Enclosure cleaning

### Common Issues:

1. **Audio Quality Issues**:
   - Check microphone alignment
   - Verify sampling rate configuration
   - Test in different acoustic environments

2. **Camera Focus Problems**:
   - Adjust lens focus manually
   - Check for lens contamination
   - Verify lighting conditions

3. **Network Connectivity**:
   - Verify WiFi signal strength
   - Check mDNS configuration
   - Test with different access points

4. **Power Issues**:
   - Measure 3.3V rail voltage
   - Check current consumption
   - Test with different power supplies

### Spare Parts:
Keep spare components for quick repairs:
- I2S microphones
- OLED displays
- Tactile switches
- Camera modules
- Voltage regulators

## Customization Options

### Hardware Modifications:
1. **Enhanced Audio**: Add second microphone for stereo
2. **External Storage**: Add microSD slot for local recording
3. **Battery Backup**: Add Li-Po battery with charging
4. **PoE Support**: Add Ethernet with Power over Ethernet
5. **Environmental Sensors**: Add temperature/humidity sensor

### Enclosure Options:
- **Wall Mount**: Flush mount with cable management
- **Table Stand**: Adjustable angle stand
- **Ceiling Mount**: Discrete ceiling installation
- **Portable Case**: Carrying case with battery

### Integration Options:
- **Room Control**: Integration with smart room systems
- **Calendar Integration**: Auto-schedule based on calendar
- **Access Control**: Integration with door access systems
- **Notification Systems**: Integration with messaging platforms

## Support and Documentation

### Additional Resources:
- [Firmware Repository](https://github.com/your-org/ai-meeting-assistant-firmware)
- [Cloud Backend Documentation](../cloud-backend/README.md)
- [API Documentation](../documentation/API_DOCUMENTATION.md)
- [Troubleshooting Guide](../documentation/TROUBLESHOOTING.md)

### Contact Information:
- **Hardware Support**: hardware@vinshanks.com
- **Technical Documentation**: docs@vinshanks.com
- **Parts and Ordering**: parts@vinshanks.com

### Warranty:
- **Duration**: 1 year from date of purchase
- **Coverage**: Manufacturing defects
- **Exclusions**: Physical damage, water damage, unauthorized modifications
- **Service**: Return to factory for repair/replacement