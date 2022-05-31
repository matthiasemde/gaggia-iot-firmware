#include "../include/actors.h"

//// Class BinaryActor ////

// Constructor
BinaryActor::BinaryActor(uint8_t controlPin, uint8_t initialState) {
    this->controlPin = controlPin;
    this->state = initialState;

    digitalWrite(controlPin, initialState);
    pinMode(controlPin, OUTPUT);
}

// Accessors
uint8_t BinaryActor::getState() {
    return state;
}

// Mutators
void BinaryActor::setState(uint8_t newState) {
    state = newState;
    digitalWrite(controlPin, newState);
}

//// Class PwmActor ////

// Constructor
PwmActor::PwmActor(uint8_t controlPin, uint8_t pwmChannel ,double pwmFrequency, uint8_t pwmResolution) {
    this->controlPin = controlPin;
    this->pwmChannel = pwmChannel;
    this->maxDutyCycle = pow(2, pwmResolution) - 1;

    digitalWrite(controlPin, LOW);

    ledcSetup(pwmChannel, pwmFrequency, pwmResolution);
}

// Accessors

// Mutators
void PwmActor::setPowerLevel(float newPowerLevel) {
    // convert from float (0.0 - 1.0) to int (0 - maxDutyCycle)
    dutyCycle = (uint32_t) round(constrain(newPowerLevel, 0.0, 1.0) * maxDutyCycle);
    ledcWrite(pwmChannel, dutyCycle);
}

void PwmActor::activate() {
    if(!active) {
        ledcAttachPin(controlPin, pwmChannel);
        active = true;
    }
}

void PwmActor::deactivate() {
    if(active) {
        ledcDetachPin(controlPin);
        active = false;
    }
}

String PwmActor::status() {
    return "Duty cycle: " + String(dutyCycle) + " / " + String(maxDutyCycle) + "\n";
}