#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <Arduino.h>

#include "config.h"

#include "sensors.h"
#include "actors.h"
#include "pid.h"

typedef struct _Temperatures {
    float brew;
    float steam;
} Temperatures;

typedef struct _Pressures {
    float brew;
    float preinfusion;
} Pressures;

typedef struct _Configuration {
    Temperatures temps;
    Pressures pressures;
    uint16_t preinfusionTime;
} Configuration;
    

enum class PumpControlMode {
    PRESSURE,
    FLOW
};

namespace Control {
    // Accessors
    float getRawTemperature();
    float getSmoothedTemperature();

    float getRawPressure();
    float getSmoothedPressure();

    Configuration getActiveConfiguration();

    // Mutators
    void setTemperatureTarget(float newTarget);
    void setPressureTarget(float newTarget);
    void setFlowTarget(float newTarget);

    void setBrewTemperature(float newTarget);
    void setSteamTemperature(float newTarget);

    void openSolenoid();
    void closeSolenoid();

    // Update function needs to be called each loop
    void update();

    String status();
};

#endif