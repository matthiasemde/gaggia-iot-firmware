#include "../../include/routes/configuration.h"

void ConfigurationRouter::attach(WebServer* server) {
    server->on("/configuration", HTTP_GET, [server]() {
        DynamicJsonDocument res(1024);
        configuration_t config = Control::getActiveConfiguration();

        res["temps"]["brew"] = config.temps.brew;
        res["temps"]["steam"] = config.temps.steam;

        res["pressures"]["brew"] = config.pressures.brew;
        res["pressures"]["preinfusion"] = config.pressures.preinfusion;
        res["preinfusionTime"] = config.preinfusionTime;

        String response;
        serializeJson(res, response);

        server->send(200, "application/json", response);
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

    server->on("/configuration/time", HTTP_POST, [server]() {
        uint16_t responseCode = 400; // bad request
        for (uint8_t i = 0; i < server->args(); i++) { 
            if (server->argName(i) == "brew") {
                // Control::setBrewPressure(server->arg(i).toFloat());
                // responseCode = 202;
            } else if (server->argName(i) == "preinfusion") {
                Control::setPreinfusionTime(server->arg(i).toInt());
                responseCode = 202;
            }
        }
        server->send(responseCode);
    });

    server->on("/configuration/temp-pid-coefs", HTTP_POST, [server]() {
        uint16_t responseCode = 400; // bad request
        pidCoefs_t newCoefs;
        for (uint8_t i = 0; i < server->args(); i++) { 
            if (server->argName(i) == "kp") {
                newCoefs.kp = server->arg(i).toFloat();
                responseCode = 202;
            } else if (server->argName(i) == "ki") {
                newCoefs.ki = server->arg(i).toFloat();
                responseCode = 202;
            } else if (server->argName(i) == "kd") {
                newCoefs.kd = server->arg(i).toFloat();
                responseCode = 202;
            }
        }
        if(responseCode == 202) Control::setTemperaturePIDCoefs(newCoefs);
        server->send(responseCode);
    });

    server->on("/configuration/pressure-pid-coefs", HTTP_POST, [server]() {
        uint16_t responseCode = 400; // bad request
        pidCoefs_t newCoefs;
        for (uint8_t i = 0; i < server->args(); i++) { 
            if (server->argName(i) == "kp") {
                newCoefs.kp = server->arg(i).toFloat();
                responseCode = 202;
            } else if (server->argName(i) == "ki") {
                newCoefs.ki = server->arg(i).toFloat();
                responseCode = 202;
            } else if (server->argName(i) == "kd") {
                newCoefs.kd = server->arg(i).toFloat();
                responseCode = 202;
            }
        }
        if(responseCode == 202) Control::setPressurePIDCoefs(newCoefs);
        server->send(responseCode);
    });
}