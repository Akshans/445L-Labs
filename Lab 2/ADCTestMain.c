// ADCTestMain.c
// Runs on TM4C123
// This program periodically samples ADC channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.
// Daniel Valvano
// September 5, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

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

// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V 
#include <stdint.h>
#include "ADCSWTrigger.h"
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "fixed.h"
#include "ST7735.h"


#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define LEDS      (*((volatile uint32_t *)0x40025038))
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08
#define WHEELSIZE 8
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

volatile uint32_t ADCvalue;
// This debug function initializes Timer0A to request interrupts
// at a 100 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void){
  volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = 799999;         // start value for 100 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}

	double first;
	double second;
	uint32_t times[1000];
	uint32_t tData[1000];
	double jitter;
	uint32_t counter = 0;

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  PF2 ^= 0x04;                   // profile
  PF2 ^= 0x04;                   // profile
  ADCvalue = ADC0_InSeq3();
  PF2 ^= 0x04;                   // profile
	

	/*if(counter != 1000){
		tData[counter] = ADCvalue;
		times[counter] = TIMER1_TAR_R;
		
		counter++;
	}*/
}

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


const uint32_t COLORWHEEL[WHEELSIZE] = {RED, RED+GREEN, GREEN, GREEN+BLUE, BLUE, BLUE+RED, RED+GREEN+BLUE, 0};

void UserTask(void){
  static uint32_t i = 0;
  LEDS = COLORWHEEL[i&(WHEELSIZE-1)];
  i = i + 1;
}

void initEverything(void){
	uint32_t delay;
	PLL_Init(Bus80MHz);              // bus clock at 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x20;       // activate port F
//	ADC0_SAC_R |= 0x02;							// hardware avging
  ADC0_InitSWTriggerSeq3_Ch9();       // allow time to finish activating
	Timer0A_Init100HzInt();  
	//GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  //GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  //GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
  GPIO_PORTF_DIR_R |= 0x0E;        // make PF3-1 output (PF3-1 built-in LEDs)
  GPIO_PORTF_AFSEL_R &= ~0x0E;     // disable alt funct on PF3-1
  GPIO_PORTF_DEN_R |= 0x0E;        // enable digital I/O on PF3-1
                                   // configure PF3-1 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFF000F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;          // disable analog functionality on PF
  LEDS = 0;                        // turn LEDs off
	ST7735_InitR(INITR_REDTAB);				//LCD init
	ST7735_FillScreen(0);  // set screen to black
  ST7735_SetCursor(0,0);
//  Timer1_Init(&UserTask, 4000);    // initialize timer2 (20,000 Hz)
  //Timer1_Init(&UserTask, 5000000); // initialize timer1 (16 Hz)
//  Timer1_Init(&UserTask, 80000000);// initialize timer1 (1 Hz)
  Timer1_Init(); // initialize timer1 (slowest rate)
}

void calcTimes(void){
	double tdifMax = -1.000;
	double tdifMin = -1.000;
	double tdif = 0.000;
	int32_t timed[999];
	
	for(int i = 0; i < 999; i++){
		timed[i] = times[i] - times[i+1];
	}
	
	//we're gonna throw out the first ADC value
	tData[0] = -1;
	
	for(int i = 0; i < 998; i++){
		//uint32_t time1 = times[i+1];

		if(timed[i] != 0x000C3500){
			tData[i+1] = -1;
		}
		tdif = timed[i] - timed[i+1];
		
		
		if(tdifMax == -1.000 && tdifMin == -1.000){
			tdifMax = tdif;
			tdifMin = tdif;
		}
		if(tdif < tdifMin){
			tdifMin = tdif;
		}
		if(tdif > tdifMax){
			tdifMax = tdif;
		}
	}
	
	if(tdifMin < 0.000){
		jitter = tdifMax - tdifMin;
	}
	else{
		jitter = tdifMax + tdifMin;
	}
}

uint32_t ADCout[1000];
uint32_t ADCocc[1000];
uint32_t numPoints;

void calcData(void){
	
	for(int i = 0; i < 1000; i++){
		ADCout[i] = -1;
		ADCocc[i] = 0;
	}
	
	for(int i = 0; i < 1000; i++){
		if(tData[i] != -1){
			int32_t j = 0;
			while(ADCout[j] != -1){
				//numPoints++;
				if(ADCout[j] == tData[i]){
					break;
				}
				j++;
				//should never fail to find open slot
			}
			ADCout[j] = tData[i];
			ADCocc[j]++;
		}
	}
}

void printGraph(void){
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
	
}
	


int main(void){
	/*	THIS IS THE ACTUAL CODE~~~~~~~~~~~~~~~~~~~
  PLL_Init(Bus80MHz);                   // 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x20;            // activate port F
  ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
  Timer0A_Init100HzInt();               // set up Timer0A for 100 Hz interrupts
  GPIO_PORTF_DIR_R |= 0x06;             // make PF2, PF1 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x06;          // disable alt funct on PF2, PF1
  GPIO_PORTF_DEN_R |= 0x06;             // enable digital I/O on PF2, PF1
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00F)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF
  PF2 = 0;                      // turn off LED
  EnableInterrupts();
  while(1){
    PF1 ^= 0x02;  // toggles when running in main
  }*/
	
	// THIS IS MY CODE ~~~~~~~~~~~~~

	initEverything();

	EnableInterrupts();
  while(1){
    //WaitForInterrupt();
		PF1 ^= 0x02;
		if(counter == 1000){
			DisableInterrupts();
			break;
		}
  }
	

	//do calculations
	uint32_t testval = times[0];
	calcTimes();
	//testval = jitter;
	calcData();
	
	printGraph();
	int bp = 0;

}




