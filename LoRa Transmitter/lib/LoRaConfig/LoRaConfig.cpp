#include "LoRaConfig.h"

void testfunc() {
    Serial.println("LoRaConfig test function called");
}

// LoRa class implementation


LoRa::LoRa(uint8_t M0_pin, uint8_t M1_pin, uint8_t LoRa_RX, uint8_t LoRa_TX)
    : _loraRxPin(LoRa_RX), _loraTxPin(LoRa_TX), _m0Pin(M0_pin), _m1Pin(M1_pin),
    _loraModule(&Serial1, _auxPin, UART_BPS_RATE_9600)
{

    
    if (_m0Pin == -1 && _m1Pin == -1) {
        // Pins externally pulled LOW or HIGH
        _externalModePins = true;
    }
    else {
        _externalModePins = false;
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
    Serial1.begin(9600, SERIAL_8N1, _loraRxPin, _loraTxPin);


    // Start LoRa module
    _loraModule.begin();

    delay(100); // Allow time for module to initialize

}


void LoRa::setConfigMode() {
    if (!_externalModePins) {
        digitalWrite(_m0Pin, HIGH);
        digitalWrite(_m1Pin, HIGH);
        _isConfigMode = true;
        _isNormalMode = false;
        delay(500); // Time for module to switch to config mode
    }
    

}

void LoRa::setNormalMode() {
    if (!_externalModePins) {
        digitalWrite(_m0Pin, LOW);
        digitalWrite(_m1Pin, LOW);
        _isConfigMode = false;
        _isNormalMode = true;
        delay(500); // Time for module to switch to normal mode
    }
   
}


void LoRa::printConfiguration() {
    ResponseStructContainer c;
    c = _loraModule.getConfiguration();
    Configuration configuration = *(Configuration*) c.data;

    Serial.println("CURRENT CONFIGURATION:");
    Serial.println("----------------------------------------");

    Serial.print("Address: 0x");
    Serial.print(configuration.ADDH, HEX);
    Serial.print(configuration.ADDL, HEX);
    Serial.print(" | Channel: ");
    Serial.print(configuration.CHAN, DEC);
    Serial.print(" (");
    Serial.print(862 + configuration.CHAN);
    Serial.println(" MHz)");
    
    Serial.print("Transmission Mode: ");
    Serial.println(configuration.OPTION.getFixedTransmissionDescription());
    
    Serial.print("UART Baud: ");
    Serial.println(configuration.SPED.getUARTBaudRate());
    
    Serial.print("Air Data Rate: ");
    Serial.println(configuration.SPED.getAirDataRate());
    
    Serial.print("Wakeup Time: ");
    Serial.println(configuration.OPTION.getWirelessWakeUPTimeDescription());
    
    Serial.print("TX Power: ");
    Serial.println(configuration.OPTION.getTransmissionPowerDescription());
    Serial.println("----------------------------------------");

    c.close();
}

bool LoRa::config(uint8_t high, uint8_t low, uint8_t channel) {

    bool success = false;

    ResponseStructContainer c;
    c = _loraModule.getConfiguration();
    Configuration configuration = *(Configuration*) c.data;

    configuration.ADDH = high;
    configuration.ADDL = low;
    configuration.CHAN = channel; // recommended 0x30 for 910MHz
    // Set new channel for message transmission
    _channel = channel;
    // Set to transparent transmission
    configuration.OPTION.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
    
    // Match UART baud to 9600
    configuration.SPED.uartBaudRate = UART_BPS_9600;
    
    // Match air data rate to 2.4kbps
    configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
    
    // Set other settings
    configuration.SPED.uartParity = MODE_00_8N1;
    configuration.OPTION.wirelessWakeupTime = WAKE_UP_250;
    configuration.OPTION.fec = FEC_1_ON;
    configuration.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;
    configuration.OPTION.transmissionPower = POWER_30;
    
    // Save configuration
    ResponseStatus rs = _loraModule.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
    
    if (rs.code == 1) {
        //Passed
        success = true;

        // Power cycle to apply new settings
        // HOW DO I POWER CYCLE??
        setNormalMode();
        delay(1000);
        setConfigMode();
        delay(1000);

    } else {
        //config failed
        success = false;
        // Serial.println(rs.getResponseDescription());
    }

    c.close();

    return success;
}

void LoRa::sendBroadcastMessage(const String message) {
    
    if (_isNormalMode == true) {
        ResponseStatus rs = _loraModule.sendBroadcastFixedMessage(_channel, message);
    }
}
void LoRa::sendMessage( uint8_t ADDH, uint8_t ADDL, const String message) {
    
    if (_isNormalMode == true) {
        ResponseStatus rs = _loraModule.sendFixedMessage(ADDH, ADDL, _channel, message);
    }
}

void LoRa::receiveMessage() {

    if (_isNormalMode == true) {
        // receiveMessage returns a ResponseContainer (not ResponseStatus)
        ResponseContainer rc = _loraModule.receiveMessage();

        // check the status code (1 = success) and that data is non-null
        if (rc.status.code == 1 && rc.data != nullptr) {
            // rc.data is a C string for simple messages;
            _lastMessage = rc.data;
        }

    }

}

void LoRa::printLastMessage() {
    Serial.println("Last Message Received: " + _lastMessage);
}
