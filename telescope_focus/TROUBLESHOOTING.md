# Troubleshooting Guide - Telescope Focus Control

This guide helps you diagnose and fix common issues with the telescope focus control system.

## Quick Diagnostics

### Check Communication
```
Test: Tap STOP button on M5Stack
Expected: 
  - M5Stack shows "Sent: SUCCESS" and "ACK: ID=X"
  - Xiao Serial shows "Received: Cmd=STOP"
Result: _______________
```

### Check Motor Power
```
Test: Send any movement command
Expected: Motor makes noise/vibration
Result: _______________
```

### Check Direction
```
Test: Send FOCUS_IN, then FOCUS_OUT
Expected: Motor moves in opposite directions
Result: _______________
```

---

## Problem Categories

### 🔴 Critical Issues (System Won't Work)

#### 1. No Communication Between Devices

**Symptoms:**
- M5Stack shows "Sent: FAILED" or "Timeout: No ACK"
- Xiao Serial Monitor shows no received messages
- No response from receiver

**Diagnosis Steps:**
1. Check Xiao Serial Monitor shows "ESP-NOW initialized"
2. Verify M5Stack shows its MAC address during startup
3. Confirm MAC address in sender code matches Xiao's MAC

**Solutions:**

**A. Wrong MAC Address**
```cpp
// In m5stack_core2_sender.ino
// Make sure this matches your Xiao's MAC (from get_mac_address.ino)
uint8_t receiverMAC[] = {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6};
```

**B. ESP-NOW Not Initialized**
- Re-upload code to both devices
- Check Serial output for initialization errors
- Ensure WiFi.mode(WIFI_STA) is called

**C. Power/Reset Issue**
- Power cycle both devices
- Press reset button on both
- Try different USB cables/ports

---

#### 2. Motor Doesn't Move At All

**Symptoms:**
- Commands are acknowledged
- No motor movement, sound, or vibration
- Serial shows "Movement stopped" immediately

**Diagnosis Steps:**
1. Check ENABLE pin connection (should be connected to D2)
2. Measure voltage on VMOT (should be ~12V)
3. Check if motor gets warm (indicates power but no movement)

**Solutions:**

**A. ENABLE Pin Issue**
```cpp
// In xiao_esp32s3_receiver.ino
// ENABLE should be active LOW
digitalWrite(ENABLE_PIN, LOW);  // Motor enabled
digitalWrite(ENABLE_PIN, HIGH); // Motor disabled
```

**B. No Motor Power**
- Check 12V power supply is connected to VMOT
- Verify GND connection from PSU to DRV8825
- Test PSU output with multimeter

**C. Wrong Pin Definitions**
```cpp
// Verify in xiao_esp32s3_receiver.ino
#define STEP_PIN      D0    
#define DIR_PIN       D1    
#define ENABLE_PIN    D2    
```

**D. Motor Coil Disconnected**
- Test continuity of motor coils with multimeter
- Each coil should show resistance (typically 1-5 ohms)
- Check all 4 wires are firmly connected to A1, A2, B1, B2

---

#### 3. DRV8825 Overheating/Shutting Down

**Symptoms:**
- Driver IC gets very hot to touch
- Motor stops after brief movement
- Thermal shutdown (built-in protection)

**Solutions:**

**A. Reduce Current Limit**
1. Turn off power
2. Adjust potentiometer on DRV8825 (turn counterclockwise to reduce)
3. Measure Vref with multimeter
4. Formula: Vref = Motor_Current / 2
5. For 1A motor: Vref should be 0.5V

**B. Add Heat Sink**
- Attach small heat sink to DRV8825 IC
- Can use thermal adhesive or thermal pad
- Ensure airflow around driver

**C. Reduce Motor Speed**
```cpp
// In xiao_esp32s3_receiver.ino
stepDelay = 1000000 / 250; // Slower: 250 steps/sec instead of 500
```

---

### 🟡 Movement Issues

#### 4. Motor Moves in Wrong Direction

**Symptoms:**
- FOCUS_IN moves out, or vice versa
- Position increases when it should decrease

**Solutions:**

**A. Swap One Coil (Hardware)**
- Power off system
- Swap either A1↔A2 OR B1↔B2 (not both!)
- Power back on and test

**B. Invert Direction (Software)**
```cpp
// In xiao_esp32s3_receiver.ino, modify setDirection()
void setDirection(bool forward) {
  digitalWrite(DIR_PIN, !forward); // Added negation
  direction = forward;
  delayMicroseconds(5);
}
```

