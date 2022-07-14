#include "../../include/routes/info.h"

void InfoRouter::attach(WebServer* server, NTPClient* timeClient) {
    server->on("/info/coffeeCount", HTTP_GET, [server]() {
        DynamicJsonDocument res(32);
        res["coffeeCount"] = Storage::loadCoffeeCount();

        uint8_t resLength = measureJsonPretty(res);
        char *buf = (char*) malloc(resLength * sizeof(char));
        
        serializeJsonPretty(res, buf, resLength);

        server->send(200, "application/json", (const char *)buf);

        free(buf);
    });

    server->on("/info/sensors", HTTP_GET, [server, timeClient]() {
        DynamicJsonDocument res(1024);

        float temperature, pressure;

        Control::getSmoothedTemperature(&temperature);
        Control::getSmoothedTemperature(&pressure);

        res["time"] = timeClient->getEpochTime();
        res["temperature"] = temperature;
        res["pressure"] = pressure;

        uint8_t resLength = measureJsonPretty(res);
        char *buf = (char*) malloc(resLength * sizeof(char));
        
        serializeJsonPretty(res, buf, resLength);
        server->send(200, "application/json", (const char *)buf);

        free(buf);
    });
}