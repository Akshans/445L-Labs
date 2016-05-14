#include "Gyroscope.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

int works = 0;
//PORT C
void GyroInit(void)
{
	 SYSCTL_RCGCI2C_R |= 0x0001;           // activate I2C0
  SYSCTL_RCGCGPIO_R |= 0x0003;          // activate port c
  while((SYSCTL_PRGPIO_R&0x0002) == 0){};// ready?

  GPIO_PORTB_AFSEL_R |= 0x0C;           // 3) enable alt funct on PB2,3
  GPIO_PORTB_ODR_R |= 0x08;             // 4) enable open drain on PB3 only
  GPIO_PORTB_DEN_R |= 0x0C;             // 5) enable digital I/O on PB2,3
                                        // 6) configure PB2,3 as I2C
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFF00FF)+0x00003300;
  GPIO_PORTB_AMSEL_R &= ~0x0C;          // 7) disable analog functionality on PB2,3
  I2C0_MCR_R = I2C_MCR_MFE;      // 9) master function enable
  I2C0_MTPR_R = 24;              // 8) configure for 100 kbps clock
		
	
	while(1){
		uint32_t sw2 = GPIO_PORTB_DATA_R & 0x00000002;
	if(sw2>0){
		works = 1;
	}
	}
}
