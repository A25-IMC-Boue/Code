#include "LoRaConfig.h"

// Pin definitions
#define ESP_RX 18  // ESP32 RX -> LoRa TX
#define ESP_TX 17  // ESP32 TX -> LoRa RX
#define LoRa_M0 10  
#define LoRa_M1 11  
#define LoRa_AUX_PIN -1 // Not connected

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("Creating LoRa object");
    LoRa LoRaModule(LoRa_M0, LoRa_M1, ESP_RX, ESP_TX);

    LoRaModule.setConfigMode();
    LoRaModule.begin();
    LoRaModule.printConfiguration();

    
    bool configSuccess = LoRaModule.config(0x01, 0x02, 0x30);
    if (configSuccess) {
        Serial.println("LoRa module configured successfully");
    } else {
        Serial.println("Failed to configure LoRa module");
    }
    delay(3000);
    Serial.println("Updated Configuration:");
    LoRaModule.printConfiguration();
    
    Serial.println("Sending normal mode:");
    LoRaModule.setNormalMode();
}

void loop() {
    // Your loop code here
    testfunc();
    delay(5000);

}