#ifndef __INDEX_ROUTER_H__
#define __INDEX_ROUTER_H__

#include <WebServer.h>

#include "../modules/io.h"

namespace IndexRouter {
    void sayHi(WebServer *server);
    void attach(WebServer *server);
};

#endif