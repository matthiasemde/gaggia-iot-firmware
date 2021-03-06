#ifndef __FSM_H__
#define __FSM_H__

#include <Arduino.h>

#include"../RTOSConfig.h"

#include "control/control.h"
#include "io.h"

typedef enum {
    UNINITIALIZED,
    IDLE,
    HEATING,
    // AT_BREW_TEMPERATURE,
    STEAMING,
    PREINFUSION,
    BREWING,
    SAFETY_OFF
} state_t;

namespace FSM {
    void init();
    state_t getState();
    void vTaskUpdate(void * parameters);
    String status();
}

#endif