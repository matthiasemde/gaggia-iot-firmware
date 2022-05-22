#ifndef __SENSORS__
#define __SENSORS__

#include "config.h"
#include <Adafruit_MAX31865.h>

#include "pid.h" 

#define NUM_SENSORS 1

#define TEMP_IDX 0

#define TEMP_PMW_FREQ 100

#define TEMP_RREF 430.0


class Sensor {
private:
    float value;
    float controlTarget;
    float controlValue;
public:
    String displayName;
    Sensor(String name);
    float getValue();
    float getControlTarget();
    void setValue(float newValue);
    void setControlTarget(float newTarget);
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