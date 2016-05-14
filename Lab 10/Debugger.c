#include "..//inc//tm4c123gh6pm.h"
#include <stdint.h>
#include "PLL.h"

volatile uint32_t inputTime1, inputTime2, inputTime, icTime1, icTime2, icTime;
void Timer1_Init(void){ 
  volatile uint32_t delay; 
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1 
  delay = SYSCTL_RCGCTIMER_R;   // allow time to finish activating 
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup 
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode 
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, down-count  
  TIMER1_TAILR_R = 0xFFFFFFFF;  // 4) reload value 
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution 
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A 
}