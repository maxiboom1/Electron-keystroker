#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <EEPROM.h>
#include <ArduinoJson.h> // Ensure ESP32-compatible version

#define EEPROM_SIZE 240 // Matches our structure

// Config structure
struct Config {
  IPAddress deviceIp;         // 0-3
  bool tcpEnabled;            // 4
  IPAddress tcpIp;            // 5-8
  uint16_t tcpPort;           // 9-10
  bool tcpSecure;             // 11
  char tcpUser[32];           // 12-43
  char tcpPassword[32];       // 44-75
  bool httpEnabled;           // 76
  char httpUrl[64];           // 77-140
  bool httpSecure;            // 141
  char httpUser[32];          // 142-173
  char httpPassword[32];      // 174-205
  bool serialEnabled;         // 206
  char adminPassword[32];     // 207-238
  uint8_t configFlag;         // 239 (0xAA if configured)
};

// Forward declarations
void saveConfig(const Config& config);
void loadConfig(Config& config);

void initConfig(Config& config) {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Failed to initialize EEPROM");
    return;
  }
  
  if (EEPROM.read(239) != 0xAA) {
    // Defaults (as per project document)
    config.deviceIp = IPAddress(10, 168, 0, 177); // Default IP
    config.tcpEnabled = false;
    config.tcpIp = IPAddress(0, 0, 0, 0); // Empty
    config.tcpPort = 0;
    config.tcpSecure = false;
    memset(config.tcpUser, 0, 32);
    memset(config.tcpPassword, 0, 32);
    config.httpEnabled = false;
    memset(config.httpUrl, 0, 64); // Empty URL
    config.httpSecure = false;
    memset(config.httpUser, 0, 32);
    memset(config.httpPassword, 0, 32);
    config.serialEnabled = false;
    strncpy(config.adminPassword, "admin", 32);
    config.configFlag = 0xAA;
    saveConfig(config);
    Serial.println("EEPROM initialized with defaults, IP: 10.168.0.177");
  } else {
    loadConfig(config);
    Serial.println("EEPROM configured, loading existing config...");
  }

  Serial.print("Loaded IP from EEPROM: ");
  Serial.println(config.deviceIp);
}

void loadConfig(Config& config) {
  // Manually read IPAddress byte-by-byte to ensure correctness
  config.deviceIp[0] = EEPROM.read(0);
  config.deviceIp[1] = EEPROM.read(1);
  config.deviceIp[2] = EEPROM.read(2);
  config.deviceIp[3] = EEPROM.read(3);
  // Read other fields
  config.tcpEnabled = EEPROM.read(4);
  config.tcpIp[0] = EEPROM.read(5);
  config.tcpIp[1] = EEPROM.read(6);
  config.tcpIp[2] = EEPROM.read(7);
  config.tcpIp[3] = EEPROM.read(8);
  config.tcpPort = (EEPROM.read(10) << 8) | EEPROM.read(9); // Read as uint16_t
  config.tcpSecure = EEPROM.read(11);
  EEPROM.get(12, config.tcpUser); // Read char arrays
  EEPROM.get(44, config.tcpPassword);
  config.httpEnabled = EEPROM.read(76);
  EEPROM.get(77, config.httpUrl);
  config.httpSecure = EEPROM.read(141);
  EEPROM.get(142, config.httpUser);
  EEPROM.get(174, config.httpPassword);
  config.serialEnabled = EEPROM.read(206);
  EEPROM.get(207, config.adminPassword);
  config.configFlag = EEPROM.read(239);
}

void saveConfig(const Config& config) {
  // Manually write IPAddress byte-by-byte to ensure correctness
  EEPROM.write(0, config.deviceIp[0]);
  EEPROM.write(1, config.deviceIp[1]);
  EEPROM.write(2, config.deviceIp[2]);
  EEPROM.write(3, config.deviceIp[3]);
  // Write other fields
  EEPROM.write(4, config.tcpEnabled);
  EEPROM.write(5, config.tcpIp[0]);
  EEPROM.write(6, config.tcpIp[1]);
  EEPROM.write(7, config.tcpIp[2]);
  EEPROM.write(8, config.tcpIp[3]);
  EEPROM.write(9, (config.tcpPort & 0xFF)); // Low byte
  EEPROM.write(10, ((config.tcpPort >> 8) & 0xFF)); // High byte
  EEPROM.write(11, config.tcpSecure);
  EEPROM.put(12, config.tcpUser); // Write char arrays
  EEPROM.put(44, config.tcpPassword);
  EEPROM.write(76, config.httpEnabled);
  EEPROM.put(77, config.httpUrl);
  EEPROM.write(141, config.httpSecure);
  EEPROM.put(142, config.httpUser);
  EEPROM.put(174, config.httpPassword);
  EEPROM.write(206, config.serialEnabled);
  EEPROM.put(207, config.adminPassword);
  EEPROM.write(239, 0xAA); // Config flag
  EEPROM.commit();
  Serial.println("Config saved to EEPROM");
  Serial.print("Saved IP: ");
  Serial.println(config.deviceIp);
}

// Helper to parse JSON POST
bool parseJsonConfig(String json, Config& config) {
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error) return false;
  
  // Parse IP (example, extend for all fields)
  if (doc.containsKey("ip")) {
    String ipStr = doc["ip"].as<const char*>();
    config.deviceIp.fromString(ipStr);
  }
  if (doc.containsKey("tcpEnabled")) config.tcpEnabled = doc["tcpEnabled"].as<bool>();
  if (doc.containsKey("tcpIp")) {
    String tcpIpStr = doc["tcpIp"].as<const char*>();
    config.tcpIp.fromString(tcpIpStr);
  }
  if (doc.containsKey("tcpPort")) config.tcpPort = doc["tcpPort"].as<uint16_t>();
  if (doc.containsKey("tcpSecure")) config.tcpSecure = doc["tcpSecure"].as<bool>();
  if (doc.containsKey("tcpUser")) strncpy(config.tcpUser, doc["tcpUser"].as<const char*>(), 32);
  if (doc.containsKey("tcpPassword")) strncpy(config.tcpPassword, doc["tcpPassword"].as<const char*>(), 32);
  if (doc.containsKey("httpEnabled")) config.httpEnabled = doc["httpEnabled"].as<bool>();
  if (doc.containsKey("httpUrl")) strncpy(config.httpUrl, doc["httpUrl"].as<const char*>(), 64);
  if (doc.containsKey("httpSecure")) config.httpSecure = doc["httpSecure"].as<bool>();
  if (doc.containsKey("httpUser")) strncpy(config.httpUser, doc["httpUser"].as<const char*>(), 32);
  if (doc.containsKey("httpPassword")) strncpy(config.httpPassword, doc["httpPassword"].as<const char*>(), 32);
  if (doc.containsKey("serialEnabled")) config.serialEnabled = doc["serialEnabled"].as<bool>();
  if (doc.containsKey("adminPassword")) strncpy(config.adminPassword, doc["adminPassword"].as<const char*>(), 32);
  return true;
}

#endif