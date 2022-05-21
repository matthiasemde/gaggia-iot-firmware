#include <ArduinoJson.h>

#include "../../include/routes/directControl.h"

void attachDirectControlRoutes(ESP8266WebServer* server, NTPClient* timeClient, Sensor** sensors) {
    
    server->on("/direct-control/temperature", HTTP_POST, [server, sensors]() {
        uint16_t responseCode = 400; // bad request
        for (uint8_t i = 0; i < server->args(); i++) { 
            if (server->argName(i) == "controlValue") {
                uint16_t newTarget = server->arg(i).toInt();
                if(newTarget > 0 && newTarget <= MAX_TEMP) {
                    sensors[TEMP_IDX]->setControlTarget(newTarget);
                    responseCode = 202;
                }
            }
        }
        server->send(responseCode);
    });
}