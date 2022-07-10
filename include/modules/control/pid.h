#ifndef __PID_H__
#define __PID_H__

#include <Arduino.h>
#include "../../RTOSConfig.h"

typedef struct {
    float kp, ki, kd;
} pidCoefs_t;

class PID {
private:
    uint16_t period; // period of controller updates in ms
    float dt; // period in seconds for calculating integral and derivative

    TaskHandle_t taskHandle; // handle to task

    QueueHandle_t lastErrorQueue;
    QueueHandle_t integralQueue;

    QueueHandle_t controlTargetQueue; // queue for passing control target values
    QueueHandle_t inputQueue; // queue for passing input values
    QueueHandle_t controlValueQueue; // queue for passing control value values

    QueueHandle_t pidCoefsQueue; // queue holding coeficients for P-, I- and D-term

    SemaphoreHandle_t resetSemaphore; // semaphore for resetting integral

    // margin for the conditional integral used to prevent integral windup:
    // https://en.wikipedia.org/wiki/Integral_windup
    float condIntegralMargin;
public:
    PID(
        float controlTarget,
        uint16_t period,
        float condIntegralMargin
    );
    void getControlTarget(float * controlTarget);
    void getControlValue(float * controlValue);
    void setPIDCoefs(pidCoefs_t newCoefs);
    void setControlTarget(float newControlTarget);
    void setInput(float newInput);
    void reset();
    static void vTaskUpdate(void * params);
    String status();
};

#endif