#include "../../include/routes/index.h"

void IndexRouter::sayHi(ESP8266WebServer *server) {
    IO::sayHello();
    server->send(200, F("text/html"),
    F("No, this is Patrick!"));
}

void IndexRouter::attach(ESP8266WebServer *server) {
    server->on("/", HTTP_GET, [server]() {
        sayHi(server);
    });
}