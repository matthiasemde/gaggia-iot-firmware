#ifndef __SENSORS__
#define __SENSORS__

#include "config.h"
#include <Adafruit_MAX31865.h>

#include "pid.h" 

class Sensor {
private:
    float value;
    uint16_t controlTarget;
    uint16_t maxTarget;
    float controlValue;
public:
    String displayName;
    Sensor(String name, uint16_t maxTarget);
    float getValue();
    uint16_t getControlTarget();
    void setValue(float newValue);
    void setControlTarget(uint16_t newTarget);
    void setControlValue(float newControlValue);
    virtual void updateValue() = 0;
    virtual void updateController() = 0;
    virtual String status() = 0;
};

class TemperatureSensor : public Sensor {
private:
    Adafruit_MAX31865* maxBoard;
    PID* controller;
public:
    TemperatureSensor();
    void updateValue();
    void updateController();
    String status();
};

#endif