#include "../../include/modules/io.h"

namespace {
    uint8_t tuneIndex;

    typedef struct {
        bool pause;
        uint16_t duration;
        note_t note;
        uint8_t octave;
    } beat_t;

    const beat_t tune[] = {
        {false, 2, NOTE_C, 4},
        {false, 2, NOTE_E, 4},
        {false, 2, NOTE_E, 4},
        {false, 2, NOTE_G, 4},
        {false, 2, NOTE_A, 4},
        {false, 2, NOTE_G, 4},
        {false, 4, NOTE_G, 4},
        {false, 2, NOTE_C, 5},
        {false, 2, NOTE_G, 4},
        {false, 2, NOTE_G, 4},
        {false, 2, NOTE_E, 4},
        {false, 2, NOTE_E, 4},
        {false, 1, NOTE_D, 4},
        {false, 1, NOTE_C, 4},
        {false, 4, NOTE_D, 4},
        // {false, 2, NOTE_F, 4},
        // {false, 2, NOTE_F, 4},
    };

    esp_timer_handle_t tuneTimerHandle;
    esp_timer_cb_t tuneTimerCallback = [](void* arg) {
        if (tuneIndex < sizeof(tune) / sizeof(beat_t)) {
            if(tune[tuneIndex].pause) {
                ledcWrite(PIEZO_CHANNEL, 0);
            } else {
                ledcWriteNote(PIEZO_CHANNEL, tune[tuneIndex].note, tune[tuneIndex].octave);
            }
            esp_timer_start_once(tuneTimerHandle, tune[tuneIndex].duration * (uint64_t) 4e5);
            tuneIndex++;
        } else {
            ledcDetachPin(PIEZO);
            esp_timer_delete(tuneTimerHandle);
        }
    };

    esp_timer_create_args_t tuneTimerArgs = {
        tuneTimerCallback,
        NULL,
        ESP_TIMER_TASK,
        "TuneTimer"
    };

    buttonState_t buttonState;

    void IRAM_ATTR powerButtonISR() {
        buttonState.power = true;
    }

    void IRAM_ATTR pumpButtonISR() {
        buttonState.pump = !digitalRead(PUMP_BUTTON);
    }

    void IRAM_ATTR steamButtonISR() {
        buttonState.steam = !digitalRead(STEAM_BUTTON);
    }

}

void IO::init() {
    ledcSetup(PIEZO_CHANNEL, 5000, 8);

    pinMode(POWER_BUTTON, INPUT_PULLUP);
    pinMode(PUMP_BUTTON, INPUT_PULLUP);
    pinMode(STEAM_BUTTON, INPUT_PULLUP);

    buttonState.power = false;
    buttonState.pump = !digitalRead(PUMP_BUTTON);
    buttonState.steam = !digitalRead(STEAM_BUTTON);

    attachInterrupt(POWER_BUTTON, powerButtonISR, RISING);
    attachInterrupt(PUMP_BUTTON, pumpButtonISR, CHANGE);
    attachInterrupt(STEAM_BUTTON, steamButtonISR, CHANGE);

    digitalWrite(POWER_BUTTON_LED, LOW);
    pinMode(POWER_BUTTON_LED, OUTPUT);
    digitalWrite(PUMP_BUTTON_LED, LOW);
    pinMode(PUMP_BUTTON_LED, OUTPUT);
    digitalWrite(STEAM_BUTTON_LED, LOW);
    pinMode(STEAM_BUTTON_LED, OUTPUT);
}

void IO::sayHello() {
    tuneIndex = 0;

    esp_timer_create(&tuneTimerArgs, &tuneTimerHandle);
    ledcAttachPin(PIEZO, 15);
    esp_timer_start_once(tuneTimerHandle, (uint64_t) 1);
}

buttonState_t IO::getButtonState() {
    return buttonState;
}

void IO::turnOffLEDs() {
    digitalWrite(POWER_BUTTON_LED, LOW);
    digitalWrite(PUMP_BUTTON_LED, LOW);
    digitalWrite(STEAM_BUTTON_LED, LOW);
}

void IO::setPowerButtonLED(bool newState) {
    digitalWrite(POWER_BUTTON_LED, newState);
}

void IO::setPumpButtonLED(bool newState) {
    digitalWrite(PUMP_BUTTON_LED, newState);
}

void IO::setSteamButtonLED(bool newState) {
    digitalWrite(STEAM_BUTTON_LED, newState);
}

String IO::status() {
    String status = "";
    status += (String) "Button state:\nPower Flag: " +
        ((buttonState.power) ? "set" : "clear") +
        "\nPump: " +
        ((buttonState.pump) ? "on" : "off") +
        "\nSteam: " +
        ((buttonState.steam) ? "on" : "off");
    return status;
}

void IO::clearPowerButton() {
    buttonState.power = false;
}