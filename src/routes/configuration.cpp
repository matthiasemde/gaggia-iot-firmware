#include "../../include/routes/configuration.h"

void ConfigurationRouter::attach(WebServer* server) {
        server->on("/configuration", HTTP_GET, [server]() {
            DynamicJsonDocument res(1024);
            Configuration config = Control::getActiveConfiguration();
    
            res["temps"]["brew"] = config.temps.brew;
            res["temps"]["steam"] = config.temps.steam;

            res["pressures"]["brew"] = config.pressures.brew;
            res["pressures"]["preinfusion"] = config.pressures.preinfusion;
            res["preinfusionTime"] = config.preinfusionTime;

            uint8_t resLength = measureJsonPretty(res);
            char *buf = (char*) malloc(resLength * sizeof(char));
            
            serializeJsonPretty(res, buf, resLength);

            server->send(200, "application/json", (const char *)buf);

            free(buf);
        });

        server->on("/configuration/temperature", HTTP_POST, [server]() {
            uint16_t responseCode = 400; // bad request
            for (uint8_t i = 0; i < server->args(); i++) { 
                if (server->argName(i) == "brew") {
                    Control::setBrewTemperature(server->arg(i).toFloat());
                    responseCode = 202;
                } else if (server->argName(i) == "steam") {
                    Control::setSteamTemperature(server->arg(i).toFloat());
                    responseCode = 202;
                }
            }
            server->send(responseCode);
        });

        server->on("/configuration/pressure", HTTP_POST, [server]() {
            uint16_t responseCode = 400; // bad request
            for (uint8_t i = 0; i < server->args(); i++) { 
                if (server->argName(i) == "brew") {
                    Control::setBrewPressure(server->arg(i).toFloat());
                    responseCode = 202;
                } else if (server->argName(i) == "preinfusion") {
                    Control::setPreinfusionPressure(server->arg(i).toFloat());
                    responseCode = 202;
                }
            }
            server->send(responseCode);
        });
    }