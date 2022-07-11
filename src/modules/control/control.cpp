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

    SemaphoreHandle_t noTempControlValue = NULL;
    SemaphoreHandle_t noPressureControlValue = NULL;
    
    // This Semaphore is used as a flag to signal that the thermal runaway protection triggered
    SemaphoreHandle_t TRPTrigger = NULL;

    TaskHandle_t xHandle = NULL;
       
    void vTaskTRP(void* parameters) {
        float temperature, diffToTarget = 0.0, lastDiffToTarget = 0.0, controlTarget = 0.0;
        for( ;; ) {            
            if(temperatureSensor->faultDetected()) {
                xSemaphoreGive(TRPTrigger);
            }
            
            temperature = temperatureSensor->getSmoothedValue();

            try {
                temperatureController->getControlTarget(&controlTarget);
            } catch(...) {
                controlTarget = 0.0;
            }

            diffToTarget = controlTarget - temperatureSensor->getSmoothedValue();

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

            vTaskDelay(pdMS_TO_TICKS(TRP_INTERVAL * 1000));
        }
    } 
}


void Control::init() {
    if (!initialized) {
        activeConfig = Storage::loadConfiguration();
        temperatureController->setPIDCoefs(activeConfig.temperaturePIDCoefs);
        pressureController->setPIDCoefs(activeConfig.pressurePIDCoefs);
        pump->activate();
        
        noTempControlValue = xSemaphoreCreateBinary();
        noPressureControlValue = xSemaphoreCreateBinary();

        TRPTrigger = xSemaphoreCreateBinary();

        xTaskCreate(
            vTaskUpdate,
            "CONTROL",
            CONTROL_TASK_STACK_SIZE,
            NULL,
            CONTROL_TASK_PRIORITY,
            &xHandle
        );

        if (DO_THERMAL_RUNAWAY_PROTECTION) {
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

bool Control::temperatureAnomalyDetected() {
    return DO_THERMAL_RUNAWAY_PROTECTION && xSemaphoreTake(TRPTrigger, 0) == pdTRUE;
}

// Mutators
void Control::turnOnHeater() {
    heaterBlock->activate();
}

void Control::shutOffHeater() {
    heaterBlock->deactivate();
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
void Control::vTaskUpdate(void * parameters) {
    float tempControlValue = 0.0, pressureControlValue = 0.0;

    for( ;; ) {
        // Update sensors
        temperatureSensor->update();
        pressureSensor->update();
        // flowSensor->update();


        // Update controllers
        temperatureController->setInput(temperatureSensor->getSmoothedValue());
        try {
            temperatureController->getControlValue(&tempControlValue);
            heaterBlock->setPowerLevel(tempControlValue);
            xSemaphoreTake(noTempControlValue, 0);
        } catch (std::exception e) {
            xSemaphoreGive(noTempControlValue);
        }


        if (controlMode == PumpControlMode::PRESSURE) {
            pressureController->setInput(pressureSensor->getSmoothedValue());
            try {
                pressureController->getControlValue(&pressureControlValue);
                if(pressureControlValue > 0) {
                    pump->setPowerLevel(pressureControlValue);
                    pumpMaster->activate();
                } else {
                    pump->setPowerLevel(0.0f);
                    pumpMaster->deactivate();
                }
                xSemaphoreTake(noPressureControlValue, 0);
            } catch (std::exception e) {
                xSemaphoreGive(noPressureControlValue);
            }
        } else if (controlMode == PumpControlMode::FLOW) {
            // if (flowController->update(flowSensor->getSmoothedValue(), &nextControlValue)) {
            //     pump->setPowerLevel(nextControlValue);
            // }
        }
        
        vTaskDelay(pdMS_TO_TICKS(CONTROL_TASK_DELAY));
    }
}

String Control::status() {
    String status = "Remaining stack size: " + String(uxTaskGetStackHighWaterMark(xHandle)) + "\n";
    status += "Temperature sensor:\n";
    status += temperatureSensor->status();
    status += "\nTemperature controller:\n";
    if(xSemaphoreTake(noTempControlValue, 0) == pdTRUE) {
        status += "NO CONTROL VALUE\n"; 
        xSemaphoreGive(noTempControlValue);
    }
    status += temperatureController->status();
    status += "\nHeater Block:\n";
    status += heaterBlock->status();

    status += "\nPressure sensor:\n";
    status += pressureSensor->status();
    status += "\nPressure controller:\n";
    if(xSemaphoreTake(noPressureControlValue, 0) == pdTRUE) {
        status += "NO CONTROL VALUE\n"; 
        xSemaphoreGive(noPressureControlValue);
    }
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