#ifndef __IO_H__
#define __IO_H__

#include <Arduino.h>

#include "config.h"

#define PUMP_BUTTON_MASK 0x01
#define STEAM_BUTTON_MASK 0x02

namespace IO {
    void init();
    void sayHello();
    uint8_t getButtonState();
    String status();
}

#endif