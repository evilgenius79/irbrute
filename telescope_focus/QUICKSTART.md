# Quick Start Guide - Telescope Focus Control

This guide will help you get the telescope focus control system running quickly.

## Prerequisites

- [ ] M5Stack Core2
- [ ] Seeed Studio Xiao ESP32-S3
- [ ] DRV8825 Stepper Motor Driver
- [ ] NEMA 17 Stepper Motor
- [ ] 12V Power Supply (2A minimum)
- [ ] Jumper wires
- [ ] USB-C cables for programming
- [ ] Arduino IDE or PlatformIO

## Step 1: Get MAC Address (5 minutes)

### 1.1 Upload MAC Finder to Xiao
1. Open `get_mac_address.ino` in Arduino IDE
2. Select Board: "XIAO_ESP32S3"
3. Connect Xiao via USB-C
4. Upload sketch
5. Open Serial Monitor (115200 baud)
6. **Copy the MAC address** shown in array format, e.g.:
   ```
   uint8_t receiverMAC[] = {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6};
   ```

## Step 2: Setup Receiver (10 minutes)

### 2.1 Wire the Hardware
Follow the wiring diagram in `WIRING.md`:

**Xiao to DRV8825:**
```
Xiao D0  → DRV8825 STEP
Xiao D1  → DRV8825 DIR
Xiao D2  → DRV8825 ENABLE
Xiao GND → DRV8825 GND
```

**DRV8825 Microstepping (for 1/16 step):**
```
MS1 → VCC (3.3V or 5V)
MS2 → VCC (3.3V or 5V)
MS3 → VCC (3.3V or 5V)
```

**DRV8825 to Motor:**
```
A1, A2 → Motor Coil A (Red/Blue)
B1, B2 → Motor Coil B (Green/Black)
```

**DRV8825 Power:**
```
VMOT → 12V+ (from power supply)
GND  → 12V- (from power supply)
```

### 2.2 Upload Receiver Code
1. Open `xiao_esp32s3_receiver.ino` in Arduino IDE
2. Select Board: "XIAO_ESP32S3"
3. Connect Xiao via USB-C (can be connected while wired)
4. Upload sketch
5. Open Serial Monitor to verify it's working

### 2.3 Test Motor (Optional but Recommended)
1. Check Serial Monitor shows "ESP-NOW initialized"
2. Motor should be disabled (not holding position)
3. Manually rotate motor shaft - should turn freely

## Step 3: Setup Sender (10 minutes)

### 3.1 Update MAC Address
1. Open `m5stack_core2_sender.ino` in Arduino IDE
2. Find line 15: `uint8_t receiverMAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};`
3. **Replace with the MAC address from Step 1.1**
4. Save the file

### 3.2 Install M5Core2 Library
In Arduino IDE:
1. Go to Tools → Manage Libraries
2. Search for "M5Core2"
3. Install "M5Core2" by M5Stack

### 3.3 Upload Sender Code
1. Select Board: "M5Stack-Core2"
2. Connect M5Stack via USB-C
3. Upload sketch
4. Screen should show "Telescope Focus" interface

## Step 4: Test Communication (5 minutes)

### 4.1 Basic Test
1. Make sure both devices are powered
2. On M5Stack screen, tap "STOP" button
3. Check Xiao Serial Monitor - should show:
   ```
   Received: Cmd=STOP, Val=0, ID=1
   Movement stopped
   Send Status: Success
   ```
4. Check M5Stack screen - should show:
   ```
   Send: SUCCESS
   ACK: ID=1
   ```

### 4.2 Movement Test (without load)
1. Tap "IN x1" button on M5Stack
2. Motor should step once (you'll hear/feel a small movement)
3. Tap "OUT x1" button
4. Motor should step in opposite direction
5. Try larger movements: "IN x10", "OUT x10"

## Step 5: Final Setup (10 minutes)

### 5.1 Adjust Current Limit on DRV8825
**IMPORTANT**: Set before connecting heavy load!

1. Disconnect motor temporarily
2. Power up DRV8825
3. Use multimeter to measure voltage on Vref pin (potentiometer)
4. For 1A motor: Set Vref to 0.5V (Formula: Vref = Current/2)
5. Adjust using small screwdriver on potentiometer
6. Reconnect motor

### 5.2 Mount to Telescope
1. Attach motor to telescope focuser (coupling or belt)
2. Test movement with M5Stack controls
3. Verify direction (IN = closer focus, OUT = farther focus)
4. If direction is backwards, swap motor coil wires or change code

### 5.3 Set Position Limits
If you need different limits:
1. Edit `xiao_esp32s3_receiver.ino`
2. Change: `#define MAX_POSITION 10000` to your value
3. Re-upload to Xiao

## Troubleshooting

### No Communication
- [ ] Verify MAC address in sender matches receiver
- [ ] Check both devices are powered
- [ ] Restart both devices
- [ ] Verify Serial Monitor shows ESP-NOW initialized

### Motor Not Moving
- [ ] Check ENABLE pin connection (D2)
- [ ] Verify motor power supply (12V connected to VMOT)
- [ ] Test motor connections (should have continuity on each coil)
- [ ] Check Serial Monitor for error messages

### Motor Moves Wrong Direction
- [ ] Swap either Coil A or Coil B wires (not both)
- OR
- [ ] In receiver code, change line 87: `setDirection(!forward)` 

### Motor Skips Steps
- [ ] Increase current limit (adjust Vref)
- [ ] Decrease speed (increase stepDelay)
- [ ] Check for loose wiring
- [ ] Add decoupling capacitor (100µF)

### Position Drifts
- [ ] Motor not holding when stopped - enable holding torque
- [ ] In receiver, modify stopMovement():
  ```cpp
  void stopMovement() {
    isMoving = false;
    // enableMotor(false);  // Comment this out to hold position
  }
  ```

## Next Steps

### Customize the Interface
Edit M5Stack code to:
- Change button layout
- Modify step sizes
- Add preset positions
- Show current position on screen

### Add Features
- Position presets
- Speed control
- Auto-focus routine
- Position memory (EEPROM)
- Limit switches for auto-homing

### Optimize Performance
- Fine-tune acceleration/deceleration
- Implement speed ramping
- Add position encoder feedback
- Temperature compensation

## Support

- Check `README.md` for detailed documentation
- See `WIRING.md` for complete wiring diagrams
- Review code comments for customization options

## Success Checklist

- [ ] MAC address obtained and updated in sender
- [ ] Receiver wired correctly
- [ ] Communication working (ACK messages received)
- [ ] Motor moves in both directions
- [ ] Current limit set appropriately
- [ ] Mounted to telescope and tested
- [ ] Position limits configured
- [ ] Direction verified

**Congratulations!** Your wireless telescope focus control is ready to use!
