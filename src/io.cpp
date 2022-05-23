#include "../include/io.h"

void IO::init() {
    digitalWrite(PIEZO, LOW);
    pinMode(PIEZO, OUTPUT);

    pinMode(PUMP_BUTTON, INPUT_PULLUP);
    pinMode(STEAM_BUTTON, INPUT_PULLUP);
}

void IO::sayHello() {
    tone(PIEZO, 1000, 1000);
}

uint8_t IO::getButtonState() {
    uint8_t buttonState = 0;
    buttonState =
        !(digitalRead(PUMP_BUTTON)) * PUMP_BUTTON_MASK | 
        !(digitalRead(STEAM_BUTTON)) * STEAM_BUTTON_MASK;
    return buttonState;
}