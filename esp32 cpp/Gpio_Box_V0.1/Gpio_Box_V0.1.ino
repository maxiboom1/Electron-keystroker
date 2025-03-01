#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <ArduinoJson.h> // Ensure ESP32-compatible version
#include "HttpClient.h"
#include "EthManager.h"
#include "ConfigManager.h"
#include "WebServer.h"

// Network Configuration
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
Config config; // Global config
IPAddress gateway(10, 168, 0, 1); // Adjust to your router's gateway
IPAddress subnet(255, 255, 255, 0); // Adjust to your subnet mask

const char* server = "10.168.0.10";
const int port = 3123;
const char* url = "/api";

#define W5500_CS 15
int lastPinState = HIGH;
int inputPin = 16; // Placeholder, replace with GPI pins later

#define I2C_SDA 21
#define I2C_SCL 22

LiquidCrystal_I2C lcd(0x27, 16, 2);

bool ethConnected = false;
unsigned long lastEthCheck = 0;
const unsigned long ethCheckInterval = 5000;

class MyEthernetServer : public EthernetServer {
public:
  MyEthernetServer(uint16_t port) : EthernetServer(port) {}
  void begin(uint16_t port = 0) override {
    if (port != 0) {
      EthernetServer::begin();
    } else {
      EthernetServer::begin();
    }
    Serial.println("Server begin called");
  }
};

MyEthernetServer webServer(80);

void setup() {
  Serial.begin(9600);
  pinMode(inputPin, INPUT_PULLUP);

  Wire.begin(I2C_SDA, I2C_SCL); // ESP32 syntax
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");

  initConfig(config);
  Serial.print("Initial IP: ");
  Serial.println(config.deviceIp);

  Ethernet.init(W5500_CS);
  attemptEthernetInit(config.deviceIp); // Pass config.deviceIp directly
  Serial.print("Ethernet IP after init: ");
  Serial.println(Ethernet.localIP());
  Serial.print("Link status: ");
  Serial.println(Ethernet.linkStatus() == LinkON ? "ON" : "OFF");

  if (Ethernet.localIP() == IPAddress(0, 0, 0, 0)) {
    Serial.println("Warning: Ethernet failed to set IP. Attempting DHCP...");
    if (Ethernet.begin(mac)) {
      Serial.print("DHCP IP assigned: ");
      Serial.println(Ethernet.localIP());
    } else {
      Serial.println("DHCP failed. Ethernet not connected.");
      ethConnected = false;
    }
  } else {
    ethConnected = true;
  }

  webServer.begin();
  Serial.println("Web server started at: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  handleWebClient(webServer);

  unsigned long currentMillis = millis();
  if (currentMillis - lastEthCheck >= ethCheckInterval) {
    updateEthernetStatus(lcd);
    lastEthCheck = currentMillis;
  }

  if (Serial.available()) {
    String received = Serial.readStringUntil('\n');
    heartbeatHandler(received);
  }

  int currentPinState = digitalRead(inputPin);
  if (lastPinState != currentPinState) {
    delay(50);
    if (currentPinState == LOW) {
      if (ethConnected) {
        Serial.println("tick");
        sendHttpPost(server, port, url);
        lcd.setCursor(0, 1);
        lcd.print("Sent tick      ");
      } else {
        Serial.println("tick (not sent - no Ethernet)");
        lcd.setCursor(0, 1);
        lcd.print("No Eth - Tick  ");
      }
    }
  }
}

void heartbeatHandler(String received) {
  received.trim();
  if (received == "heartbeat") {
    Serial.println("ack");
    lcd.setCursor(0, 1);
    lcd.print("Heartbeat      ");
  }
}