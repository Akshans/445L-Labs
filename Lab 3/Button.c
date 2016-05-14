#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include <stdio.h>
#include "Button.h"
#include "ST7735.h"
#include "fixed.h"
/*SYSCTL_RCGC2_R |= 0x00000010; //Activate Port E
	int delay = SYSCTL_RCGC2_R;	//allow time for clock to start. NOT SURE IF I SHOULD UNCOMMENT THIS
	GPIO_PORTE_LOCK_R = 0x4C4F434B; //DON'T KNOW IF THIS HEX NUMBER IS RIGHT
	GPIO_PORTE_CR_R = 0xFF; 	//ALLOW CHANGES TO pe7-0
	GPIO_AMSEL_R = 0x00;	//disable analog on PF
	GPIO_PORTE_DIR_R = 0x0E; 	//PE7-4, PE0 in, PE3-1 out. CHANGE THIS.
	GPIO_PORTE_AFSEL_R = 0x00	//disable alt function on PE7-0
	GPIO_PORTE_PUR_R = 0x11; //enable pull up on PE0 AND PE4. DO I NEED THIS?
	GPIO_PORTE_DEN_R |= 0x0E; //enalbe digital I/O on PF7-0	*/
	/**********************************************************/
/*	SYSCTL_RCGCGPIO_R |= 0x00000020; //Activate Port F
	while((SYSCTL_PRGPIO_R&0x00000020) == 0){};// allow time to finish activating
                             // unlock GPIO Port F Commit Register
	GPIO_PORTF_DIR_R &= ~0x10; //Make PF4 in (built in button)
	GPIO_PORTF_DEN_R |= 0x10;	//enable I/0 on PE0
	GPIO_PORTF_PUR_R |= 0x10;	
	GPIO_PORTF_IS_R &= ~0x10;	//edge sensitive
	GPIO_PORTF_IBE_R &=~0x10;	//PF4 is not both edges
	GPIO_PORTF_IEV_R |=0x10;	//PF4 is falling edge event
	GPIO_PORTF_ICR_R |= 0x10;	//clear flag 1? 
	GPIO_PORTF_IM_R |= 0x10;	//PF4 is not both edges
	NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000;
	NVIC_EN0_R = 0x40000000;*/
void Button_Init(void)
{
	
	//EnableInterrupts();*/
	SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
	while((SYSCTL_PRGPIO_R&0x00000020) == 0){};// allow time to finish activating
                             // unlock GPIO Port F Commit Register
	 GPIO_PORTF_DIR_R &= ~0x10;    // (c) make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x10;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x10;     //     enable digital I/O on PF4   
  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R &=~0x10;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x10;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
  //EnableInterrupts();           // (i) Clears the I bit
	
}

/*If screen is at display, do */
void GPIO_PortF_Handler(void)
{
	GPIO_PORTF_ICR_R |= 0x10;      // acknowledge flag4
	ST7735_InitR(INITR_REDTAB);				//LCD init
	ST7735_FillScreen(0);  // set screen to black
  ST7735_SetCursor(0,0);
	
	//GPIO_PORTF_IM_R &=~0x10;
	//const int32_t CircleXbuf[180] = { 2000, 1999, 1995, 1989, 1981, 1970, 1956, 1941, 1923, 1902, 1879, 1854, 1827, 1798, 1766, 1732, 1696, 1658, 1618, 1576, 1532, 1486, 1439, 1389, 1338, 1286, 1231, 1176, 1118, 1060, 1000, 939, 877, 813, 749, 684, 618, 551, 484, 416, 347, 278, 209, 140, 70, 0, -70, -140, -209, -278, -347, -416, -484, -551, -618, -684, -749, -813, -877, -939, -1000, -1060, -1118, -1176, -1231, -1286, -1338, -1389, -1439, -1486, -1532, -1576, -1618, -1658, -1696, -1732, -1766, -1798, -1827, -1854, -1879, -1902, -1923, -1941, -1956, -1970, -1981, -1989, -1995, -1999, -2000, -1999, -1995, -1989, -1981, -1970, -1956, -1941, -1923, -1902, -1879, -1854, -1827, -1798, -1766, -1732, -1696, -1658, -1618, -1576, -1532, -1486, -1439, -1389, -1338, -1286, -1231, -1176, -1118, -1060, -1000, -939, -877, -813, -749, -684, -618, -551, -484, -416, -347, -278, -209, -140, -70, 0, 70, 140, 209, 278, 347, 416, 484, 551, 618, 684, 749, 813, 877, 939, 1000, 1060, 1118, 1176, 1231, 1286, 1338, 1389, 1439, 1486, 1532, 1576, 1618, 1658, 1696, 1732, 1766, 1798, 1827, 1854, 1879, 1902, 1923, 1941, 1956, 1970, 1981, 1989, 1995, 1999
//};
	ST7735_SetCursor(0,0);
	printf("Hi\n");
	//ST7735_XYplotInit("Circle",-2500, 2500, -2500, 2500);
	
	
}


