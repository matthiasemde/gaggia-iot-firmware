#include "../../include/routes/sensors.h"
#include <ArduinoJson.h>

void attachSensorRoutes(ESP8266WebServer *server, NTPClient *timeClient) {
    server->on("/sensors/", HTTP_GET, [server, timeClient]() {
        DynamicJsonDocument res(1024);

        res["time"] = timeClient->getEpochTime();
        res["temperature"] = 45.4;
        res["pressure"] = 7.64;

        uint8_t resLength = measureJsonPretty(res);
        char *buf = (char*) malloc(resLength * sizeof(char));
        
        serializeJsonPretty(res, buf, resLength);
        server->send(200, "json", (const char *)buf);

        free(buf);
    });
}