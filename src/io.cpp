#include "../include/io.h"

void IO::init() {
    ledcSetup(PIEZO_CHANNEL, 5000, 8);

    pinMode(PUMP_BUTTON, INPUT_PULLUP);
    pinMode(STEAM_BUTTON, INPUT_PULLUP);
}

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

void IO::sayHello() {
    tuneIndex = 0;

    esp_timer_create(&tuneTimerArgs, &tuneTimerHandle);
    ledcAttachPin(PIEZO, 15);
    esp_timer_start_once(tuneTimerHandle, (uint64_t) 1);
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