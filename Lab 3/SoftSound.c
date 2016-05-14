// 0.Documentation Section 
// SoftSound.c
// Runs on LM4F120 or TM4C123
// Use the SysTick timer to request interrupts at 500Hz.
// Jonathan Valvano
// November 3, 2013

// PA5---1.5k---headphones---ground 
// To make sound we need to create an oscillating wave.
// 1. Pre-processor Directives Section
#include "PLL.h"
#include "Button.h"

// Constant declarations to access port registers using 
// symbolic names instead of addresses
#define GPIO_PORTA_DATA_R       (*((volatile unsigned long *)0x400043FC))
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_AMSEL_R      (*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4000452C))
#define GPIO_PORTA_DR8R_R       (*((volatile unsigned long *)0x40004508))

#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define NVIC_SYS_PRI3_R         (*((volatile unsigned long *)0xE000ED20))  // Sys. Handlers 12 to 15 Priority
#define NVIC_ST_CTRL_R          (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile unsigned long *)0xE000E018))

// 2. Declarations Section
//   Global Variables

//   Function Prototypes
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void Sound_Init(void);        // start sound output


// 3. Subroutines Section
// MAIN: Mandatory for a C Program to be executable
int main(void){
  PLL_Init();                 // 80 MHz
  //Sound_Init();               // initialize output and interrupts
	Button_Init();
  EnableInterrupts();
  while(1){                   // interrupts every 500us
		//check iinputs poll interrupt for buttons?
		//display state
		//go to next state
    WaitForInterrupt();
  }
}




