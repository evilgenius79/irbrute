# irbrute

![alt text](https://github.com/kdpkdp/irbrute/blob/main/circuit.png?raw=true)

[![IMAGE ALT TEXT](http://img.youtube.com/vi/a9-3Va7cq7k/0.jpg)](https://www.youtube.com/watch?v=a9-3Va7cq7k "Demo")

## Projects

### Original: IR Remote Control Brute Force
The original IR remote control project for ESP8266.

### New: Telescope Focus Control (ESP-NOW)
Wireless telescope focus control using ESP-NOW protocol between M5Stack Core2 and Xiao ESP32-S3 with stepper motor control.

📁 **[See telescope_focus folder for complete documentation](telescope_focus/)**

**Features:**
- ESP-NOW wireless communication (no WiFi router needed)
- M5Stack Core2 touchscreen controller
- Xiao ESP32-S3 receiver with DRV8825 stepper driver
- NEMA 17 stepper motor control for telescope focuser
- Bidirectional communication with acknowledgments
- Error reporting and position tracking
- Multiple movement speeds (coarse and fine control)

**Quick Links:**
- [Quick Start Guide](telescope_focus/QUICKSTART.md)
- [Complete README](telescope_focus/README.md)
- [Wiring Guide](telescope_focus/WIRING.md)
- [M5Stack Sender Code](telescope_focus/m5stack_core2_sender.ino)
- [Xiao Receiver Code](telescope_focus/xiao_esp32s3_receiver.ino)

