#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebServer.h>

#include "../modules/storage.h"
#include "../modules/control/control.h"

namespace ConfigurationRouter {
    void attach(WebServer* server);
};

#endif