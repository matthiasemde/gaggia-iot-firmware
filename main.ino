#include <NTPClient.h>
#include <WiFiUdp.h>

#include <WiFi.h>
#include <WebServer.h>

#include "include/credentials.h"
#include "include/routes/index.h"
#include "include/routes/info.h"
#include "include/routes/directControl.h"
#include "include/routes/configuration.h"

#include "include/modules/io.h"
#include "include/modules/storage.h"
#include "include/modules/control/control.h"
#include "include/modules/fsm.h"

// Create Server 
auto server = new WebServer(80);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

uint32_t lastStatusUpdate = 0;

void setup() {
    Serial.begin(115200);

    // initialize all modules
    Storage::init();
    Control::init();
    IO::init();
    FSM::init();

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // wait for connection
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

    // initialize api
    IndexRouter::attach(server);
    InfoRouter::attach(server, &timeClient);
    DirectControlRouter::attach(server, &timeClient);
    ConfigurationRouter::attach(server);

    // start server
    server->begin();
    Serial.println("HTTP server started");
}

void loop(void) {

    server->handleClient();
    timeClient.update();

    state_t gaggiaState = FSM::update();

    if(gaggiaState != UNINITIALIZED) {
        Control::update();
    }

    // Status update
    if (lastStatusUpdate + 3000 < millis()) {
        lastStatusUpdate = millis();
        Serial.println("\n/////////////// Status update ///////////////\nat " + timeClient.getFormattedTime());
        Serial.println("\n///////// FSM status /////////\n" + FSM::status());
        Serial.println("\n///////// Control status /////////\n" + Control::status());
        Serial.println("\n/////////    IO status   /////////\n" + IO::status());
    }
}
