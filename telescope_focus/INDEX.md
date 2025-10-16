# Telescope Focus Control - Documentation Index

Welcome to the Telescope Focus Control project! This ESP-NOW-based wireless system allows you to control a telescope focuser using an M5Stack Core2 touchscreen controller and a Xiao ESP32-S3 with stepper motor driver.

## 📚 Documentation Files

### Getting Started (Read These First!)

1. **[README.md](README.md)** - Main project overview and features
   - Hardware requirements
   - Software setup instructions
   - Usage guide
   - Configuration options

2. **[QUICKSTART.md](QUICKSTART.md)** - Step-by-step setup guide
   - 5-step quick start (30 minutes total)
   - MAC address configuration
   - Hardware assembly
   - Testing procedures
   - Success checklist

3. **[WIRING.md](WIRING.md)** - Complete wiring diagrams
   - Visual connection diagrams
   - Pin assignments
   - Power supply specifications
   - Microstepping configuration
   - Safety warnings

### Technical Documentation

4. **[ARCHITECTURE.md](ARCHITECTURE.md)** - System architecture
   - System overview diagrams
   - Communication protocol
   - Data structures
   - Command reference
   - State machines
   - Performance specs

5. **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)** - Problem solving
   - Common issues and solutions
   - Diagnostic procedures
   - Error code reference
   - Hardware testing
   - Debug techniques

### Code Files

6. **[m5stack_core2_sender.ino](m5stack_core2_sender.ino)** - M5Stack sender
   - Touchscreen interface
   - ESP-NOW sender
   - Command generator
   - Status display

7. **[xiao_esp32s3_receiver.ino](xiao_esp32s3_receiver.ino)** - Xiao receiver
   - ESP-NOW receiver
   - DRV8825 stepper control
   - Position tracking
   - Error handling

8. **[get_mac_address.ino](get_mac_address.ino)** - MAC finder utility
   - Get device MAC address
   - Format for code

### Configuration

9. **[telescope_focus_config.h](telescope_focus_config.h)** - Shared config
   - Protocol structures
   - Command definitions
   - Default values
   - Constants

10. **[platformio.ini](platformio.ini)** - Build configuration
    - PlatformIO setup
    - Environment configs
    - Library dependencies

## 🚀 Quick Navigation

### I want to...

#### Build the System
→ Start with [QUICKSTART.md](QUICKSTART.md)

#### Understand How It Works
→ Read [ARCHITECTURE.md](ARCHITECTURE.md)

#### Connect the Hardware
→ Follow [WIRING.md](WIRING.md)

#### Fix a Problem
→ Check [TROUBLESHOOTING.md](TROUBLESHOOTING.md)

#### Customize the Code
→ Read [README.md](README.md) Configuration section

#### Get Device MAC Address
→ Use [get_mac_address.ino](get_mac_address.ino)

## 📋 Project Overview

### What This System Does
- **Wireless Control**: Send commands from M5Stack Core2 to Xiao ESP32-S3 via ESP-NOW
- **Motor Control**: Drive NEMA 17 stepper motor through DRV8825 driver
- **Focus Adjustment**: Precise telescope focus control with multiple speed settings
- **Bidirectional Communication**: Acknowledgments and error reporting
- **Position Tracking**: Real-time position monitoring with limits

### Hardware Components
- **Controller**: M5Stack Core2 (touchscreen, ESP32-based)
- **Receiver**: Seeed Studio Xiao ESP32-S3
- **Driver**: DRV8825 Stepper Motor Driver
- **Motor**: NEMA 17 Stepper Motor
- **Power**: 12V DC power supply (2A minimum)

### Key Features
- ✅ No WiFi router required (ESP-NOW direct communication)
- ✅ Touchscreen interface with visual feedback
- ✅ Multiple movement speeds (coarse and fine control)
- ✅ Position limits and error handling
- ✅ Acknowledgment system for reliable operation
- ✅ Configurable microstepping (1/16 default)
- ✅ Real-time position tracking

## 🛠️ Setup Summary

### Step 1: Get MAC Address (5 min)
Upload `get_mac_address.ino` to Xiao, copy the MAC address

