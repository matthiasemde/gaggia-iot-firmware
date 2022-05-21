#include <NTPClient.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

#include "include/credentials.h"
#include "include/routes/index.h"
#include "include/routes/info.h"

#include "include/sensors.h"

// Create Server 
ESP8266WebServer server(80);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

Sensor* sensors[NUM_SENSORS];

void setup() {
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // initialize hardware
    sensors[TEMP_IDX] = new TemperatureSensor();


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
    attachIndexRoutes(&server);
    attachSensorRoutes(&server, &timeClient, sensors);

    // Start server
    server.begin();
    Serial.println("HTTP server started");
}

void loop(void) {
    server.handleClient();
    timeClient.update();

    {
        Sensor** s = sensors;
        for(int _=0; _ < NUM_SENSORS; _++) {
            (*s)->updateValue();
            (*s)->updateControler();
            s++;
            if(millis() % 1000 == 0) {
                Serial.println("Status of " + (*s)->displayName + ": " + (*s)->status());
            }
        }
    }

}
