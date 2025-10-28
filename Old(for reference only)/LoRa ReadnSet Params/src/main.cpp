#include "Arduino.h"
#include "LoRa_E32.h"

#define LoRa_RX_PIN 18
#define LoRa_TX_PIN 17
#define LoRa_AUX_PIN -1
#define M0_PIN 4   // Define your M0 pin
#define M1_PIN 5   // Define your M1 pin

LoRa_E32 e32ttl(&Serial1, LoRa_AUX_PIN, UART_BPS_RATE_9600);

void printParameters(struct Configuration configuration);

void setup() {
    Serial.begin(115200);
    delay(3000);
    
    Serial.println("\n=== LoRa E32 Configuration Diagnostic ===");
    
    // Set up M0 and M1 pins as outputs
    pinMode(M0_PIN, OUTPUT);
    pinMode(M1_PIN, OUTPUT);
    
    // Force configuration mode (M0=HIGH, M1=HIGH)
    digitalWrite(M0_PIN, HIGH);
    digitalWrite(M1_PIN, HIGH);
    Serial.println("✓ M0 and M1 set to HIGH via GPIO");
    delay(1000);
    
    // Initialize serial
    Serial1.begin(9600, SERIAL_8N1, LoRa_RX_PIN, LoRa_TX_PIN);
    delay(100);
    
    // Flush any garbage
    while(Serial1.available()) Serial1.read();
    
    e32ttl.begin();
    delay(500);
    
    Serial.println("\n--- Reading Current Configuration (Attempt 1) ---");
    ResponseStructContainer c = e32ttl.getConfiguration();
    if (c.status.code != 1) {
        Serial.print("❌ Failed to read config! Status: ");
        Serial.println(c.status.getResponseDescription());
        Serial.println("\nTROUBLESHOOT:");
        Serial.println("1. Check wiring: TX->RX, RX->TX");
        Serial.println("2. Verify 3.3V power (NOT 5V!)");
        Serial.println("3. Check M0 and M1 are truly HIGH");
        Serial.println("4. Try different LoRa module");
        c.close();
        return;
    }
    
    Configuration configuration = *(Configuration*) c.data;
    Serial.println("CURRENT CONFIGURATION:");
    printParameters(configuration);
    c.close();
    
    // Try reading again to see if we get consistent data
    Serial.println("\n--- Reading Current Configuration (Attempt 2) ---");
    delay(500);
    c = e32ttl.getConfiguration();
    Configuration configuration2 = *(Configuration*) c.data;
    printParameters(configuration2);
    c.close();
    
    // Check if both reads match
    if (configuration.ADDH != configuration2.ADDH || 
        configuration.ADDL != configuration2.ADDL ||
        configuration.CHAN != configuration2.CHAN) {
        Serial.println("\n⚠️ WARNING: Two reads gave different results!");
        Serial.println("This indicates communication problems.");
        Serial.println("Check your wiring and power supply.");
        return;
    }
    
    Serial.println("\n✓ Reads are consistent - proceeding with configuration");
    
    // Now configure
    Serial.println("\n--- Applying New Configuration ---");
    
    // Start fresh - re-read config
    c = e32ttl.getConfiguration();
    configuration = *(Configuration*) c.data;
    c.close();
    
    // Set desired values
    configuration.ADDH = 0x01;
    configuration.ADDL = 0x02;
    configuration.CHAN = 0x30;  // Channel 48
    configuration.OPTION.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
    configuration.SPED.uartBaudRate = UART_BPS_9600;
    configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
    configuration.SPED.uartParity = MODE_00_8N1;
    configuration.OPTION.wirelessWakeupTime = WAKE_UP_250;
    configuration.OPTION.fec = FEC_1_ON;
    configuration.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;
    configuration.OPTION.transmissionPower = POWER_30;
    
    Serial.println("Target configuration:");
    Serial.println("  Address: 0x0102");
    Serial.println("  Channel: 48 (910 MHz)");
    Serial.println("  UART: 9600bps");
    Serial.println("  Air Rate: 2.4kbps");
    
    // Try WRITE_CFG_PWR_DWN_LOSE first (temporary, doesn't save to EEPROM)
    Serial.println("\nAttempting temporary configuration (LOSE mode)...");
    ResponseStatus rs = e32ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_LOSE);
    
    Serial.print("Status: ");
    Serial.println(rs.getResponseDescription());
    
    if (rs.code != 1) {
        Serial.println("❌ Configuration failed!");
        Serial.println("Module is not responding to write commands.");
        Serial.println("This could mean:");
        Serial.println("1. Module is defective");
        Serial.println("2. Module is not in config mode (M0/M1 not HIGH)");
        Serial.println("3. Wrong module variant/firmware");
        return;
    }
    
    delay(1000);
    
    // Read back immediately
    Serial.println("\n--- Verifying Configuration ---");
    c = e32ttl.getConfiguration();
    Configuration newConfig = *(Configuration*) c.data;
    
    Serial.println("READ BACK:");
    printParameters(newConfig);
    c.close();
    
    // Verify each field
    bool success = true;
    if (newConfig.ADDH != 0x01) {
        Serial.println("❌ ADDH mismatch!");
        success = false;
    }
    if (newConfig.ADDL != 0x02) {
        Serial.println("❌ ADDL mismatch!");
        success = false;
    }
    if (newConfig.CHAN != 0x30) {
        Serial.println("❌ CHAN mismatch!");
        success = false;
    }
    
    if (success) {
        Serial.println("\n✅ Configuration verified!");
        Serial.println("Now saving permanently...");
        
        // Save permanently
        rs = e32ttl.setConfiguration(newConfig, WRITE_CFG_PWR_DWN_SAVE);
        Serial.print("Save Status: ");
        Serial.println(rs.getResponseDescription());
        
        if (rs.code == 1) {
            Serial.println("✅ Configuration saved to EEPROM!");
            Serial.println("\nNext steps:");
            Serial.println("1. Power cycle the module");
            Serial.println("2. Set M0 and M1 to LOW");
            Serial.println("3. Upload your transmitter/receiver code");
        }
    } else {
        Serial.println("\n❌ CONFIGURATION FAILED!");
        Serial.println("\nPOSSIBLE CAUSES:");
        Serial.println("1. Module variant doesn't support these settings");
        Serial.println("2. Module firmware issue");
        Serial.println("3. Module is defective");
        Serial.println("\nTRY:");
        Serial.println("- Different LoRa module");
        Serial.println("- Factory reset (tie both M0 and M1 to ground for 5 sec)");
    }
}

void loop() {
    // Nothing
}

void printParameters(struct Configuration configuration) {
    Serial.println("----------------------------------------");
    Serial.print("Address: 0x");
    if (configuration.ADDH < 0x10) Serial.print("0");
    Serial.print(configuration.ADDH, HEX);
    if (configuration.ADDL < 0x10) Serial.print("0");
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


/*
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
    ResponseStatus rs = e32ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_LOSE);
    
    Serial.print("Configuration Status: ");
    Serial.println(rs.getResponseDescription());
    
    if (rs.code == 1) {
        Serial.println("✓ Configuration saved successfully!");
        
        // CRITICAL FIX: Wait for module to restart
        delay(2000);
        
    
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
*/