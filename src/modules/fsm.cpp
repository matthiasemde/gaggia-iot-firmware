#include "../../include/modules/fsm.h"

namespace {
    state_t state = UNINITIALIZED;
    uint32_t brewTimer;

    void goIdle() {
        IO::clearPowerButton();
        Control::setTemperatureTarget(0.0);
        Control::setPressureTarget(0.0);
        state = IDLE;
    }
}

void FSM::init() {
    // state = IDLE;
}

state_t FSM::update() {
    buttonState_t buttonState = IO::getButtonState();

    switch(state) {
        //////////////   UNINITIALIEZD  ///////////////
        case UNINITIALIZED:
            // TODO: check if all modules are initialized
            state = IDLE;
            break;

        //////////////       IDLE       ///////////////
        case IDLE:
            if (buttonState.power) {
                IO::clearPowerButton();
                Control::setTemperatureTarget(Control::getActiveConfiguration().temps.brew);
                state = HEATING;
            }
            break;

        //////////////      HEATING     ///////////////
        case HEATING:
            if (buttonState.power) goIdle();
            else if(buttonState.pump) {
                Control::setPressureTarget(Control::getActiveConfiguration().pressures.preinfusion);
                brewTimer = millis();
                state = PREINFUSION;
            }
            break;

        //////////////     PREINFUSION  ///////////////
        case PREINFUSION:
            if (buttonState.power) goIdle();
            else if (!buttonState.pump) {
                Control::setPressureTarget(0.0);
                state = HEATING;
            } else if ((millis() - brewTimer) > Control::getActiveConfiguration().preinfusionTime * 1000) {
                Control::setPressureTarget(Control::getActiveConfiguration().pressures.brew);
                state = BREWING;
            }
            break;

        //////////////      HEATING     ///////////////
        case BREWING:
            if (buttonState.power) goIdle();
            else if (!buttonState.pump) {
                Control::setPressureTarget(0.0);
                state = HEATING;
            }
            break;
    }
    return state;
}

String FSM::status() {
    String status = "";
    switch (state) {
        case UNINITIALIZED:
            status = "UNINITIALIZED\n";
            break;
        case IDLE:
            status = "IDLE\n";
            break;
        case HEATING:
            status = "HEATING\n";
            break;
        case PREINFUSION:
            status = "PREINFUSION\nBrew timer: " + (String) (round(millis() - brewTimer)) + "s\n";
            break;
        case BREWING:
            status = "BREWING\nBrew timer: " + (String) (round(millis() - brewTimer)) + "s\n";
            break;
    }
    return status;
}