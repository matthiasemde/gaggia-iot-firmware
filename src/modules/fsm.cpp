#include "../../include/modules/fsm.h"

namespace {
    state_t state = UNINITIALIZED;
    uint32_t brewTimerStart;
    uint32_t brewTimerStop;

    void goIdle() {
        IO::clearPowerButton();
        IO::turnOffLights();
        Control::setPressureTarget(0.0);
        Control::closeSolenoid();
        Control::setTemperatureTarget(0.0);
        state = IDLE;
    }

    void enterHeating() {
        IO::setPumpButtonLight(LightState::OFF);
        IO::setSteamButtonLight(LightState::OFF);
        Control::setPressureTarget(0.0);
        Control::closeSolenoid();
        Control::setTemperatureTarget(Control::getActiveConfiguration().temps.brew);
        state = HEATING;
    }

    void enterSteaming() {
        IO::setSteamButtonLight(LightState::ON);
        Control::setPressureTarget(0.0);
        Control::closeSolenoid();
        Control::setTemperatureTarget(Control::getActiveConfiguration().temps.steam);
        state = STEAMING;
    }
}

void FSM::init() {
    // state = IDLE;
}

state_t FSM::update() {
    buttonState_t buttonState = IO::getButtonState();

    /*
    * Thermal runaway protection
    * If there is an error with the temperature sensor or
    * the heating element is on, but the temperature is not
    * changing, then shut off the heater
    */
    if(Control::temperatureAnomalyDetected()) {
        Control::shutOffHeater();
        Control::openSolenoid();
        state = SAFETY_OFF;
    }

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
                IO::setPowerButtonLight(LightState::ON);
                enterHeating();
            }
            break;

        //////////////      HEATING     ///////////////
        case HEATING:
            if (buttonState.power) goIdle();
            else if(buttonState.pump) {
                IO::setPumpButtonLight(LightState::ON);
                Control::setPressureTarget(Control::getActiveConfiguration().pressures.preinfusion);
                Control::openSolenoid();
                brewTimerStart = millis();
                state = PREINFUSION;
            } else if(buttonState.steam) enterSteaming();
            break;

        //////////////    PREINFUSION   ///////////////
        case PREINFUSION:
            if (buttonState.power) goIdle();
            else if (!buttonState.pump) enterHeating();
            else if ((millis() - brewTimerStart) > Control::getActiveConfiguration().preinfusionTime * 1000) {
                Control::setPressureTarget(Control::getActiveConfiguration().pressures.brew);
                state = BREWING;
            }
            break;

        //////////////      BREWING     ///////////////
        case BREWING:
            brewTimerStop = millis();
            if (buttonState.power) goIdle();
            else if (!buttonState.pump) enterHeating();
            break;

        //////////////      STEAMING    ///////////////
        case STEAMING:
            if (buttonState.power) goIdle();
            else if (!buttonState.steam) enterHeating();
            else if (buttonState.pump) // TODO: figure out what to do here ¯\_(ツ)_/¯
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
        case STEAMING:
            status = "STEAMING\n";
            break;
        case PREINFUSION:
            status = "PREINFUSION\nBrew timer: " + (String) (round((millis() - brewTimerStart)/1000)) + "s\n";
            break;
        case BREWING:
            status = "BREWING\nBrew timer: " + (String) (round((millis() - brewTimerStart)/1000)) + "s\n";
            break;
        case SAFETY_OFF:
            status = "SAFETY OFF\n";
            break;
        default:
            status = "UNKNOWN\n";
    }
    return status;
}