---

#### 5. Motor Skips Steps / Loses Position

**Symptoms:**
- Motor makes grinding/clicking sounds
- Position drifts over time
- Not reaching target position
- Stuttering movement

**Solutions:**

**A. Increase Current Limit**
1. Adjust Vref potentiometer (turn clockwise slightly)
2. Monitor motor temperature
3. Don't exceed motor's rated current

**B. Decrease Speed**
```cpp
// In xiao_esp32s3_receiver.ino
stepDelay = 1000000 / 300; // Slower movement
```

**C. Check Mechanical Load**
- Ensure focuser moves freely
- Reduce friction/binding
- Check coupling alignment

**D. Add Decoupling Capacitor**
- Solder 100µF capacitor across VMOT and GND
- Place close to DRV8825
- Observe polarity (+/-)

**E. Increase Microstepping**
```cpp
// If currently using full step, try 1/4 or 1/8
// Set MS1, MS2, MS3 appropriately
// Update code:
#define MICROSTEPS 8  // Instead of 16
```

---

#### 6. Erratic or Jittery Movement

**Symptoms:**
- Motor vibrates but doesn't move smoothly
- Random direction changes
- Unpredictable behavior

**Solutions:**

**A. Check Microstepping Configuration**
Verify MS1, MS2, MS3 match code:
```
For 1/16: MS1=HIGH, MS2=HIGH, MS3=HIGH
Code: #define MICROSTEPS 16
```

**B. Add Noise Filtering**
```cpp
// Add capacitors:
// - 100µF electrolytic on VMOT to GND
// - 0.1µF ceramic on each logic pin to GND
```

**C. Check Wiring**
- Use twisted pair for STEP and DIR signals
- Keep motor wires away from signal wires
- Shorten wire lengths if possible

**D. Improve Grounding**
- Ensure common ground between all components
- Use star grounding topology
- Check for ground loops

---

### 🟢 Fine-Tuning Issues

#### 7. Position Drifts When Idle

**Symptoms:**
- Focus changes even when not commanded
- Motor doesn't hold position
- Gravity causes movement

**Solutions:**

**A. Enable Holding Torque**
```cpp
// In xiao_esp32s3_receiver.ino
void stopMovement() {
  isMoving = false;
  targetSteps = 0;
  stepsTaken = 0;
  // enableMotor(false);  // COMMENT OUT this line
  // Motor stays enabled to hold position
}
```

**B. Mechanical Solution**
- Add brake mechanism to focuser
- Use worm gear (self-locking)
- Add friction adjuster

---

#### 8. Movements Are Too Coarse

**Symptoms:**
- Can't achieve fine focus
- Steps are too large
- Need finer control

**Solutions:**

**A. Use Smaller Step Sizes**
```cpp
// In m5stack_core2_sender.ino
// Change button values:
sendCommand("FOCUS_IN", 1);   // Instead of 10 or 100
```

**B. Add More Fine Control Buttons**
```cpp
// Add ultra-fine controls
M5.Lcd.fillRect(x, y, w, h, ORANGE);
M5.Lcd.print("IN x0.1"); // Fractional control
// Note: Requires sub-step interpolation
```

**C. Increase Microstepping**
- Change from 1/16 to 1/32 (if driver supports)
- Provides finer resolution
- May reduce torque

---

#### 9. Response Too Slow

**Symptoms:**
- Long delay between command and action
- ACK timeout messages
- Sluggish performance

**Solutions:**

**A. Check WiFi Interference**
- Move devices closer together
- Avoid WiFi-congested areas
- Change WiFi channel if applicable

**B. Optimize Code**
```cpp
// In receiver loop()
delayMicroseconds(100); // Reduce from higher value
```

**C. Increase ESP-NOW Priority**
```cpp
// In setup(), add:
esp_wifi_set_ps(WIFI_PS_NONE); // Disable power saving
```

---

### 🔧 Configuration Issues

#### 10. Position Limit Errors

**Symptoms:**
- "ERROR: Code=1" or "ERROR: Code=2"
- Can't move in certain direction
- Unexpected limit reached

**Solutions:**

**A. Adjust Position Limits**
```cpp
// In xiao_esp32s3_receiver.ino
#define MIN_POSITION  0
#define MAX_POSITION  20000  // Increase if needed
```

**B. Reset Position**
```cpp
// Add to receiver:
else if (strcmp(incomingMessage.command, "RESET_POS") == 0) {
  currentPosition = 0;
  sendResponse("ACK", 0, incomingMessage.id);
}
```

