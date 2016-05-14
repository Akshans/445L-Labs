#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"

volatile uint32_t Period;              // (1/clock) units
volatile uint32_t First;               // Timer0A first edge
volatile int32_t Done;                 // set each rising
//Initialize tachometer
void PeriodMeasure_Init(void);
//read most recent measurement in 0.1rps 
//void Timer0A_Handler();