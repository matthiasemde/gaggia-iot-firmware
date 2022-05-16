#ifndef __INDEXROUTER__
#define __INDEXROUTER__

#include <ESP8266WebServer.h>

void sayHi(ESP8266WebServer *server);

void attachIndexRoutes(ESP8266WebServer *server);

#endif