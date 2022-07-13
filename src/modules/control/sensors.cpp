#include "../../../include/modules/control/sensors.h"

namespace {
    void IRAM_ATTR dataReadyISR(void * parameters) {
        TaskHandle_t* xHandle = (TaskHandle_t*)parameters;
        if(*xHandle != NULL) {
            BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
            vTaskNotifyGiveFromISR(*xHandle, &pxHigherPriorityTaskWoken);
            if(pxHigherPriorityTaskWoken == pdTRUE) {
               portYIELD_FROM_ISR();
            }
        }
    }
}

//// Class Sensor ////

// Constructor

Sensor::Sensor(String name, float smoothingCoefficient, uint16_t pollFrequency) {
    this->updateMode = POLL;
    this->pollDelay = 1000/pollFrequency;

    init(name, smoothingCoefficient);
}

Sensor::Sensor(String name, float smoothingCoefficient, gpio_num_t rdyPin) {
    this->updateMode = ISR;
    this->rdyPin = rdyPin;

    pinMode(rdyPin, INPUT);

    attachInterruptArg(rdyPin, dataReadyISR, (void*) &this->taskHandle, RISING);

    init(name, smoothingCoefficient);
}

void Sensor::init(String name, float smooothingCoefficient) {
    this->displayName = name;
    
    this->rawValueQueue = xQueueCreate(1, sizeof(float));
    this->smoothedValueQueue = xQueueCreate(1, sizeof(float));
    this->smoothingCoefficient = smoothingCoefficient;

    xTaskCreate(
        vTaskUpdate,
        "Sensor",
        SENSOR_TASK_STACK_SIZE,
        this,
        SENSOR_TASK_PRIORITY,
        &this->taskHandle
    );
}


// Accessors
void Sensor::getRawValue(float * rawValue) {
    xQueuePeek(rawValueQueue, rawValue, 10);
}

void Sensor::getSmoothedValue(float * smoothedValue) {
    xQueuePeek(smoothedValueQueue, smoothedValue, 10);
}

// Mutators
void Sensor::setRawValue(float newValue) {
    xQueueOverwrite(rawValueQueue, &newValue);

    // implement running average
    smoothedValue =
        smoothedValue * smoothingCoefficient +
        newValue * (1-smoothingCoefficient);

    xQueueOverwrite(smoothedValueQueue, &smoothedValue);
}

void Sensor::vTaskUpdate(void * params) {
    Sensor * sensor = (Sensor *) params;

    TickType_t lastWakeTime = xTaskGetTickCount();

    for (;;) {
        switch (sensor->updateMode) {
            case POLL:
                sensor->update();
                vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(sensor->pollDelay));
                break;
            case ISR:
                ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500));
                sensor->update();
                break;
            default:
                Serial.println("Invalid update mode"); throw std::__throw_runtime_error;
        }
    }
}

//// Class TemperaturSensor ////

// Constructor
TemperatureSensor::TemperatureSensor(gpio_num_t csPin, float rRef, float smoothingCoefficient, gpio_num_t rdyPin) 
: Sensor("Temperature", smoothingCoefficient, rdyPin) {
    init(csPin, rRef);
}

TemperatureSensor::TemperatureSensor(gpio_num_t csPin, float rRef, float smoothingCoefficient, uint16_t pollFrequency) 
: Sensor("Temperature", smoothingCoefficient, pollFrequency) {
    init(csPin, rRef);
}

void TemperatureSensor::init(gpio_num_t csPin, float rRef) {
    this->rRef = rRef;
    this->maxBoard = new Adafruit_MAX31865(
        csPin,
        SPI_DI_PIN,
        SPI_DO_PIN,
        SPI_CLK_PIN
    );
    this->maxBoard->enable50Hz(true);
    this->maxBoard->begin(MAX31865_3WIRE);
}

void TemperatureSensor::update() {
    setRawValue(maxBoard->temperature(100, rRef));
}

bool TemperatureSensor::faultDetected() {
    return (bool) maxBoard->readFault();
}

String TemperatureSensor::status() {
    String status = "";
    // Check and print any faults
    uint8_t fault = maxBoard->readFault();
    if (fault) {
        status = "Fault 0x";
        status += fault;
        if (fault & MAX31865_FAULT_HIGHTHRESH) {
            status += "\nRTD High Threshold\n"; 
        }
        if (fault & MAX31865_FAULT_LOWTHRESH) {
            status = "\nRTD Low Threshold\n"; 
        }
        if (fault & MAX31865_FAULT_REFINLOW) {
            status = "\nREFIN- > 0.85 x Bias\n"; 
        }
        if (fault & MAX31865_FAULT_REFINHIGH) {
            status = "\nREFIN- < 0.85 x Bias - FORCE- open\n"; 
        }
        if (fault & MAX31865_FAULT_RTDINLOW) {
            status = "\nRTDIN- < 0.85 x Bias - FORCE- open\n"; 
        }
        if (fault & MAX31865_FAULT_OVUV) {
            status = "\nUnder/Over voltage\n"; 
        }
        maxBoard->clearFault();
    }

    float rawValue, smoothedValue;
    getSmoothedValue(&smoothedValue);
    getRawValue(&rawValue);
    status += "Smoothed value: " + String(smoothedValue) + "\nRaw value: " + String(rawValue) + "\n";

    return status;
}


//// Class PressureSensor ////

// Constructor
PressureSensor::PressureSensor(gpio_num_t inputPin, float smoothingCoefficient, uint16_t pollFrequency)
: Sensor("Pressure", smoothingCoefficient, pollFrequency) {
    this->inputPin = inputPin;
    this->slope = PRESSURE_SENSOR_SLOPE;
    this->offset = PRESSURE_SENSOR_OFFSET; 
    pinMode(inputPin, INPUT);
}

void PressureSensor::update() {
    setRawValue(float(analogRead(inputPin)) * slope + offset);
}

String PressureSensor::status() {
    float rawValue, smoothedValue;
    getSmoothedValue(&smoothedValue);
    getRawValue(&rawValue);
    return "Smoothed value: " + String(smoothedValue) + "\nRaw value: " + String(rawValue) + "\n";
}