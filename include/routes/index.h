#ifndef __INDEX_ROUTER_H__
#define __INDEX_ROUTER_H__

#include <ESP8266WebServer.h>

#include "../../include/io.h"

namespace IndexRouter {
    void sayHi(ESP8266WebServer *server);
    void attach(ESP8266WebServer *server);
};

#endif