#ifndef __CONFIG__
#define __CONFIG__

#define NUM_SENSORS 1

#define TEMP_IDX 0

#define TEMP_SPI_CS D0
#define TEMP_SPI_DI D5
#define TEMP_SPI_DO D6
#define TEMP_SPI_CLK D7

#define TEMP_RREF 430.0

#define TEMP_CTRL D1

#define TEMP_PMW_FREQ 100

const float kp = 1.0;
const float ki = 1.0;
const float kd = 1.0;

#endif