#ifndef __SENSORS__
#define __SENSORS__

#include <ESP8266WebServer.h>
#include <NTPClient.h>

void attachSensorRoutes(ESP8266WebServer *server, NTPClient *timeClient);

#endif
