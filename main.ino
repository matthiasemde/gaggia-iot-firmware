#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "include/credentials.h"
#include "include/router.h"

// Create Server 
ESP8266WebServer server(80);

// Create Router
Router router(&server);

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.print("\nConnected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());


    // init routing
    router.init();

    // Start server
    server.begin();
    Serial.println("HTTP server started");
}

void loop(void) {
    server.handleClient();
}
