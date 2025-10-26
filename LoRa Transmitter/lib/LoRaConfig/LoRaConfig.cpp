#include "LoRaConfig.h"

void testfunc() {
    Serial.println("LoRaConfig test function called");
}

// LoRa class implementation


LoRa::LoRa(uint8_t M0_pin, uint8_t M1_pin, uint8_t LoRa_RX, uint8_t LoRa_TX)
    : _loraRxPin(LoRa_RX), _loraTxPin(LoRa_TX), _m0Pin(M0_pin), _m1Pin(M1_pin),
    _loraModule(&Serial1, _auxPin, UART_BPS_RATE_9600)
{

    
    if (_m0Pin != -1 || _m1Pin != -1) {
        // Pins externally pulled LOW or HIGH
        _externalModePins = true;
    }

}

LoRa::~LoRa() {
    // Destructor implementation
}

void LoRa::begin() {
    // Setting pin modes of M0 and M1
    if (!_externalModePins) {
        pinMode(_m0Pin, OUTPUT);
        pinMode(_m1Pin, OUTPUT);
    }

    // Start LoRa module
    
}


void LoRa::setConfigMode() {

}

void LoRa::setNormalMode() {

}

void LoRa::readConfiguration() {

}
void LoRa::config(uint8_t high, uint8_t low, uint8_t channel) {

}

void LoRa::sendMessage(const String message) {

}


