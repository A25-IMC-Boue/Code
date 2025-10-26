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

        void sendMessage(const String message);
    
    private:
        // Pins
        uint8_t LoRa_RX_pin;
        uint8_t LoRa_TX_pin;
        uint8_t AUX_pin = -1; // Not used in this class
        uint8_t M0_pin;
        uint8_t M1_pin;   

        // LoRa module object
        LoRa_E32 loraModule; 



};


////////////////////////////////////////////////////////
///// Functions
////////////////////////////////////////////////////////

// Tiny function to test inclusion of this file
void testfunc();

// Putting LoRa module in configuration mode
void LoRaConfigMode(int8_t M0_pin, int8_t M1_pin);

// LoRa in normal running mode
void LoRaNormalMode(int8_t M0_pin, int8_t M1_pin);


// Setting Address High and Low in default transmission mode
// default: High : 0x01 and Low : 0x02 with channel 0x10
bool configLoRa(struct Configuration current, uint8_t high = 0x01, uint8_t low = 0x02, uint8_t channel = 0x10);

// Print current configuration to Serial
// Warning only works if module in config mode!
void printLoRaParams(struct Configuration current);









#endif // LORACONFIG_H