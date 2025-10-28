#include "LoRaConfig.h"
#include "pinDef.h"

//Instanciate LoRa object
LoRa LoRaModule(LoRa_M0, LoRa_M1, ESP_RX, ESP_TX);

void setup() {
    Serial.begin(115200);
    delay(500);

    LoRaModule.setConfigMode();
    LoRaModule.begin();
    LoRaModule.printConfiguration();

    bool configSuccess = LoRaModule.config(0x01, 0x02, 0x30);
    if (configSuccess) {
        Serial.println("LoRa module configured successfully");
    } else {
        Serial.println("Failed to configure LoRa module");
    }
    delay(1000);
    Serial.println("Updated Configuration:");
    LoRaModule.printConfiguration();
    
    Serial.println("Sending normal mode:");
    LoRaModule.setNormalMode();
}

void loop() {
    // Your loop code here
    LoRaModule.sendBroadcastMessage("Hello from transmitter");
    delay(2000);

}