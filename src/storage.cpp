#include "../include/Storage.h"

const uint16_t addressBrewTemp = 0;
const uint16_t addressSteamTemp = 10;
const uint16_t addressBrewPressure = 20;
const uint16_t addressPreinfusionPressure = 30;
const uint16_t addressPreinfusionTime = 40;
const uint16_t addressNumberOfCoffees = 50;


// Accessors
float loadBrewTemperature() {    
    float brewTemp;
    EEPROM.get(addressBrewTemp, brewTemp);    
    return brewTemp;
}

float loadSteamTemperature() {
    float steamTemp;
    EEPROM.get(addressSteamTemp, steamTemp);    
    return steamTemp;
}

float loadBrewPressure() {
    float brewPressure;
    EEPROM.get(addressBrewPressure, brewPressure);
    return brewPressure;
}

float loadPreinfusionPressure() {
    float preinfusionPressure;
    EEPROM.get(addressPreinfusionPressure, preinfusionPressure);
    return preinfusionPressure;    
}

float loadPreinfusionTime() {
    uint16_t preinfusionTime;
    EEPROM.get(addressPreinfusionTime, preinfusionTime);
    return preinfusionTime;     
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

uint16_t Storage::loadNumberOfCoffees() {
    uint16_t numberOfCoffees;
    EEPROM.get(addressNumberOfCoffees, numberOfCoffees);
    return numberOfCoffees;
}

// Mutators
void Storage::storeBrewTemperature(float newTemp) {
    // EEPROM.update(addressBrewTemp, newTemp);
    
}
void Storage::storeSteamTemperature(float newTemp) {
    
}

void Storage::storeBrewPressure(float newPressure) {
    
}
void Storage::storePreinfusionPressure(float newPressure) {
    
}
void Storage::storePreinfusionTime(float newTime) {
    
}

void Storage::storeNumberOfCoffees(uint16_t newCount) {
    
}