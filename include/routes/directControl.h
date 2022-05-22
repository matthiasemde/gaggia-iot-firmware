#ifndef __DIRECT_CONTROL__
#define __DIRECT_CONTROL__

#include <ESP8266WebServer.h>
#include <NTPClient.h>

#include "../../include/sensors.h"

void attachDirectControlRoutes(ESP8266WebServer *server, NTPClient *timeClient, Sensor** sensors);

#endif
