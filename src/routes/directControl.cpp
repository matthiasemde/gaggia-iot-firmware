#include "../../include/routes/directControl.h"

namespace DirectControlRouter {
    void attach(WebServer* server, NTPClient* timeClient) {
        
        server->on("/direct-control/", HTTP_POST, [server]() {
            uint16_t responseCode = 400; // bad request
            for (uint8_t i = 0; i < server->args(); i++) { 
                if (server->argName(i) == "temperature") {
                    float newTarget = server->arg(i).toFloat();
                    if(newTarget > 0 && newTarget <= MAX_TEMP_TARGET) {
                        Control::setTemperatureTarget(newTarget);
                        responseCode = 202;
                    }
                } else if (server->argName(i) == "pressure") {
                    Control::setPressureTarget(server->arg(i).toFloat());
                    responseCode = 202;
                } else if (server->argName(i) == "solenoid") {
                    if(server->arg(i) == "open") {
                        Control::openSolenoid();
                        responseCode = 202;
                    } else if(server->arg(i) == "close") {
                        Control::closeSolenoid();
                        responseCode = 202;
                    }
                }
            }
            server->send(responseCode);
        });
    }
}