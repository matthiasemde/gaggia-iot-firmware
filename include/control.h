#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <Arduino.h>

#include "config.h"

#include "sensors.h"
#include "actors.h"
#include "pid.h"

typedef struct {
    float brew, steam;
} temperatures_t;

typedef struct {
    float brew, preinfusion;
} pressures_t;

typedef struct {
    temperatures_t temps;
    pidCoefs_t temperaturePIDCoefs;
    pressures_t pressures;
    pidCoefs_t pressurePIDCoefs;
    uint16_t preinfusionTime;
} configuration_t;
    

enum class PumpControlMode {
    PRESSURE,
    FLOW
};

namespace Control {
    void init();

    // Accessors
    float getRawTemperature();
    float getSmoothedTemperature();

    float getRawPressure();
    float getSmoothedPressure();

    configuration_t getActiveConfiguration();

    // Mutators
    void setTemperatureTarget(float newTarget);
    void setPressureTarget(float newTarget);
    void setFlowTarget(float newTarget);

    void setBrewTemperature(float newValue);
    void setSteamTemperature(float newValue);

    void setBrewPressure(float newValue);
    void setPreinfusionPressure(float newValue);

    void setPreinfusionTime(uint16_t newValue);

    void setTemperaturePIDCoefs(pidCoefs_t newCoefs);
    void setPressurePIDCoefs(pidCoefs_t newCoefs);

    void openSolenoid();
    void closeSolenoid();

    // Update function needs to be called each loop
    void update();

    String status();
};

#endif