/*
 * LoRa E32-TTL-100 with ESP32-S3 DevKitC-1
 * Get configuration
 * 
 * E32-TTL-100 ---- ESP32-S3
 * M0         ---- Pulled HIGH (tied to VCC for normal mode)
 * M1         ---- Pulled HIGH (tied to VCC for sleep/config mode)
 * TX         ---- GPIO 18 (ESP32 RX)
 * RX         ---- GPIO 17 (ESP32 TX)
 * AUX        ---- Not connected
 * VCC        ---- 3.3V
 * GND        ---- GND
 *
 * Note: M0 and M1 both HIGH = Sleep/Configuration mode
 *       To enter configuration mode, power cycle the module
 *       or temporarily connect M0 and M1 HIGH
 */

#include "Arduino.h"
#include "LoRa_E32.h"

// Pin definitions
#define LoRa_RX_PIN 18  // ESP32 RX -> LoRa TX (ESP32 receives from LoRa)
#define LoRa_TX_PIN 17  // ESP32 TX -> LoRa RX (ESP32 sends to LoRa)
#define LoRa_M0_PIN -1  // Not connected (pulled HIGH externally)
#define LoRa_M1_PIN -1  // Not connected (pulled HIGH externally)
#define LoRa_AUX_PIN -1 // Not connected

//Important defines for module

// Initialize E32 with HardwareSerial
// Constructor: LoRa_E32(HardwareSerial* serial, byte auxPin, UART_BPS_RATE bpsRate)
// Since AUX is not connected, we'll use a dummy pin or handle it differently
// For ESP32, we need to manually configure Serial1 pins first
LoRa_E32 e32ttl(&Serial1, LoRa_AUX_PIN, UART_BPS_RATE_9600);

void printParameters(struct Configuration configuration);
void printModuleInformation(struct ModuleInformation moduleInformation);
void configureModule(struct Configuration current);
void getConfiguration();

void setup() {
    // USB Serial for debugging
    Serial.begin(115200);
    delay(3000);
    
    Serial.println("LoRa E32 Configuration Reader - ESP32-S3");
    Serial.println("========================================");
    
    // Manually configure Serial1 to use GPIO17 (TX) and GPIO18 (RX)
    Serial1.begin(9600, SERIAL_8N1, LoRa_RX_PIN, LoRa_TX_PIN);
    
    // Start the E32 module
    // Note: Since M0 and M1 are tied HIGH, module should be in configuration mode
    e32ttl.begin();
    
    Serial.println("Module initialized. Attempting to read configuration...");
    delay(500);
    
    // Get configuration
    ResponseStructContainer c;
    c = e32ttl.getConfiguration();
    Configuration configuration = *(Configuration*) c.data;
    
    Serial.print("Configuration Status: ");
    Serial.println(c.status.getResponseDescription());
    Serial.print("Status Code: ");
    Serial.println(c.status.code);
    
    
    if (c.status.code == 1) {
        printParameters(configuration);

        configuration.ADDH = 0x01;
        configuration.ADDL = 0x02;
        configuration.CHAN = 0x30; //Max 66 = 928 MHz
        ResponseStatus setRes = e32ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);

        delay(2000);
        c = e32ttl.getConfiguration();
        configuration = *(Configuration*) c.data;
        printParameters(configuration);


    } else {
        Serial.println("Failed to read configuration!");
        Serial.println("Check:");
        Serial.println("- Wire connections (TX/RX)");
        Serial.println("- M0 and M1 are HIGH (configuration mode)");
        Serial.println("- Power supply is 3.3V");
        Serial.println("- Module is powered on");
    }

    c.close();

    // Get module information
    ResponseStructContainer cMi;
    cMi = e32ttl.getModuleInformation();
    ModuleInformation mi = *(ModuleInformation*)cMi.data;
    
    Serial.print("Module Info Status: ");
    Serial.println(cMi.status.getResponseDescription());
    Serial.print("Status Code: ");
    Serial.println(cMi.status.code);
    
    if (cMi.status.code == 1) {
        printModuleInformation(mi);
    } else {
        Serial.println("Failed to read module information!");
    }
    
    cMi.close();
    
    //Setting operating mode:
    // Example: change channel immediately after reading config
    // Set desired channel index (consult module docs for valid values)
    //configuration.ADDH = 0x01; // Set address high byte
    //configuration.ADDL = 0x02; // Set address low byte
    //configuration.CHAN = 0x00; // module range: 862 to 931 MHz: 
    //Chanel 0x00 = 862 channel 0x10 = 863 MHz, channel 0x10=channel 16 = 878 MHz max 


    Serial.println("Updating channel...");
    //ResponseStatus setRes = e32ttl.setConfiguration(configuration); // uses default saveType

    Serial.print("New Configuration: ");
    
    // If module needs to be power-cycled to enter config mode, do that before this call.
    
    Serial.println("\nSetup complete!");
}

void loop() {
    // Nothing to do in loop
    Serial.print("hello world!");
    delay(5000);
}

