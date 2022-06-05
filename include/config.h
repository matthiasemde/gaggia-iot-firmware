#ifndef __CONFIG__
#define __CONFIG__

#define MAX_TEMP_TARGET 140

#define TEMP_CS_PIN GPIO_NUM_17
#define SPI_DI_PIN GPIO_NUM_5
#define SPI_DO_PIN GPIO_NUM_18
#define SPI_CLK_PIN GPIO_NUM_19

#define TEMP_RREF 430.0


const float kp = 1.0;
const float ki = 1.0;
const float kd = 1.0;

#define PIEZO GPIO_NUM_21
#define PIEZO_CHANNEL 15

#define POWER_BUTTON GPIO_NUM_33
#define PUMP_BUTTON GPIO_NUM_25
#define STEAM_BUTTON GPIO_NUM_26

#define SOLENOID_VALVE_PIN GPIO_NUM_32
#define HEATER_BLOCK_PIN GPIO_NUM_27
#define PUMP_PIN GPIO_NUM_22

#define HEATER_BLOCK_PWM_CHANNEL 0
#define HEATER_BLOCK_PWM_FREQUENCY 1

#define PUMP_PWM_CHANNEL 3
#define PUMP_PWM_FREQUENCY 1000 // Kemo M150 Pulse Converter requires 1-10 kHz

#define PRESSURE_SENSOR_PIN GPIO_NUM_35

#define PRESSURE_SENSOR_AT_1_BAR 170

const float smoothingCoefsficient = 0.5;

#endif