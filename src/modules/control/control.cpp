#include "../../../include/modules/control/control.h"
#include "../../../include/modules/storage.h"

namespace {
    bool initialized = false;

    auto controlMode = PumpControlMode::PRESSURE;

    SemaphoreHandle_t activeConfigMutex = NULL;
    configuration_t activeConfig;

    // Sensors
    auto temperatureSensor = new TemperatureSensor(TEMP_CS_PIN, TEMP_RREF, TEMP_SMOOTHING_COEFFICIENT, TEMP_RDY_PIN);
    auto pressureSensor = new PressureSensor(PRESSURE_SENSOR_PIN, PRESSURE_SMOOTHING_COEFFICIENT, 0.01);
    Sensor* sensors[2] = {
        (Sensor*)temperatureSensor,
        (Sensor*)pressureSensor
    };

    // Controllers
    auto temperatureController = new PID(
        [](float * input) { Control::getSmoothedTemperature(input); },
        [](float * controlValue) { Control::setHeaterPower(controlValue); },
        100,
        5.0f
    );
    auto pressureController = new PID(
        [](float * input) { Control::getSmoothedPressure(input); },
        [](float * controlValue) { Control::setPumpPower(controlValue); },
        100,
        0.5f
    );
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
        PUMP_PWM_MIN_DUTY_CYCLE,
        PUMP_PWM_MAX_DUTY_CYCLE,
        PUMP_INVERTED
    );

    // This Semaphore is used as a flag to signal that the thermal runaway protection triggered
    SemaphoreHandle_t TRPTrigger = NULL;

    void vTaskTRP(void* parameters) {
        float temperature, diffToTarget = 0.0, lastDiffToTarget = 0.0, controlTarget = 0.0;

        TickType_t lastWakeTime = xTaskGetTickCount();
        for( ;; ) {            
            if(temperatureSensor->faultDetected()) {
                xSemaphoreGive(TRPTrigger);
            }
            
            temperatureSensor->getSmoothedValue(&temperature);

            try {
                temperatureController->getControlTarget(&controlTarget);
            } catch(...) {
                controlTarget = 0.0;
            }

            diffToTarget = controlTarget - temperature;

            // if the temperature or control target are negative, something is wrong
            if(temperature < 0 || controlTarget < 0) {
                xSemaphoreGive(TRPTrigger);
            // check every few seconds if the temperature is around the target or has sufficiently moved towards it
            } else if(diffToTarget > TRP_MAX_DIFF_TO_TARGET && diffToTarget - lastDiffToTarget < 5.0f) {
                xSemaphoreGive(TRPTrigger);
            }

            // Serial.println("TRP:\nTemperature: " + 
            //     String(temperature) + "\nControl Target: " + String(controlTarget) +
            //     "\nDiff to Target: " + String(diffToTarget) +
            //     "\nLast Diff to Target: " + String(lastDiffToTarget)
            // );

            lastDiffToTarget = diffToTarget;

            vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(TRP_INTERVAL * 1000));
        }
    } 
}


/*
* Init function
*/

void Control::init() {
    if (!initialized) {
        activeConfigMutex = xSemaphoreCreateMutex();
        if(xSemaphoreTake(activeConfigMutex, 1000) == pdFALSE) {
            Serial.println("\nConfig semaphore unavailable!\n"); throw std::__throw_runtime_error;
        }
        activeConfig = Storage::loadConfiguration();
        xSemaphoreGive(activeConfigMutex);

        temperatureController->setPIDCoefs(activeConfig.temperaturePIDCoefs);
        pressureController->setPIDCoefs(activeConfig.pressurePIDCoefs);

        pump->activate();

        if (DO_THERMAL_RUNAWAY_PROTECTION) {
            TRPTrigger = xSemaphoreCreateBinary();

            xTaskCreate(
                vTaskTRP,
                "THERMAL RUNAWAY PROTECTION",
                TRP_TASK_STACK_SIZE,
                NULL,
                TRP_TASK_PRIORITY,
                NULL
            );
        }

        initialized = true;
    }
}

// Accessors
void Control::getRawTemperature(float * value) {
    temperatureSensor->getRawValue(value);
}

void Control::getSmoothedTemperature(float * value) {
    temperatureSensor->getSmoothedValue(value);
}

void Control::getRawPressure(float * value) {
    pressureSensor->getRawValue(value);
}

void Control::getSmoothedPressure(float * value) {
    pressureSensor->getSmoothedValue(value);
}

