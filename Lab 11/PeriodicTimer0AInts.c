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
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "PLL.h"
#include "Timer0A.h"
#include "MAX5353.h"
#include "Button.h"
#include "Joystick.h"
#include "ST7735.h"
#include "Menu.h"
#include "Timer2.h"
#include "Timer3.h"
#include "Systick.h"
#include "Game.h"

#define C_2 11945   // 65.406 Hz			
#define DF_1 11274   // 69.296 Hz			
#define D_1 10641   // 73.416 Hz			
#define EF_1 10044   // 77.782 Hz			
#define E_1 9480   // 82.407 Hz			
#define F_1 8948   // 87.307 Hz			
#define GF_1 8446   // 92.499 Hz			
#define G_1 7972   // 97.999 Hz			
#define AF_1 7525   // 103.826 Hz			
#define A_1 7102   // 110.000 Hz			
#define BF_1 6704   // 116.541 Hz			
#define B_1 6327   // 123.471 Hz			
#define C_1 5972   // 130.813 Hz			
#define DF0 5637   // 138.591 Hz			
#define D0 5321   // 146.832 Hz			
#define EF0 5022   // 155.563 Hz			
#define E0 4740   // 164.814 Hz			
#define F0 4474   // 174.614 Hz			
#define GF0 4223   // 184.997 Hz			
#define G0 3986   // 195.998 Hz			
#define AF0 3762   // 207.652 Hz			
#define A0 3551   // 220.000 Hz			
#define BF0 3352   // 233.082 Hz			
#define B0 3164   // 246.942 Hz			
#define C0 2986   // 261.626 Hz			
#define DF 2819   // 277.183 Hz			
#define D 2660   // 293.665 Hz			
#define EF 2511   // 311.127 Hz			
#define E 2370   // 329.628 Hz			
#define F 3579   // 349.228 Hz			
#define GF 2112   // 369.994 Hz			
#define G 3189   // 391.995 Hz			
#define AF 1881   // 415.305 Hz			
#define A 1776   // 440.000 Hz			
#define BF 2681   // 466.164 Hz			
#define B 1582   // 493.883 Hz			
#define C 2389   // 523.251 Hz			
#define DF1 1409   // 554.365 Hz			
#define D1 1330   // 587.330 Hz			
#define EF1 1256   // 622.254 Hz			
#define E1 1185   // 659.255 Hz			
#define F1 1119   // 698.456 Hz			
#define GF1 1056   // 739.989 Hz			
#define G1 997   // 783.991 Hz			
#define AF1 941   // 830.609 Hz			
#define A1 888   // 880.000 Hz			
#define BF1 838   // 932.328 Hz			
#define B1 791   // 987.767 Hz			
#define C1 747   // 1046.502 Hz			
#define DF2 705   // 1108.731 Hz			
#define D2 665   // 1174.659 Hz			
#define EF2 628   // 1244.508 Hz			
#define E2 593   // 1318.510 Hz			
#define F2 559   // 1396.913 Hz			
#define GF2 528   // 1479.978 Hz			
#define G2 498   // 1567.982 Hz			
#define AF2 470   // 1661.219 Hz			
#define A2 444   // 1760.000 Hz			
#define BF2 419   // 1864.655 Hz			
#define B2 395   // 1975.533 Hz			
#define C2 373   // 2093.005 Hz			

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

// which delays 3*ulCount cycles
#ifdef __TI_COMPILER_VERSION__
	//Code Composer Studio Code
	void Delay(uint32_t ulCount){
	__asm (	"loop    subs    r0, #1\n"
			"    bne     loop\n");
}

#else
	//Keil uVision Code
	__asm void
	Delay(uint32_t ulCount)
	{
    subs    r0, #1
    bne     Delay
    bx      lr
	}
