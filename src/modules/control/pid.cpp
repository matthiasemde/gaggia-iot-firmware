#include "../../../include/modules/control/pid.h"

PID::PID(float controlTarget, uint16_t period, float condIntegralMargin) {
    this->lastErrorQueue = xQueueCreate(1, sizeof(float));
    this->integralQueue = xQueueCreate(1, sizeof(float));

    this->controlTargetQueue = xQueueCreate(1, sizeof(float));
    this->inputQueue = xQueueCreate(1, sizeof(float));
    this->controlValueQueue = xQueueCreate(1, sizeof(float));

    this->pidCoefsQueue = xQueueCreate(1, sizeof(pidCoefs_t));

    this->resetSemaphore = xSemaphoreCreateBinary();

    this->period = period;
    this->dt = (float)period / 1000;

    this->condIntegralMargin = condIntegralMargin;

    xTaskCreate(
        vTaskUpdate,
        "PID",
        configMINIMAL_STACK_SIZE,
        this,
        PID_TASK_PRIORITY,
        &this->taskHandle
    );
}


// Accessors

float PID::getControlTarget() {
    float controlTarget;
    return xQueuePeek(controlTargetQueue, &controlTarget, 0) == pdTRUE ? controlTarget : -1;
}

float PID::getControlValue() {
    float controlValue;
    return xQueuePeek(controlValueQueue, &controlValue, 0) == pdTRUE ? controlValue : -1;
}


// Mutators

void PID::setPIDCoefs(pidCoefs_t newCoefs) {
    if(newCoefs.kp >= 0 && newCoefs.ki >= 0 && newCoefs.kd >= 0) {
        xQueueOverwrite(pidCoefsQueue, &newCoefs);
    }
}

void PID::setControlTarget(float newControlTarget) {
    xQueueOverwrite(controlTargetQueue, &newControlTarget);
}

void PID::setInput(float newInput) {
    xQueueOverwrite(inputQueue, &newInput);
}


// Update Task

void PID::vTaskUpdate(void * parameters) {
    PID * pid = (PID *)parameters;
    float controlTarget, input, controlValue;
    float error, lastError, derivative, integral;
    pidCoefs_t pidCoefs = {0.0f, 0.0f, 0.0f};

    controlTarget = input = controlValue = 0.0;
    error = derivative = integral = 0.0;

    for ( ;; ) {
        if(xSemaphoreTake(pid->resetSemaphore, 0) == pdTRUE) {
            integral = 0;
            lastError = 0;
        }

        xQueuePeek(pid->controlTargetQueue, &controlTarget, 0);
        xQueuePeek(pid->inputQueue, &input, 0);

        error = controlTarget - input;

        if(abs(error) < pid->condIntegralMargin) {
            integral += error * pid->dt;
        } else {
            integral = 0;
        }
        
        derivative = (error - lastError) * pid->dt;

        xQueuePeek(pid->pidCoefsQueue, &pidCoefs, 0);

        controlValue = 
            pidCoefs.kp * error + 
            pidCoefs.ki * integral +
            pidCoefs.kd * derivative;

        xQueueOverwrite(pid->controlValueQueue, &controlValue);


        lastError = error;

        xQueueOverwrite(pid->lastErrorQueue, &lastError);
        xQueueOverwrite(pid->integralQueue, &integral);

        vTaskDelay(pdMS_TO_TICKS(pid->period));
    }
};

void PID::reset() {
    xSemaphoreGive(resetSemaphore);
}

String PID::status() {
    float controlTarget, input, controlValue, lastError, integral;
    pidCoefs_t pidCoefs;

    xQueuePeek(controlTargetQueue, &controlTarget, pdMS_TO_TICKS(10));
    xQueuePeek(pidCoefsQueue, &pidCoefs, pdMS_TO_TICKS(10));
    xQueuePeek(inputQueue, &input, pdMS_TO_TICKS(10));
    xQueuePeek(integralQueue, &integral, pdMS_TO_TICKS(10));
    xQueuePeek(controlValueQueue, &controlValue, pdMS_TO_TICKS(10));

    String status = "Control Target:\t " + String(controlTarget) +
        "\nCoefficients:\t (p " + String(pidCoefs.kp) + ", i " + String(pidCoefs.ki) + ", d " + String(pidCoefs.kd) + ")" +
        "\nLast Error:\t\t " + String(lastError) +
        "\nIntegral:\t\t " + String(integral) +
        "\nControl Value:\t " + String(controlValue) + "\n";
    return status;
}