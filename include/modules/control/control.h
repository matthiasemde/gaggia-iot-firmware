#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <Arduino.h>

#include "sensors.h"
#include "actors.h"
#include "pid.h"

#include "../../RTOSConfig.h"
#include "../../config.h"

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
    void getRawTemperature(float * value);
    void getSmoothedTemperature(float * value);

    void getRawPressure(float * value);
    void getSmoothedPressure(float * value);

    configuration_t getActiveConfiguration();

    bool temperatureAnomalyDetected();

    // Mutators
    void turnOnHeater();
    void shutOffHeater();
    void setHeaterPower(float * powerLevel);
    void setPumpPower(float * powerLevel);

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

    String status();
};

#endif