#include "../include/io.h"

void IO::init() {
    digitalWrite(PIEZO, LOW);
    pinMode(PIEZO, OUTPUT);
}

void IO::sayHello() {
    tone(PIEZO, 1000, 1000);
}