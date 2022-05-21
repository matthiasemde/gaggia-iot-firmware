#ifndef __INFO__
#define __INFO__

#include <ESP8266WebServer.h>
#include <NTPClient.h>

#include "../../include/sensors.h"

void attachSensorRoutes(ESP8266WebServer *server, NTPClient *timeClient, Sensor** sensors);

#endif
