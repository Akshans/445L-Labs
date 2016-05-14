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

void buttonInit(void){
	SYSCTL_RCGCGPIO_R |= 0x10;									//activate port e
	volatile int32_t delay = SYSCTL_RCGCGPIO_R; //delay
	GPIO_PORTE_PCTL_R &= ~0x00F0FFFF; 					//GPIO PE0-3 and PE5
	GPIO_PORTE_DIR_R &= ~0x2F;									//PE0-3 and PE5 inputs
	GPIO_PORTE_AFSEL_R &= ~0x2F;								//disable alt functions
	GPIO_PORTE_DEN_R |= 0x2F;									//enable digital IO
	GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFF0F0000)+0x00000000;
	GPIO_PORTE_AMSEL_R &= ~0x2F;								//disable analog functionality
	
	/*volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x00000020;  // 1) activate clock for Port F
  delay = SYSCTL_RCGCGPIO_R;        // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital I/O on PF4-0*/
}