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

// Constructor
PwmActor::PwmActor(uint8_t controlPin, uint32_t pwmFrequency, int pwmResolution) {
    this->controlPin = controlPin;
    this->pwmResolution = pwmResolution;

    analogWrite(this->controlPin, 0);
    analogWriteFreq(pwmFrequency);
    analogWriteResolution(this->pwmResolution);
    pinMode(this->controlPin, OUTPUT);
}

// Accessors
// float PwmActor::getPowerLevel() {
//     return this->pwmLevel
// }

// Mutators
void PwmActor::setPowerLevel(float newPowerLevel) {
    // convert from float (0.0 - 1.0) to int (0 - pwmResolution)
    analogWrite(
        this->controlPin,
        (int) round(constrain(newPowerLevel, 0.0, 1.0) * (this->pwmResolution-1))
    );
}