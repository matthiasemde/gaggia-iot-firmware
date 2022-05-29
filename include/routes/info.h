#ifndef __INFO_H__
#define __INFO_H__

#include <WebServer.h>
#include <NTPClient.h>
#include <ArduinoJson.h>

#include "../../include/control.h"
#include "../../include/storage.h"

namespace InfoRouter {
    void attach(WebServer *server, NTPClient *timeClient);
};

#endif
