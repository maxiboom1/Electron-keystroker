#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Ethernet.h>
#include "ConfigManager.h"
#include <ArduinoJson.h> // Include ArduinoJson for JSON parsing

extern Config config; // From main sketch
extern LiquidCrystal_I2C lcd; // For LCD updates

// Forward declarations
void sendConfigPage(EthernetClient& client);
void handleSaveSettings(EthernetClient& client, String body);
void sendLoginPage(EthernetClient& client);
void handleLogin(EthernetClient& client, String body);

// Local helper function for JSON parsing
bool parseJsonConfig(String json, Config& config) {
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error) return false;
  
  // Parse network settings
  if (doc.containsKey("ip")) {
    String ipStr = doc["ip"].as<const char*>();
    config.deviceIp.fromString(ipStr);
  }
  if (doc.containsKey("gateway")) {
    String gatewayStr = doc["gateway"].as<const char*>();
    config.gateway.fromString(gatewayStr);
  }
  if (doc.containsKey("subnetMask")) {
    String subnetStr = doc["subnetMask"].as<const char*>();
    config.subnetMask.fromString(subnetStr);
  }

  // Parse TCP settings
  if (doc.containsKey("tcpEnabled")) config.tcpEnabled = doc["tcpEnabled"].as<bool>();
  if (doc.containsKey("tcpIp")) {
    String tcpIpStr = doc["tcpIp"].as<const char*>();
    config.tcpIp.fromString(tcpIpStr);
  }
  if (doc.containsKey("tcpPort")) config.tcpPort = doc["tcpPort"].as<uint16_t>();
  if (doc.containsKey("tcpSecure")) config.tcpSecure = doc["tcpSecure"].as<bool>();
  if (doc.containsKey("tcpUser")) strncpy(config.tcpUser, doc["tcpUser"].as<const char*>(), 32);
  if (doc.containsKey("tcpPassword")) strncpy(config.tcpPassword, doc["tcpPassword"].as<const char*>(), 32);

  // Parse HTTP settings
  if (doc.containsKey("httpEnabled")) config.httpEnabled = doc["httpEnabled"].as<bool>();
  if (doc.containsKey("httpUrl")) strncpy(config.httpUrl, doc["httpUrl"].as<const char*>(), 64);
  if (doc.containsKey("httpSecure")) config.httpSecure = doc["httpSecure"].as<bool>();
  if (doc.containsKey("httpUser")) strncpy(config.httpUser, doc["httpUser"].as<const char*>(), 32);
  if (doc.containsKey("httpPassword")) strncpy(config.httpPassword, doc["httpPassword"].as<const char*>(), 32);

  // Parse Serial settings
  if (doc.containsKey("serialEnabled")) config.serialEnabled = doc["serialEnabled"].as<bool>();

  // Parse Admin settings
  if (doc.containsKey("adminPassword")) strncpy(config.adminPassword, doc["adminPassword"].as<const char*>(), 32);

  return true;
}

void handleWebClient(EthernetServer& server) {
  EthernetClient client = server.available();
  if (client) {
    String request = "";
    String body = "";
    bool headersComplete = false;
    int contentLength = -1;
    int bodyBytesRead = 0;
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;

        if (!headersComplete && request.endsWith("\r\n\r\n")) {
          headersComplete = true;

          // Extract Content-Length
          int lengthPos = request.indexOf("Content-Length: ");
          if (lengthPos != -1) {
            int lengthEnd = request.indexOf('\r', lengthPos);
            String lengthStr = request.substring(lengthPos + 16, lengthEnd);
            contentLength = lengthStr.toInt();
          }

          // Handle login form submission
          if (request.startsWith("POST /login ")) {
            while (client.available() && bodyBytesRead < contentLength) {
              char b = client.read();
              body += b;
              bodyBytesRead++;
            }
            handleLogin(client, body);
            return;
          }

          // Handle config save request
          if (request.startsWith("POST /save ")) {
            while (client.available() && bodyBytesRead < contentLength) {
              char b = client.read();
              body += b;
              bodyBytesRead++;
            }
            handleSaveSettings(client, body);
            return;
          }

          // If not a POST request, always show the login form
          sendLoginPage(client);
          return;
        }
      }
    }

    client.stop();
  }
}

void sendLoginPage(EthernetClient& client) {
  String loginPage = "<!DOCTYPE html><html><head>"
                     "<style>body {font-family: Arial; }</style></head>"
                     "<body><h2>Login</h2>"
                     "<form method='POST' action='/login'>"
                     "User: <input type='text' name='user'><br>"
                     "Password: <input type='password' name='password'><br>"
                     "<input type='submit' value='Login'></form>"
                     "</body></html>";

  client.print("HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html\r\n"
               "Content-Length: " + String(loginPage.length()) + "\r\n"
               "Connection: close\r\n\r\n" + loginPage);
}

