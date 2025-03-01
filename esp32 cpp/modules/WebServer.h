#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Ethernet.h>
#include "ConfigManager.h" // For saveIpConfig

extern IPAddress deviceIp; // From main sketch
extern LiquidCrystal_I2C lcd; // From main sketch for LCD updates

// Forward declarations
void sendConfigPage(EthernetClient& client);
void handleSaveIp(EthernetClient& client, String request);

void handleWebClient(EthernetServer& server) {
  EthernetClient client = server.available();
  if (client) {
    String request = ""; // Headers
    String body = "";    // POST body
    bool headersComplete = false;
    int contentLength = -1;
    int bodyBytesRead = 0;
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (!headersComplete) {
          request += c;
          if (request.endsWith("\r\n\r\n")) {
            headersComplete = true;

            // Check for Content-Length header
            int lengthPos = request.indexOf("Content-Length: ");
            if (lengthPos != -1) {
              int lengthEnd = request.indexOf('\r', lengthPos);
              String lengthStr = request.substring(lengthPos + 16, lengthEnd);
              contentLength = lengthStr.toInt();
            }

            // Handle GET immediately (no body expected)
            if (request.startsWith("GET / ")) {
              sendConfigPage(client);
              break;
            }
            // For POST, wait for body
            else if (!request.startsWith("POST /save ")) {
              String response = "HTTP/1.1 204 No Content\r\n"
                                "Connection: close\r\n"
                                "\r\n";
              client.print(response);
              break;
            }
          }
        } else if (contentLength > 0) {
          // Collect body for POST
          body += c;
          bodyBytesRead++;

          // Check if entire body is received
          if (bodyBytesRead == contentLength) {
            handleSaveIp(client, body);
            break;
          }
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("Client disconnected");
  } else {
    static unsigned long lastDebug = 0;
    if (millis() - lastDebug > 5000) {
      lastDebug = millis();
    }
  }
}

void sendConfigPage(EthernetClient& client) {
  String ipStr = Ethernet.localIP().toString();
  String htmlContent = "<!DOCTYPE html><html><body>"
                       "<h2>ESP32 IP Configuration</h2>"
                       "<form action=\"/save\" method=\"POST\">"
                       "IP Address: <input type=\"text\" name=\"ip\" value=\"" + ipStr + "\"><br>"
                       "<input type=\"submit\" value=\"Save\">"
                       "</form></body></html>";
  int contentLength = htmlContent.length();

  String response = "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: " + String(contentLength) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n" + htmlContent;
  
  client.print(response);
  Serial.println("Sent config page with Content-Length: " + String(contentLength));
}

void handleSaveIp(EthernetClient& client, String body) {
  int ipStart = body.indexOf("ip=") + 3;
  String ipStr = body.substring(ipStart); // Body is just "ip=10.168.0.177", no \r yet
  IPAddress newIp;
  String htmlContent;
  int contentLength;

  if (newIp.fromString(ipStr)) {
    saveIpConfig(newIp);
    deviceIp = newIp;
    Ethernet.begin(mac, deviceIp, gateway, gateway, subnet);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IP:");
    lcd.setCursor(3, 0);
    lcd.print(Ethernet.localIP());
    lcd.setCursor(0, 1);
    lcd.print("New IP Saved");

    htmlContent = "<h2>IP Saved: " + newIp.toString() + "<br><a href=\"/\">Back</a></h2>";
    contentLength = htmlContent.length();
  } else {
    htmlContent = "<h2>Invalid IP format</h2>";
    contentLength = htmlContent.length();
  }

  String response = "HTTP/1.1 " + String(newIp.fromString(ipStr) ? "200 OK" : "400 Bad Request") + "\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: " + String(contentLength) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n" + htmlContent;
  
  client.print(response);
}

#endif