configuration_t Control::getActiveConfiguration() {
    configuration_t config;
    if(xSemaphoreTake(activeConfigMutex, 10) == pdFALSE) {
        Serial.println("\nConfig semaphore unavailable!\n"); throw std::__throw_runtime_error;
    }
    config = activeConfig;
    xSemaphoreGive(activeConfigMutex);
    return config;
}

bool Control::temperatureAnomalyDetected() {
    return DO_THERMAL_RUNAWAY_PROTECTION && xSemaphoreTake(TRPTrigger, 0) == pdTRUE;
}

// Mutators
void Control::turnOnHeater() {
    heaterBlock->activate(); // atomic operation so no need to lock
}

void Control::shutOffHeater() {
    heaterBlock->deactivate(); // also an atomic operation
}

void Control::setHeaterPower(float * powerLevel) {
    heaterBlock->setPowerLevel(powerLevel);
}

void Control::setPumpPower(float * powerLevel) {
    if(*powerLevel > 0) {
        pump->setPowerLevel(powerLevel);
        pumpMaster->activate();
    } else {
        pumpMaster->deactivate();
        float zeroLevel = 0.0f;
        pump->setPowerLevel(&zeroLevel);
    }
}

void Control::setTemperatureTarget(float newTarget) {
    temperatureController->setControlTarget(
        constrain(newTarget, 0, MAX_TEMP_TARGET)
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
    if(xSemaphoreTake(activeConfigMutex, 10) == pdFALSE) {
        Serial.println("\nConfig semaphore unavailable!\n"); throw std::__throw_runtime_error;
    }
    activeConfig.temps.brew = newValue;
    xSemaphoreGive(activeConfigMutex);
    Storage::storeBrewTemperature(newValue);
}

void Control::setSteamTemperature(float newValue) {
    if(xSemaphoreTake(activeConfigMutex, 10) == pdFALSE) {
        Serial.println("\nConfig semaphore unavailable!\n"); throw std::__throw_runtime_error;
    }
    activeConfig.temps.steam = newValue;
    xSemaphoreGive(activeConfigMutex);
    Storage::storeSteamTemperature(newValue);
}

void Control::setBrewPressure(float newValue) {
    if(xSemaphoreTake(activeConfigMutex, 10) == pdFALSE) {
        Serial.println("\nConfig semaphore unavailable!\n"); throw std::__throw_runtime_error;
    }
    activeConfig.pressures.brew = newValue;
    xSemaphoreGive(activeConfigMutex);
    Storage::storeBrewPressure(newValue);
}

void Control::setPreinfusionPressure(float newValue) {
    if(xSemaphoreTake(activeConfigMutex, 10) == pdFALSE) {
        Serial.println("\nConfig semaphore unavailable!\n"); throw std::__throw_runtime_error;
    }
    activeConfig.pressures.preinfusion = newValue;
    xSemaphoreGive(activeConfigMutex);
    Storage::storePreinfusionPressure(newValue);
}

void Control::setPreinfusionTime(uint16_t newValue) {
    if(xSemaphoreTake(activeConfigMutex, 10) == pdFALSE) {
        Serial.println("\nConfig semaphore unavailable!\n"); throw std::__throw_runtime_error;
    }
    activeConfig.preinfusionTime = newValue;
    xSemaphoreGive(activeConfigMutex);
    Storage::storePreinfusionTime(newValue);
}

void Control::setTemperaturePIDCoefs(pidCoefs_t newCoefs) {
    if(xSemaphoreTake(activeConfigMutex, 10) == pdFALSE) {
        Serial.println("\nConfig semaphore unavailable!\n"); throw std::__throw_runtime_error;
    }
    activeConfig.temperaturePIDCoefs = newCoefs;
    xSemaphoreGive(activeConfigMutex);
    
    temperatureController->setPIDCoefs(newCoefs);
    Storage::storeTemperaturePIDCoefs(newCoefs);
}

void Control::setPressurePIDCoefs(pidCoefs_t newCoefs) {
    if(xSemaphoreTake(activeConfigMutex, 10) == pdFALSE) {
        Serial.println("\nConfig semaphore unavailable!\n"); throw std::__throw_runtime_error;
    }
    activeConfig.pressurePIDCoefs = newCoefs;
    xSemaphoreGive(activeConfigMutex);

    pressureController->setPIDCoefs(newCoefs);
    Storage::storePressurePIDCoefs(newCoefs);
}

void Control::openSolenoid() {
    solenoidValve->activate();
}

void Control::closeSolenoid() {
    solenoidValve->deactivate();
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