#include <ArduinoJson.h>

#include "../../include/routes/info.h"

void attachSensorRoutes(ESP8266WebServer* server, NTPClient* timeClient, Sensor** sensors) {
    server->on("/info/sensors", HTTP_GET, [server, timeClient, sensors]() {
        DynamicJsonDocument res(1024);

        res["time"] = timeClient->getEpochTime();
        res["temperature"] = sensors[TEMP_IDX]->getValue();
        res["pressure"] = 7.64;

        uint8_t resLength = measureJsonPretty(res);
        char *buf = (char*) malloc(resLength * sizeof(char));
        
        serializeJsonPretty(res, buf, resLength);
        server->send(200, "application/json", (const char *)buf);

        free(buf);
    });
}