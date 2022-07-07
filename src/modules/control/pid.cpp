#include "../../../include/modules/control/pid.h"

PID::PID(float controlTarget, uint16_t period, float condIntegralMargin) {
    this->controlTarget = controlTarget;

    this->period = period;
    this->dt = (float)period / 1000;

    this->condIntegralMargin = condIntegralMargin;

    this->lastUpdate = millis();
}

float PID::getControlTarget() {
    return controlTarget;
}

float PID::getControlValue() {
    return controlValue;
}

void PID::setPIDCoefs(pidCoefs_t newCoefs) {
    if (newCoefs.kp >= 0) kp = newCoefs.kp;
    if (newCoefs.ki >= 0) ki = newCoefs.ki;
    if (newCoefs.kd >= 0) kd = newCoefs.kd;
}

void PID::setControlTarget(float newControlTarget) {
    controlTarget = newControlTarget;
}

bool PID::update(float input) {
    uint32_t now = millis();
    if (lastUpdate + period < now) {
        lastUpdate = now;
        
        float error = controlTarget - input;

        if(abs(error) < condIntegralMargin) {
            integral += error * dt;
        } else {
            integral = 0;
        }
        
        float derivative = (error - lastError) * dt;

        controlValue = 
            kp * error + 
            ki * integral +
            kd * derivative;

        lastError = error;

        return true;
    }
    return false;
};

void PID::reset() {
    integral = 0;
    lastError = 0;
}

String PID::status() {
    String status = "Control Target:\t " + String(controlTarget) +
        "\nCoefficients:\t (p " + String(kp) + ", i " + String(ki) + ", d " + String(kd) + ")" +
        "\nLast Error:\t\t " + String(lastError) +
        "\nIntegral:\t\t " + String(integral) +
        "\nControl Value:\t " + String(controlValue) + "\n";
    return status;
}