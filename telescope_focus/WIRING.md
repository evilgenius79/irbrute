# Telescope Focus Control - Wiring Guide

## Complete Wiring Diagram

```
┌─────────────────────────────────────────────────────────────────────┐
│                    Telescope Focus Control System                    │
└─────────────────────────────────────────────────────────────────────┘

┌──────────────────┐                              ┌──────────────────┐
│  M5Stack Core2   │                              │  Xiao ESP32-S3   │
│   (Controller)   │      ESP-NOW Protocol        │   (Receiver)     │
│                  │◄────────────────────────────►│                  │
│                  │         Wireless             │                  │
│                  │                              │                  │
│  - Touch UI      │                              │  D0 (STEP) ──────┼─┐
│  - Send Commands │                              │  D1 (DIR)  ──────┼─┤
│  - Show Status   │                              │  D2 (EN)   ──────┼─┤
│                  │                              │  GND ────────────┼─┤
└──────────────────┘                              └──────────────────┘ │
                                                                        │
                                                   ┌────────────────────┘
                                                   │
                                                   ▼
                                            ┌──────────────┐
                                            │   DRV8825    │
                                            │ Stepper      │
                                            │ Driver       │
                                            ├──────────────┤
                                            │ STEP   ◄─────┼── D0
                                            │ DIR    ◄─────┼── D1
                                            │ ENABLE ◄─────┼── D2
                                            │ GND    ◄─────┼── GND
                                            │              │
                                            │ VMOT   ◄─────┼── 12V PSU (+)
                                            │ GND    ◄─────┼── 12V PSU (-)
                                            │              │
                                            │ MS1    ◄─────┼── VCC (for 1/16)
                                            │ MS2    ◄─────┼── VCC (for 1/16)
                                            │ MS3    ◄─────┼── VCC (for 1/16)
                                            │              │
                                            │ A1, A2 ──────┼─┐
                                            │ B1, B2 ──────┼─┤
                                            └──────────────┘ │
                                                             │
                                                             ▼
                                                      ┌──────────────┐
                                                      │  NEMA 17     │
                                                      │  Stepper     │
                                                      │  Motor       │
                                                      ├──────────────┤
                                                      │ Coil A ◄─────┼── A1, A2
                                                      │ Coil B ◄─────┼── B1, B2
                                                      │              │
                                                      │ (Attached to │
                                                      │  telescope   │
                                                      │  focuser)    │
                                                      └──────────────┘

┌──────────────────┐
│  12V Power       │
│  Supply          │
├──────────────────┤
│  +12V ───────────┼── VMOT (DRV8825)
│  GND  ───────────┼── GND (DRV8825)
│                  │
│  Recommended:    │
│  2A minimum      │
└──────────────────┘
```

## Pin Assignments

### Xiao ESP32-S3 to DRV8825
| Xiao Pin | DRV8825 Pin | Function | Description |
|----------|-------------|----------|-------------|
| D0 (GPIO0) | STEP | Output | Step pulse signal |
| D1 (GPIO1) | DIR | Output | Direction control |
| D2 (GPIO2) | ENABLE | Output | Enable/Disable motor (Active LOW) |
| GND | GND | Ground | Common ground |

### DRV8825 to NEMA 17 Motor
| DRV8825 Pin | Motor Wire | Color (typical) |
|-------------|------------|-----------------|
| A1 | Coil A+ | Red |
| A2 | Coil A- | Blue |
| B1 | Coil B+ | Green |
| B2 | Coil B- | Black |

### DRV8825 Power
| DRV8825 Pin | Connection | Notes |
|-------------|------------|-------|
| VMOT | 12V+ | Motor power supply positive |
| GND | 12V- | Motor power supply ground |

### DRV8825 Microstepping (1/16 step)
| Pin | Connection | Notes |
|-----|------------|-------|
| MS1 | VCC (3.3V or 5V) | HIGH for 1/16 step |
| MS2 | VCC (3.3V or 5V) | HIGH for 1/16 step |
| MS3 | VCC (3.3V or 5V) | HIGH for 1/16 step |

## Microstepping Configuration Table

Configure MS1, MS2, MS3 pins for desired resolution:

| MS1 | MS2 | MS3 | Microstep Resolution |
|-----|-----|-----|---------------------|
| L   | L   | L   | Full step |
| H   | L   | L   | Half step (1/2) |
| L   | H   | L   | Quarter step (1/4) |
| H   | H   | L   | Eighth step (1/8) |
| H   | H   | H   | Sixteenth step (1/16) |

**Recommended**: 1/16 microstepping for smooth, quiet operation

## Power Supply Recommendations

### For NEMA 17 Motor
- **Voltage**: 12V DC (can work with 8-24V)
- **Current**: 2A minimum (check motor specs)
- **Type**: Regulated DC power supply
- **Note**: Motor current is limited by DRV8825, not power supply

### For Xiao ESP32-S3
- **USB Power**: 5V from USB-C port
- **Alternative**: 3.3V/5V pin (if not using USB)

### Important Notes
1. **Separate Power Domains**: 
   - Motor power (12V) connects only to DRV8825 VMOT/GND
   - Logic power (3.3V/5V) for Xiao and DRV8825 logic
   - Share common ground between all components

2. **Decoupling Capacitors**:
   - Add 100µF electrolytic capacitor across VMOT and GND (close to DRV8825)
   - Add 0.1µF ceramic capacitor for noise filtering

3. **Current Limiting**:
   - Adjust DRV8825 current limit using potentiometer
   - Formula: Vref = Current_Limit / 2
   - Example: For 1A motor, set Vref to 0.5V

## Assembly Steps

1. **Mount DRV8825 on breadboard or PCB**
2. **Configure microstepping** (MS1, MS2, MS3 to VCC for 1/16)
3. **Connect Xiao ESP32-S3**:
   - STEP to D0
   - DIR to D1  
   - ENABLE to D2
   - GND to GND
4. **Connect motor power**:
   - 12V+ to VMOT
   - 12V- to GND
   - Add 100µF capacitor
5. **Connect NEMA 17**:
   - Identify coils with multimeter (continuity test)
   - Connect Coil A to A1, A2
   - Connect Coil B to B1, B2
6. **Adjust current limit** (use multimeter to measure Vref)
7. **Test setup** with serial monitor before connecting motor
8. **Mount motor to telescope focuser**

## Safety Warnings

⚠️ **IMPORTANT**:
- Never disconnect motor while powered (can damage driver)
- Ensure proper current limit setting to prevent motor overheating
- Use heat sink on DRV8825 if running at high currents
- Double-check polarity before applying power
- Start with low speed/current for testing

## Testing Procedure

1. Power only the Xiao (via USB) - verify serial output shows MAC address
2. Power the DRV8825 (with motor disconnected) - verify ENABLE works
3. Connect motor and test with small movements (1-10 steps)
4. Verify direction control works correctly
5. Test position limits and error handling
6. Verify ESP-NOW communication with M5Stack
7. Full operational test with telescope focuser

## Troubleshooting Quick Reference

| Problem | Likely Cause | Solution |
|---------|--------------|----------|
| Motor doesn't move | ENABLE pin HIGH | Check D2 connection |
| Motor vibrates | Microstepping wrong | Verify MS1/MS2/MS3 pins |
| Random movements | Noise on STEP pin | Add decoupling capacitors |
| Driver overheats | Current too high | Adjust Vref potentiometer |
| Position drifts | Missed steps | Reduce speed, increase current |
| No ESP-NOW comm | Wrong MAC address | Update sender with receiver MAC |
