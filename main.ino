#include <NTPClient.h>
#include <WiFiUdp.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "include/credentials.h"
#include "include/routes/index.h"
#include "include/routes/info.h"
#include "include/routes/directControl.h"

#include "include/io.h"
#include "include/control.h"

// Create Server 
ESP8266WebServer* server = new ESP8266WebServer(80);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

uint32_t lastStatusUpdate = 0;

void setup() {
    Serial.begin(115200);

    // initialize input output module
    IO::init();


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

    // start NTP client
    timeClient.begin();

    // init routing
    IndexRouter::attach(server);
    SensorRouter::attach(server, &timeClient);
    DirectControlRouter::attach(server, &timeClient);

    // Start server
    server->begin();
    Serial.println("HTTP server started");
}

void loop(void) {

    server->handleClient();
    timeClient.update();

    Control::update();

    // Status update
    if (lastStatusUpdate + 5000 < millis()) {
        lastStatusUpdate = millis();
        Serial.println("\n/////////////// Status update ///////////////\nat " + timeClient.getFormattedTime());
        Serial.println("\n///////// Control status /////////\n" + Control::status());
        Serial.println("\n/////////    IO status   /////////\n" + IO::status());
    }
}
