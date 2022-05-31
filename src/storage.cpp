#include "../include/Storage.h"

namespace {
    const char keyBrewTemp[] = "BT";
    const char keySteamTemp[] = "ST";
    
    const char keyBrewPressure[] = "BP";
    const char keyPreinfusionPressure[] = "PP";
    const char keyPreinfusionTime[] = "PT";

    const char keyTempKp[] = "TKP";
    const char keyTempKi[] = "TKI";
    const char keyTempKd[] = "TKD";

    const char keyPressureKp[] = "PKP";
    const char keyPressureKi[] = "PKI";
    const char keyPressureKd[] = "PKD";

    const char keyCoffeeCount[] = "CC";

    bool initialized = false;

    Preferences* preferences;
}

void Storage::init() {
    if(!initialized) {
        preferences = new Preferences();
        preferences->begin("myStorage", false);
        initialized = true;
    }
}

configuration_t Storage::loadConfiguration() {
    temperatures_t temps;
    temps.brew = preferences->getFloat(keyBrewTemp, 96.0f);;
    temps.steam = preferences->getFloat(keySteamTemp, 130.0f);

    pressures_t pressures;
    pressures.brew = preferences->getFloat(keyBrewPressure, 9.0f);
    pressures.preinfusion = preferences->getFloat(keyPreinfusionPressure, 2.0f);
    
    pidCoefs_t temperaturePIDCoefs;
    temperaturePIDCoefs.kp = preferences->getFloat(keyTempKp, 1.0f);
    temperaturePIDCoefs.ki = preferences->getFloat(keyTempKi, 1.0f);
    temperaturePIDCoefs.kd = preferences->getFloat(keyTempKd, 1.0f);

    pidCoefs_t pressurePIDCoefs;
    pressurePIDCoefs.kp = preferences->getFloat(keyPressureKp, 1.0f);
    pressurePIDCoefs.ki = preferences->getFloat(keyPressureKi, 1.0f);
    pressurePIDCoefs.kd = preferences->getFloat(keyPressureKd, 1.0f);

    configuration_t config;
    config.temps = temps;
    config.pressures = pressures;
    config.temperaturePIDCoefs = temperaturePIDCoefs;
    config.pressurePIDCoefs = pressurePIDCoefs;
    config.preinfusionTime = preferences->getUShort(keyPreinfusionTime, 10);

    return config;
} 

uint16_t Storage::loadCoffeeCount() {
    return preferences->getUShort(keyCoffeeCount, 0);
}

// Mutators
void Storage::storeBrewTemperature(float newTemp) {
    preferences->putFloat(keyBrewTemp, newTemp);
}

void Storage::storeSteamTemperature(float newTemp) {
    preferences->putFloat(keySteamTemp, newTemp);
}

void Storage::storeBrewPressure(float newPressure) {
    preferences->putFloat(keyBrewPressure, newPressure);
}

void Storage::storePreinfusionPressure(float newPressure) {
    preferences->putFloat(keyPreinfusionPressure, newPressure);
}

void Storage::storePreinfusionTime(uint16_t newTime) {
    preferences->putUShort(keyPreinfusionTime, newTime);    
}

void Storage::storeTemperaturePIDCoefs(pidCoefs_t newCoefs) {
    if(newCoefs.kp >= 0) preferences->putFloat("TKP", newCoefs.kp);
    if(newCoefs.ki >= 0) preferences->putFloat("TKI", newCoefs.ki);
    if(newCoefs.kd >= 0) preferences->putFloat("TKD", newCoefs.kd);
}

void Storage::storePressurePIDCoefs(pidCoefs_t newCoefs) {
    if(newCoefs.kp >= 0) preferences->putFloat("PKP", newCoefs.kp);
    if(newCoefs.ki >= 0) preferences->putFloat("PKI", newCoefs.ki);
    if(newCoefs.kd >= 0) preferences->putFloat("PKD", newCoefs.kd);
}

void Storage::storeCoffeeCount(uint16_t newCount) {
    preferences->putUShort(keyCoffeeCount, newCount);
}