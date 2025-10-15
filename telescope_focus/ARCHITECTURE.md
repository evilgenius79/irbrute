# Telescope Focus Control - System Architecture

## System Overview

```
┌─────────────────────────────────────────────────────────────────────┐
│                    TELESCOPE FOCUS CONTROL SYSTEM                    │
│                          ESP-NOW Protocol                            │
└─────────────────────────────────────────────────────────────────────┘

┌────────────────────────┐                    ┌─────────────────────────┐
│   M5Stack Core2        │                    │   Xiao ESP32-S3         │
│   (Controller/Sender)  │                    │   (Receiver/Driver)     │
├────────────────────────┤                    ├─────────────────────────┤
│ - Touch Interface      │                    │ - ESP-NOW Receiver      │
│ - ESP-NOW Sender       │◄──────────────────►│ - Stepper Control       │
│ - Command Generator    │  Wireless (2.4GHz) │ - Position Tracking     │
│ - Status Display       │                    │ - Error Handling        │
└────────────────────────┘                    └─────────────────────────┘
                                                          │
                                                          │ GPIO (3 pins)
                                                          ▼
                                               ┌─────────────────────────┐
                                               │   DRV8825 Driver        │
                                               ├─────────────────────────┤
                                               │ - Microstepping (1/16)  │
                                               │ - Current Control       │
                                               │ - Direction Control     │
                                               └─────────────────────────┘
                                                          │
                                                          │ Motor Control
                                                          ▼
                                               ┌─────────────────────────┐
                                               │   NEMA 17 Stepper       │
                                               ├─────────────────────────┤
                                               │ - 200 steps/rev         │
                                               │ - Coupled to focuser    │
                                               └─────────────────────────┘
                                                          │
                                                          │ Mechanical
                                                          ▼
                                               ┌─────────────────────────┐
                                               │   Telescope Focuser     │
                                               └─────────────────────────┘
```

## Communication Protocol

### Message Flow

```
Sender (M5Stack)                         Receiver (Xiao)
      │                                         │
      │  1. User taps button                    │
      │     (e.g., "FOCUS IN")                  │
      │                                         │
      │  2. Create command message              │
      │     - command: "FOCUS_IN"               │
      │     - value: 100                        │
      │     - id: 1                             │
      │                                         │
      │  3. Send via ESP-NOW                    │
      ├────────────────────────────────────────►│
      │                                         │
      │                              4. Receive & validate
      │                              5. Process command
      │                              6. Control stepper
      │                              7. Create response
      │                                 - status: "ACK"
      │                                 - value: position
      │                                 - id: 1
      │                                         │
      │  8. Receive response                    │
      │◄────────────────────────────────────────┤
      │                                         │
      │  9. Display on screen                   │
      │     "ACK: ID=1"                        │
      │                                         │
```

### Error Handling Flow

```
Sender                                   Receiver
  │                                         │
  │  Command: "FOCUS_OUT 100"              │
  ├───────────────────────────────────────►│
  │                                         │
  │                              Check position limit
  │                              Current: 9950
  │                              Target: 10050 ✗ (exceeds 10000)
  │                                         │
  │  Response: ERROR, Code: 2              │
  │◄────────────────────────────────────────┤
  │                                         │
  │  Display: "ERROR: Code=2 ID=1"         │
  │                                         │
```

## Data Structures

### Command Message (Sender → Receiver)
```cpp
struct struct_message {
    char command[32];     // Command string
    int value;            // Parameter value
    unsigned long id;     // Unique message ID
}
```

**Example:**
```
command = "FOCUS_IN"
value   = 100
id      = 12345
```

### Response Message (Receiver → Sender)
```cpp
struct struct_response {
    char status[32];      // Response status
    int value;            // Return value
    unsigned long id;     // Matching message ID
}
```

**Example:**
```
status = "ACK"
value  = 5000 (current position)
id     = 12345 (matches request)
```

## Command Reference

### Movement Commands

| Command | Parameter | Description | Response |
|---------|-----------|-------------|----------|
| FOCUS_IN | steps (int) | Move focus inward | ACK with position |
| FOCUS_OUT | steps (int) | Move focus outward | ACK with position |
| STOP | 0 | Stop all movement | ACK with position |
| SET_POSITION | position (int) | Move to absolute position | ACK with position |
| GET_POSITION | 0 | Query current position | POSITION with value |
| HOME | 0 | Return to home (position 0) | ACK with 0 |

### Error Codes

| Code | Meaning | Description |
|------|---------|-------------|
| 1 | MIN_LIMIT | Movement would go below minimum position |
| 2 | MAX_LIMIT | Movement would exceed maximum position |
| 3 | INVALID_POS | Target position out of valid range |
| 4 | UNKNOWN_CMD | Command not recognized |

## Hardware Pin Mapping

### Xiao ESP32-S3 GPIO Assignments

```
Pin     Function        Direction   Connected To
────────────────────────────────────────────────────
D0      STEP            Output      DRV8825 STEP
D1      DIR             Output      DRV8825 DIR
D2      ENABLE          Output      DRV8825 ENABLE
GND     Ground          -           DRV8825 GND
```

### DRV8825 Pin Configuration

