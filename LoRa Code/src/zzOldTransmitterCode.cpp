#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "LoRa_E32.h"

//LoRa
#define FREQUENCY_868  // Use 868 for 862-931 MHz range
#define E32_TTL_1W     // For 30dBm power support

// Pin definitions
#define LoRa_RX 18  // ESP32 RX -> LoRa TX
#define LoRa_TX 17  // ESP32 TX -> LoRa RX
#define LoRa_M0 -1  // Not connected (pulled LOW for normal mode)
#define LoRa_M1 -1  // Not connected (pulled LOW for normal mode)
#define LoRa_AUX_PIN -1 // Not connected

// Create LoRa object - REMOVE the manual Serial1.begin() call!
LoRa_E32 e32ttl(&Serial1, LoRa_AUX_PIN, UART_BPS_RATE_9600);

//RGB
#define RGB_PIN 48
#define NUMPIXELS 1 

Adafruit_NeoPixel pixels(NUMPIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);

// Message structure
struct Message {
  char type[5];
  char message[6];
  float temperature;
};

void setup() {
  // Start RGB pixel
  pixels.begin();
  pixels.setPixelColor(0, pixels.Color(0, 0, 255)); // Blue = starting
  pixels.show();

  // Start serial port
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n=== LoRa E32 Transmitter ===");
  
  // Manually configure Serial1 pins for ESP32
  Serial1.begin(9600, SERIAL_8N1, LoRa_RX, LoRa_TX);
  delay(100);
  
  // Initialize the E32 module
  e32ttl.begin();
  delay(100);
  
  Serial.println("Module initialized");
  Serial.println("Starting transmission...");
  
  // Green = ready
  pixels.setPixelColor(0, pixels.Color(0, 255, 0));
  pixels.show();
}

int messageCount = 0;

void loop() {
  // Red = transmitting
  pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  pixels.show();
  
  // Send simple string message
  String msg = "Hello " + String(messageCount++);
  
  Serial.println("\n--- Sending Message ---");
  Serial.print("Message: ");
  Serial.println(msg);
  
  // USE BROADCAST or FIXED TRANSMISSION
  // Option A: Broadcast to all devices on channel 0x30
  ResponseStatus rs = e32ttl.sendBroadcastFixedMessage(0x30, msg);
  
  // Option B: Send to specific address (0x01, 0x02) on channel 0x30
  // ResponseStatus rs = e32ttl.sendFixedMessage(0x01, 0x02, 0x30, msg);
  
  Serial.print("Status: ");
  Serial.println(rs.getResponseDescription());
  Serial.print("Code: ");
  Serial.println(rs.code);
  
  if (rs.code == 1) {
    // Green flash = success
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    Serial.println("✓ Message sent successfully!");
  } else {
    // Yellow = error
    pixels.setPixelColor(0, pixels.Color(255, 255, 0));
    Serial.println("✗ Failed to send message!");
  }
  pixels.show();
  
  Serial.println("----------------------\n");
  
  delay(3000); // Send every 3 seconds
}