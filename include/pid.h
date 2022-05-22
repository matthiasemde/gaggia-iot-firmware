#ifndef __PID_H__
#define __PID_H__

class PID {
private:
    float integral = 0; // integral over error
    float lastError = 0; // last error for calculating derivative
    uint32_t lastUpdate; // timestamp of last update
    uint16_t period; // period of controller updates in ms
    float dt; // period in seconds for calculating integral and derivative

    // float (*getInput)(); // address of controller input 
    float controlTarget;

    float kp, ki, kd; // koefficients for P-, I- and D-term
public:
    PID(
        float kp,
        float ki,
        float kd,
        float controlTarget,
        uint16_t period
    );
    void setControlTarget(float newControlTarget);
    bool update(float input, float* nextControlValue);
};

#endif