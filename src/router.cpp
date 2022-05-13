#include "../include/router.h"

Router::Router(ESP8266WebServer *_server) {
    server = _server;        
}

void Router::init() {
    server->on("/", HTTP_GET, [this]() {
        server->send(200, F("text/html"),
        F("No, this is Patrick!"));
    });
}
