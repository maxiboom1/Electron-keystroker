#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <EEPROM.h>

// EEPROM Configuration
#define EEPROM_SIZE 16 // 4 bytes for IP, 1 byte for flag (5 bytes total, rounded up)
#define IP_ADDR_OFFSET 0
#define FLAG_OFFSET 4
#define CONFIGURED_FLAG 0xAA // Arbitrary value to indicate IP is set

// Function to initialize EEPROM and load IP
void initConfig(IPAddress& deviceIp) {
  EEPROM.begin(EEPROM_SIZE);
  
  // Check if IP is configured
  if (EEPROM.read(FLAG_OFFSET) == CONFIGURED_FLAG) {
    // Load IP from EEPROM
    byte ipBytes[4];
    for (int i = 0; i < 4; i++) {
      ipBytes[i] = EEPROM.read(IP_ADDR_OFFSET + i);
    }
    deviceIp = IPAddress(ipBytes);
    Serial.print("Loaded IP from EEPROM: ");
    Serial.println(deviceIp);
  } else {
    // Default IP if not configured
    deviceIp = IPAddress(10, 168, 0, 177);
    Serial.println("No IP in EEPROM, using default: 10.168.0.177");
  }
}

// Function to save IP to EEPROM
void saveIpConfig(const IPAddress& newIp) {
  // Save IP bytes
  for (int i = 0; i < 4; i++) {
    EEPROM.write(IP_ADDR_OFFSET + i, newIp[i]);
  }
  // Set configured flag
  EEPROM.write(FLAG_OFFSET, CONFIGURED_FLAG);
  EEPROM.commit();
  
  Serial.print("Saved IP to EEPROM: ");
  Serial.println(newIp);
}

#endif