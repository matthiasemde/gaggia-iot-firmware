#include "../../include/routes/index.h"
#include "../../include/io.h"

void sayHi(ESP8266WebServer *server) {
    IO::sayHello();
    server->send(200, F("text/html"),
    F("No, this is Patrick!"));
}

void attachIndexRoutes(ESP8266WebServer *server) {
    server->on("/", HTTP_GET, [server]() {
        sayHi(server);
    });
}