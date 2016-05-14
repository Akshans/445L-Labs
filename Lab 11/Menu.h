#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include "ST7735.h"
#include "PLL.h"
#include "inc/tm4c123gh6pm.h"
#include "math.h"
#include "Game.h"

uint32_t getMenu(void);
void setMenu(int mNum);
void menuInit(void);	//inits LCD
void titleScreen(void);	//LCD configs
void gameMode(void);	//game mode
void setTimeMode(void);
void setPrecisionMode(void);
void setGameFlag(void);
void difficulty(void);	//difficulty
//MISSING THE GAME
void highScoreScreen(void);
void menuAct(uint32_t button);




//start game, game mode 1)time mode 2) precision, difficulty 1)ez 2)min 3)hard, high score