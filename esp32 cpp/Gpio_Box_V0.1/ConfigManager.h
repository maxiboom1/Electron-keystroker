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

  // Debug: Read current EEPROM bytes for IP (offsets 0-3) and configFlag (239)
  Serial.print("Checking EEPROM IP (0-3) before reset: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(EEPROM.read(i), HEX);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("Checking config flag at offset 239: ");
  Serial.println(EEPROM.read(239), HEX);

  // Temporary: Reset EEPROM to ensure defaults (remove after testing)
  Serial.println("Temporarily resetting EEPROM to defaults...");
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  Serial.println("EEPROM cleared to zeros");

  // Set defaults manually
  IPAddress defaultIp(10, 168, 0, 177);
  EEPROM.write(0, defaultIp[0]); // IP: 10.168.0.177
  EEPROM.write(1, defaultIp[1]);
  EEPROM.write(2, defaultIp[2]);
  EEPROM.write(3, defaultIp[3]);
  EEPROM.write(4, false);   // tcpEnabled = false
  EEPROM.write(5, 0);       // tcpIp = 0.0.0.0
  EEPROM.write(6, 0);
  EEPROM.write(7, 0);
  EEPROM.write(8, 0);
  EEPROM.write(9, 0);       // tcpPort = 0
  EEPROM.write(10, 0);
  EEPROM.write(11, false);  // tcpSecure = false
  for (int i = 12; i < 76; i++) EEPROM.write(i, 0); // Clear tcpUser, tcpPassword
  EEPROM.write(76, false);  // httpEnabled = false
  for (int i = 77; i < 141; i++) EEPROM.write(i, 0); // Clear httpUrl
  EEPROM.write(141, false); // httpSecure = false
  for (int i = 142; i < 206; i++) EEPROM.write(i, 0); // Clear httpUser, httpPassword
  EEPROM.write(206, false); // serialEnabled = false
  for (int i = 207; i < 239; i++) EEPROM.write(i, 0); // Clear adminPassword
  EEPROM.write(239, 0xAA);  // Set configFlag
  EEPROM.commit();
  Serial.println("EEPROM initialized with defaults, IP: 10.168.0.177");

  // Load the defaults into config
  loadConfig(config);
  Serial.print("Loaded IP from EEPROM: ");
  Serial.println(config.deviceIp);

  // Debug: Verify EEPROM IP after write/load
  Serial.print("EEPROM IP (0-3) after operation: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(EEPROM.read(i), HEX);
    Serial.print(" ");
  }
  Serial.println();
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

  Serial.print("Loaded config from EEPROM, IP: ");
  Serial.println(config.deviceIp);
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