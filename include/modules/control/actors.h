#ifndef __ACTORS_H__
#define __ACTORS_H__

#include <Arduino.h>

#include "soc/mcpwm_struct.h"
#include "soc/mcpwm_reg.h"
#include "driver/mcpwm.h"
#include "../../config.h"

enum BinaryActorState {
    ACTIVE = 1,
    INACTIVE = 0
};

class BinaryActor {
private:
    BinaryActorState state;
    gpio_num_t controlPin;
    bool inverted;
    SemaphoreHandle_t mutex;
public:
    BinaryActor(gpio_num_t controlPin, bool inverted = false, BinaryActorState initialState = INACTIVE);
    uint8_t getState();
    void activate();
    void deactivate();
};

class PwmActor {
private:
    gpio_num_t controlPin;
    mcpwm_dev_t* pwmDevice;
    uint8_t prescale;
    mcpwm_timer_t pwmTimer;
    uint32_t dutyCycle;
    uint16_t maxDutyCycle;
    bool active = false;
    float minOut, maxOut;
public:
    PwmActor(
        gpio_num_t controlPin,
        uint8_t pwmChannel,
        double pwmFrequency,
        float minOut = 0.0,
        float maxOut = 1.0,
        bool inverted = false
    );
    void setPowerLevel(float newPwmLevel);
    void activate();
    void deactivate();
    String status();
};

#endif