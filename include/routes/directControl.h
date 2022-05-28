#ifndef __DIRECT_CONTROL_H__
#define __DIRECT_CONTROL_H__

#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <ArduinoJson.h>

#include "../control.h"

namespace DirectControlRouter {
    void attach(ESP8266WebServer *server, NTPClient *timeClient);
};

#endif
