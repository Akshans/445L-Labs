// PeriodicTimer0AInts.c
// Runs on LM4F120/TM4C123
// Use Timer0A in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
  Program 7.5, example 7.6

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// oscilloscope or LED connected to PF3-1 for period measurement
// When using the color wheel, the blue LED on PF2 is on for four
// consecutive interrupts then off for four consecutive interrupts.
// Blue is off for: dark, red, yellow, green
// Blue is on for: light blue, blue, purple, white
// Therefore, the frequency of the pulse measured on PF2 is 1/8 of
// the frequency of the Timer0A interrupts.

/*3 ways to initiate ADC conversion process.
-Software
-Timer
-PWM0
How to know when ADC conversion is done? 
When the INR3 flag in the ADC0_RIS_R is set*/

#include "..//inc//tm4c123gh6pm.h"
#include <stdint.h>
#include "PLL.h"
#include "Timer0A.h"
#include "ADCSWTrigger.h"
#include "UART.h"
#include "FIFO.h"
#include "ST7735.h"
#include "math.h"
//#include "fixed.h"

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define LEDS      (*((volatile uint32_t *)0x40025038))
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08
#define WHEELSIZE 8           // must be an integer multiple of 2
                              //    red, yellow,    green, light blue, blue, purple,   white,          dark
const long COLORWHEEL[WHEELSIZE] = {RED, RED+GREEN, GREEN, GREEN+BLUE, BLUE, BLUE+RED, RED+GREEN+BLUE, 0};


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
uint32_t convertTemp(uint32_t ADCvalue);

void UserTask(void){
  static int i = 0;
  LEDS = COLORWHEEL[i&(WHEELSIZE-1)];
  i = i + 1;
}
// if desired interrupt frequency is f, Timer0A_Init parameter is busfrequency/f
#define F1000HZ (80000000/1000)
#define F20HZ (80000000/20)
//debug code
//How do I sample at a 1000Hz

extern volatile uint32_t tData[100];
extern volatile uint32_t counter;
extern volatile uint32_t ADCvalue;
extern volatile uint32_t status;
uint32_t ADCRef[53]={0,222,264,306,348,392,436,482,528,575,623,							
     672,722,772,824,877,931,986,1041,1098,1156,							
     1215,1275,1337,1399,1463,1527,1593,1661,1729,1799,							
     1870,1942,2015,2090,2166,2244,2323,2403,2485,2568,							
     2652,2738,2826,2915,3005,3097,3190,3285,3382,3480,3579,4096};							

uint16_t TempRef[53]={4000,4000,3940,3880,3820,3760,3700,3640,3580,3520,3460,		
     3400,3340,3280,3220,3160,3100,3040,2980,2920,2860,		
     2800,2740,2680,2620,2560,2500,2440,2380,2320,2260,		
     2200,2140,2080,2020,1960,1900,1840,1780,1720,1660,		
     1600,1540,1480,1420,1360,1300,1240,1180,1120,1060,1000,1000};		
void ST7735_DecOut3(int32_t n)
	{
		if(n>9999 || n<-9999)
		{
			ST7735_OutString("**.**");
			return;
		}
		if(n<0)
		{
			ST7735_OutString("-");
			n = 0-n;
		}
		int32_t m = n;
		for(int i=3; i>=0; i--)
			{
				int num = (int)pow(10.0,(double)i);
				n = m/num; //1000/1000 = 1
				m=m % num; //1000 gets changed to 000	
				ST7735_OutUDec(n);
				if(i ==2)
				{
					ST7735_OutString(".");
				}
			}		
	}
/*void printGraph(void){
	//print data to LCD in a graph form
	//get number of entries, max and min, for X
	//use lab 1 subroutines
	
	//get min and max X
	int32_t xMin, xMax, yMax; //yMin is 0
	xMin = ADCout[0];
	xMax = 0;
	yMax = 0;
	numPoints = 0;
	for(int i = 0; i < 1000; i++){
		if(ADCout[i] != -1){
			numPoints++;
		}
	}
	for(int i = 0; i < numPoints; i++){
		if(ADCout[i] < xMin){
			xMin = ADCout[i];
		}
		if(ADCout[i] > xMax){
			xMax = ADCout[i];
		}
		if(ADCocc[i] > yMax){
			yMax = ADCocc[i];
		}
	}
	ST7735_XYplotInit("ADC values", xMin,xMax,0,yMax);
	
	//remake arrays (inefficient due to making so many arrays??)
	//int32_t ADCx[numPoints];
	//int32_t ADCy[numPoints];
	
	ST7735_XYplot(numPoints, (int32_t *)ADCout, (int32_t *)ADCocc);
	
}*/
int x = 0, y = 0;

uint32_t convertTemp(uint32_t ADCvalue)
{
	int i = 0;
	for(i = 0; i<51; i++)
	{
		if(ADCvalue<ADCRef[i])
			break;
	}
	return TempRef[i-1] - 273.15;
}
int main(void){ 
  PLL_Init(Bus80MHz);              // bus clock at 50 MHz
	
  SYSCTL_RCGCGPIO_R |= 0x20;       // activate port F
  while((SYSCTL_PRGPIO_R&0x0020) == 0){};// ready?
  GPIO_PORTF_DIR_R |= 0x0E;        // make PF3-1 output (PF3-1 built-in LEDs)
  GPIO_PORTF_AFSEL_R &= ~0x0E;     // disable alt funct on PF3-1
  GPIO_PORTF_DEN_R |= 0x0E;        // enable digital I/O on PF3-1
                                   // configure PF3-1 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;          // disable analog functionality on PF
  LEDS = 0;                        // turn all LEDs off
//  Timer0A_Init(&UserTask, F20KHZ);     // initialize timer0A (20,000 Hz)
	TxFifo_Init();
  Timer0A_Init(F20HZ);  // initialize timer0A (16 Hz)
	UART_Init();              // initialize UART device
	
	ADC0_InitSWTriggerSeq3_Ch9();
  EnableInterrupts();
	int work=0;
	uint32_t temp = 0;
	status = 0;
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(0);
			for(int i = 0; i<100; i++)
	{
		UART_OutUDec(2);
		UART_OutString(" ");
		
	}
	//ST7735_XYplotInit("Temperature", 0,128,160,10);
  while(1){
		PF1 ^= 0x02;
		//ST7735_PlotClear(10, 160);
		
		ST7735_SetCursor(0,0);
		if(status == 1)
		{
			
			ST7735_OutUDec(ADCvalue);
			if(ADCvalue<1000)
			{
				ST7735_SetCursor(3,0);
				ST7735_OutString(" ");
			}
			ST7735_SetCursor(0,1);
			ST7735_DecOut3(convertTemp(ADCvalue));
			ST7735_OutString(" deg. Celsius");
			y = 50;
			ST7735_DrawPixel(x,y,ST7735_Color565(0,0,0));
			UART_OutUDec(ADCvalue);
			UART_OutString(", ");
			//ST7735_PlotClear(0, 5);
			x = (x+1)%128;
			if(x == 127)
			{
			//	ST7735_PlotClear(10, 160);
			}
			status = 0;
		}
		
	/*	if(counter == 100){
			DisableInterrupts();
			break;
		}	*/
  }
	/*UART_OutString("\nSample values: ");
	for(int i = 0; i<100; i++)
	{
		UART_OutUDec(tData[i]);
		UART_OutString(" ");
		
	}*/
}