#endif
	//*********LED/HEARTBEAT INIT**************************************************
	void PortFInit(){
	SYSCTL_RCGCGPIO_R |= 0x20;       // activate port F
  while((SYSCTL_PRGPIO_R&0x0020) == 0){};// ready?
  GPIO_PORTF_DIR_R |= 0x0E;        // make PF3-1 output (PF3-1 built-in LEDs)
  GPIO_PORTF_AFSEL_R &= ~0x0E;     // disable alt funct on PF3-1
  GPIO_PORTF_DEN_R |= 0x0E;        // enable digital I/O on PF3-1
                                   // configure PF3-1 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;          // disable analog functionality on PF
  LEDS = 0;                        // turn all LEDs off
	}
//***********GAMEPLAY************************************************************//
	//1. Title Screen. 2. Mode 3. Game 4. High Score. 
	//How to implement joystick 
extern volatile int gameFlag;
extern volatile int timeMode;
extern volatile int precisionMode;
extern volatile uint32_t button;
extern volatile uint32_t ADCvalue[2];
//uint32_t ADCvalue[2];
extern volatile uint32_t seed;

//***********DAC*****************************************************************//
struct Song {
    uint16_t  Wave[32];
    uint16_t  dt[32];
};
// this is a table of Timer delays needed to output
// the next sine point for the given note
// Each element corresponds to an actual note,
// starting with C_2 and ending with C2, by half steps
/*const unsigned short SineUpdateDelay[60] = {
11945,11274,10641,10044,9480,8948,8446,7972,7525,7102,6704
,6327,5972,5637,5321,5022,4740,4474,4223,3986,3762,3551,3352
,3164,2986,2819,2660,2511,2370,2237,2112,1993,1881,1776,1676,1582
,1493,1409,1330,1256,1185,1119,1056,997,941,888,838,791,747,705,665
,628,593,559,528,498,470,444,419,395
};*/

void UserTask(void){
  static int i = 0;
  LEDS = COLORWHEEL[i&(WHEELSIZE-1)];
  i = i + 1;
}
typedef struct Note {
    const uint32_t note;
    long  time;
} Note;
#define clockCycle 80000000
#define whole (clockCycle*2) //seconds
#define quarter (clockCycle*2/4)
#define eighth (clockCycle*2/8)
#define half (clockCycle*2/2)
#define dottedHalf (clockCycle*2*3/4)
//Note song[1] = {{G,eighth}};
Note song[34] = {{G,quarter},{F,quarter},{G,quarter},{F,eighth},{G,quarter},{F,eighth},{BF,half},{F,quarter},
{G,quarter},{F,quarter},{G,quarter},{F,eighth},{G,quarter},{F,eighth},{C,half},{G,quarter},
{G,quarter},{F,quarter},{G,quarter},{F,eighth},{G,quarter},{F,eighth},{BF,dottedHalf},
{G,eighth},{F,eighth},{G,eighth},{0,eighth},{G,eighth},{0,eighth},{G,quarter},{F,quarter},{F,eighth},{0,eighth},{F,quarter}};


extern volatile int noteISave;
extern volatile int songISave;
extern volatile int isPlaying;
extern volatile int soundISave;
extern volatile int playPause;
	


const unsigned short sine[64]={
  1024,1122,1219,1314,1407,1495,1580,1658,1731,1797,1855,
  1906,1948,1981,2005,2019,2024,2019,2005,1981,1948,1906,
  1855,1797,1731,1658,1580,1495,1407,1314,1219,1122,1024,
  926,829,734,641,553,468,390,317,251,193,142,
  100,67,43,29,24,29,43,67,100,142,193,
  251,317,390,468,553,641,734,829,926
};
void Timer1_Init(uint32_t period){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  //PeriodicTask = task;          // user function
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = period-1;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // 8) priority 5
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}
int songIndex = 0;
void Timer1A_Handler(void)
{
	//feed frequency value in timer0
	
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;
	if(songISave == 34){
		//songISave = 0;
		isPlaying = 0;
	return;}
	TIMER1_TAILR_R = song[songISave].time - 1;
	TIMER0_TAILR_R = song[songISave].note - 1;
	songISave++;
}
//************TIME FUNCTIONS*************************************************************
uint32_t first = 0;
uint32_t second = 0;

