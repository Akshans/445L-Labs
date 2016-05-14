#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include "ST7735.h"
#include "PLL.h"
#include "inc/tm4c123gh6pm.h"
#include "math.h"
#include "Menu.h"

int menuNum = 0;
uint32_t cursor = 0;
volatile int timeMode = 0;
volatile int precisionMode = 0;
volatile int gameFlag = 0;
volatile int highscoreFlag = 0;

uint32_t getMenu(void){
	/*
	MENUS
	0 = start screen
	1 = set mode
	2 = play game
	3 = high score
	
	4 = alarm on/off
	5 = set tone
	6 = set snooze
	
	*/
	return (uint32_t)menuNum;
}

void setMenu(int mNum){
	menuNum = mNum;
}

void menuInit(void){
		ST7735_FillScreen(0);
		ST7735_SetCursor(0,0);	
}

//START SCREEN
void titleScreen(void){
	ST7735_SetCursor(0,0);
	ST7735_OutString("BULLSEYE!!\r");
	/*if(cursor == 0){
		ST7735_OutString("1. Start >\r");
	}
	else{
		ST7735_OutString("1. Start  \r");
	}
	if(cursor == 1){
		ST7735_OutString("2. Set display >\r");
	}
	else{
		ST7735_OutString("2. Set display  \r");
	}
	if(cursor == 2){
		ST7735_OutString("3. Set alarm >\r");
	}
	else{
		ST7735_OutString("3. Set alarm  \r");
	}
	if(cursor == 3){
		ST7735_OutString("3. Alarm on/off >\r");
	}
	else{
		ST7735_OutString("3. Alarm on/off  \r");
	}
	if(cursor == 4){
		ST7735_OutString("4. Set tone >\r");
	}
	else{
		ST7735_OutString("4. Set tone  \r");
	}
	if(cursor == 5){
		ST7735_OutString("5. Set snooze >\r");
	}
	else{
		ST7735_OutString("5. Set snooze  \r");
	}*/
}

int mhours = -1;
int mminutes = -1;
//********GAME MODE*************************************************************************
void gameMode(void){
	//wire it to where if the joystick goes down, it goes to the next option and the button clicks it. 
	ST7735_SetCursor(0,0);
	
	ST7735_OutString("GAME MODE\r");
	ST7735_SetCursor(0,1);
	if(cursor == 1 )
	{
		if(timeMode == 0){
			ST7735_OutString("TIME MODE >\r");
		}else{
			ST7735_OutString("TIME MODE*>\r");
		}	
	}
	else
	{
		if(timeMode == 0){
			ST7735_OutString("TIME MODE \r");
		}else{
			ST7735_OutString("TIME MODE* \r");
		}	
	}
	ST7735_SetCursor(0,2);
	if(cursor == 2)
	{
		if(precisionMode == 0){
			ST7735_OutString("PRECISION MODE >\r");
		}else{
			ST7735_OutString("PRECISION MODE*>\r");
		}	
	}
	else
	{
		if(precisionMode == 0){
			ST7735_OutString("PRECISION MODE \r");
		}else{
			ST7735_OutString("PRECISION MODE*\r");
		}	
	}
	ST7735_SetCursor(0,4);
	if(cursor == 4 )
	{
		if(timeMode == 0){
			ST7735_OutString("");
		}else{
			ST7735_OutString("PLAY GAME>\r");
		}	
	}
	else
	{
		if(timeMode == 0){
			ST7735_OutString("");
		}else{
			ST7735_OutString("PLAY GAME \r");
		}	
	}
	
	
}

void setTimeMode(void){
	timeMode = 1;
	precisionMode = 0;
	ST7735_SetCursor(0,0);
	gameFlag = 0;
	highscoreFlag = 0;
}
void setPrecisionMode(void){
	timeMode = 1;
	precisionMode = 0;
	gameFlag = 0;
	highscoreFlag = 0;
}
//***********DIFFICULTY*************************************************************
void difficulty(void){
	//wire it to where if the joystick goes down, it goes to the next option and the button clicks it. 
	ST7735_SetCursor(0,0);
	
	ST7735_OutString("DIFFICULTY\r");
	if(cursor == 1)
	{
		ST7735_OutString("EASY >\r");
	}
	else
	{
		ST7735_OutString("EASY\r");
	}
	if(cursor == 2)
	{
		ST7735_OutString("MEDIUM >\r");
	}
	else
	{
		ST7735_OutString("MEDIUM\r");
	}
	if(cursor == 3)
	{
		ST7735_OutString("HARD >\r");
	}
	else
	{
		ST7735_OutString("HARD\r");
	}
}

//***********PLAY GAME SETTINGS*****************************************************
void setGameFlag(void){
	gameFlag = 1;
}
void highScoreScreen(void){
	//wire it to where if the joystick goes down, it goes to the next option and the button clicks it. 
	ST7735_SetCursor(0,0);
	
	ST7735_OutString("HIGH SCORE\r");
	if(cursor == 1)
	{
		ST7735_OutString("AKSHANS >\r");
	}
	else
	{
		ST7735_OutString("AKSHANS\r");
	}
	
}

/*BUTTONS
0 = click
1 = up
2 = down*/

void menuAct(uint32_t button){ 
	/*MENUS
	0 = start screen
	1 = set mode
	2 = play game
	3 = high score
	*/
	if(getMenu() == 0){ //START SCREEN~~~~~~~~
		menuMain();
		if(button == 0){
			setMenu(1);
			menuInit();
		}
	}
//DELETED ALARM CLOCK MENU = 8
	else if(getMenu() == 1){ //GAME MODE~~~~~~~~~~~~~~~
		//1) TIME MODE
		//2) PRECISION MODE
		gameMode();
		
		if(button == 1){	//UP
			//prolly could make it a switch case but its 4am rn
			if(cursor==1){
				if(timeMode == 0 && precisionMode == 0){
					cursor = 2;
				}
				else{
					cursor = 4;
				}
			}
			else if(cursor == 2){
				cursor = 1;
			}
			else if(cursor == 4){
				cursor = 2;
			}
		}	
		if(button == 2){	//DOWN
			
			if(cursor==1){
				cursor = 2;
			}
			else if(cursor == 2){
				if(timeMode == 0 && precisionMode == 0){
					cursor = 1;
				}
				else{
					cursor = 4;
				}
			}
			else if(cursor == 4){
				cursor = 1;
			}
		}		
		else if(button == 0){	//SELECT
			if(cursor==1){
				setTimeMode();
			}
			else if(cursor = 2){
				setPrecisionMode();
			}
			else{
				setMenu(2);
				menuInit();
			}
		}
	}
	else if(getMenu() == 2){ //PLAY GAME
		gameFlag = 1;
		
	}
}
