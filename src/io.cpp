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
    return
        !(digitalRead(PUMP_BUTTON)) * PUMP_BUTTON_MASK | 
        !(digitalRead(STEAM_BUTTON)) * STEAM_BUTTON_MASK;
}

String IO::status() {
    String status = "";
    uint8_t buttonState = getButtonState();
    status += (String) "Button state:\nPump: " +
        ((buttonState & PUMP_BUTTON_MASK) ? "on" : "off") +
        "\nSteam: " +
        ((buttonState & STEAM_BUTTON_MASK) ? "on" : "off");
    return status;
}