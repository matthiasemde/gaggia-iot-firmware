#include "../../../include/modules/control/control.h"
#include "../../../include/modules/storage.h"

namespace {
    bool initialized = false;

    auto controlMode = PumpControlMode::PRESSURE;

    configuration_t activeConfig;

    // Sensors
    auto temperatureSensor = new TemperatureSensor(TEMP_CS_PIN, TEMP_RREF, smoothingCoefsficient);
    auto pressureSensor = new PressureSensor(PRESSURE_SENSOR_PIN, smoothingCoefsficient);
    Sensor* sensors[2] = {
        (Sensor*)temperatureSensor,
        (Sensor*)pressureSensor
    };

    // Controllers
    auto temperatureController = new PID(-1.0, 100, 5.0f);
    auto pressureController = new PID(-1.0, 100, 0.5f);
    // auto flowController = new PID(kp, ki, kd, 0, 100);
    PID* controllers[2] = {
        temperatureController,
        pressureController
    };

    // Actors
    auto solenoidValve = new BinaryActor(SOLENOID_VALVE_PIN, SOLENOID_INVERTED);

    auto heaterBlock = new PwmActor(
        HEATER_BLOCK_PIN,
        HEATER_BLOCK_PWM_CHANNEL,
        HEATER_BLOCK_PWM_FREQUENCY
    );

    auto pumpMaster = new BinaryActor(PUMP_MASTER_PIN, PUMP_MASTER_INVERTED);

    auto pump = new PwmActor(
        PUMP_PIN,
        PUMP_PWM_CHANNEL,
        PUMP_PWM_FREQUENCY,
        0.1f,
        0.9f,
        PUMP_INVERTED
    );
}


void Control::init() {
    if (!initialized) {
        activeConfig = Storage::loadConfiguration();
        temperatureController->setPIDCoefs(activeConfig.temperaturePIDCoefs);
        pressureController->setPIDCoefs(activeConfig.pressurePIDCoefs);
        initialized = true;
    }
}

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

configuration_t Control::getActiveConfiguration() {
    return activeConfig;
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

void Control::setBrewTemperature(float newValue) {
    activeConfig.temps.brew = newValue;
    Storage::storeBrewTemperature(newValue);
}

void Control::setSteamTemperature(float newValue) {
    activeConfig.temps.steam = newValue;
    Storage::storeSteamTemperature(newValue);
}

void Control::setBrewPressure(float newValue) {
    activeConfig.pressures.brew = newValue;
    Storage::storeBrewPressure(newValue);
}

void Control::setPreinfusionPressure(float newValue) {
    activeConfig.pressures.preinfusion = newValue;
    Storage::storePreinfusionPressure(newValue);
}

void Control::setPreinfusionTime(uint16_t newValue) {
    activeConfig.preinfusionTime = newValue;
    Storage::storePreinfusionTime(newValue);
}

void Control::setTemperaturePIDCoefs(pidCoefs_t newCoefs) {
    activeConfig.temperaturePIDCoefs = newCoefs;
    temperatureController->setPIDCoefs(newCoefs);
    Storage::storeTemperaturePIDCoefs(newCoefs);
}

void Control::setPressurePIDCoefs(pidCoefs_t newCoefs) {
    activeConfig.pressurePIDCoefs = newCoefs;
    pressureController->setPIDCoefs(newCoefs);
    Storage::storePressurePIDCoefs(newCoefs);
}

void Control::openSolenoid() {
    solenoidValve->activate();
}

void Control::closeSolenoid() {
    solenoidValve->deactivate();
}

// Update function needs be called each loop
void Control::update() {

    // Update sensors
    temperatureSensor->update();
    pressureSensor->update();
    // flowSensor->update();


    // Update controllers
    if (temperatureController->update(temperatureSensor->getSmoothedValue())) {
        heaterBlock->setPowerLevel(temperatureController->getControlValue());
    }

    if (controlMode == PumpControlMode::PRESSURE) {
        if (pressureController->update(pressureSensor->getSmoothedValue())) {
            auto newControlValue = pressureController->getControlValue();
            if(newControlValue > 0) {
                pump->setPowerLevel(newControlValue);
                pumpMaster->activate();
            } else {
                pump->setPowerLevel(0.0f);
                pumpMaster->deactivate();
            }
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
    status += "\nTemperature controller:\n";
    status += temperatureController->status();
    status += "\nHeater Block:\n";
    status += heaterBlock->status();

    status += "\nPressure sensor:\n";
    status += pressureSensor->status();
    status += "\nPressure controller:\n";
    status += pressureController->status();
    status += "\nPump:\n";
    status += (String)"Master: " + (pumpMaster->getState() ? "ON" : "OFF") + "\n";
    status += pump->status();

    // status += "\n\nFlow sensor:\n";
    // status += flowSensor->status();
    // status += "\nFlow controller:\n";
    // status += flowController->status();

    status += (String)"\nSolenoid valve: " + (solenoidValve->getState() ? "Open" : "Closed") + "\n";
    return status;
}