void handleLogin(EthernetClient& client, String body) {
  String user, pass;
  if (body.indexOf("user=") != -1 && body.indexOf("password=") != -1) {
    int userStart = body.indexOf("user=") + 5;
    int userEnd = body.indexOf("&", userStart);
    user = body.substring(userStart, userEnd);

    int passStart = body.indexOf("password=") + 9;
    pass = body.substring(passStart);

    if (user == "admin" && pass == String(config.adminPassword)) {
      sendConfigPage(client);
      return;
    }
  }

  // If authentication fails
  client.print("HTTP/1.1 401 Unauthorized\r\n"
               "Content-Type: text/html\r\n"
               "Connection: close\r\n\r\n"
               "<h2>Login Failed. Try again.</h2>");
}

void sendConfigPage(EthernetClient& client) {
  String ipStr = config.deviceIp.toString();
  String gatewayStr = config.gateway.toString();
  String subnetStr = config.subnetMask.toString();
  String tcpIpStr = config.tcpIp.toString();

  String htmlContent = "<!DOCTYPE html><html><head>"
                       "<style>body {font-family: Arial; }input {margin:2.5px;}</style></head>"
                       "<body><h2>GPIO Box Configuration</h2><hr>"
                       "<form id='configForm' onsubmit='sendJson(event)'>"
                       "<h3>Network Settings</h3>"
                       "IP: <input type='text' name='ip' value='" + ipStr + "'><br>"
                       "Gateway: <input type='text' name='gateway' value='" + gatewayStr + "'><br>"
                       "Subnet Mask: <input type='text' name='subnetMask' value='" + subnetStr + "'><br><hr>"

                       "<h3>TCP Settings</h3>"
                       "Enabled: <input type='checkbox' name='tcpEnabled' " + (config.tcpEnabled ? "checked" : "") + "><br>"
                       "IP: <input type='text' name='tcpIp' value='" + tcpIpStr + "'><br>"
                       "Port: <input type='number' name='tcpPort' value='" + String(config.tcpPort) + "'><br>"
                       "Secure Mode: <input type='checkbox' name='tcpSecure' " + (config.tcpSecure ? "checked" : "") + "><br>"
                       "User: <input type='text' name='tcpUser' value='" + String(config.tcpUser) + "'><br>"
                       "Password: <input type='password' name='tcpPassword' value='" + String(config.tcpPassword) + "'><br><hr>"

                       "<h3>HTTP Settings</h3>"
                       "Enabled: <input type='checkbox' name='httpEnabled' " + (config.httpEnabled ? "checked" : "") + "><br>"
                       "URL: <input type='text' name='httpUrl' value='" + String(config.httpUrl) + "'><br>"
                       "Secure Mode: <input type='checkbox' name='httpSecure' " + (config.httpSecure ? "checked" : "") + "><br>"
                       "User: <input type='text' name='httpUser' value='" + String(config.httpUser) + "'><br>"
                       "Password: <input type='password' name='httpPassword' value='" + String(config.httpPassword) + "'><br><hr>"

                       "<h3>Serial Settings</h3>"
                       "Enabled: <input type='checkbox' name='serialEnabled' " + (config.serialEnabled ? "checked" : "") + "><br><hr>"

                       "<h3>Admin Access</h3>"
                       "Password: <input type='password' name='adminPassword' value='" + String(config.adminPassword) + "'><br>"
                       "<input type='submit' value='Save'><br><br>"

                       "</form>"
                       "<script>"
                       "function sendJson(event) {"
                       "  event.preventDefault();"
                       "  let data = {"
                       "    ip: document.querySelector('input[name=ip]').value,"
                       "    gateway: document.querySelector('input[name=gateway]').value,"
                       "    subnetMask: document.querySelector('input[name=subnetMask]').value,"
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

void handleSaveSettings(EthernetClient& client, String body) {
  Config newConfig = config; // Copy current config

  // Use parseJsonConfig to parse the JSON body
  if (!parseJsonConfig(body, newConfig)) {
    String response = "HTTP/1.1 400 Bad Request\r\n"
                      "Content-Type: text/html\r\n"
                      "Content-Length: 20\r\n"
                      "Connection: close\r\n"
                      "\r\n<h2>Invalid JSON</h2>";
    client.print(response);
    Serial.println("Invalid JSON with Content-Length: 20");
    return;
  }

  // Save the new configuration
  saveConfig(newConfig);
  config = newConfig; // Update global config

  // Send success response
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