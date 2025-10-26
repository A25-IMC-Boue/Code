#ifndef LORACONFIG_H
#define LORACONFIG_H

//Dependencies
#include <Arduino.h>
#include "LoRa_E32.h"


// LoRa handler class

class LoRa {
    public:
        LoRa(uint8_t M0_pin=-1, uint8_t M1_pin=-1, uint8_t LoRa_RX=18, uint8_t LoRa_TX=17);
        ~LoRa();

        // Initializes Lora module and Starts UART Serial1
        void begin();

        // Set configuration mode (M0 and M1 both HIGH or both LOW)
        void setConfigMode(); // M0 = HIGH, M1 = HIGH
        void setNormalMode(); // M0 = LOW, M1 = LOW

        // Read current configuration
        void readConfiguration();

        // Configure LoRa module parameters
        void config(uint8_t high = 0x01, uint8_t low = 0x02, uint8_t channel = 0x10);

        // Send string message
        void sendMessage(const String message);
    
    private:
        // Pins
        uint8_t _loraRxPin;
        uint8_t _loraTxPin;
        uint8_t _auxPin = -1; // Not used in this class
        uint8_t _m0Pin;
        uint8_t _m1Pin;

        // LoRa module object
        LoRa_E32 _loraModule; 

        // If m0 and m1 pins are connected externally
        bool _externalModePins = false;



};


////////////////////////////////////////////////////////
///// Functions
////////////////////////////////////////////////////////

// Tiny function to test inclusion of this file
void testfunc();

#endif // LORACONFIG_H