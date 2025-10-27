#ifndef LORACONFIG_H
#define LORACONFIG_H

//Dependencies
#include <Arduino.h>
#include "LoRa_E32.h"


// LoRa handler class

/**
 * @brief LoRa configuration and communication class
 * 
 * Handles initialization, sending, and receiving data through the E32 LoRa module
 */
class LoRa {
    public:
        /**
         * @brief Construct a new LoRa object
         * 
         * @param txPin TX pin number
         * @param rxPin RX pin number
         * @param m0Pin M0 control pin
         * @param m1Pin M1 control pin
         */    
        LoRa(uint8_t M0_pin=-1, uint8_t M1_pin=-1, uint8_t LoRa_RX=18, uint8_t LoRa_TX=17);
        ~LoRa();

        // Initializes Lora module and Starts UART Serial1
        void begin();

        // Set configuration mode (M0 and M1 both HIGH or both LOW)
        void setConfigMode(); // M0 = HIGH, M1 = HIGH
        void setNormalMode(); // M0 = LOW, M1 = LOW

        // Print out current configuration to Serial
        void printConfiguration();

        // Configure LoRa module parameters
        bool config(uint8_t high = 0x01, uint8_t low = 0x02, uint8_t channel = 0x30);

        // Send string message
        void sendBroadcastMessage(const String message);
        void sendMessage( uint8_t ADDH=0x01, uint8_t ADDL=0x02, const String message = "");

        void receiveMessage();

        void printLastMessage();

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
        bool _externalModePins = true;

        // State of the LoRa module (e.g., config or normal) 
        bool _isConfigMode = false;
        bool _isNormalMode = true;

        // Channel for fixed message sending
        uint8_t _channel = 0x30;

        // Last message received
        String _lastMessage = "";


};


////////////////////////////////////////////////////////
///// Functions
////////////////////////////////////////////////////////

// Tiny function to test inclusion of this file
void testfunc();

#endif // LORACONFIG_H