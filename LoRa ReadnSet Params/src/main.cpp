#include "Arduino.h"
#include "LoRa_E32.h"

#define LoRa_RX_PIN 18
#define LoRa_TX_PIN 17
#define LoRa_AUX_PIN -1

LoRa_E32 e32ttl(&Serial1, LoRa_AUX_PIN, UART_BPS_RATE_9600);

void printParameters(struct Configuration configuration);

void setup() {
    Serial.begin(115200);
    delay(3000);
    
    Serial.println("\n=== Fixing Module 2 Configuration ===");
    Serial.println("NOTE: M0 and M1 must be HIGH!");
    Serial.println("======================================\n");
    
    Serial1.begin(9600, SERIAL_8N1, LoRa_RX_PIN, LoRa_TX_PIN);
    e32ttl.begin();
    delay(500);
    
    // Read current configuration
    ResponseStructContainer c;
    c = e32ttl.getConfiguration();
    Configuration configuration = *(Configuration*) c.data;
    
    Serial.println("CURRENT CONFIGURATION:");
    printParameters(configuration);
    
    // Fix the configuration to match Module 1
    Serial.println("\n--- Applying New Configuration ---");
    
    configuration.ADDH = 0x01;
    configuration.ADDL = 0x02;
    configuration.CHAN = 0x30;  // Channel 48 = 910 MHz
    
    // CRITICAL: Set to transparent transmission
    configuration.OPTION.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
    
    // CRITICAL: Match UART baud to 9600
    configuration.SPED.uartBaudRate = UART_BPS_9600;
    
    // CRITICAL: Match air data rate to 2.4kbps
    configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
    
    // Set other settings to match Module 1
    configuration.SPED.uartParity = MODE_00_8N1;
    configuration.OPTION.wirelessWakeupTime = WAKE_UP_250;
    configuration.OPTION.fec = FEC_1_ON;
    configuration.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;
    configuration.OPTION.transmissionPower = POWER_30;
    
    // Save configuration
    ResponseStatus rs = e32ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
    
    Serial.print("Configuration Status: ");
    Serial.println(rs.getResponseDescription());
    
    if (rs.code == 1) {
        Serial.println("✓ Configuration saved successfully!");
        
        delay(1000);
        
        // Read back to verify
        c = e32ttl.getConfiguration();
        configuration = *(Configuration*) c.data;
        
        Serial.println("\nNEW CONFIGURATION:");
        printParameters(configuration);
        
        Serial.println("\n=== SUCCESS ===");
        Serial.println("Module 2 is now configured to match Module 1!");
        Serial.println("Both modules should now communicate.");
        Serial.println("\nNEXT STEPS:");
        Serial.println("1. Set M0 and M1 to LOW (normal mode)");
        Serial.println("2. Upload transmitter code to Module 1");
        Serial.println("3. Upload receiver code to Module 2");
        Serial.println("4. They should communicate!");
        
    } else {
        Serial.println("❌ Failed to save configuration!");
        Serial.println("Check: M0 and M1 are HIGH");
    }
    
    c.close();
}

void loop() {
    delay(5000);
}

void printParameters(struct Configuration configuration) {
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
}
