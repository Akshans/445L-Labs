#include <stdio.h>
#include <stdint.h>
#include "string.h"
//#include "ST7735.h"
#include "PLL.h"
//#include "fixed.h"
#include "..//inc//tm4c123gh6pm.h"
#include "math.h"
//#include "ADCSWTrigger.h"
//#include "menu.h"
//#include "clock.h"
#include "button.h"

//PORT E DIGITAL ANALOG FOR JOYSTICK. 
void buttonInit(void){
	SYSCTL_RCGCGPIO_R |= 0x10;									//activate port e
	volatile int32_t delay = SYSCTL_RCGCGPIO_R; //delay
	//GPIO_PORTE_PCTL_R &= ~0x00F0FFFF; 					//GPIO PE0-3 and PE5
	GPIO_PORTE_DIR_R &= ~0x01;									//PE0-3 and PE5 inputs
	GPIO_PORTE_AFSEL_R &= ~0x01;								//disable alt functions
	GPIO_PORTE_DEN_R |= 0x01;									//enable digital IO
	GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFF0F0000)+0x00000000;
	GPIO_PORTE_AMSEL_R &= ~0x01;								//disable analog functionality
}