#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebServer.h>

#include "../storage.h"
#include "../control.h"

namespace ConfigurationRouter {
    void attach(WebServer* server);
};

#endif