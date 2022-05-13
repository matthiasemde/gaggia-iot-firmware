#ifndef __HANDLER__
#define __HANDLER__

#include <ESP8266WebServer.h>

class Router {
private: 
    ESP8266WebServer *server;
public:
    Router(ESP8266WebServer *_server);
    void init();
};

#endif