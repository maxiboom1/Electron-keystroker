#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include "HttpClient.h"
#include "EthManager.h"
#include "ConfigManager.h"
#include "WebServer.h" 

// Network Configuration
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress deviceIp; // Set by ConfigManager
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// HTTP Settings
const char* server = "10.168.0.10";
const int port = 3123;
const char* url = "/api";

// Pin Definitions
#define W5500_CS 15
int lastPinState = HIGH;
int inputPin = 16;

// I2C Pins for ESP32
#define I2C_SDA 21
#define I2C_SCL 22

// LCD Setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Ethernet Status Tracking
bool ethConnected = false;
unsigned long lastEthCheck = 0;
const unsigned long ethCheckInterval = 5000;

// Custom EthernetServer subclass
class MyEthernetServer : public EthernetServer {
public:
  MyEthernetServer(uint16_t port) : EthernetServer(port) {}
  void begin(uint16_t port = 0) override {
    if (port != 0) {
      EthernetServer::begin();
    } else {
      EthernetServer::begin();
    }
  }
};

// Web Server
MyEthernetServer webServer(80);

void setup() {
  Serial.begin(9600);
  pinMode(inputPin, INPUT_PULLUP);

  Wire.begin(I2C_SDA, I2C_SCL);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("I/O Systems");
  lcd.setCursor(0, 1);
  lcd.print("Gpio Box");
  delay(2000);
  initConfig(deviceIp);

  Ethernet.init(W5500_CS);
  attemptEthernetInit(lcd);
  Serial.print("Ethernet IP after init: ");
  Serial.println(Ethernet.localIP());
  Serial.print("Link status: ");
  Serial.println(Ethernet.linkStatus() == LinkON ? "ON" : "OFF");

  webServer.begin();
}

void loop() {
  handleWebClient(webServer); // Pass webServer to the function

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
  lastPinState = currentPinState;
}

void heartbeatHandler(String received) {
  received.trim();
  if (received == "heartbeat") {
    Serial.println("ack");
    lcd.setCursor(0, 1);
    lcd.print("Heartbeat      ");
  }
}