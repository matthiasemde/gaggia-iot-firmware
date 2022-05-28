#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>

#include "../storage.h"
#include "../control.h"

namespace ConfigurationRouter {
    void attach(ESP8266WebServer* server);
};

#endif