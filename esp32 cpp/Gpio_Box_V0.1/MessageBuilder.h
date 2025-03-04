#ifndef MESSAGE_BUILDER_H
#define MESSAGE_BUILDER_H

#include <ArduinoJson.h>
#include "ConfigManager.h" // Access stored config for user credentials

class MessageBuilder {
public:
    static String constructMessage(const String& event, const String& state) {
        StaticJsonDocument<128> doc;
        doc["event"] = event;
        doc["state"] = state;

        // Retrieve user credentials if Secure Mode is enabled
        if (config.tcpSecure || config.httpSecure) {
            doc["user"] = String(config.adminPassword); // Replace with stored user if needed
            doc["password"] = ""; // Password can be retrieved if needed
        } else {
            doc["user"] = "";
            doc["password"] = "";
        }

        String jsonString;
        serializeJson(doc, jsonString);
        return jsonString;
    }
};

#endif
