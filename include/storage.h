#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <Arduino.h>
#include <Preferences.h>

#include "control.h"

namespace Storage {
    void init();

    // Accessors
    configuration_t loadConfiguration();
    uint16_t loadCoffeeCount();

    // Mutators
    void storeBrewTemperature(float newTemp);
    void storeSteamTemperature(float newTemp);

    void storeBrewPressure(float newPressure);
    void storePreinfusionPressure(float newPressure);
    void storePreinfusionTime(uint16_t newTime);

    void storeTemperaturePIDCoefs(pidCoefs_t newCoefs);
    void storePressurePIDCoefs(pidCoefs_t newCoefs);

    void storeConfiguration(configuration_t config);
    
    void storeCoffeeCount(uint16_t newCount);
}

#endif