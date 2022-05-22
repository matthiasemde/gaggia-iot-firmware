#include "../include/io.h"

void init() {
    digitalWrite(PIEZO, LOW);
    pinMode(PIEZO, OUTPUT);
}

void sayHello() {
    tone(PIEZO, 1000, 1000);
}