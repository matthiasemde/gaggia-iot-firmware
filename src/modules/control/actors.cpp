#include "../../../include/modules/control/actors.h"

//// Class BinaryActor ////

// Constructor
BinaryActor::BinaryActor(gpio_num_t controlPin, bool inverted, BinaryActorState initialState) {
    this->controlPin = controlPin;
    this->inverted = inverted;

    this->mutex = xSemaphoreCreateMutex();

    initialState ? activate() : deactivate();
    pinMode(controlPin, OUTPUT);
}

// Accessors
uint8_t BinaryActor::getState() {
    return state;
}

// Mutators
void BinaryActor::activate() {
    xSemaphoreTake(mutex, 10); 
    state = ACTIVE;
    digitalWrite(controlPin, inverted ? LOW : HIGH);
    xSemaphoreGive(mutex);
}

void BinaryActor::deactivate() {
    xSemaphoreTake(mutex, 10);
    state = INACTIVE;
    digitalWrite(controlPin, inverted ? HIGH : LOW);
    xSemaphoreGive(mutex);
}

//// Class PwmActor ////
double tempScale;

// Constructor
PwmActor::PwmActor(
    gpio_num_t controlPin,
    uint8_t pwmChannel,
    double pwmFrequency,
    float minOut,
    float maxOut,
    bool inverted
) {
    this->controlPin = controlPin;
    this->minOut = minOut;
    this->maxOut = maxOut;

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
    pwmDevice->channel[pwmTimer].generator[MCPWM_OPR_A].utez = inverted ? 1 : 2;   // (1 = low 2 = high) at start of the timer period
    pwmDevice->channel[pwmTimer].generator[MCPWM_OPR_A].utea = inverted ? 2 : 1;   // (1 = low 2 = high) on compare match
    pwmDevice->timer[pwmTimer].mode.mode = MCPWM_UP_COUNTER;        // Set timer 0 to increment
    pwmDevice->timer[pwmTimer].mode.start = 2;                      // Set timer 0 to free-run

    float pwmLevel = 0.0;
    setPowerLevel(&pwmLevel);
}

// Accessors

// Mutators
void PwmActor::setPowerLevel(float * newPowerLevel) {
    // convert from float (minOut - maxOut) to int (0 - maxDutyCycle)
    dutyCycle = (uint16_t) round((active ? constrain(*newPowerLevel, minOut, maxOut) : minOut) * maxDutyCycle);
    pwmDevice->channel[pwmTimer].cmpr_value[MCPWM_OPR_A].val = dutyCycle;
}

void PwmActor::activate() {
    active = true;
}

void PwmActor::deactivate() {
    active = false;
    // setPowerLevel(0.0); this would be nice, but would mean that setPowerLevel() would become a critical section
}

String PwmActor::status() {
    return (!active ? "INACTIVE" : "Prescaler: " + String(prescale)+ "^2\n" +
        "Duty cycle: " + String(dutyCycle) + " / " + String(maxDutyCycle)) + "\n";
}