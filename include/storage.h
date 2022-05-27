#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <Arduino.h>
#include <EEPROM.h>

typedef struct _Temperatures {
    float brew;
    float steam;
} Temperatures;

typedef struct _Pressures {
    float brew;
    float preinfusion;
} Pressures;

typedef struct _Configuration {
    Temperatures temps;
    Pressures pressures;
    uint16_t preinfusionTime;
} Configuration;
    

namespace Storage {
    Configuration loadConfiguration();

    // Mutators
    void storeBrewTemperature(float newTemp);
    void storeSteamTemperature(float newTemp);

    void storeBrewPressure(float newPressure);
    void storePreinfusionPressure(float newPressure);
    void storePreinfusionTime(uint16_t newTime);

    void storeConfiguration(Configuration config);
    
    uint16_t loadNumberOfCoffees();
    void increaseNumberOfCoffees();
}

#endif