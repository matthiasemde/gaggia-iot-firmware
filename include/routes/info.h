#ifndef __INFO_H__
#define __INFO_H__

#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <ArduinoJson.h>

#include "../../include/control.h"

namespace SensorRouter {
    void attach(ESP8266WebServer *server, NTPClient *timeClient);
};

#endif
