#include "../include/pid.h"

PID::PID(
    float kp,
    float ki,
    float kd,
    float target,
    uint16_t period
) {
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;    
    this->target = target;

    this->period = period;
    this->dt = (float)period / 1000;

    this->lastUpdate = millis();
}

void PID::setTarget(float newTarget) {
    this->target = newTarget;
}

void PID::update() {
    uint32_t now = millis();
    if (this->lastUpdate + this->period > now) {
        this->lastUpdate = now;
        
        float error = this->target - *(this->input);

        this->integral += error * this->dt;
        
        float derivative = (error - this->lastError) * this->dt;

        this->action = 
            this->kp * error + 
            this->ki * this->integral +
            this->kd * derivative;

        this->lastError = error;
    }
};

float PID::getAction() {
    return this->action;
}