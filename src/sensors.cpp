#include "../include/sensors.h"

Sensor::Sensor(String name, uint16_t maxTarget) {
    this->displayName = name;
    this->maxTarget = maxTarget;
}

float Sensor::getValue() {
    return this->value;
}

uint16_t Sensor::getControlTarget() {
    return this->controlTarget;
}

void Sensor::setValue(float newValue) {
    this->value = newValue;
}

void Sensor::setControlTarget(uint16_t newTarget) {
    this->controlTarget = (newTarget < this->maxTarget) ? newTarget : this->maxTarget;
}

TemperatureSensor::TemperatureSensor() 
: Sensor("Temperature", MAX_TEMP) {
    this->setControlTarget(0);
    this->maxBoard = new Adafruit_MAX31865(
        TEMP_SPI_CS,
        TEMP_SPI_DI,
        TEMP_SPI_DO,
        TEMP_SPI_CLK
    );
    this->maxBoard->begin(MAX31865_3WIRE);

    this->controller = new PID(kp, ki, kd, 0, 100);

    analogWrite(TEMP_CTRL, 0);
    analogWriteFreq(TEMP_PMW_FREQ);
    pinMode(TEMP_CTRL, OUTPUT);
}

void TemperatureSensor::updateValue() {
    this->setValue(this->maxBoard->temperature(100, TEMP_RREF));
}

void TemperatureSensor::updateController() {
    float nextControlValue;
    if (this->controller->update(this->getValue(), &nextControlValue)) {
        this->setControlValue(nextControlValue);
        analogWrite(TEMP_CTRL, (int) constrain((nextControlValue * 1023), 0, 1023));
    }
}

String TemperatureSensor::status() {
    String status = "";
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
    status += "\nValue: " + String(this->getValue()) + "\nControlTarget: " + String(this->getControlTarget()) + "\n";

    return status;
}