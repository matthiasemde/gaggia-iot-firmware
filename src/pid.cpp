#include "../include/pid.h"

PID::PID(
    float kp,
    float ki,
    float kd,
    float controlTarget,
    uint16_t period
) {
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;

    this->controlTarget = controlTarget;

    this->period = period;
    this->dt = (float)period / 1000;

    this->lastUpdate = millis();
}

void PID::setControlTarget(float newControlTarget) {
    this->controlTarget = newControlTarget;
}

bool PID::update(float input, float* nextControlValue) {
    uint32_t now = millis();
    if (this->lastUpdate + this->period > now) {
        this->lastUpdate = now;
        
        float error = this->controlTarget - input;

        this->integral += error * this->dt;
        
        float derivative = (error - this->lastError) * this->dt;

        *nextControlValue = 
            this->kp * error + 
            this->ki * this->integral +
            this->kd * derivative;

        this->lastError = error;

        return true;
    }
    return false;
};