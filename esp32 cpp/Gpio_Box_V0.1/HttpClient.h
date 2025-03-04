#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <Ethernet.h>
#include "ConfigManager.h"

extern Config config; // Access stored config settings

void sendHttpPost(const String& message) {
    if (strlen(config.httpUrl) == 0) {
        Serial.println("URL is empty. Skipping HTTP send.");
        return;
    }

    EthernetClient client;

    if (client.connect(config.httpUrl, 80)) {  // Assuming port 80 for HTTP
        // Start HTTP Request
        client.print("POST ");
        client.print(config.httpUrl); 
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(config.httpUrl);
        client.println("Connection: close");
        client.println("Content-Type: application/json");

        client.print("Content-Length: ");
        client.println(message.length());
        client.println();  // End of headers

        // Send payload
        client.print(message);
        Serial.println("HTTP message sent.");

        client.stop();  // Close connection
    } else {
        Serial.println("HTTP connection failed.");
    }
}

#endif
