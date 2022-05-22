#ifndef __INFO_H__
#define __INFO_H__

#include <ESP8266WebServer.h>
#include <NTPClient.h>

#include "../../include/sensors.h"

void attachSensorRoutes(ESP8266WebServer *server, NTPClient *timeClient, Sensor** sensors);

#endif
