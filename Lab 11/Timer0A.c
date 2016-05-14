// Timer0A.c
// Runs on LM4F120/TM4C123
// Use Timer0A in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers"
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015
   Volume 1, Program 9.8

  "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Volume 2, Program 7.5, example 7.6

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
#include <stdint.h>
#include "..//inc//tm4c123gh6pm.h"
#include "MAX5353.h"

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
#define F 2237   // 349.228 Hz			
#define GF 2112   // 369.994 Hz			
#define G 1993   // 391.995 Hz			
#define AF 1881   // 415.305 Hz			
#define A 1776   // 440.000 Hz			
#define BF 1676   // 466.164 Hz			
#define B 1582   // 493.883 Hz			
#define C 1493   // 523.251 Hz			
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

#define PF2       (*((volatile uint32_t *)0x40025010))
volatile int noteISave;
volatile int isPlaying;
volatile int songISave;
volatile int soundISave;
volatile int playPause;
/*extern volatile int noteISave;
extern volatile int songISave;
extern volatile int isPlaying;
extern volatile int soundISave;*/

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void (*PeriodicTask)(void);   // user function

uint16_t Time;

const unsigned short Trumpet64[64]={
  1024,1122,1219,1314,1407,1495,1580,1658,1731,1797,1855,
  1906,1948,1981,2005,2019,2024,2019,2005,1981,1948,1906,
  1855,1797,1731,1658,1580,1495,1407,1314,1219,1122,1024,
  926,829,734,641,553,468,390,317,251,193,142,
  100,67,43,29,24,29,43,67,100,142,193,
  251,317,390,468,553,641,734,829,926
};
const unsigned short Bassoon64[64]={
1068, 1169, 1175, 1161, 1130, 1113, 1102, 1076, 1032, 985, 963, 987, 1082, 1343, 1737, 1863, 
1575, 1031, 538, 309, 330, 472, 626, 807, 1038, 1270, 1420, 1461, 1375, 1201, 1005, 819, 658, 
532, 496, 594, 804, 1055, 1248, 1323, 1233, 1049, 895, 826, 826, 850, 862, 861, 899, 961, 1006, 
1023, 1046, 1092, 1177, 1224, 1186, 1133, 1098, 1102, 1109, 1076, 1027, 1003};

const unsigned short Instruments[2][64] = {{
  1024,1122,1219,1314,1407,1495,1580,1658,1731,1797,1855,
  1906,1948,1981,2005,2019,2024,2019,2005,1981,1948,1906,
  1855,1797,1731,1658,1580,1495,1407,1314,1219,1122,1024,
  926,829,734,641,553,468,390,317,251,193,142,
  100,67,43,29,24,29,43,67,100,142,193,
  251,317,390,468,553,641,734,829,926
},{
1068, 1169, 1175, 1161, 1130, 1113, 1102, 1076, 1032, 985, 963, 987, 1082, 1343, 1737, 1863, 
1575, 1031, 538, 309, 330, 472, 626, 807, 1038, 1270, 1420, 1461, 1375, 1201, 1005, 819, 658, 
532, 496, 594, 804, 1055, 1248, 1323, 1233, 1049, 895, 826, 826, 850, 862, 861, 899, 961, 1006, 
1023, 1046, 1092, 1177, 1224, 1186, 1133, 1098, 1102, 1109, 1076, 1027, 1003}};
// ***************** Timer0A_Init ****************
// Activate TIMER0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer0A_Init(uint32_t period){
	long sr;
  sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  //PeriodicTask = task;          // user function
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAILR_R = period-1;    // 4) reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
  EndCritical(sr);
}

int noteI = 0;

void Timer0A_Handler(void){
	PF2 ^= 0x04;
	TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer0A timeout
	//Time = (Time+1) & 0x1F;
	//b a g a b b b
	if(isPlaying==0){
		DAC_OutRaw(0);
		//noteISave = noteI;
		return;
	}
	noteISave = (noteISave + 1) & 0x3F;	//Ok 
	DAC_OutRaw(Instruments[soundISave][noteISave]);	//OK
		
  
 // (*PeriodicTask)();                // execute user task
}
