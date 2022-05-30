#include "../include/Storage.h"

namespace {
    const char keyBrewTemp[] = "BT";
    const char keySteamTemp[] = "ST";
    const char keyBrewPressure[] = "BP";
    const char keyPreinfusionPressure[] = "PP";
    const char keyPreinfusionTime[] = "PT";
    const char keyCoffeeCount[] = "CC";

    bool initialized = false;

    Preferences* preferences;

    // Accessors
    float loadBrewTemperature() {    
        return preferences->getFloat(keyBrewTemp, 96.0f);    
    }

    float loadSteamTemperature() {
        return preferences->getFloat(keySteamTemp, 130.0f);    
    }

    float loadBrewPressure() {
        return preferences->getFloat(keyBrewPressure, 9.0f);
    }

    float loadPreinfusionPressure() {
        return preferences->getFloat(keyPreinfusionPressure, 2.0f);
    }

    uint16_t loadPreinfusionTime() {
        return preferences->getUShort(keyPreinfusionTime, 10);
    }
}

void Storage::init() {
    if(!initialized) {
        preferences = new Preferences();
        preferences->begin("myStorage", false);
        initialized = true;
    }
}

Configuration Storage::loadConfiguration() {
    Temperatures temps;
    temps.brew = loadBrewTemperature();
    temps.steam = loadSteamTemperature();

    Pressures pressures;
    pressures.brew = loadBrewPressure();
    pressures.preinfusion = loadPreinfusionPressure();
    
    Configuration config;
    config.temps = temps;
    config.pressures = pressures;
    config.preinfusionTime = loadPreinfusionTime();

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

void Storage::storeCoffeeCount(uint16_t newCount) {
    preferences->putUShort(keyCoffeeCount, newCount);
}