volatile int seedFlag = 0;
void TimeTask(){
	if(timeMode == 1){
		SetTimer();
	}
}
void SetTimer(void){
	int poop = 0;
}

//************JOYSTICK HANDLER*******************************************************
//UP: ADC0 0	DOWN: ADC0 4095	LEFT: ADC0 4095	RIGHT: 0
// if desired interrupt frequency is f, Timer0A_Init parameter is busfrequency/f
#define F16HZ (80000000/16)
#define F8HZ (80000000/16)
#define F20KHZ (80000000/20000)
//debug code
uint32_t xADCPrev = 2000;
uint32_t yFlag = 0;
double diff = 0;
void JoystickCapture(void){
	JoystickHandler(ADCvalue);		//If it's 0 when yprev was 2000 or when yprev was 0
	//1950 3980
	if(ADCvalue[0]>400 && ADCvalue[0] < 4080){
		yFlag = 0;
	}
	//0-2000 2050-4095
		//Up yADC = ADCvalue[0];
			//4085
			if(ADCvalue[0]>4085 &&gameFlag == 0){
				
			//	JoystickHandler(ADCvalue);
				Delay(100000); 
				if(yFlag == 0){	//Move joystick back to center after up-press
					yFlag = 1;
					menuAct(2);
				}
			}
		//Down yADC = ADCvalue[0];
			//15
			else if(ADCvalue[0]<290 && gameFlag == 0){
			//	JoystickHandler(ADCvalue);
					Delay(100000); 
			if(yFlag == 0){	//Move joystick back to center after down-press
					yFlag = 1;
					menuAct(1);
				}
			}
			else if(gameFlag == 0){
				menuAct(3);	//Don't send any workable button, just go to the menu. 
			}
}
//***********MAIN***************************************************************
uint32_t gameplayTime = 0, gameplayTime1 = 0, gameplayTime2 = 0;
int main(void){ 
  PLL_Init(Bus80MHz);              // bus clock at 80 MHz
	DAC_Init(sine[0]);
	buttonInit();
	JoystickInit();
	ST7735_InitR(INITR_REDTAB);
	SysTick_Init();
	//I DELETED SOME CODE INITIALIZING PORT F LEDS AND TIMER0A INIT STUFF. SEE LAB 5 FOR DETAILS
	PortFInit();
	//EnemyInit();
	Timer0A_Init(song[songISave].note);  // initialize timer0A (16 Hz)
	Timer3_Init(/*&TimeTask,F16HZ*/);
  isPlaying = 0;
	noteISave = 0;
	songISave = 0;
	soundISave = 0;
	EnableInterrupts();
 // playPause = 0; //play = 1, pause = 0
		
  while(1){
	
		PF1 ^= 0x02;
		first = TIMER3_TAR_R;
		uint32_t sw1 = GPIO_PORTE_DATA_R & 0x00000001;	
		//SWITCH
		if(sw1 > 0){
			//wait for unpress	
			while(sw1 > 0){
				sw1 = GPIO_PORTE_DATA_R & 0x00000001;
				if(gameFlag == 1){
					WaitForInterrupt();
				}	
			}
			
			if(seedFlag == 0){	
				second = TIMER3_TAR_R;
				seed = (first - second)%100;
				TIMER3_CTL_R = 0x00000000;
				seedFlag = 1;
			}
			if(gameFlag == 1)
			{
				//Precision mode/g
				isPlaying = 1;
				
				CheckEnemyDefeat();
				
				WaitForInterrupt();
				
				
			}	
				else{
				
					menuAct(0);
				}
		}
		
		if(gameFlag == 0){
			JoystickCapture();
		}
		else if (gameFlag == 1){
			TIMER3_CTL_R = 0x00000001;
			
			//first = TIMER3_TAR_R;
			Gameplay();
		}		
  }
}
