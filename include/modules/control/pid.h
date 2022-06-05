#ifndef __PID_H__
#define __PID_H__

#include <Arduino.h>

typedef struct {
    float kp = -1.0, ki = -1.0, kd = -1.0;
} pidCoefs_t;

class PID {
private:
    float integral = 0; // integral over error
    
    float lastError = 0; // last error for calculating derivative

    uint32_t lastUpdate; // timestamp of last update
    uint16_t period; // period of controller updates in ms
    float dt; // period in seconds for calculating integral and derivative

    float controlTarget;

    float controlValue = 0; // current control value

    float kp = 1.0, ki = 1.0, kd = 1.0; // Coefsficients for P-, I- and D-term

    // margin for the conditional integral used to prevent integral windup:
    // https://en.wikipedia.org/wiki/Integral_windup
    float condIntegralMargin;
public:
    PID(
        float controlTarget,
        uint16_t period,
        float condIntegralMargin
    );
    float getControlValue();
    void setControlTarget(float newControlTarget);
    void setPIDCoefs(pidCoefs_t newCoefs);
    void reset();
    bool update(float input);
    String status();
};

#endif