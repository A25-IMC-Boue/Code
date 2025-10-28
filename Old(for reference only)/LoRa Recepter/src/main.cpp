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

// Message structure - MUST match transmitter!
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
  
  Serial.println("\n\n=== LoRa E32 Receiver ===");
  
  // Manually configure Serial1 pins for ESP32
  Serial1.begin(9600, SERIAL_8N1, LoRa_RX, LoRa_TX);
  delay(100);
  
  // Initialize the E32 module
  e32ttl.begin();
  delay(100);
  
  Serial.println("Module initialized");
  Serial.println("Listening for messages...");
  
  // Green = ready to receive
  pixels.setPixelColor(0, pixels.Color(0, 255, 0));
  pixels.show();
}

void loop() {
  // Check if data available
  if (e32ttl.available() > 0) {
    Serial.println("\n--- Message Received! ---");
    
    // Flash white to indicate reception
    pixels.setPixelColor(0, pixels.Color(255, 255, 255));
    pixels.show();
    
    // Receive as string (simple messages)
    ResponseContainer rs = e32ttl.receiveMessage();
    
    Serial.print("Status: ");
    Serial.println(rs.status.getResponseDescription());
    
    if (rs.status.code == 1) {
      Serial.print("Message: ");
      Serial.println(rs.data);
      
      // Green flash on success
      pixels.setPixelColor(0, pixels.Color(0, 255, 0));
      pixels.show();
      delay(100);
    } else {
      Serial.println("Error receiving message!");
      // Red flash on error
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));
      pixels.show();
      delay(100);
    }
    
    Serial.println("------------------------\n");
  }
  
  // Small delay to avoid spamming
  delay(100);
}