**C. Use Home Command**
- Send HOME command to reset to 0
- Manually set position with SET_POSITION

---

### 📱 M5Stack Display Issues

#### 11. Touchscreen Not Responsive

**Solutions:**
- Calibrate touch: M5.Touch.calibration()
- Clean screen surface
- Update M5Core2 library
- Check USB power (needs sufficient current)

#### 12. Display Shows Wrong Information

**Solutions:**
- Clear display areas before updating:
```cpp
M5.Lcd.fillRect(0, 200, 320, 20, BLACK);
```
- Use proper text color contrast
- Increase font size for readability

---

## Diagnostic Commands

### Test Communication
```cpp
// From Arduino IDE Serial Monitor (connected to M5Stack)
// This won't work directly but shows concept
// Best to add debug commands to the code
```

### Xiao Serial Monitor Commands
Add to receiver code:
```cpp
// In loop(), check for Serial input:
if (Serial.available()) {
  char cmd = Serial.read();
  switch(cmd) {
    case 'p': Serial.printf("Position: %d\n", currentPosition); break;
    case 'e': enableMotor(!motorEnabled); break;
    case 's': stopMovement(); break;
  }
}
```

---

## Hardware Testing Procedures

### Test 1: Verify DRV8825 Power
```
1. Disconnect motor
2. Power on DRV8825
3. Measure VMOT to GND: Should be ~12V
4. Measure logic pins to GND: Should be 0V or 3.3V
```

### Test 2: Verify Motor Coils
```
1. Disconnect from driver
2. Measure resistance:
   - A1 to A2: 1-5 ohms (Coil A)
   - B1 to B2: 1-5 ohms (Coil B)
   - A1 to B1: Open circuit (infinite resistance)
```

### Test 3: Verify Signal Lines
```
1. Use oscilloscope or logic analyzer
2. Monitor STEP pin during movement
3. Should see square wave pulses
4. Monitor DIR pin - should be HIGH or LOW, stable
```

### Test 4: Verify Current Limit
```
1. Disconnect motor
2. Measure Vref on DRV8825
3. Calculate: Motor_Current = Vref × 2
4. Adjust if needed
```

---

## Common Error Messages

| Error | Meaning | Solution |
|-------|---------|----------|
| Sent: FAILED | ESP-NOW send failed | Check MAC address, restart devices |
| Timeout: No ACK | No response from receiver | Check receiver power, ESP-NOW init |
| ERROR: Code=1 | Min position limit | Increase MIN_POSITION or move out first |
| ERROR: Code=2 | Max position limit | Increase MAX_POSITION or move in first |
| ERROR: Code=3 | Invalid position | Check target value is in range |
| ERROR: Code=4 | Unknown command | Verify command string spelling |

---

## Getting Help

### Information to Collect:
1. Hardware setup (photo)
2. Serial Monitor output (both devices)
3. M5Stack screen (photo)
4. Code modifications made
5. Power supply specifications
6. Motor specifications

### Debug Checklist:
- [ ] Verified MAC addresses match
- [ ] Checked all wiring connections
- [ ] Measured power supply voltages
- [ ] Tested motor coils with multimeter
- [ ] Reviewed Serial Monitor for errors
- [ ] Tried with fresh code upload
- [ ] Power cycled all devices
- [ ] Checked for loose connections

### Where to Ask:
- GitHub Issues: [Repository URL]
- Arduino Forums: Electronics section
- Reddit: r/arduino, r/esp32
- M5Stack Community Forum
- Discord: ESP32 servers

---

## Advanced Debugging

### Enable Verbose Logging
```cpp
// Add to receiver setup():
esp_log_level_set("*", ESP_LOG_VERBOSE);

// Add debug prints:
Serial.printf("DEBUG: Current=%d Target=%d Steps=%d\n", 
              currentPosition, targetSteps, stepsTaken);
```

### Monitor ESP-NOW Status
```cpp
// In OnDataSent callback:
Serial.printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X Status: %s\n",
              mac_addr[0], mac_addr[1], mac_addr[2],
              mac_addr[3], mac_addr[4], mac_addr[5],
              status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
```

### Test Without Motor
```cpp
// Comment out actual stepping:
void performStep() {
  // digitalWrite(STEP_PIN, HIGH);
  // delayMicroseconds(5);
  // digitalWrite(STEP_PIN, LOW);
  
  Serial.println("Virtual step");
  
  if (direction) currentPosition++;
  else currentPosition--;
}
```

This allows testing communication and logic without motor load.
