#include "../include/sensors.h"

Sensor::Sensor(String name) {
    this->displayName = name;
}

float_t Sensor::getValue() {
    return this->value;
}

float_t Sensor::getControlTarget() {
    return this->controlTarget;
}

void Sensor::setValue(float_t newValue) {
    this->value = newValue;
}

void Sensor::setControlTarget(float_t newTarget) {
    this->controlTarget = newTarget;
}

TemperatureSensor::TemperatureSensor() : Sensor("Temperature") {
    this->setControlTarget(0);
    this->maxBoard = new Adafruit_MAX31865(
        TEMP_SPI_CS,
        TEMP_SPI_DI,
        TEMP_SPI_DO,
        TEMP_SPI_CLK
    );
    this->maxBoard->begin(MAX31865_3WIRE);

    this->controller = new PID(kp, ki, kd, 0, 100);
}

void TemperatureSensor::updateValue() {
    this->setValue(this->maxBoard->temperature(100, TEMP_RREF));
}

void TemperatureSensor::updateController() {
    this->controller->update();
}

String TemperatureSensor::status() {
    String status = "Ok";
    // Check and print any faults
    uint8_t fault = this->maxBoard->readFault();
    if (fault) {
        status = "Fault 0x";
        status += fault;
        if (fault & MAX31865_FAULT_HIGHTHRESH) {
            status += "\nRTD High Threshold"; 
        }
        if (fault & MAX31865_FAULT_LOWTHRESH) {
            status = "\nRTD Low Threshold"; 
        }
        if (fault & MAX31865_FAULT_REFINLOW) {
            status = "\nREFIN- > 0.85 x Bias"; 
        }
        if (fault & MAX31865_FAULT_REFINHIGH) {
            status = "\nREFIN- < 0.85 x Bias - FORCE- open"; 
        }
        if (fault & MAX31865_FAULT_RTDINLOW) {
            status = "\nRTDIN- < 0.85 x Bias - FORCE- open"; 
        }
        if (fault & MAX31865_FAULT_OVUV) {
            status = "\nUnder/Over voltage"; 
        }
        this->maxBoard->clearFault();
    }
    return status;
}