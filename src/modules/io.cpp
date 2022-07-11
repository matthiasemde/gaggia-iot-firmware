#include "../../include/modules/io.h"

namespace {
    SemaphoreHandle_t piezoSemaphore = NULL;

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
            xSemaphoreGive(piezoSemaphore);
        }
    };

    esp_timer_create_args_t tuneTimerArgs = {
        tuneTimerCallback,
        NULL,
        ESP_TIMER_TASK,
        "TuneTimer"
    };

    // Define the volatile variables which are changed by the button ISRs
    // note that powerButtonFlag is merely a flag which indicates that the button
    // has been pressed and will need to be cleared by calling clearPowerButton()
    volatile bool powerButtonFlag = false;
    volatile bool pumpButtonState = false;
    volatile bool steamButtonState = false;

    void IRAM_ATTR powerButtonISR() {
        powerButtonFlag = true;
    }

    void IRAM_ATTR pumpButtonISR() {
        pumpButtonState = !digitalRead(PUMP_BUTTON);
    }

    void IRAM_ATTR steamButtonISR() {
        steamButtonState = !digitalRead(STEAM_BUTTON);
    }

    auto powerButtonLED = new BinaryActor(POWER_BUTTON_LIGHT);
    auto pumpButtonLED = new BinaryActor(PUMP_BUTTON_LIGHT);
    auto steamButtonLED = new BinaryActor(STEAM_BUTTON_LIGHT);
}

void IO::init() {
    piezoSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(piezoSemaphore);

    ledcSetup(PIEZO_CHANNEL, 5000, 8);

    pinMode(POWER_BUTTON, INPUT_PULLUP);
    pinMode(PUMP_BUTTON, INPUT_PULLUP);
    pinMode(STEAM_BUTTON, INPUT_PULLUP);

    // no critical section here, since this is called before ISRs are attached
    powerButtonFlag = false;
    pumpButtonState = !digitalRead(PUMP_BUTTON);
    steamButtonState = !digitalRead(STEAM_BUTTON);

    attachInterrupt(POWER_BUTTON, powerButtonISR, FALLING);
    attachInterrupt(PUMP_BUTTON, pumpButtonISR, CHANGE);
    attachInterrupt(STEAM_BUTTON, steamButtonISR, CHANGE);
}

void IO::sayHello() {
    if (xSemaphoreTake(piezoSemaphore, 0) == pdTRUE) {
        tuneIndex = 0;
        esp_timer_create(&tuneTimerArgs, &tuneTimerHandle);
        ledcAttachPin(PIEZO, 15);
        esp_timer_start_once(tuneTimerHandle, (uint64_t) 1);
    }
}

buttonState_t IO::getButtonState() {
    return {(powerButtonFlag | (pumpButtonState << 1) | (steamButtonState << 2))};
}

void IO::turnOffLights() {
    powerButtonLED->deactivate();
    pumpButtonLED->deactivate();
    steamButtonLED->deactivate();
}

void IO::setPowerButtonLight(LightState newState) {
    powerButtonLED->activate();
}

void IO::setPumpButtonLight(LightState newState) {
    pumpButtonLED->activate();
}

void IO::setSteamButtonLight(LightState newState) {
    steamButtonLED->activate();
}

String IO::status() {
    String status = "";
    status += (String) "Button state:\nPower Flag: " +
        ((powerButtonFlag) ? "set" : "clear") +
        "\nPump: " +
        ((pumpButtonState) ? "on" : "off") +
        "\nSteam: " +
        ((steamButtonState) ? "on" : "off");
    return status;
}

void IO::clearPowerButton() {
    powerButtonFlag = false;
}