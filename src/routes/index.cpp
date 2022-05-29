#include "../../include/routes/index.h"

void IndexRouter::sayHi(WebServer *server) {
    IO::sayHello();
    server->send(200, F("text/html"),
    F("No, this is Patrick!"));
}

void IndexRouter::attach(WebServer *server) {
    server->on("/", HTTP_GET, [server]() {
        sayHi(server);
    });
}