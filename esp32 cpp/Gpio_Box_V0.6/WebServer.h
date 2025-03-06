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
    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
        Serial.print("JSON Parsing Failed: ");
        Serial.println(error.f_str());
        return false;
    }

    // Update only the fields that exist in the received JSON
    if (doc.containsKey("ip")) config.deviceIp.fromString(doc["ip"].as<const char*>());
    if (doc.containsKey("gateway")) config.gateway.fromString(doc["gateway"].as<const char*>());
    if (doc.containsKey("subnetMask")) config.subnetMask.fromString(doc["subnetMask"].as<const char*>());

    if (doc.containsKey("tcpEnabled")) config.tcpEnabled = doc["tcpEnabled"].as<bool>();
    if (doc.containsKey("tcpIp")) config.tcpIp.fromString(doc["tcpIp"].as<const char*>());
    if (doc.containsKey("tcpPort")) config.tcpPort = doc["tcpPort"].as<uint16_t>();
    if (doc.containsKey("tcpSecure")) config.tcpSecure = doc["tcpSecure"].as<bool>();
    if (doc.containsKey("tcpUser")) strncpy(config.tcpUser, doc["tcpUser"].as<const char*>(), sizeof(config.tcpUser));
    if (doc.containsKey("tcpPassword")) strncpy(config.tcpPassword, doc["tcpPassword"].as<const char*>(), sizeof(config.tcpPassword));

    if (doc.containsKey("httpEnabled")) config.httpEnabled = doc["httpEnabled"].as<bool>();
    if (doc.containsKey("httpUrl")) strncpy(config.httpUrl, doc["httpUrl"].as<const char*>(), sizeof(config.httpUrl));
    if (doc.containsKey("httpSecure")) config.httpSecure = doc["httpSecure"].as<bool>();
    if (doc.containsKey("httpUser")) strncpy(config.httpUser, doc["httpUser"].as<const char*>(), sizeof(config.httpUser));
    if (doc.containsKey("httpPassword")) strncpy(config.httpPassword, doc["httpPassword"].as<const char*>(), sizeof(config.httpPassword));

    if (doc.containsKey("serialEnabled")) config.serialEnabled = doc["serialEnabled"].as<bool>();

    // Only update the admin password if the user entered a new one
    if (doc.containsKey("adminPassword")) {
        String newPassword = doc["adminPassword"].as<const char*>();
        if (!newPassword.isEmpty()) {  // Only set if a new password is provided
            strncpy(config.adminPassword, newPassword.c_str(), sizeof(config.adminPassword));
        }
    }

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
  String htmlContent = "<!DOCTYPE html><html><head>"
                         "<style>body {font-family: Arial; }input {margin:2.5px;}</style></head>"
                         "<body><h2>GPIO Box Configuration</h2><hr>"
                         "<form id='configForm' onsubmit='return validateForm(event)'>"

                         "<h3>Network Settings</h3>"
                         "IP: <input type='text' name='ip' id='ip' value='" + config.deviceIp.toString() + "'><br>"
                         "Gateway: <input type='text' name='gateway' id='gateway' value='" + config.gateway.toString() + "'><br>"
                         "Subnet Mask: <input type='text' name='subnetMask' id='subnetMask' value='" + config.subnetMask.toString() + "'><br><hr>"

                         "<h3>TCP Settings</h3>"
                         "Enabled: <input type='checkbox' name='tcpEnabled' id='tcpEnabled' onchange='toggleSecureMode(\"tcp\")' " + (config.tcpEnabled ? "checked" : "") + "><br>"
                         "IP: <input type='text' name='tcpIp' id='tcpIp' value='" + config.tcpIp.toString() + "'><br>"
                         "Port: <input type='number' name='tcpPort' id='tcpPort' value='" + String(config.tcpPort) + "'><br>"
                         "Secure Mode: <input type='checkbox' name='tcpSecure' id='tcpSecure' onchange='toggleSecureMode(\"tcp\")' " + (config.tcpSecure ? "checked" : "") + "><br>"
                         "User: <input type='text' name='tcpUser' id='tcpUser' value='" + String(config.tcpUser) + "'><br>"
                         "Password: <input type='password' name='tcpPassword' id='tcpPassword' value='" + String(config.tcpPassword) + "'><br><hr>"

                         "<h3>HTTP Settings</h3>"
                         "Enabled: <input type='checkbox' name='httpEnabled' id='httpEnabled' onchange='toggleSecureMode(\"http\")' " + (config.httpEnabled ? "checked" : "") + "><br>"
                         "URL: <input type='text' name='httpUrl' id='httpUrl' value='" + String(config.httpUrl) + "'><br>"
                         "Secure Mode: <input type='checkbox' name='httpSecure' id='httpSecure' onchange='toggleSecureMode(\"http\")' " + (config.httpSecure ? "checked" : "") + "><br>"
                         "User: <input type='text' name='httpUser' id='httpUser' value='" + String(config.httpUser) + "'><br>"
                         "Password: <input type='password' name='httpPassword' id='httpPassword' value='" + String(config.httpPassword) + "'><br><hr>"

                         "<h3>Serial Settings</h3>"
                         "Enabled: <input type='checkbox' name='serialEnabled' id='serialEnabled' " + (config.serialEnabled ? "checked" : "") + "><br><hr>"

                         "<h3>Admin Access</h3>"
                         "New Password: <input type='password' name='adminPassword' id='adminPassword' value=''><br>"
                         "<input type='submit' value='Save'><br><br>"

                         "</form>";

  String script1 = "<script>"
                     "function validateForm(event) {"
                     "  if (event) event.preventDefault();"
                     "  function isValidIP(ip) {"
                     "    return /^((25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)$/.test(ip);"
                     "  }"
                     "  function isValidPort(port) {"
                     "    return /^[1-9][0-9]{0,4}$/.test(port) && port > 0 && port <= 65535;"
                     "  }"
                     "  function isValidUrl(url) {"
                     "    try {"
                     "      let parsedUrl = new URL(url);"
                     "      return (parsedUrl.protocol === 'http:' || parsedUrl.protocol === 'https:') && parsedUrl.hostname.length > 0;"
                     "    } catch (e) { return false; }"
                     "  }"

                     "  let ip = document.getElementById('ip').value;"
                     "  let gateway = document.getElementById('gateway').value;"
                     "  let subnetMask = document.getElementById('subnetMask').value;"
                     "  let tcpIp = document.getElementById('tcpIp').value;"
                     "  let tcpPort = document.getElementById('tcpPort').value;"
                     "  let httpUrl = document.getElementById('httpUrl').value;"
                     "  let adminPassword = document.getElementById('adminPassword').value;"

                     "  if (!isValidIP(ip) || !isValidIP(gateway) || !isValidIP(subnetMask)) {"
                     "    alert('Invalid IP Address, Gateway, or Subnet Mask!');"
                     "    return false;"
                     "  }"
                     
                     "  sendJson();"
                     "  return false;"
                     "}";

  String script2 = "function sendJson() {"
                      "  let data = {"
                      "    ip: document.getElementById('ip').value,"
                      "    gateway: document.getElementById('gateway').value,"
                      "    subnetMask: document.getElementById('subnetMask').value,"
                      "    serialEnabled: document.getElementById('serialEnabled').checked"
                      "  };"

                      "  if (document.getElementById('tcpEnabled').checked) {"
                      "    data.tcpEnabled = true;"
                      "    data.tcpIp = document.getElementById('tcpIp').value;"
                      "    data.tcpPort = parseInt(document.getElementById('tcpPort').value) || 0;"
                      "    data.tcpSecure = document.getElementById('tcpSecure').checked;"
                      "    data.tcpUser = document.getElementById('tcpUser').value;"
                      "    data.tcpPassword = document.getElementById('tcpPassword').value;"
                      "  }"

                      "  if (document.getElementById('httpEnabled').checked) {"
                      "    data.httpEnabled = true;"
                      "    data.httpUrl = document.getElementById('httpUrl').value;"
                      "    data.httpSecure = document.getElementById('httpSecure').checked;"
                      "    data.httpUser = document.getElementById('httpUser').value;"
                      "    data.httpPassword = document.getElementById('httpPassword').value;"
                      "  }"

                      "  let adminPassword = document.getElementById('adminPassword').value;"
                      "  if (adminPassword.length > 0) {"
                      "    data.adminPassword = adminPassword;"
                      "  }"
                      "  console.log(data);"
                      "  fetch('/save', {"
                      "    method: 'POST',"
                      "    headers: {'Content-Type': 'application/json'},"
                      "    body: JSON.stringify(data)"
                      "  }).then(response => {"
                      "    if (response.ok) {"
                      "      alert('Configuration Saved Successfully!');"
                      "      location.reload();"
                      "    } else {"
                      "      alert('Error saving configuration!');"
                      "    }"
                      "  });"
                      "} "
                      "function toggleSecureMode(type) {"
                      "let secureCheckbox = document.getElementById(type + 'Secure');"
                      "let userField = document.getElementById(type + 'User');"
                      "let passwordField = document.getElementById(type + 'Password');"
                      "if (secureCheckbox.checked) {"
                      "    userField.disabled = false;"
                      "    passwordField.disabled = false;"
                      "} else {"
                      "    userField.disabled = true;"
                      "    passwordField.disabled = true;"
                      "   }"
                      "}"
                      "window.onload = function() {"
                      "toggleSecureMode('tcp');"
                      "toggleSecureMode('http');"
                      "  };"
                  "</script>";

  
  String footer = "<footer style='border-top: 1px solid #ccc; padding: 10px; text-align: center;'>GPIO Box v1.0</footer></body></html>";
  
  int contentLength = htmlContent.length() + script1.length() + script2.length() + footer.length();

  String header = "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: " + String(contentLength) + "\r\n"
                "Connection: close\r\n\r\n";

  client.print(header);
  delay(50);
  client.print(htmlContent);
  delay(50);
  client.print(script1);
  delay(50);
  client.print(script2);
  delay(50);
  client.print(footer);
}

void handleSaveSettings(EthernetClient& client, String body) {
    Config newConfig = config; // Copy current config to preserve existing values
    //Serial.println(config);
    // Parse the JSON body into newConfig
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

    // Save the updated configuration
    saveConfig(newConfig);
    config = newConfig; // Apply the new configuration globally

    // Send success response
    String htmlContent = "<h2>Config Saved<br><a href=\"/\">Back</a></h2>";
    int contentLength = htmlContent.length();

    String response = "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/html\r\n"
                      "Content-Length: " + String(contentLength) + "\r\n"
                      "Connection: close\r\n"
                      "\r\n" + htmlContent;
    
    client.print(response);
    Serial.println("Config saved and applied");
}

#endif