### Step 2: Wire Hardware (10 min)
Connect Xiao → DRV8825 → NEMA 17 motor (see WIRING.md)

### Step 3: Upload Receiver Code (5 min)
Upload `xiao_esp32s3_receiver.ino` to Xiao

### Step 4: Upload Sender Code (5 min)
Update MAC address in `m5stack_core2_sender.ino` and upload to M5Stack

### Step 5: Test System (5 min)
Verify communication and motor movement

**Total Time: ~30 minutes**

## 🔧 Customization Options

### Change Movement Speeds
```cpp
// In m5stack_core2_sender.ino
sendCommand("FOCUS_IN", 50);  // Change step count
```

### Adjust Motor Speed
```cpp
// In xiao_esp32s3_receiver.ino
stepDelay = 1000000 / 500;  // Change 500 to desired steps/sec
```

### Modify Position Limits
```cpp
// In xiao_esp32s3_receiver.ino
#define MIN_POSITION  0
#define MAX_POSITION  10000
```

### Change Pin Assignments
```cpp
// In xiao_esp32s3_receiver.ino
#define STEP_PIN      D0
#define DIR_PIN       D1
#define ENABLE_PIN    D2
```

## 📊 Specifications

### Communication
- **Protocol**: ESP-NOW (2.4GHz)
- **Range**: Up to 100m line-of-sight
- **Latency**: ~10-20ms typical
- **Timeout**: 1 second

### Motor Control
- **Resolution**: 0.1125° per step (1/16 microstepping)
- **Speed**: 500 steps/second (configurable)
- **Position Range**: 0-10,000 steps (configurable)
- **Accuracy**: ±1 step

### Power Requirements
- **M5Stack**: 5V USB, ~300mA
- **Xiao**: 5V USB, ~100mA
- **Motor System**: 12V, ~2A

## 🆘 Common Issues

| Issue | Quick Fix | Details |
|-------|-----------|---------|
| No communication | Check MAC address | [TROUBLESHOOTING.md](TROUBLESHOOTING.md) |
| Motor not moving | Check ENABLE pin | [TROUBLESHOOTING.md](TROUBLESHOOTING.md) |
| Wrong direction | Swap motor coil | [TROUBLESHOOTING.md](TROUBLESHOOTING.md) |
| Skipping steps | Increase current | [TROUBLESHOOTING.md](TROUBLESHOOTING.md) |
| Position drift | Enable holding torque | [TROUBLESHOOTING.md](TROUBLESHOOTING.md) |

## 🔬 Advanced Topics

### Add Encoder Feedback
- Implement closed-loop position verification
- Use rotary encoder on focuser
- Compare commanded vs. actual position

### Speed Ramping
- Implement acceleration/deceleration profiles
- Smoother starts and stops
- Reduce mechanical stress

### Position Presets
- Save favorite focus positions to EEPROM
- Quick recall buttons
- Temperature-compensated positions

### Auto-Focus
- Integrate with camera feedback
- Star size analysis
- Automated focus routine

## 📝 License

This project is provided as-is for educational and hobbyist use.

## 🤝 Contributing

- Report issues on GitHub
- Submit improvements via pull requests
- Share your telescope setup photos
- Help improve documentation

## 📞 Support

- GitHub Issues: Report bugs and feature requests
- Documentation: All guides in this folder
- Community: Arduino, ESP32, astronomy forums

## ✅ Checklist for Success

- [ ] Read QUICKSTART.md
- [ ] Gather all hardware components
- [ ] Get MAC addresses from both devices
- [ ] Wire hardware following WIRING.md
- [ ] Upload receiver code
- [ ] Update and upload sender code
- [ ] Test communication
- [ ] Test motor movement
- [ ] Adjust current limit
- [ ] Mount to telescope
- [ ] Set position limits
- [ ] Verify focus direction
- [ ] Enjoy wireless focus control!

---

**Start here**: [QUICKSTART.md](QUICKSTART.md) → Get your system running in 30 minutes!

**Need help**: [TROUBLESHOOTING.md](TROUBLESHOOTING.md) → Solutions to common problems

**Want details**: [ARCHITECTURE.md](ARCHITECTURE.md) → Deep dive into the system
