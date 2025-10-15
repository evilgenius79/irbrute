# Telescope Focus Control via ESP-NOW

This project implements wireless telescope focus control using ESP-NOW protocol for communication between an M5Stack Core2 (controller) and a Xiao ESP32-S3 (motor driver).

## Hardware Requirements

### M5Stack Core2 (Sender/Controller)
- M5Stack Core2 device
- Built-in touchscreen for control interface
- WiFi/ESP-NOW capable

### Xiao ESP32-S3 (Receiver/Motor Driver)
- Seeed Studio Xiao ESP32-S3
- DRV8825 Stepper Motor Driver
- NEMA 17 Stepper Motor
- Power supply (12V recommended for motor)

## Wiring Diagram

### DRV8825 to Xiao ESP32-S3 Connections:
```
DRV8825          Xiao ESP32-S3
--------         -------------
STEP       <-->  D0 (GPIO 0)
DIR        <-->  D1 (GPIO 1)
ENABLE     <-->  D2 (GPIO 2)
GND        <-->  GND
VMOT       <-->  12V (Motor Power Supply)
GND        <-->  GND (Motor Power Supply)
```

### DRV8825 to NEMA 17 Stepper:
```
DRV8825          NEMA 17
--------         --------
A1, A2     <-->  Coil A (typically Red/Blue)
B1, B2     <-->  Coil B (typically Green/Black)
```

### DRV8825 Microstepping Configuration (MS1, MS2, MS3):
For 1/16 microstepping (recommended):
- MS1: HIGH
- MS2: HIGH  
- MS3: HIGH

## Software Setup

### 1. M5Stack Core2 Setup

1. Open `m5stack_core2_sender.ino` in Arduino IDE
2. Install required library:
   ```
   - M5Core2 (by M5Stack)
   ```
3. **Important**: Update the receiver MAC address:
   - Upload the receiver code first to get the Xiao's MAC address from Serial Monitor
   - Replace `receiverMAC[]` in line 15 with your Xiao's MAC address:
   ```cpp
   uint8_t receiverMAC[] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};
   ```
4. Select board: "M5Stack-Core2"
5. Upload the sketch

### 2. Xiao ESP32-S3 Setup

1. Open `xiao_esp32s3_receiver.ino` in Arduino IDE
2. No additional libraries required (uses built-in ESP-NOW)
3. Configure pins if needed (default: D0, D1, D2)
4. Adjust motor settings:
   - `STEPS_PER_REV`: Steps per revolution (200 for standard NEMA 17)
   - `MICROSTEPS`: Microstepping setting (16 for 1/16 stepping)
   - `MAX_POSITION`: Maximum focus position limit
5. Select board: "XIAO_ESP32S3"
6. Upload the sketch
7. Open Serial Monitor (115200 baud) to see the MAC address
8. Copy this MAC address to the M5Stack Core2 code

## Usage

### M5Stack Core2 Interface

The touchscreen displays the following controls:

#### Main Buttons (100 steps):
- **FOCUS IN** (Blue): Move focus inward by 100 steps
- **FOCUS OUT** (Green): Move focus outward by 100 steps  
- **STOP** (Red): Stop all movement immediately

#### Fine Control Buttons:
- **IN x10**: Move inward by 10 steps
- **OUT x10**: Move outward by 10 steps
- **IN x1**: Move inward by 1 step
- **OUT x1**: Move outward by 1 step

### Status Display

The screen shows:
- Command being sent (Command, Value, Message ID)
- Send status (SUCCESS/FAILED)
- Response from receiver (ACK/ERROR/POSITION)
- Timeout notifications if no response

### Error Codes

When receiver sends ERROR response:
- **Code 1**: Movement would exceed minimum position limit
- **Code 2**: Movement would exceed maximum position limit
- **Code 3**: Invalid target position
- **Code 4**: Unknown command

## Communication Protocol

### Command Structure
```cpp
struct_message {
  char command[32];     // Command name
  int value;            // Parameter value
  unsigned long id;     // Message ID
}
```

### Response Structure
```cpp
struct_response {
  char status[32];      // "ACK", "ERROR", "POSITION"
  int value;            // Current position or error code
  unsigned long id;     // Message ID being acknowledged
}
```

### Supported Commands
- `FOCUS_IN`: Move focus inward by specified steps
- `FOCUS_OUT`: Move focus outward by specified steps
- `STOP`: Stop current movement
- `SET_POSITION`: Move to absolute position
- `GET_POSITION`: Request current position
- `HOME`: Move to home position (0)

## Configuration

### Adjusting Motor Speed
In `xiao_esp32s3_receiver.ino`, modify:
```cpp
stepDelay = 1000000 / 500; // 500 steps/sec
```
Higher values = slower movement
Lower values = faster movement

### Position Limits
```cpp
#define MIN_POSITION  0
#define MAX_POSITION  10000
```

### Microstepping
Match the microstepping in code to hardware settings:
```cpp
#define MICROSTEPS 16  // Must match MS1, MS2, MS3 pins
```

## Troubleshooting

### No Communication
1. Verify both devices show their MAC addresses in Serial Monitor
2. Confirm sender has correct receiver MAC address
3. Check both devices are powered on
4. Ensure WiFi is in Station mode on both devices

### Motor Not Moving
1. Check DRV8825 connections (STEP, DIR, ENABLE pins)
2. Verify motor power supply (VMOT) is connected and adequate (12V recommended)
3. Check ENABLE pin is active (should be LOW when enabled)
4. Verify microstepping pins match code configuration
5. Check motor coil connections

### Erratic Movement
1. Add capacitor (100µF) across motor power supply
2. Adjust step delay for smoother operation
3. Check for loose connections
4. Ensure adequate power supply current rating

### Position Drift
1. Enable holding torque by keeping motor enabled after movement
2. Use smaller microsteps for finer control
3. Consider adding position encoder for feedback

## Advanced Features

### Adding Position Feedback
You could add an encoder to verify position:
```cpp
// Add encoder library and implement position verification
```

### Speed Ramping
Implement acceleration/deceleration:
```cpp
// Gradually increase/decrease step delay at start/end of movement
```

### Auto-Homing
Add limit switch for auto-homing:
```cpp
// Define home switch pin and implement homing routine
```

## License

This code is provided as-is for educational and hobbyist use.

## Credits

Based on the irbrute IR remote control project.
Adapted for telescope focus control with ESP-NOW wireless communication.
