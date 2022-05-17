#include "../../include/routes/sensors.h"
#include <ArduinoJson.h>

void attachSensorRoutes(ESP8266WebServer *server) {
    server->on("/sensors/", HTTP_GET, [server]() {
        DynamicJsonDocument res(1024);

        res["temperature"] = 45.4;
        res["pressure"] = 7.64;

        // serializeJsonPretty(res, server);
        char buf[200];
        serializeJsonPretty(res, buf, 200);
        server->send(200, "json", (const char *)buf);
    });
}