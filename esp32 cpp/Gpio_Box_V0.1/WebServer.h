#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Ethernet.h>
#include "ConfigManager.h"

extern Config config; // From main sketch
extern LiquidCrystal_I2C lcd; // For LCD updates

// Forward declarations
void sendConfigPage(EthernetClient& client);
void handleSaveIp(EthernetClient& client, String body);

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

            // Handle GET immediately
            if (request.startsWith("GET / ")) {
              sendConfigPage(client);
              break;
            }
            // For POST, wait for body
            else if (request.startsWith("POST /save ")) {
              // Continue to read body
            } else {
              String response = "HTTP/1.1 204 No Content\r\n"
                                "Connection: close\r\n"
                                "\r\n";
              client.print(response);
              break;
            }
          }
        } else if (contentLength > 0) {
          body += c;
          bodyBytesRead++;
          if (bodyBytesRead == contentLength) {
            Serial.println("Full body received: " + body);
            handleSaveIp(client, body);
            break;
          }
        }
      }
    }
    delay(1);
    client.stop();
  } else {
    static unsigned long lastDebug = 0;
    if (millis() - lastDebug > 5000) {
      lastDebug = millis();
    }
  }
}

void sendConfigPage(EthernetClient& client) {
  String ipStr = config.deviceIp.toString();
  String tcpIpStr = config.tcpIp.toString();
  String htmlContent = "<!DOCTYPE html><html><body style='font-family: Arial; margin: 20px;'>"
                       "<h2 style='border-bottom: 1px solid #ccc;'>GPIO Box Configuration</h2>"
                       "<form id='configForm' onsubmit='sendJson(event)'>"
                       "<h3>Device Address</h3>"
                       "IP: <input type='text' name='ip' value='" + ipStr + "'><br><br>"

                       "<h3>TCP Settings</h3>"
                       "Enabled: <input type='checkbox' name='tcpEnabled' " + (config.tcpEnabled ? "checked" : "") + "><br>"
                       "IP: <input type='text' name='tcpIp' value='" + tcpIpStr + "'><br>"
                       "Port: <input type='number' name='tcpPort' value='" + String(config.tcpPort) + "'><br>"
                       "Secure Mode: <input type='checkbox' name='tcpSecure' " + (config.tcpSecure ? "checked" : "") + "><br>"
                       "User: <input type='text' name='tcpUser' value='" + String(config.tcpUser) + "'><br>"
                       "Password: <input type='password' name='tcpPassword' value='" + String(config.tcpPassword) + "'><br><br>"

                       "<h3>HTTP Settings</h3>"
                       "Enabled: <input type='checkbox' name='httpEnabled' " + (config.httpEnabled ? "checked" : "") + "><br>"
                       "URL: <input type='text' name='httpUrl' value='" + String(config.httpUrl) + "'><br>"
                       "Secure Mode: <input type='checkbox' name='httpSecure' " + (config.httpSecure ? "checked" : "") + "><br>"
                       "User: <input type='text' name='httpUser' value='" + String(config.httpUser) + "'><br>"
                       "Password: <input type='password' name='httpPassword' value='" + String(config.httpPassword) + "'><br><br>"

                       "<h3>Serial Settings</h3>"
                       "Enabled: <input type='checkbox' name='serialEnabled' " + (config.serialEnabled ? "checked" : "") + "><br><br>"

                       "<h3>Admin Access</h3>"
                       "Password: <input type='password' name='adminPassword' value='" + String(config.adminPassword) + "'><br>"
                       "<input type='submit' value='Save'><br><br>"

                       "</form>"
                       "<script>"
                       "function sendJson(event) {"
                       "  event.preventDefault();"
                       "  let data = {"
                       "    ip: document.querySelector('input[name=ip]').value,"
                       "    tcpEnabled: document.querySelector('input[name=tcpEnabled]').checked,"
                       "    tcpIp: document.querySelector('input[name=tcpIp]').value,"
                       "    tcpPort: parseInt(document.querySelector('input[name=tcpPort]').value) || 0,"
                       "    tcpSecure: document.querySelector('input[name=tcpSecure]').checked,"
                       "    tcpUser: document.querySelector('input[name=tcpUser]').value,"
                       "    tcpPassword: document.querySelector('input[name=tcpPassword]').value,"
                       "    httpEnabled: document.querySelector('input[name=httpEnabled]').checked,"
                       "    httpUrl: document.querySelector('input[name=httpUrl]').value,"
                       "    httpSecure: document.querySelector('input[name=httpSecure]').checked,"
                       "    httpUser: document.querySelector('input[name=httpUser]').value,"
                       "    httpPassword: document.querySelector('input[name=httpPassword]').value,"
                       "    serialEnabled: document.querySelector('input[name=serialEnabled]').checked,"
                       "    adminPassword: document.querySelector('input[name=adminPassword]').value"
                       "  };"
                       "  fetch('/save', {"
                       "    method: 'POST',"
                       "    headers: {'Content-Type': 'application/json'},"
                       "    body: JSON.stringify(data)"
                       "  }).then(response => location.reload());"
                       "}"
                       "</script>"
                       "<footer style='border-top: 1px solid #ccc; padding: 10px; text-align: center;'>GPIO Box v1.0</footer>"
                       "</body></html>";

  int contentLength = htmlContent.length();

  String response = "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: " + String(contentLength) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n" + htmlContent;
  


  // Calculate actual bytes sent for debugging
  int bytesSent = 0;
  for (int i = 0; i < response.length(); i++) {
    client.print(response.charAt(i));
    bytesSent++;
  }
}

