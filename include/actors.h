#ifndef __ACTORS_H__
#define __ACTORS_H__

#include <Arduino.h>

#include "config.h"

enum class SolenoidState {
    OPEN = HIGH,
    CLOSED = LOW
};

class BinaryActor {
private:
    uint8_t state;
    uint8_t controlPin;
public:
    BinaryActor(uint8_t controlPin, uint8_t initialState);
    uint8_t getState();
    void setState(uint8_t newState);
};

class PwmActor {
private:
    uint8_t controlPin;
    uint8_t pwmChannel;
    uint32_t dutyCycle;
    uint16_t maxDutyCycle;
    bool active = false;
public:
    PwmActor(uint8_t controlPin, uint8_t pwmChannel, double pwmFrequency, uint8_t pwmResolution);
    void setPowerLevel(float newPwmLevel);
    void activate();
    void deactivate();
    String status();
};

#endif