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

#include "..//inc//tm4c123gh6pm.h"
#include <stdint.h>
#include "PLL.h"
#include "Timer0A.h"
#include "PWM.h"
#include "motor.h"
#include "Timer2.h"
#include "tach.h"
#include "ST7735.h"
#include "math.h"
#include "Button.h"
#include "Debugger.h"
//#include "Timer0A.h"

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
void LCDInit(void);
void outputSpeed(uint32_t speed);
void ST7735_DecOut3(uint32_t n);

extern volatile uint32_t Period;	//24 bit, 12.5ns units
extern volatile int32_t Done;                 // set each rising
int c = 0, d = 0, e = 0;
uint32_t Speed;	//motor speed
int32_t y;
int32_t E;	//speed error
int32_t U, Ui, Up, eIdx=0;	//duty cycles 40 to 
int32_t desiredSpeed = 1200, savedSpeed=1200, toggle=1; //1200*0.025rpw = 30
//**DEBUGGING VARS*************************************************
int32_t errorArray[100];	
double average = 0;		
extern volatile uint32_t icTime1;
extern volatile uint32_t icTime2; 
extern volatile uint32_t icTime;	//Time in integral controller ISR
extern volatile uint32_t inputTime;	//Time in input capture ISR
//*****************************************************************
void IntegralController(void){
	//Tha Integral Controller
	/*//-Calculate error as the difference b/w desired and actual speed
	frequency = 1/period
	speed = frequency/4 = 1/(4*period)
	E = desired - input speed
	-Execute control equation to determine next output
	-Adjust the power of the actuator to drive the error to zero. This is linearly related to 
	the error. 
		Input capture and PWM output
	*/
	icTime1 = TIMER1_TAR_R;	//DEBUGGING
	Speed = 800000000/(Period); //
	E = desiredSpeed - Speed;
	
	//*******DEBUGGING*************************
	if(eIdx<100)
	{
		errorArray[eIdx] = E;
		eIdx++;
	}
	//*****************************************
	
	Ui = Ui-1 + (12*E)/256;	//Discrete Integral
	if(Ui<100) Ui = 100;
	if(Ui>39900) Ui = 39900;
	
	Up = 13*E/256;	//Proportional
	
	U = Up+Ui;
	//U = U+(3*E)/64;
	if(U<100) U = 100;
	if(U>39900) U = 39900;
	setMotorDuty(U);
	icTime2 = TIMER1_TAR_R;	//DEBUGGING
	icTime = icTime1 - icTime2;	//DEBUGGING
	inputTime = inputTime; //DEBUGGING
}

void LCDInit(void)
{
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(0);
	ST7735_SetCursor(0,0);
	ST7735_OutString("MOTOR CONTROL");
	ST7735_SetCursor(0,1);
	ST7735_OutString("Des. RPS: ");
	ST7735_OutUDec(desiredSpeed*0.025);
	ST7735_PlotClear(32,159);
}

void ST7735_DecOut3(uint32_t n)
	{
		
		
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

	int j=0;
void outputSpeed(uint32_t speed)
{
	//y = Speed*(128/1600)+32; //speed*(range of points/range of speeds)
	ST7735_SetCursor(0,1);
	ST7735_OutString("Des. Speed: ");
	ST7735_OutUDec(desiredSpeed*0.025);
	ST7735_OutString(" rps");
	ST7735_SetCursor(0,2);
	ST7735_OutString("Act. Speed: ");	//Change this to crap in YouTube video
	uint32_t check = speed*0.025;
	if(check<100)
	{
		ST7735_OutUDec(speed*0.025);
	}
	ST7735_OutString(" rps");
	//ST7735_DecOut3(speed*0.025);
	//ST7735_PlotNextEraslotLine(desiredSpeed*0.025);
//	ST7735_PlotLine(desiredSpeed*0.025);
	ST7735_PlotPoints(128*desiredSpeed/4000+32, 128*speed/4000+32);
	j = (j+1)%128;
	if(j==0)
	{
		//Plot clear
		ST7735_PlotNextErase();
		//ST7735_PlotClear(32,159);
	}
	//ST7735_PlotNext();
}
// if desired interrupt frequency is f, Timer0A_Init parameter is busfrequency/f
#define F16HZ (80000000/16)
#define F20KHZ (80000000/20000)
uint16_t duty = 0;
//debug code
int main(void){ 
	//should start at 20rps
  PLL_Init(Bus80MHz);              // bus clock at 50 MHz
	LCDInit();
	buttonInit();
	PeriodMeasure_Init();
	Timer1_Init();
 /* SYSCTL_RCGCGPIO_R |= 0x20;       // activate port F
  while((SYSCTL_PRGPIO_R&0x0020) == 0){};// ready?
  GPIO_PORTF_DIR_R |= 0x0E;        // make PF3-1 output (PF3-1 built-in LEDs)
  GPIO_PORTF_AFSEL_R &= ~0x0E;     // disable alt funct on PF3-1
  GPIO_PORTF_DEN_R |= 0x0E;        // enable digital I/O on PF3-1
                                   // configure PF3-1 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;          // disable analog functionality on PF
  LEDS = 0;                        // turn all LEDs off*/
//  Timer0A_Init(&UserTask, F20KHZ);     // initialize timer0A (20,000 Hz)
	MotorInit(40000, 30000);
	//Timer0A_Init(&LCDInit, F16HZ);
  Timer2_Init(&IntegralController, F20KHZ);  // Integral Controller
	
	
  EnableInterrupts();
		//Set power delivered to controller using PWM
		//2 Buttons will increase/decrease speed. 
		//tachometer measures speed
		//

  while(1){
		
		uint32_t sw1 = GPIO_PORTE_DATA_R & 0x00000001;
		if(sw1 > 0){
			c++;
			//wait for unpress
			while(sw1 > 0){
				sw1 = GPIO_PORTE_DATA_R & 0x00000001;
			}
			c++;
			//Increase duty
			if(toggle==1)
			{
				int rps;
				desiredSpeed = desiredSpeed + 200;
				if(desiredSpeed>2400){desiredSpeed = 2400;}
				savedSpeed = desiredSpeed;
			}
			
		}
		uint32_t sw2 = GPIO_PORTE_DATA_R & 0x00000002;
		if(sw2 > 0){
			//wait for unpress
			while(sw2 > 0){
				sw2 = GPIO_PORTE_DATA_R & 0x00000002;
			}
			d++;
			//Decrease duty
			if(toggle==1)
			{
				desiredSpeed = desiredSpeed - 200;
				if(desiredSpeed<1000){desiredSpeed=1000;}
			}
		}
		//***DEBUGGING*************************************
		//Average Error
		if(eIdx == 100)
		{
			for(int i = 0; i<100; i++)
			{
				average = average + (double)errorArray[i];
			}
			average = average/100;
		}
		//*************************************************
		outputSpeed(Speed);
  }
}
