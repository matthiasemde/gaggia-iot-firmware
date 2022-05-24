#include "../../include/routes/directControl.h"

namespace DirectControlRouter {
    void attach(ESP8266WebServer* server, NTPClient* timeClient) {
        
        server->on("/direct-control/temperature", HTTP_POST, [server]() {
            uint16_t responseCode = 400; // bad request
            for (uint8_t i = 0; i < server->args(); i++) { 
                if (server->argName(i) == "controlValue") {
                    uint16_t newTarget = server->arg(i).toInt();
                    if(newTarget > 0 && newTarget <= MAX_TEMP_TARGET) {
                        Control::setTemperatureTarget(newTarget);
                        responseCode = 202;
                    }
                }
            }
            server->send(responseCode);
        });
    }
}