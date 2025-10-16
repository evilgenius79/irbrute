/**
 * Xiao ESP32-S3 - Telescope Focus Receiver (ESP-NOW + DRV8825 Stepper Driver)
 * 
 * This code runs on Xiao ESP32-S3 and receives focus control commands
 * from M5Stack Core2 via ESP-NOW, then controls a NEMA 17 stepper motor
 * through a DRV8825 driver.
 * 
 * DRV8825 Connections:
 * - STEP pin: GPIO D0 (default, configurable)
 * - DIR pin: GPIO D1 (default, configurable)
 * - ENABLE pin: GPIO D2 (default, configurable)
 * - MS1, MS2, MS3: Configure for microstepping (set via hardware)
 */

#include <esp_now.h>
#include <WiFi.h>

// DRV8825 Pin Configuration
#define STEP_PIN      D0    // Step pin
#define DIR_PIN       D1    // Direction pin
#define ENABLE_PIN    D2    // Enable pin (LOW = enabled, HIGH = disabled)

// Stepper Motor Configuration
#define STEPS_PER_REV 200   // NEMA 17 standard (1.8° per step)
#define MICROSTEPS    16    // DRV8825 microstepping (set via MS1, MS2, MS3 pins)
#define MAX_SPEED     1000  // Maximum steps per second
#define ACCELERATION  500   // Steps per second^2

// Focus Position Limits
#define MIN_POSITION  0
#define MAX_POSITION  10000

// Structure for receiving commands
typedef struct struct_message {
  char command[32];     // Command: "FOCUS_IN", "FOCUS_OUT", "STOP", "SET_POSITION"
  int value;            // Value (steps or position)
  unsigned long id;     // Message ID for tracking
} struct_message;

// Structure for sending responses
typedef struct struct_response {
  char status[32];      // "ACK", "ERROR", "POSITION"
  int value;            // Current position or error code
  unsigned long id;     // Message ID being acknowledged
} struct_response;

struct_message incomingMessage;
struct_response outgoingResponse;

// Motor state
int currentPosition = 0;
bool motorEnabled = false;
bool isMoving = false;

// Timing for non-blocking stepper control
unsigned long lastStepTime = 0;
unsigned long stepDelay = 1000; // microseconds between steps
int targetSteps = 0;
int stepsTaken = 0;
bool direction = true; // true = forward (OUT), false = backward (IN)

void setupStepper() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
  digitalWrite(ENABLE_PIN, HIGH); // Disabled initially
  
  // Calculate step delay for moderate speed (500 steps/sec)
  stepDelay = 1000000 / 500; // microseconds
}

void enableMotor(bool enable) {
  digitalWrite(ENABLE_PIN, !enable); // DRV8825 is active LOW
  motorEnabled = enable;
}

void setDirection(bool forward) {
  digitalWrite(DIR_PIN, forward ? HIGH : LOW);
  direction = forward;
  delayMicroseconds(5); // Direction setup time
}

void performStep() {
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(5); // Minimum pulse width
  digitalWrite(STEP_PIN, LOW);
  
  // Update position
  if (direction) {
    currentPosition++;
  } else {
    currentPosition--;
  }
  
  // Constrain position
  if (currentPosition < MIN_POSITION) currentPosition = MIN_POSITION;
  if (currentPosition > MAX_POSITION) currentPosition = MAX_POSITION;
}

void startMovement(int steps, bool dir) {
  if (steps == 0) {
    stopMovement();
    return;
  }
  
  enableMotor(true);
  setDirection(dir);
  targetSteps = abs(steps);
  stepsTaken = 0;
  isMoving = true;
  lastStepTime = micros();
}

void stopMovement() {
  isMoving = false;
  targetSteps = 0;
  stepsTaken = 0;
  enableMotor(false);
}

void updateStepper() {
  if (!isMoving) return;
  
  unsigned long currentTime = micros();
  
  if (currentTime - lastStepTime >= stepDelay) {
    if (stepsTaken < targetSteps) {
      performStep();
      stepsTaken++;
      lastStepTime = currentTime;
    } else {
      stopMovement();
    }
  }
}

