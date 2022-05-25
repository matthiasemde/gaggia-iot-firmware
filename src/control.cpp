#include "../include/control.h"

auto controlMode = PumpControlMode::PRESSURE;

// Sensors
auto temperatureSensor = new TemperatureSensor(TEMP_CS_PIN, TEMP_RREF, smoothingCoefficient);
auto pressureSensor = new PressureSensor(PRESSURE_SENSOR_PIN, smoothingCoefficient);
Sensor* sensors[2] = {
    (Sensor*)temperatureSensor,
    (Sensor*)pressureSensor
};

// Controllers
auto temperatureController = new PID(kp, ki, kd, 0, 100);
auto pressureController = new PID(kp, ki, kd, 0, 100);
// auto flowController = new PID(kp, ki, kd, 0, 100);
PID* controllers[2] = {
    temperatureController,
    pressureController
};

// Actors
auto solenoidValve = new BinaryActor(SOLENOID_VALVE_PIN, (uint8_t) SolenoidState::CLOSED);

auto heaterBlock = new PwmActor(HEATER_BLOCK_PIN, PMW_FREQUENCY, PWM_RESOLUTION);
auto pump = new PwmActor(PUMP_PIN, PMW_FREQUENCY, PWM_RESOLUTION);



// Accessors
float Control::getRawTemperature() {
    return temperatureSensor->getRawValue();    
}

float Control::getSmoothedTemperature() {
    return temperatureSensor->getSmoothedValue();    
}

float Control::getRawPressure() {
    return pressureSensor->getRawValue();
}

float Control::getSmoothedPressure() {
    return pressureSensor->getSmoothedValue();
}

// Mutators
void Control::setTemperatureTarget(float newTarget) {
    temperatureController->setControlTarget(
        constrain(newTarget, 20, MAX_TEMP_TARGET)
    );
}

void Control::setPressureTarget(float newTarget) {
    controlMode = PumpControlMode::PRESSURE;
    pressureController->setControlTarget(newTarget);
}

void Control::setFlowTarget(float newTarget) {
    controlMode = PumpControlMode::FLOW;
    // flowController->setControlTarget(newTarget);
}

void Control::openSolenoid() {
    solenoidValve->setState((uint8_t) SolenoidState::OPEN);
}

void Control::closeSolenoid() {
    solenoidValve->setState((uint8_t) SolenoidState::CLOSED);
}

// Update function needs be called each loop
void Control::update() {

    // Update sensors

    temperatureSensor->update();
    pressureSensor->update();
    // flowSensor->update();


    // Update controllers
    float nextControlValue;
    if (temperatureController->update(temperatureSensor->getSmoothedValue(), &nextControlValue)) {
        heaterBlock->setPowerLevel(nextControlValue);
    }
    if (controlMode == PumpControlMode::PRESSURE) {
        if (pressureController->update(pressureSensor->getSmoothedValue(), &nextControlValue)) {
            pump->setPowerLevel(nextControlValue);
        }
    } else if (controlMode == PumpControlMode::FLOW) {
        // if (flowController->update(flowSensor->getSmoothedValue(), &nextControlValue)) {
        //     pump->setPowerLevel(nextControlValue);
        // }
    }
}

String Control::status() {
    String status = "";
    status += "Temperature sensor:\n";
    status += temperatureSensor->status();
    status += "\nPressure sensor:\n";
    status += pressureSensor->status();
    // status += "\nFlow sensor:\n";
    // status += flowSensor->status();

    status += "\nTemperature controller:\n";
    status += temperatureController->status();
    status += "\nPressure controller:\n";
    status += pressureController->status();
    // status += "\nFlow controller:\n";
    // status += flowController->status();

    status += "\nSolenoid valve: " + solenoidValve->getState();
    return status;
}