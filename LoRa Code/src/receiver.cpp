#include "LoRaConfig.h"
#include <Adafruit_NeoPixel.h>
#include "pinDef.h"

//instanciate NeoPixel
#define RGB_PIN 48
#define NUMPIXELS 1 
Adafruit_NeoPixel pixels(NUMPIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);

//Instanciate LoRa object
LoRa LoRaModule(LoRa_M0, LoRa_M1, ESP_RX, ESP_TX);

void setup() {
    //Start up Pixel for visual without serial
    pixels.begin();
    pixels.setPixelColor(0, pixels.Color(0, 0, 255)); // Blue = starting
    pixels.setBrightness(50); // make it easier on the eyes jeez
    pixels.show();


    //Start up serial for debug 
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
    delay(3000);
    Serial.println("Updated Configuration:");
    LoRaModule.printConfiguration();
    
    Serial.println("Sending normal mode:");
    LoRaModule.setNormalMode();

    //Green = ready to receive
    pixels.setPixelColor(0, pixels.Color(0, 255, 0)); //Green
    pixels.show();
}

void loop() {
    if (LoRaModule.checkForMessage()) {
        // Message received - flash white
        pixels.setPixelColor(0, pixels.Color(255, 255, 255)); //White = message received
        pixels.setBrightness(100); // bright!
        pixels.show();
        delay(100);

        LoRaModule.printLastMessage();
    }
    else {
        //Green = ready to receive
        pixels.setPixelColor(0, pixels.Color(0, 255, 0)); //Green
        pixels.setBrightness(50); // make it easier on the eyes jeez
        pixels.show();
        delay(100);
    }

}