void sendResponse(const char* status, int value, unsigned long msgId) {
  strcpy(outgoingResponse.status, status);
  outgoingResponse.value = value;
  outgoingResponse.id = msgId;
  
  // Send to all registered peers (sender)
  esp_err_t result = esp_now_send(NULL, (uint8_t *)&outgoingResponse, sizeof(outgoingResponse));
  
  if (result == ESP_OK) {
    Serial.printf("Response sent: %s, Value: %d, ID: %lu\n", status, value, msgId);
  } else {
    Serial.println("Error sending response");
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));
  
  Serial.printf("Received: Cmd=%s, Val=%d, ID=%lu from ", 
                incomingMessage.command, incomingMessage.value, incomingMessage.id);
  // Print sender MAC
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", mac[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
  
  // Process command
  if (strcmp(incomingMessage.command, "FOCUS_IN") == 0) {
    int steps = incomingMessage.value;
    if (currentPosition - steps < MIN_POSITION) {
      steps = currentPosition - MIN_POSITION;
      if (steps <= 0) {
        sendResponse("ERROR", 1, incomingMessage.id); // Error: already at limit
        Serial.println("Error: Already at minimum position");
        return;
      }
    }
    startMovement(steps, false); // false = IN (decrease position)
    sendResponse("ACK", currentPosition, incomingMessage.id);
  } 
  else if (strcmp(incomingMessage.command, "FOCUS_OUT") == 0) {
    int steps = incomingMessage.value;
    if (currentPosition + steps > MAX_POSITION) {
      steps = MAX_POSITION - currentPosition;
      if (steps <= 0) {
        sendResponse("ERROR", 2, incomingMessage.id); // Error: already at limit
        Serial.println("Error: Already at maximum position");
        return;
      }
    }
    startMovement(steps, true); // true = OUT (increase position)
    sendResponse("ACK", currentPosition, incomingMessage.id);
  }
  else if (strcmp(incomingMessage.command, "STOP") == 0) {
    stopMovement();
    sendResponse("ACK", currentPosition, incomingMessage.id);
    Serial.println("Movement stopped");
  }
  else if (strcmp(incomingMessage.command, "SET_POSITION") == 0) {
    int targetPos = incomingMessage.value;
    if (targetPos < MIN_POSITION || targetPos > MAX_POSITION) {
      sendResponse("ERROR", 3, incomingMessage.id); // Error: invalid position
      Serial.printf("Error: Invalid position %d (range: %d-%d)\n", targetPos, MIN_POSITION, MAX_POSITION);
    } else {
      int steps = targetPos - currentPosition;
      if (steps != 0) {
        startMovement(abs(steps), steps > 0);
      }
      sendResponse("ACK", currentPosition, incomingMessage.id);
      Serial.printf("Moving to position: %d\n", targetPos);
    }
  }
  else if (strcmp(incomingMessage.command, "GET_POSITION") == 0) {
    sendResponse("POSITION", currentPosition, incomingMessage.id);
    Serial.printf("Position requested: %d\n", currentPosition);
  }
  else if (strcmp(incomingMessage.command, "HOME") == 0) {
    // Home command - move to position 0
    if (currentPosition != 0) {
      startMovement(currentPosition, false); // Move to 0
      currentPosition = 0; // Reset position
      sendResponse("ACK", 0, incomingMessage.id);
      Serial.println("Homing to position 0");
    } else {
      sendResponse("ACK", 0, incomingMessage.id);
      Serial.println("Already at home position");
    }
  }
  else {
    sendResponse("ERROR", 4, incomingMessage.id); // Error: unknown command
    Serial.printf("Error: Unknown command '%s'\n", incomingMessage.command);
  }
}

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Xiao ESP32-S3 Telescope Focus Receiver");
  
  // Initialize stepper
  setupStepper();
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  // Print MAC address
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register callbacks
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);
  
  Serial.println("ESP-NOW initialized. Waiting for commands...");
}

void loop() {
  updateStepper();
  
  // Optionally send position updates periodically
  static unsigned long lastPositionUpdate = 0;
  if (millis() - lastPositionUpdate > 5000) { // Every 5 seconds
    Serial.printf("Current Position: %d, Moving: %s\n", 
                  currentPosition, isMoving ? "Yes" : "No");
    lastPositionUpdate = millis();
  }
  
  delayMicroseconds(100); // Small delay to prevent tight loop
}
