#ifndef __IO_H__
#define __IO_H__

#include <Arduino.h>

#include "../config.h"

typedef union {
    uint8_t value;
    struct {
        uint8_t power: 1;
        uint8_t pump : 1;
        uint8_t steam : 1;
    };
} buttonState_t;

namespace IO {
    void init();
    void sayHello();
    buttonState_t getButtonState();
    void clearPowerButton();
    String status();
}

#endif