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
        PID_TASK_STACK_SIZE,
        this,
        PID_TASK_PRIORITY,
        &this->taskHandle
    );
}


// Accessors

void PID::getControlTarget(float * controlTarget) {
    if (xQueuePeek(controlTargetQueue, controlTarget, 0) == pdFALSE) *controlTarget = 0.0;
}

void PID::getControlValue(float * controlValue) {
    if(xQueuePeek(controlValueQueue, controlValue, 0) == pdFALSE) *controlValue = 0.0;
}


// Mutators

void PID::setPIDCoefs(pidCoefs_t newCoefs) {
    if(newCoefs.kp >= 0 && newCoefs.ki >= 0 && newCoefs.kd >= 0) {
        xQueueOverwrite(pidCoefsQueue, &newCoefs);
    }
}

void PID::setControlTarget(float newControlTarget) {
    xQueueOverwrite(controlTargetQueue, &newControlTarget);
    resetIntegral();
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

    input = controlValue = 0.0;
    error = derivative = integral = 0.0;

    TickType_t lastWakeTime = xTaskGetTickCount();

    for ( ;; ) {
        if(xSemaphoreTake(pid->resetSemaphore, 0) == pdTRUE) {
            integral = 0;
            lastError = 0;
        }

        if(xQueuePeek(pid->controlTargetQueue, &controlTarget, 0) == pdTRUE) {
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
        }

        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(pid->period));
    }
};

void PID::resetIntegral() {
    xSemaphoreGive(resetSemaphore);
}

String PID::status() {
    float controlTarget, controlValue, lastError, integral;
    pidCoefs_t pidCoefs;

    String status = "Remaining stack size: " + String(uxTaskGetStackHighWaterMark(taskHandle)) +
        "\nControl Target:\t" + (xQueuePeek(controlTargetQueue, &controlTarget, 10) == pdTRUE ? String(controlTarget) : "Queue empty!") +
        "\nCoefficients:\t" +  (xQueuePeek(pidCoefsQueue, &pidCoefs, 10) == pdTRUE ? "(p " + String(pidCoefs.kp) + ", i " + String(pidCoefs.ki) + ", d " + String(pidCoefs.kd) + ")" : "Queue empty!") +
        "\nError:\t\t\t" + (xQueuePeek(lastErrorQueue, &lastError, 10) == pdTRUE ? String(lastError) : "Queue empty!") +
        "\nIntegral:\t\t" + (xQueuePeek(integralQueue, &integral, 10) == pdTRUE ? String(integral) : "Queue empty!") +
        "\nControl Value:\t" + (xQueuePeek(controlValueQueue, &controlValue, 10) == pdTRUE ? String(controlValue) : "Queue empty!") + "\n";
    return status;
}