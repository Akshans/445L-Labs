#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
//Initialize PWM
void MotorInit(uint16_t period, uint16_t duty);
//Set Duty Cycle
void setMotorDuty(uint16_t duty);
//Turn off motor
void turnMotorOff();