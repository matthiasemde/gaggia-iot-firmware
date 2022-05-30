#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <Arduino.h>
#include <Preferences.h>

#include "control.h"

namespace Storage {
    void init();

    // Accessors
    Configuration loadConfiguration();
    uint16_t loadCoffeeCount();

    // Mutators
    void storeBrewTemperature(float newTemp);
    void storeSteamTemperature(float newTemp);

    void storeBrewPressure(float newPressure);
    void storePreinfusionPressure(float newPressure);
    void storePreinfusionTime(uint16_t newTime);

    void storeConfiguration(Configuration config);
    
    void storeCoffeeCount(uint16_t newCount);
}

#endif