#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <Arduino.h>

#include "config.h"

#include "sensors.h"
#include "actors.h"
#include "pid.h"

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

    // Mutators
    void setTemperatureTarget(float newTarget);
    void setPressureTarget(float newTarget);
    void setFlowTarget(float newTarget);

    void openSolenoid();
    void closeSolenoid();

    // Update function needs to be called each loop
    void update();

    String status();
};

#endif