```
Pin         Function        Connected To
────────────────────────────────────────────
STEP        Step pulse      Xiao D0
DIR         Direction       Xiao D1
ENABLE      Enable (active LOW)  Xiao D2
GND         Ground          Xiao GND + PSU GND
VMOT        Motor voltage   12V PSU (+)
A1, A2      Coil A          Motor Coil A
B1, B2      Coil B          Motor Coil B
MS1         Microstep 1     VCC (for 1/16)
MS2         Microstep 2     VCC (for 1/16)
MS3         Microstep 3     VCC (for 1/16)
```

## User Interface Layout (M5Stack Core2)

```
┌─────────────────────────────────────────────┐
│          Telescope Focus Control            │
├─────────────────────────────────────────────┤
│                                             │
│  ┌────────┐  ┌────────┐  ┌────────┐       │
│  │ FOCUS  │  │ FOCUS  │  │ STOP   │       │
│  │   IN   │  │  OUT   │  │        │       │
│  │        │  │        │  │        │       │
│  └────────┘  └────────┘  └────────┘       │
│   (100 steps) (100 steps)                  │
│                                             │
│  ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐      │
│  │IN x10│ │OUT x10│ │IN x1│ │OUT x1│      │
│  └──────┘ └──────┘ └──────┘ └──────┘      │
│                                             │
│  Command: FOCUS_IN Val: 100 ID: 42         │
│  Status:  Sent: SUCCESS                    │
│  Response: ACK: ID=42                      │
└─────────────────────────────────────────────┘
```

## State Machine (Receiver)

```
                    ┌─────────────┐
                    │    IDLE     │
                    │ (Motor OFF) │
                    └──────┬──────┘
                           │
              Command      │
              Received     │
                           ▼
                    ┌─────────────┐
              ┌─────┤  VALIDATE   │
              │     │   Command   │
              │     └──────┬──────┘
              │            │
     Invalid  │            │ Valid
              │            ▼
              │     ┌─────────────┐
              │     │   ENABLE    │
              │     │   MOTOR     │
              │     └──────┬──────┘
              │            │
              │            ▼
              │     ┌─────────────┐
              │     │   MOVING    │◄────┐
              │     │ (Step Loop) │     │
              │     └──────┬──────┘     │
              │            │            │
              │            │ Steps      │
              │            │ Remaining  │
              │            │            │
              │            └────────────┘
              │            │
              │            │ Complete
              │            ▼
              │     ┌─────────────┐
              └────►│ SEND        │
                    │ RESPONSE    │
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
                    │  DISABLE    │
                    │   MOTOR     │
                    └──────┬──────┘
                           │
                           ▼
                         IDLE
```

## Timing Specifications

### ESP-NOW Communication
- **Latency**: ~10-20ms typical
- **Timeout**: 1000ms (1 second)
- **Retry**: 3 attempts on timeout
- **Range**: Up to 100m line-of-sight

### Stepper Motor Control
- **Default Speed**: 500 steps/second
- **Step Delay**: 2000 microseconds
- **Direction Setup**: 5 microseconds
- **Pulse Width**: 5 microseconds minimum

### Position Tracking
- **Update Rate**: Real-time (every step)
- **Range**: 0 to 10,000 (configurable)
- **Resolution**: 1 step (0.1125° at 1/16 microstep)

## Power Requirements

### M5Stack Core2
- **Input**: 5V USB-C or battery
- **Current**: ~300mA active
- **Battery**: Built-in 390mAh

### Xiao ESP32-S3
- **Input**: 5V USB-C or 3.3V/5V pin
- **Current**: ~100mA active
- **GPIO**: 3.3V logic level

### DRV8825 + NEMA 17
- **Motor Supply**: 8-35V (12V recommended)
- **Motor Current**: Up to 1.5A per coil
- **Logic Supply**: 3.3V/5V from Xiao
- **Total System**: ~2A at 12V minimum

## Performance Characteristics

### Movement Precision
- **Full Step**: 1.8° (200 steps/rev)
- **Microstep (1/16)**: 0.1125° (3200 steps/rev)
- **Minimum Movement**: 1 microstep
- **Repeatability**: ±1 step

### Focus Range (Example)
- **Steps**: 10,000 (0 to 10,000)
- **Revolutions**: ~3.125 at 1/16 microstep
- **Linear Travel**: Depends on focuser gearing

### Response Times
- **Command to ACK**: ~20ms
- **Single Step**: ~2ms
- **100 Steps**: ~200ms
- **Full Range**: ~20 seconds

## Safety Features

1. **Position Limits**: Software limits prevent over-travel
2. **Error Reporting**: All errors sent back to controller
3. **Stop Command**: Immediate halt capability
4. **Timeout Detection**: Communication failure detection
5. **Current Limiting**: Hardware protection via DRV8825
6. **Motor Disable**: Automatic disable when idle (configurable)

## Future Enhancements

- [ ] Speed/acceleration profiles
- [ ] Position presets (save favorite positions)
- [ ] Auto-focus routine (temperature compensation)
- [ ] Encoder feedback for closed-loop control
- [ ] Limit switches for auto-homing
- [ ] EEPROM storage for position memory
- [ ] Web interface for remote control
- [ ] Integration with telescope mount control
