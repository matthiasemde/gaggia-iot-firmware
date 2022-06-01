#include "../include/actors.h"

//// Class BinaryActor ////

// Constructor
BinaryActor::BinaryActor(uint8_t controlPin, uint8_t initialState) {
    this->controlPin = controlPin;
    this->state = initialState;

    digitalWrite(controlPin, initialState);
    pinMode(controlPin, OUTPUT);
}

// Accessors
uint8_t BinaryActor::getState() {
    return state;
}

// Mutators
void BinaryActor::setState(uint8_t newState) {
    state = newState;
    digitalWrite(controlPin, newState);
}

//// Class PwmActor ////
double tempScale;

// Constructor
PwmActor::PwmActor(gpio_num_t controlPin, uint8_t pwmChannel ,double pwmFrequency) {
    this->controlPin = controlPin;

    mcpwm_unit_t pwmUnit;

    if(pwmChannel < MCPWM_TIMER_MAX) {
        pwmUnit = MCPWM_UNIT_0;
        pwmDevice = &MCPWM0;
    } else if(pwmChannel < MCPWM_TIMER_MAX * 2) {
        pwmUnit = MCPWM_UNIT_1;
        pwmDevice = &MCPWM1;
    } else {
        throw "Invalid PWM channel (Must be between 0 and 5)";
    }

    this->pwmTimer = (mcpwm_timer_t)(pwmChannel % MCPWM_TIMER_MAX);

    pwmDevice->timer_sel.operator0_sel = 0;
    pwmDevice->timer_sel.operator1_sel = 1;
    pwmDevice->timer_sel.operator2_sel = 2;

    mcpwm_gpio_init(
        pwmUnit,
        (mcpwm_io_signals_t) ((pwmChannel % MCPWM_TIMER_MAX) * 2),
        controlPin
    );

    // Calculate the PWM period (maxDutyCycle): dt = (4kHz / pwmFrequency) - 1
    double scale = sqrt(160.0e6 / (pwmFrequency * 1024.0));
    prescale = (uint8_t)(constrain(int(scale), 0, 255));
    maxDutyCycle = (uint16_t)int(round(160.0e6 / (pwmFrequency * sq((double)prescale)) - 1));

    pwmDevice->clk_cfg.prescale = prescale;                     // Set the 160MHz clock prescaler to 399 (160MHz/(199+1)=1600kHz)
    pwmDevice->timer[pwmTimer].period.prescale = prescale;      // Set timer 0 prescaler to 399 (800kHz/(199+1))=16kHz)
    pwmDevice->timer[pwmTimer].period.period = maxDutyCycle;        // Set the period to the max duty cycle     
    pwmDevice->channel[pwmTimer].generator[MCPWM_OPR_A].utez = 2;   // Set the PWM0A ouput to go high at the start of the timer period
    pwmDevice->channel[pwmTimer].generator[MCPWM_OPR_A].utea = 1;   // Clear on compare match
    pwmDevice->timer[pwmTimer].mode.mode = MCPWM_UP_COUNTER;        // Set timer 0 to increment
    pwmDevice->timer[pwmTimer].mode.start = 2;                      // Set timer 0 to free-run

    pwmDevice->channel[pwmTimer].cmpr_value[MCPWM_OPR_A].val = 0;   // Set the counter compare
}

// Accessors

// Mutators
void PwmActor::setPowerLevel(float newPowerLevel) {
    // convert from float (0.0 - 1.0) to int (0 - maxDutyCycle)
    dutyCycle = (uint16_t) round(constrain(newPowerLevel, 0.0, 1.0) * maxDutyCycle);
    pwmDevice->channel[pwmTimer].cmpr_value[MCPWM_OPR_A].val = dutyCycle;
}

void PwmActor::activate() {
    if(!active) {
        // ledcAttachPin(controlPin, pwmChannel);
        active = true;
    }
}

void PwmActor::deactivate() {
    if(active) {
        // ledcDetachPin(controlPin);
        active = false;
    }
}

String PwmActor::status() {
    return "Prescaler: " + String(prescale)+ "^2\n" +
        "Duty cycle: " + String(dutyCycle) + " / " + String(maxDutyCycle) + "\n";
}