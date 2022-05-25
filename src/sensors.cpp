#include "../include/sensors.h"

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
    return this->rawValue;
}

float Sensor::getSmoothedValue() {
    return this->smoothedValue;
}

// Mutators
void Sensor::setRawValue(float newValue) {
    this->rawValue = newValue;

    // implement running average
    this->smoothedValue =
        this->smoothedValue * this->smoothingCoefficient +
        newValue * (1-this->smoothingCoefficient);
}


//// Class TemperaturSensor ////

// Constructor
TemperatureSensor::TemperatureSensor(uint8_t csPin, float rRef, float smoothingCoefficient) 
: Sensor("Temperature", smoothingCoefficient) {
    this->maxBoard = new Adafruit_MAX31865(
        csPin,
        SPI_DI_PIN,
        SPI_DO_PIN,
        SPI_CLK_PIN
    );
    this->maxBoard->begin(MAX31865_3WIRE);
}

void TemperatureSensor::update() {
    this->setRawValue(this->maxBoard->temperature(100, this->rRef));
    // Serial.println(this->maxBoard->temperature(100, this->rRef));
}

String TemperatureSensor::status() {
    String status = "";
    // Check and print any faults
    uint8_t fault = this->maxBoard->readFault();
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
        this->maxBoard->clearFault();
    }
    status += "Smoothed value: " + String(this->getSmoothedValue()) + "\nRaw value: " + String(this->getRawValue()) + "\n";

    return status;
}


//// Class PressureSensor ////

// Constructor
PressureSensor::PressureSensor(uint8_t inputPin, float smoothingCoefficient)
: Sensor("Pressure", smoothingCoefficient) {
    this->inputPin = inputPin;
    pinMode(this->inputPin, INPUT);
}

void PressureSensor::update() {
    this->setRawValue(analogRead(this->inputPin));
}

String PressureSensor::status() {
    return "TODO Pressure Sensor\n";
}