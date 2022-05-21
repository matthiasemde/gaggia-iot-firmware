#ifndef __SENSORS__
#define __SENSORS__

#include <Adafruit_MAX31865.h>

#define NUM_SENSORS 1

#define TEMP_IDX 0
#define TEMP_SPI_CS D0
#define TEMP_SPI_DI D5
#define TEMP_SPI_DO D6
#define TEMP_SPI_CLK D7
#define TEMP_RREF 430.0


class Sensor {
private:
    float_t value;
    float_t controlTarget;
public:
    String displayName;
    Sensor(String name);
    float_t getValue();
    float_t getControlTarget();
    void setValue(float_t newValue);
    void setControlTarget(float_t newTarget);
    virtual void updateValue() = 0;
    virtual void updateControler() = 0;
    virtual String status() = 0;
};

class TemperatureSensor : public Sensor {
private:
    Adafruit_MAX31865* maxBoard;
public:
    TemperatureSensor();
    void updateValue();
    void updateControler();
    String status();
};

#endif