/**
 * M5Stack Core2 - Telescope Focus Controller (ESP-NOW Sender)
 * 
 * This code runs on M5Stack Core2 and sends focus control commands
 * to a Xiao ESP32-S3 receiver via ESP-NOW protocol.
 */

#include <M5Core2.h>
#include <esp_now.h>
#include <WiFi.h>

// MAC Address of the Xiao ESP32-S3 receiver
// Replace with your Xiao's MAC address
uint8_t receiverMAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Structure for sending commands
typedef struct struct_message {
  char command[32];     // Command: "FOCUS_IN", "FOCUS_OUT", "STOP", "SET_POSITION"
  int value;            // Value (steps or position)
  unsigned long id;     // Message ID for tracking
} struct_message;

// Structure for receiving responses
typedef struct struct_response {
  char status[32];      // "ACK", "ERROR", "POSITION"
  int value;            // Current position or error code
  unsigned long id;     // Message ID being acknowledged
} struct_response;

struct_message outgoingMessage;
struct_response incomingResponse;

unsigned long messageId = 0;
bool waitingForAck = false;
unsigned long lastSendTime = 0;
const unsigned long ACK_TIMEOUT = 1000; // 1 second timeout

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  M5.Lcd.fillRect(0, 200, 320, 20, BLACK);
  M5.Lcd.setCursor(10, 200);
  M5.Lcd.setTextColor(status == ESP_NOW_SEND_SUCCESS ? GREEN : RED);
  M5.Lcd.print(status == ESP_NOW_SEND_SUCCESS ? "Sent: SUCCESS" : "Sent: FAILED");
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingResponse, incomingData, sizeof(incomingResponse));
  
  waitingForAck = false;
  
  M5.Lcd.fillRect(0, 220, 320, 20, BLACK);
  M5.Lcd.setCursor(10, 220);
  
  if (strcmp(incomingResponse.status, "ACK") == 0) {
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.printf("ACK: ID=%lu", incomingResponse.id);
  } else if (strcmp(incomingResponse.status, "ERROR") == 0) {
    M5.Lcd.setTextColor(RED);
    M5.Lcd.printf("ERROR: Code=%d ID=%lu", incomingResponse.value, incomingResponse.id);
  } else if (strcmp(incomingResponse.status, "POSITION") == 0) {
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.printf("Position: %d", incomingResponse.value);
  }
}

void sendCommand(const char* cmd, int val) {
  strcpy(outgoingMessage.command, cmd);
  outgoingMessage.value = val;
  outgoingMessage.id = ++messageId;
  
  esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)&outgoingMessage, sizeof(outgoingMessage));
  
  M5.Lcd.fillRect(0, 180, 320, 20, BLACK);
  M5.Lcd.setCursor(10, 180);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("Cmd: %s Val: %d ID: %lu", cmd, val, messageId);
  
  waitingForAck = true;
  lastSendTime = millis();
}

void drawUI() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  
  M5.Lcd.setCursor(50, 10);
  M5.Lcd.print("Telescope Focus");
  
  // Draw buttons
  M5.Lcd.fillRect(10, 50, 90, 60, BLUE);
  M5.Lcd.setCursor(20, 75);
  M5.Lcd.print("FOCUS");
  M5.Lcd.setCursor(30, 90);
  M5.Lcd.print("IN");
  
  M5.Lcd.fillRect(115, 50, 90, 60, DARKGREEN);
  M5.Lcd.setCursor(135, 75);
  M5.Lcd.print("FOCUS");
  M5.Lcd.setCursor(135, 90);
  M5.Lcd.print("OUT");
  
  M5.Lcd.fillRect(220, 50, 90, 60, RED);
  M5.Lcd.setCursor(245, 75);
  M5.Lcd.print("STOP");
  
  // Fine control buttons
  M5.Lcd.setTextSize(1);
  M5.Lcd.fillRect(10, 120, 70, 40, DARKGREY);
  M5.Lcd.setCursor(15, 135);
  M5.Lcd.print("IN x10");
  
  M5.Lcd.fillRect(90, 120, 70, 40, DARKGREY);
  M5.Lcd.setCursor(95, 135);
  M5.Lcd.print("OUT x10");
  
  M5.Lcd.fillRect(170, 120, 70, 40, DARKGREY);
  M5.Lcd.setCursor(175, 135);
  M5.Lcd.print("IN x1");
  
  M5.Lcd.fillRect(250, 120, 60, 40, DARKGREY);
  M5.Lcd.setCursor(255, 135);
  M5.Lcd.print("OUT x1");
  
  M5.Lcd.setTextSize(2);
}

void setup() {
  M5.begin();
  M5.Lcd.setTextSize(2);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  // Print MAC address
  M5.Lcd.println("MAC: " + WiFi.macAddress());
  delay(2000);
  
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    M5.Lcd.println("ESP-NOW init failed");
    return;
  }
  
  // Register callbacks
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    M5.Lcd.println("Failed to add peer");
    return;
  }
  
  drawUI();
}

void loop() {
  M5.update();
  
  // Check for touch events
  if (M5.Touch.ispressed()) {
    TouchPoint_t point = M5.Touch.getPressPoint();
    
    // Main control buttons (coarse - 100 steps)
    if (point.y > 50 && point.y < 110) {
      if (point.x > 10 && point.x < 100) {
        sendCommand("FOCUS_IN", 100);
      } else if (point.x > 115 && point.x < 205) {
        sendCommand("FOCUS_OUT", 100);
      } else if (point.x > 220 && point.x < 310) {
        sendCommand("STOP", 0);
      }
      delay(200); // Debounce
    }
    
    // Fine control buttons
    if (point.y > 120 && point.y < 160) {
      if (point.x > 10 && point.x < 80) {
        sendCommand("FOCUS_IN", 10);
      } else if (point.x > 90 && point.x < 160) {
        sendCommand("FOCUS_OUT", 10);
      } else if (point.x > 170 && point.x < 240) {
        sendCommand("FOCUS_IN", 1);
      } else if (point.x > 250 && point.x < 310) {
        sendCommand("FOCUS_OUT", 1);
      }
      delay(200); // Debounce
    }
  }
  
  // Check for ACK timeout
  if (waitingForAck && (millis() - lastSendTime > ACK_TIMEOUT)) {
    waitingForAck = false;
    M5.Lcd.fillRect(0, 220, 320, 20, BLACK);
    M5.Lcd.setCursor(10, 220);
    M5.Lcd.setTextColor(RED);
    M5.Lcd.print("Timeout: No ACK");
  }
  
  delay(10);
}
