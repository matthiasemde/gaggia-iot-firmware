#ifndef __DIRECT_CONTROL_H__
#define __DIRECT_CONTROL_H__

#include <WebServer.h>
#include <NTPClient.h>
#include <ArduinoJson.h>

#include "../control.h"

namespace DirectControlRouter {
    void attach(WebServer *server, NTPClient *timeClient);
};

#endif
