#ifndef __SENSORS_H__
#define __SENSORS_H__

#include "config.h"
#include <Adafruit_MAX31865.h>

#include "pid.h" 

class Sensor {
private:
    float rawValue;
    float smoothedValue;
    float smoothingCoefficient;
public:
    String displayName;
    
    // Constructor
    Sensor(String name, float smoothingCoefficient);

    // Accessors
    float getRawValue();
    float getSmoothedValue();
 
    // Mutators
    void setRawValue(float newValue);

    // Virtual functions to be implemented by derived classes 
    virtual void update() = 0;
    virtual String status() = 0;
};

class TemperatureSensor : public Sensor {

private:
    Adafruit_MAX31865* maxBoard;
    float rRef;
public:
    TemperatureSensor(uint8_t csPin, float rRef, float smoothingCoefficient);
    void update();
    String status();
};

class PressureSensor : public Sensor {
private:
    uint8_t inputPin;
public:
    PressureSensor(uint8_t inputPin, float smoothingCoefficient);
    void update();
    String status();
};

#endif