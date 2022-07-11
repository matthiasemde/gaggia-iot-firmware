#include <NTPClient.h>
#include <WiFiUdp.h>

#include <WiFi.h>
#include <WebServer.h>

#include "include/RTOSConfig.h"

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

void vTaskStatusUpdate(void *pvParameters) {
    for( ;; ) {
        Serial.println("\n/////////////// Status update ///////////////\nat " + timeClient.getFormattedTime());
        Serial.println("Total remaining heap: " + String(xPortGetFreeHeapSize()));
        Serial.println("Remaining stack size for status task: " + String(uxTaskGetStackHighWaterMark(NULL)));
        Serial.println("\n///////// FSM status /////////\n" + FSM::status());
        Serial.println("\n///////// Control status /////////\n" + Control::status());
        Serial.println("\n/////////    IO status   /////////\n" + IO::status());
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

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

    // create the status update task
    xTaskCreate(
        vTaskStatusUpdate,
        "STATUS",
        STATUS_TASK_STACK_SIZE,
        NULL,
        STATUS_TASK_PRIORITY,
        NULL
    );
}

void loop(void) {
    timeClient.update();
    server->handleClient();
}
