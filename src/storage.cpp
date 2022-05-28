#include "../include/Storage.h"

namespace {
    const uint16_t addressBrewTemp = 0;
    const uint16_t addressSteamTemp = 10;
    const uint16_t addressBrewPressure = 20;
    const uint16_t addressPreinfusionPressure = 30;
    const uint16_t addressPreinfusionTime = 40;
    const uint16_t addressCoffeeCount = 50;

    bool storageInitialized = false;

    void initStorage() {
        if(!storageInitialized) {
            EEPROM.begin(256);
            storageInitialized = true;
        }
    }

    template<typename T>
    void writeStorage(uint16_t address, T value) {
        initStorage();
        uint8_t *p = (uint8_t*)&value;
        for(uint8_t i = 0; i < sizeof(T); i++) {
            EEPROM.write(address + i, *p++);
        }
        EEPROM.commit();
    }

    // Accessors
    float loadBrewTemperature() {    
        initStorage();
        float brewTemp;
        EEPROM.get(addressBrewTemp, brewTemp);    
        return brewTemp;
    }

    float loadSteamTemperature() {
        initStorage();
        float steamTemp;
        EEPROM.get(addressSteamTemp, steamTemp);    
        return steamTemp;
    }

    float loadBrewPressure() {
        initStorage();
        float brewPressure;
        EEPROM.get(addressBrewPressure, brewPressure);
        return brewPressure;
    }

    float loadPreinfusionPressure() {
        initStorage();
        float preinfusionPressure;
        EEPROM.get(addressPreinfusionPressure, preinfusionPressure);
        return preinfusionPressure;    
    }

    float loadPreinfusionTime() {
        initStorage();
        uint16_t preinfusionTime;
        EEPROM.get(addressPreinfusionTime, preinfusionTime);
        return preinfusionTime;     
    }
}

Configuration Storage::loadConfiguration() {
    initStorage();
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
    initStorage();
    uint16_t CoffeeCount;
    EEPROM.get(addressCoffeeCount, CoffeeCount);
    return CoffeeCount;
}

// Mutators
void Storage::storeBrewTemperature(float newTemp) {
    writeStorage(addressBrewTemp, newTemp);
}

void Storage::storeSteamTemperature(float newTemp) {
    writeStorage(addressSteamTemp, newTemp);
}

void Storage::storeBrewPressure(float newPressure) {
    writeStorage(addressBrewPressure, newPressure);
}

void Storage::storePreinfusionPressure(float newPressure) {
    writeStorage(addressPreinfusionPressure, newPressure);
}

void Storage::storePreinfusionTime(uint16_t newTime) {
    writeStorage(addressPreinfusionTime, newTime);    
}

void Storage::storeCoffeeCount(uint16_t newCount) {
    writeStorage(addressCoffeeCount, newCount);
}