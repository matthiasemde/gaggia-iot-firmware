#ifndef __CONFIG__
#define __CONFIG__

#define MAX_TEMP_TARGET 140

#define TEMP_CS_PIN D0
#define SPI_DI_PIN D5
#define SPI_DO_PIN D6
#define SPI_CLK_PIN D7

#define TEMP_RREF 430.0

#define HEATER_BLOCK_PIN D1

const float kp = 1.0;
const float ki = 1.0;
const float kd = 1.0;

#define PIEZO D2

#define PUMP_BUTTON D3
#define STEAM_BUTTON D4

#define SOLENOID_VALVE_PIN D8
#define HEATER_BLOCK_PIN D8
#define PUMP_PIN D8

#define PMW_FREQUENCY 100
#define PWM_RESOLUTION 1024

#define PRESSURE_SENSOR_PIN D3

const float smoothingCoefficient = 0.5;

#endif