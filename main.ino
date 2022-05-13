#include <Adafruit_ESP8266.h>

void setup() {
    Serial.begin(115200);
    Serial.println("Hello World!");
}

void loop() {
    delay(1000);
    Serial.println("yes");
}