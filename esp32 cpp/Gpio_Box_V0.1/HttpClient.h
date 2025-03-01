#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <Ethernet.h>

// This func is sending post tick to the keystroker app.
void sendHttpPost(const char* server, int port, const char* url) {
  EthernetClient client;
  if (client.connect(server, port)) {
    // Start HTTP Request
    client.print("POST ");
    client.print(url); 
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println("Content-Type: application/json");

    String payload = "{\"event\": \"tick\"}";
    client.print("Content-Length: ");
    client.println(payload.length());
    client.println();  // End of headers

    // Send payload
    client.print(payload);

    // Wait for response
    while (client.available()) {
      char c = client.read();
      Serial.print(c); // Print response to serial monitor
    }

    // Close connection
    client.stop();
  } else {
    Serial.println("Connection failed.");
  }
}

#endif