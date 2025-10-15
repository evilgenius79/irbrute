/**
 * ESP-NOW Telescope Focus Control - Shared Configuration
 * 
 * This header defines the protocol structures and constants used by both
 * the M5Stack Core2 sender and Xiao ESP32-S3 receiver.
 */

#ifndef TELESCOPE_FOCUS_CONFIG_H
#define TELESCOPE_FOCUS_CONFIG_H

// Protocol Version
#define PROTOCOL_VERSION 1

// Message Structure for Commands (Sender -> Receiver)
typedef struct struct_message {
  char command[32];     // Command: "FOCUS_IN", "FOCUS_OUT", "STOP", "SET_POSITION", "GET_POSITION"
  int value;            // Value (steps or position)
  unsigned long id;     // Message ID for tracking and acknowledgment
} struct_message;

// Response Structure for Acknowledgments (Receiver -> Sender)
typedef struct struct_response {
  char status[32];      // Status: "ACK", "ERROR", "POSITION"
  int value;            // Current position or error code
  unsigned long id;     // Message ID being acknowledged
} struct_response;

// Command Definitions
#define CMD_FOCUS_IN      "FOCUS_IN"
#define CMD_FOCUS_OUT     "FOCUS_OUT"
#define CMD_STOP          "STOP"
#define CMD_SET_POSITION  "SET_POSITION"
#define CMD_GET_POSITION  "GET_POSITION"

// Response Status Definitions
#define STATUS_ACK        "ACK"
#define STATUS_ERROR      "ERROR"
#define STATUS_POSITION   "POSITION"

// Error Codes
#define ERROR_MIN_LIMIT   1  // Movement would exceed minimum position
#define ERROR_MAX_LIMIT   2  // Movement would exceed maximum position
#define ERROR_INVALID_POS 3  // Invalid target position
#define ERROR_UNKNOWN_CMD 4  // Unknown command received
#define ERROR_MOTOR_FAULT 5  // Motor driver fault
#define ERROR_TIMEOUT     6  // Communication timeout

// Default Movement Values
#define STEPS_COARSE      100  // Large movement steps
#define STEPS_MEDIUM      10   // Medium movement steps
#define STEPS_FINE        1    // Fine movement steps

// Position Limits (can be overridden in device-specific code)
#ifndef MIN_POSITION
#define MIN_POSITION      0
#endif

#ifndef MAX_POSITION
#define MAX_POSITION      10000
#endif

// Communication Timeouts
#define ACK_TIMEOUT_MS    1000  // Timeout waiting for acknowledgment
#define RETRY_COUNT       3     // Number of retries on timeout

// Stepper Motor Defaults (can be overridden)
#ifndef STEPS_PER_REV
#define STEPS_PER_REV     200   // Standard NEMA 17 (1.8° per step)
#endif

#ifndef MICROSTEPS
#define MICROSTEPS        16    // DRV8825 microstepping
#endif

#ifndef DEFAULT_SPEED
#define DEFAULT_SPEED     500   // Steps per second
#endif

#endif // TELESCOPE_FOCUS_CONFIG_H
