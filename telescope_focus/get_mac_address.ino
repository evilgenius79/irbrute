/**
 * ESP32 MAC Address Finder
 * 
 * Use this sketch to find the MAC address of your ESP32 device.
 * Upload to either M5Stack Core2 or Xiao ESP32-S3 to get its MAC address.
 * You'll need this address to configure the peer device.
 */

#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);
  
  Serial.println();
  Serial.println("==================================");
  Serial.println("   ESP32 MAC Address Finder");
  Serial.println("==================================");
  Serial.println();
  
  // Get and print the MAC address
  String macAddress = WiFi.macAddress();
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  
  Serial.println("MAC Address (String format):");
  Serial.println(macAddress);
  Serial.println();
  
  Serial.println("MAC Address (Array format for code):");
  Serial.print("uint8_t receiverMAC[] = {");
  for (int i = 0; i < 6; i++) {
    Serial.print("0x");
    if (baseMac[i] < 16) Serial.print("0");
    Serial.print(baseMac[i], HEX);
    if (i < 5) Serial.print(", ");
  }
  Serial.println("};");
  Serial.println();
  
  Serial.println("----------------------------------");
  Serial.println("Copy the array format above into");
  Serial.println("your sender code (M5Stack Core2)");
  Serial.println("----------------------------------");
}

void loop() {
  // Print MAC address every 5 seconds
  delay(5000);
  
  String macAddress = WiFi.macAddress();
  Serial.print("MAC: ");
  Serial.println(macAddress);
}
