#ifndef __PID__
#define __PID__

class PID {
private:
    float integral = 0; // integral over error
    float lastError = 0; // last error for calculating derivative
    uint32_t lastUpdate; // timestamp of last update
    uint16_t period; // period of controller updates in ms
    float dt; // period in seconds for calculating integral and derivative

    float* input; // address of controller input 
    float target;
    float action; // action which should be taken according to PID

    float kp, ki, kd; // koefficients for P-, I- and D-term
public:
    PID(float kp, float ki, float kd, float target, uint16_t period);
    void setTarget(float newTarget);
    void update();
    float getAction();
};

#endif