void handleSaveIp(EthernetClient& client, String body) {
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, body);
  if (error) {
    String response = "HTTP/1.1 400 Bad Request\r\n"
                      "Content-Type: text/html\r\n"
                      "Content-Length: 20\r\n"
                      "Connection: close\r\n"
                      "\r\n<h2>Invalid JSON</h2>";
    client.print(response);
    Serial.println("Invalid JSON with Content-Length: 20");
    return;
  }

  Config newConfig = config; // Copy current config
  if (doc.containsKey("ip")) {
    String ipStr = doc["ip"].as<const char*>();
    newConfig.deviceIp.fromString(ipStr);
  }
  if (doc.containsKey("tcpEnabled")) newConfig.tcpEnabled = doc["tcpEnabled"].as<bool>();
  if (doc.containsKey("tcpIp")) {
    String tcpIpStr = doc["tcpIp"].as<const char*>();
    newConfig.tcpIp.fromString(tcpIpStr);
  }
  if (doc.containsKey("tcpPort")) newConfig.tcpPort = doc["tcpPort"].as<uint16_t>();
  if (doc.containsKey("tcpSecure")) newConfig.tcpSecure = doc["tcpSecure"].as<bool>();
  if (doc.containsKey("tcpUser")) strncpy(newConfig.tcpUser, doc["tcpUser"].as<const char*>(), 32);
  if (doc.containsKey("tcpPassword")) strncpy(newConfig.tcpPassword, doc["tcpPassword"].as<const char*>(), 32);
  if (doc.containsKey("httpEnabled")) newConfig.httpEnabled = doc["httpEnabled"].as<bool>();
  if (doc.containsKey("httpUrl")) strncpy(newConfig.httpUrl, doc["httpUrl"].as<const char*>(), 64);
  if (doc.containsKey("httpSecure")) newConfig.httpSecure = doc["httpSecure"].as<bool>();
  if (doc.containsKey("httpUser")) strncpy(newConfig.httpUser, doc["httpUser"].as<const char*>(), 32);
  if (doc.containsKey("httpPassword")) strncpy(newConfig.httpPassword, doc["httpPassword"].as<const char*>(), 32);
  if (doc.containsKey("serialEnabled")) newConfig.serialEnabled = doc["serialEnabled"].as<bool>();
  if (doc.containsKey("adminPassword")) strncpy(newConfig.adminPassword, doc["adminPassword"].as<const char*>(), 32);

  saveConfig(newConfig);
  config = newConfig; // Update global config

  String htmlContent = "<h2>Config Saved<br><a href=\"/\">Back</a></h2>";
  int contentLength = htmlContent.length();

  String response = "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: " + String(contentLength) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n" + htmlContent;
  
  client.print(response);
  Serial.println("Config saved and applied with Content-Length: " + String(contentLength));
}

#endif