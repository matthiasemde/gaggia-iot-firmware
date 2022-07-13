#ifndef __SENSORS_H__
#define __SENSORS_H__

#include <Adafruit_MAX31865.h>

#include "../../config.h"
#include "pid.h" 

typedef enum {
    POLL,
    ISR
} updateMode_t;

class Sensor {
private:
    float smoothedValue = 0.0; // smoothed value for updating queue values. do not access from outside

    QueueHandle_t rawValueQueue, smoothedValueQueue;

    float smoothingCoefficient;
    
    updateMode_t updateMode;
    
    uint16_t pollDelay;
    
    SemaphoreHandle_t dataReady;
    gpio_num_t rdyPin;
    
    TaskHandle_t taskHandle = NULL;

    void init(String name, float smoothingCoefficient);
public:
    String displayName;
    
    // Constructor
    Sensor(String name, float smoothingCoefficient, uint16_t pollFrequency);
    Sensor(String name, float smoothingCoefficient, gpio_num_t rdyPin);

    // Accessors
    void getRawValue(float * rawValue);
    void getSmoothedValue(float * smoothedValue);
 
    // Mutators
    void setRawValue(float newValue);

    static void vTaskUpdate(void * params);

    // Virtual functions to be implemented by derived classes 
    virtual void update() = 0;
    virtual String status() = 0;
};

class TemperatureSensor : public Sensor {

private:
    Adafruit_MAX31865* maxBoard;
    float rRef;
    void init(gpio_num_t csPin, float rRef);
public:
    TemperatureSensor(gpio_num_t csPin, float rRef, float smoothingCoefficient, gpio_num_t rdyPin);
    TemperatureSensor(gpio_num_t csPin, float rRef, float smoothingCoefficient, uint16_t pollFrequency);
    void update();
    bool faultDetected();
    String status();
};

class PressureSensor : public Sensor {
private:
    gpio_num_t inputPin;
    float offset, slope; // slope and offset to convert analogRead to pressure in Bar
public:
    PressureSensor(gpio_num_t inputPin, float smoothingCoefficient, uint16_t pollFrequency);
    void update();
    String status();
};

#endif