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

    IO::init();

    // initialize hardware
    Serial.println("Initializing hardware\n");

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
    bool doStatusUpdate = (lastStatusUpdate + 1000 < millis());
    if (doStatusUpdate) {
        lastStatusUpdate = millis();
        Serial.println("\nStatus update!");
    }

    server->handleClient();
    timeClient.update();

    Control::update();

    uint8_t buttonState = IO::getButtonState();
    if(doStatusUpdate) {
        Serial.println((String)"Button state:\nPump: " +
            ((buttonState & PUMP_BUTTON_MASK) ? "on" : "off") +
            "\nSteam: " +
            ((buttonState & STEAM_BUTTON_MASK) ? "on" : "off")
        );
    }
}
