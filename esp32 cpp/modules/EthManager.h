#ifndef ETH_MANAGER_H
#define ETH_MANAGER_H

#include <Ethernet.h>

// Network Configuration (declare as extern, defined in main sketch)
extern byte mac[];
extern IPAddress deviceIp; // Changed from 'ip' to avoid confusion
extern IPAddress gateway;
extern IPAddress subnet;

// Ethernet Status Tracking (declare as extern, defined in main sketch)
extern bool ethConnected;

// Forward declaration
void updateEthernetStatus(LiquidCrystal_I2C& lcd);

void attemptEthernetInit(LiquidCrystal_I2C& lcd) {
  Ethernet.begin(mac, deviceIp, gateway, gateway, subnet);
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

void updateEthernetStatus(LiquidCrystal_I2C& lcd) {
  EthernetLinkStatus currentLink = Ethernet.linkStatus();

  if (currentLink == LinkON && !ethConnected) {
    Ethernet.begin(mac, deviceIp, gateway, gateway, subnet);  
    
    if (Ethernet.localIP() == IPAddress(0, 0, 0, 0)) {
      Ethernet.begin(mac, deviceIp, gateway, gateway, subnet);
      delay(500);      
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IP:");
    lcd.setCursor(3, 0);
    lcd.print(Ethernet.localIP());
    lcd.setCursor(0, 1);
    lcd.print("Link:OK");
    ethConnected = true;
  } 
  else if (currentLink == LinkOFF) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IP:");
    lcd.setCursor(3, 0);
    lcd.print(Ethernet.localIP());
    lcd.setCursor(0, 1);
    lcd.print("Link:Failed");
    ethConnected = false;
  }
}

#endif