void getConfiguration() {
     // Get configuration
    ResponseStructContainer c;
    c = e32ttl.getConfiguration();
    Configuration configuration = *(Configuration*) c.data;
    
    Serial.print("Configuration Status: ");
    Serial.println(c.status.getResponseDescription());
    Serial.print("Status Code: ");
    Serial.println(c.status.code);
    
    
    if (c.status.code == 1) {
        printParameters(configuration);
    } else {
        Serial.println("Failed to read configuration!");
        Serial.println("Check:");
        Serial.println("- Wire connections (TX/RX)");
        Serial.println("- M0 and M1 are HIGH (configuration mode)");
        Serial.println("- Power supply is 3.3V");
        Serial.println("- Module is powered on");
    }
    
    c.close();

}


void configureModule(struct Configuration current) {
    Serial.println("Preparing new configuration (copy of current)...");

    // Make a local copy of the current configuration
    Configuration newConfig = current;

    // --- EDIT THESE LINES to set desired parameters ---
    newConfig.ADDH = 0x01;
    newConfig.ADDL = 0x02;
    
    // Example: change channel (0..whatever module supports). Default is 0x17 (23) = 868 MHz
    newConfig.CHAN = 0x10; // channel index 
    //
    // Example: change options (transmission power etc.) — consult library/README for encoding:
    // newConfig.OPTION.transmissionPower = /* value */;
    // --------------------------------------------------

    Serial.println("Calling setConfiguration()...");
    ResponseStatus rs = e32ttl.setConfiguration(newConfig); // uses default saveType
    delay(2000); // wait for settings to apply

    Serial.print("Set Configuration Status: ");
    Serial.println(rs.getResponseDescription());
    Serial.print("Status Code: ");
    Serial.println(rs.code);

    if (rs.code == 1) {
        Serial.println("Configuration applied successfully.");
    } else {
        Serial.println("Failed to apply configuration. Check connections, mode and UART speed.");
    }
}

void printParameters(struct Configuration configuration) {
    Serial.println("========================================");
    Serial.println("CONFIGURATION PARAMETERS");
    Serial.println("========================================");
    
    Serial.print(F("HEAD: BIN="));
    Serial.print(configuration.HEAD, BIN);
    Serial.print(F(" DEC="));
    Serial.print(configuration.HEAD, DEC);
    Serial.print(F(" HEX="));
    Serial.println(configuration.HEAD, HEX);
    
    Serial.println();
    Serial.print(F("Address High (ADDH): "));
    Serial.println(configuration.ADDH, HEX);
    Serial.print(F("Address Low (ADDL):  "));
    Serial.println(configuration.ADDL, HEX);
    Serial.print(F("Channel (CHAN): "));
    Serial.print(configuration.CHAN, DEC);
    Serial.print(F(" -> "));
    Serial.println(configuration.getChannelDescription());
    
    Serial.println();
    Serial.println("SPEED SETTINGS:");
    Serial.print(F("  UART Parity:     "));
    Serial.print(configuration.SPED.uartParity, BIN);
    Serial.print(F(" -> "));
    Serial.println(configuration.SPED.getUARTParityDescription());
    
    Serial.print(F("  UART Baud Rate:  "));
    Serial.print(configuration.SPED.uartBaudRate, BIN);
    Serial.print(F(" -> "));
    Serial.println(configuration.SPED.getUARTBaudRate());
    
    Serial.print(F("  Air Data Rate:   "));
    Serial.print(configuration.SPED.airDataRate, BIN);
    Serial.print(F(" -> "));
    Serial.println(configuration.SPED.getAirDataRate());
    
    Serial.println();
    Serial.println("OPTIONS:");
    Serial.print(F("  Transmission Mode:  "));
    Serial.print(configuration.OPTION.fixedTransmission, BIN);
    Serial.print(F(" -> "));
    Serial.println(configuration.OPTION.getFixedTransmissionDescription());
    
    Serial.print(F("  IO Drive Mode:      "));
    Serial.print(configuration.OPTION.ioDriveMode, BIN);
    Serial.print(F(" -> "));
    Serial.println(configuration.OPTION.getIODroveModeDescription());
    
    Serial.print(F("  Wakeup Time:        "));
    Serial.print(configuration.OPTION.wirelessWakeupTime, BIN);
    Serial.print(F(" -> "));
    Serial.println(configuration.OPTION.getWirelessWakeUPTimeDescription());
    
    Serial.print(F("  FEC (Error Correct):"));
    Serial.print(configuration.OPTION.fec, BIN);
    Serial.print(F(" -> "));
    Serial.println(configuration.OPTION.getFECDescription());
    
    Serial.print(F("  TX Power:           "));
    Serial.print(configuration.OPTION.transmissionPower, BIN);
    Serial.print(F(" -> "));
    Serial.println(configuration.OPTION.getTransmissionPowerDescription());
    
    Serial.println("========================================");
}

void printModuleInformation(struct ModuleInformation moduleInformation) {
    Serial.println("========================================");
    Serial.println("MODULE INFORMATION");
    Serial.println("========================================");
    
    Serial.print(F("HEAD: BIN="));
    Serial.print(moduleInformation.HEAD, BIN);
    Serial.print(F(" DEC="));
    Serial.print(moduleInformation.HEAD, DEC);
    Serial.print(F(" HEX="));
    Serial.println(moduleInformation.HEAD, HEX);
    
    Serial.print(F("Frequency:  0x"));
    Serial.println(moduleInformation.frequency, HEX);
    Serial.print(F("Version:    0x"));
    Serial.println(moduleInformation.version, HEX);
    Serial.print(F("Features:   0x"));
    Serial.println(moduleInformation.features, HEX);
    
    Serial.println("========================================");


}


