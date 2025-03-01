#ifndef ETH_MANAGER_H
#define ETH_MANAGER_H

#include <Ethernet.h>
extern byte mac[];
extern LiquidCrystal_I2C lcd; // For LCD updates
extern bool ethConnected;
void updateEthernetStatus(LiquidCrystal_I2C& lcd);

// Function to initialize Ethernet
void attemptEthernetInit(const IPAddress& deviceIp) {
  Serial.println("DEBUG: ");
  Serial.println(deviceIp); // Use the passed deviceIp
  Ethernet.begin(mac, deviceIp, IPAddress(10, 168, 0, 1), IPAddress(10, 168, 0, 1), IPAddress(255, 255, 255, 0));
  delay(1000); // Give Ethernet time to initialize

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield not found.");
    lcd.setCursor(0, 1);
    lcd.print("No Ethernet HW ");
    ethConnected = false;
    return;
  }

  updateEthernetStatus(lcd); // Check initial link status
}

// Function to update Ethernet status on LCD
void updateEthernetStatus(LiquidCrystal_I2C& lcd) {
  if (Ethernet.linkStatus() == LinkON) {
    ethConnected = true;
    lcd.setCursor(0, 1);
    lcd.print("Eth Connected  ");
  } else {
    ethConnected = false;
    lcd.setCursor(0, 1);
    lcd.print("No Eth Link    ");
  }
}

#endif