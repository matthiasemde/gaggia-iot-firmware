#include "../include/actors.h"

//// Class BinaryActor ////

// Constructor
BinaryActor::BinaryActor(uint8_t controlPin, uint8_t initialState) {
    this->controlPin = controlPin;
    this->state = initialState;

    digitalWrite(this->controlPin, initialState);
    pinMode(this->controlPin, OUTPUT);
}

// Accessors
uint8_t BinaryActor::getState() {
    return this->state;
}

// Mutators
void BinaryActor::setState(uint8_t newState) {
    this->state = newState;
    digitalWrite(this->controlPin, newState);
}

//// Class PwmActor ////
uint8_t ledChannel = 0;

// Constructor
PwmActor::PwmActor(uint8_t controlPin, uint32_t pwmFrequency, int pwmResolution) {
    this->ledChannel = ledChannel;
    this->controlPin = controlPin;
    this->pwmResolution = pwmResolution;

    ledcSetup(this->ledChannel, pwmFrequency, this->pwmResolution);
    ledcAttachPin(this->controlPin, this->ledChannel);

    ledChannel++;
}

// Accessors
// float PwmActor::getPowerLevel() {
//     return this->pwmLevel
// }

// Mutators
void PwmActor::setPowerLevel(float newPowerLevel) {
    // convert from float (0.0 - 1.0) to int (0 - pwmResolution)
    ledcWrite(
        this->controlPin,
        (int) round(constrain(newPowerLevel, 0.0, 1.0) * (this->pwmResolution-1))
    );
}