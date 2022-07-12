#include "../../../include/modules/control/sensors.h"

//// Class Sensor ////

// Constructor
Sensor::Sensor(String name, float smoothingCoefficient) {
    this->displayName = name;
    
    this->rawValue = 0;
    this->smoothedValue = 0;
    this->smoothingCoefficient = smoothingCoefficient;
}

// Accessors
float Sensor::getRawValue() {
    return rawValue;
}

float Sensor::getSmoothedValue() {
    return smoothedValue;
}

// Mutators
void Sensor::setRawValue(float newValue) {
    rawValue = newValue;

    // implement running average
    smoothedValue =
        smoothedValue * smoothingCoefficient +
        newValue * (1-smoothingCoefficient);
}


//// Class TemperaturSensor ////

// Constructor
TemperatureSensor::TemperatureSensor(uint8_t csPin, float rRef, float smoothingCoefficient) 
: Sensor("Temperature", smoothingCoefficient) {
    this->rRef = rRef;
    this->maxBoard = new Adafruit_MAX31865(
        csPin,
        SPI_DI_PIN,
        SPI_DO_PIN,
        SPI_CLK_PIN
    );
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
    status += "Smoothed value: " + String(getSmoothedValue()) + "\nRaw value: " + String(getRawValue()) + "\n";

    return status;
}


//// Class PressureSensor ////

// Constructor
PressureSensor::PressureSensor(uint8_t inputPin, float smoothingCoefficient)
: Sensor("Pressure", smoothingCoefficient) {
    this->inputPin = inputPin;
    this->slope = PRESSURE_SENSOR_SLOPE;
    this->offset = PRESSURE_SENSOR_OFFSET; 
    pinMode(inputPin, INPUT);
}

void PressureSensor::update() {
    setRawValue(float(analogRead(inputPin)) * slope + offset);
}

String PressureSensor::status() {
    return "Smoothed value: " + String(getSmoothedValue()) + "\nRaw value: " + String(getRawValue()) + "\n";
}