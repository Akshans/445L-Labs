#include "..//inc//tm4c123gh6pm.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "PLL.h"
//#include "Timer0A.h"
#include "MAX5353.h"
#include "Button.h"
#include "Joystick.h"
#include "ST7735.h"
#include "Menu.h"
#include "Timer2.h"
#include "Timer3.h"
#include "Systick.h"
#include "Game.h"
#include "math.h"

#define TIME 30 //30
#define PRECTIME 4
#define ENEMIES 70
#define PRECENEMIES 10
const unsigned short enemyTarget[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x3900, 0x9AA0, 0xE3E0, 0xE3C0, 0xE3C0, 0xE3C0, 0xE3E0, 0x9AA0, 0x30E0, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1860, 0xCB80, 0xE3C0, 0xE3C0, 0xE3C0, 0xE422, 0xE422, 0xE422, 0xE3E0, 0xE3C0,
 0xE3C0, 0xD3A0, 0x1860, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x30E0, 0xE3C0, 0xE3C0, 0xEC43, 0xFE95, 0xFFFF, 0xFFFF, 0xFFFF,
 0xFFFF, 0xFFFF, 0xFE95, 0xEC43, 0xE3C0, 0xE3C0, 0x28A0, 0x0000, 0x0000, 0x0000, 0x1860, 0xE3C0, 0xE3C0, 0xF5AD, 0xFFFF, 0xFFFF,
 0xFFDE, 0xFF7C, 0xFF7C, 0xFF7C, 0xFFDE, 0xFFFF, 0xFFFF, 0xF58D, 0xE3C0, 0xE3C0, 0x1040, 0x0000, 0x0000, 0xC340, 0xE3C0, 0xF5AE,
 0xFFFF, 0xFFFF, 0xF58D, 0xE3A0, 0xE3C0, 0xE3C0, 0xE3C0, 0xE3A0, 0xF58C, 0xFFFF, 0xFFFF, 0xF5AD, 0xE3C0, 0xE3C0, 0x0000, 0x4120,
 0xE3C0, 0xEC64, 0xFFFF, 0xFFFF, 0xE422, 0xE3C0, 0xE3E0, 0xE422, 0xE422, 0xE422, 0xE3E0, 0xE3C0, 0xE401, 0xFFFF, 0xFFFF, 0xE401,
 0xE3C0, 0x1880, 0x9A80, 0xE3C0, 0xF694, 0xFFFF, 0xF5AE, 0xE3C0, 0xEC64, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xEC43, 0xE3C0,
 0xED2A, 0xFFFF, 0xFF18, 0xE3C0, 0xB300, 0xCB60, 0xE3A0, 0xFFFF, 0xFFFF, 0xE380, 0xE3E0, 0xFFFF, 0xFFFF, 0xFF39, 0xFF39, 0xFF18,
 0xFFFF, 0xFFDE, 0xE3C0, 0xE3E0, 0xFFFF, 0xFFFF, 0xE3E0, 0xE3C0, 0xE3E0, 0xE422, 0xFFFF, 0xFF5A, 0xE3A0, 0xECA6, 0xFFFF, 0xFF18,
 0xE3C0, 0xE3C0, 0xE3C0, 0xFED6, 0xFFFF, 0xEC64, 0xE3C0, 0xFFFF, 0xFFFF, 0xE3E0, 0xE3C0, 0xE3E0, 0xE422, 0xFFFF, 0xFF5A, 0xE3A0,
 0xECA6, 0xFFFF, 0xFF19, 0xE3C0, 0xE3C0, 0xE3C0, 0xFED7, 0xFFFF, 0xEC63, 0xE3C0, 0xFFFF, 0xFFFF, 0xE3E0, 0xE3C0, 0xE3E0, 0xE422,
 0xFFFF, 0xFF5A, 0xE3A0, 0xECA6, 0xFFFF, 0xFF19, 0xE3C0, 0xE3C0, 0xE3C0, 0xFED7, 0xFFFF, 0xEC63, 0xE3C0, 0xFFFF, 0xFFFF, 0xE3E0,
 0xE3C0, 0xCB60, 0xE3A0, 0xFFFF, 0xFFFF, 0xE380, 0xE3E0, 0xFFFF, 0xFFFF, 0xFF39, 0xFF39, 0xFF39, 0xFFFF, 0xFFDE, 0xE3C0, 0xE3E0,
 0xFFFF, 0xFFFF, 0xE3E0, 0xE3C0, 0x9280, 0xE3C0, 0xF694, 0xFFFF, 0xF5AE, 0xE3C0, 0xEC64, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
 0xEC43, 0xE3C0, 0xED4A, 0xFFFF, 0xFF18, 0xE3C0, 0xB300, 0x4120, 0xE3C0, 0xEC64, 0xFFFF, 0xFFFF, 0xEC42, 0xE3C0, 0xE3E0, 0xEC84,
 0xEC84, 0xEC84, 0xE3E1, 0xE3C0, 0xE401, 0xFFFF, 0xFFFF, 0xE401, 0xE3C0, 0x2080, 0x0000, 0xC340, 0xE3C0, 0xF5AE, 0xFFFF, 0xFFFF,
 0xF5AE, 0xE380, 0xE3A0, 0xE3A0, 0xE3A0, 0xE3A0, 0xF5AD, 0xFFFF, 0xFFFF, 0xF5AD, 0xE3C0, 0xDBC0, 0x0000, 0x0000, 0x1880, 0xE3E0,
 0xE3C0, 0xF5AE, 0xFFFF, 0xFFFF, 0xFFDE, 0xFF5A, 0xFF5A, 0xFF5A, 0xFFDE, 0xFFFF, 0xFFFF, 0xF5AD, 0xE3C0, 0xE3C0, 0x1040, 0x0000,
 0x0000, 0x0000, 0x30E0, 0xE3E0, 0xE3C0, 0xEC64, 0xF694, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF694, 0xEC64, 0xE3C0, 0xE3C0,
 0x30C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1880, 0xC360, 0xE3C0, 0xE3C0, 0xE3A0, 0xE422, 0xE422, 0xE422, 0xE3A0, 0xE3C0,
 0xE3C0, 0xCB60, 0x1880, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x4120, 0x9280, 0xCB60, 0xE3E0, 0xE3E0,
 0xE3C0, 0xCB60, 0x9280, 0x4120, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,

};

const unsigned short black[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,

};
extern volatile int gameFlag;
extern volatile int timeMode;
extern volatile int precisionMode;
extern volatile int seedFlag;
extern volatile uint32_t button;
extern volatile uint32_t cursor;
volatile uint32_t seed;
volatile uint32_t ADCvalue[2];
int gameOn = 1;
int x, y;
int16_t bitmapX, bitmapY;
int xPrev = 0, yPrev = 0;
//int crosshairPrevX[28];
//int crosshairPrevY[28];
int crosshairX[48];
int crosshairY[48];
int enemyX[61];
int enemyY[61];
int enemyIdx = 0;
int deltaX = 0, deltaY = 0, deltaMax = 0;
//uint32_t ADCvalue[2];
int numEnemies = 20;
int enemiesKilled = 0;
int deltaX;
int deltaY;
int enemyCenterX[70];
int enemyCenterY[70];
int enemyState[70];
int pointerArray[3];
//int pointerA, pointerB, pointerC;
int highScoreTime[5] = {0,0,0,0,0};
int highScorePrec[5] = {0,0,0,0,0};
volatile int timer = TIME;
int precScore = 0;
int score = 0;


	
	void Gameplay(void){
		JoystickControl();
		//CheckEnemyDefeat();
		OutputState();
		CheckGameEnd();
		//SysTick_Wait(1064000);
	}
	//Timer3 Handler function
	//Draw enemy if timer is zero, delete enemy, draw new one
	void Timer3Code(void){
		//if its a second. update the timer
		if(timeMode == 1 && timer != 0){
			timer--;
		}
		else if(precisionMode==1){
			timer--;
			if(timer<0){
				timer = PRECTIME;
				enemyIdx++;
			}
		}
	}
	void CheckGameEnd(){
		cursor = 0;
		if(timeMode == 1){
			CheckGameEndTime();
		}
		else if(precisionMode == 1){
			CheckGameEndPrec();
		}
	}
	void CheckGameEndTime(void){
		if(timer<=0){
				timeMode = 0;
				menuInit();
				gameFlag = 0;
				seedFlag = 0;
				score = enemiesKilled;
				for(int i = 0; i<5; i++){
					if(highScoreTime[i] == 0){
						highScoreTime[i] = enemiesKilled;
						break;
					}
				}
				setMenu(3);
			}
	}
	void CheckGameEndPrec(void){
		if(enemyIdx==10){	//or 9 idk it might error
				precisionMode = 0;
				menuInit();
				gameFlag = 0;
				seedFlag = 0;
				score = precScore;
				for(int i = 0; i<5; i++){
					if(highScoreTime[i] == 0){
						highScoreTime[i] = enemiesKilled;
						break;
					}
				}
				setMenu(3);
			}
	}
	//Misleading name. More like Game Init because it resets timer and scores
	void SpriteInit(void){
		enemiesKilled = 0;
		srand(seed);
		if(timeMode == 1){
			TimeModeInit();
		}
		else if(precisionMode == 1){
			PrecisionModeInit();
		}
		x = rand()%128;
		y = rand()%153+7;
		UpdateCrosshair(x,y);
	}
	
	void TimeModeInit(void){
		timer = TIME;
		numEnemies = ENEMIES;
		enemyIdx = 0; deltaX = 0; deltaY = 0;
		//Make the first 3 enemies show up on board
		for(int i = 0; i<3; i++){
			pointerArray[i]=i;
			
			
		}
		enemyCenterX[0] = rand()%110 + 9;		//9- 119
		enemyCenterY[0] = rand()%120+19;	//so that the whole enemy stays in the board//19-140
		enemyState[0] = 1;
		for(int i = 1; i<numEnemies; i++){
			enemyCenterX[i] = rand()%110 + 9;		//9- 119
			enemyCenterY[i] = rand()%120+19;	//so that the whole enemy stays in the board//19-140
			//range of Y: 
			enemyState[i] = 0;
		}
		//First three should be collsion free
		for(int i = 0; i<3; i++){
			CollisionAvoidance(i);
			enemyState[i] = 1;
		}
	
	}
	void PrecisionModeInit(){
		timer = PRECTIME;
		numEnemies = PRECENEMIES;
		precScore = 0;
		enemyIdx = 0;
		//Initialize first enemy
		enemyCenterX[0] = rand()%128;
		enemyCenterY[0] = rand()%149+11;	//so that the whole enemy stays in the board
		enemyState[0] = 1;
		for(int i = 1; i<numEnemies; i++){
			deltaX = abs(enemyCenterX[i] - enemyCenterX[i-1]);
			deltaY = abs(enemyCenterY[i] - enemyCenterY[i-1]);
			enemyCenterX[i] = rand()%110 + 9;		//9- 119
			enemyCenterY[i] = rand()%120+19;	//so that the whole enemy stays in the board//19-140
			//range of Y: 
			enemyState[i] = 0;
			if(deltaX <=19 && deltaY<=19){
				i--;
			}
			//ST7735_DrawPixel(enemyX[i],enemyY[i],ST7735_Color565(0,0,255));	//I might delete this later. 
		}
	}
	
	void CheckEnemyDefeat(void){
		
			if(timeMode == 1){
					CheckEnemyDefeatTime();
			}
			else if(precisionMode == 1){
					CheckEnemyDefeatPrec();
			}
	}
	//
	void CheckEnemyDefeatTime(void){
		for(int i = 0; i<3; i++){
			if(enemyState[pointerArray[i]] == 1){
				deltaX = abs(enemyCenterX[pointerArray[i]] - x);
				deltaY = abs(enemyCenterY[pointerArray[i]] - y);
			}
			if((deltaX <= 6) && (deltaY <=6)){
			//Color enemy black, but don't draw. Draw later. 
				enemyState[pointerArray[i]] = 2;	//2 means killed
				enemiesKilled++;
				
			}	
		}
		
	}
	void CheckEnemyDefeatPrec(void){
		if(enemyState[enemyIdx] == 1){
			deltaX = abs(enemyCenterX[enemyIdx] - x);
			deltaY = abs(enemyCenterY[enemyIdx] - y);
		}
		if(deltaX<=deltaY){
						deltaMax = deltaY;
		}else{
			deltaMax = deltaX;
		}
		if((deltaX < 9) && (deltaY <9)){
		//Color enemy black, but don't draw. Draw later. 
			enemyState[enemyIdx] = 2;
			enemiesKilled++;
			switch(deltaMax){
				case 0:
					precScore = precScore+9;
				break;
				case 1:
					precScore = precScore+8;
				break;
				case 2:
					precScore = precScore+7;
				break;
				case 3:
					precScore = precScore+6;
				break;
				case 4:
					precScore = precScore+5;
				break;
				case 5:
					precScore = precScore+4;
				break;
				case 6:
					precScore = precScore+3;
				break;
				case 7:
					precScore = precScore+2;
				break;
				case 8:
					precScore = precScore+1;
				break;
				default:
					precScore = 42;
				break;
			}
		}		

	}
	//I DELETED THE DELAY
	void JoystickControl(void){
		//UP: ADC1 0	DOWN: ADC1 4095	LEFT: ADC0 4095	RIGHT: 0	STILL: ADC0 [2024-2026] ADC1 [2026-2030]
		//UP: Y 	 0	DOWN: Y		 159	LEFT: X		 0		RIGHT: 126
		//Do I change the frequency of the timer or the pixel speed. 
		//MIN FREQUENCY: 
		JoystickHandler(ADCvalue);
		//Delay(678); 
		xPrev = x;
		yPrev = y;
		//data0 = Y: Down: 0-2032, Up: 2032-4095 LCD Y: 0-160, Y: 
		//data1 = X: 0-2025, 2025-4095 LCD X: 0-128		X: -2025-0, 0-2025
		//x = x+		
		if (ADCvalue[1] >3000)
		{
			x=(x-1);
			if(x<0){
				x = 128;
			}
			if(ADCvalue[1] >4050){
				x=(x-1);
				if(x<0){
					x = 128;
				}
			}
			
		}
		else if(ADCvalue[1] <1000)
		{
			x=(x+1)%128;
			//ST7735_DrawPixel(xPrev,yPrev,ST7735_Color565(0,0,0));
			if(ADCvalue[1] <300){
				x=(x+1)%128;
			}
		}
		
		if(ADCvalue[0] >3000)
		{
			y = y+1;
			if(y>159){
				y = 7;
			}
			if(ADCvalue[0]>4050){
				y = y+1;
				if(y>159){
					y = 7;
				}
			}
			//ST7735_DrawPixel(xPrev,yPrev,ST7735_Color565(0,0,0));
		}
		else if(ADCvalue[0] <1000)
		{
			y=(y-1);
			if(y<7){
				y=159;
			}
			if(ADCvalue[0]<300){
				y=(y-1);
				if(y<7){
					y=159;
				}
			}
			//ST7735_DrawPixel(xPrev,yPrev,ST7735_Color565(0,0,0));
		}
		//ST7735_DrawPixel(x,y,ST7735_Color565(255,0,0));	
	}
	
	//updates the crosshair array to the next one
	//void UpdateEnemy(void){
	//}
	void UpdateCrosshair(int x, int y/*, int* crosshairX, int* crosshairY*/){
		crosshairX[0] = x-2;
		crosshairY[0] = y-4;
		crosshairX[1] = x-1;
		crosshairY[1] = y-4;
		crosshairX[2] = x;
		crosshairY[2] = y-4;
		crosshairX[3] = x;
		crosshairY[3] = y-4;
		crosshairX[4] = x+1;
		crosshairY[4] = y-4;
		crosshairX[5] = x+2;
		crosshairY[5] = y-4;
		
		crosshairX[6] = x-2;
		crosshairY[6] = y+4;
		crosshairX[7] = x-1;
		crosshairY[7] = y+4;
		crosshairX[8] = x;
		crosshairY[8] = y+4;
		crosshairX[9] = x;
		crosshairY[9] = y+4;
		crosshairX[10] = x+1;
		crosshairY[10] = y+4;
		crosshairX[11] = x+2;
		crosshairY[11] = y+4;
		
		crosshairX[12] = x-4;
		crosshairY[12] = y-2;
		crosshairX[13] = x-4;
		crosshairY[13] = y-1;
		crosshairX[14] = x-4;
		crosshairY[14] = y;
		crosshairX[15] = x-4;
		crosshairY[15] = y+1;
		crosshairX[16] = x-4;
		crosshairY[16] = y+2;
		crosshairX[17] = x-4;	//This is extra
		crosshairY[17] = y+2;
		//right beam
		for(int i = 0; i<5; i++){
			crosshairX[18+i] = x+4;
			crosshairY[18+i] = y-2+i;
		}
		crosshairX[23] = x-6;
		crosshairY[23] = y;
		crosshairX[24] = x-5;
		crosshairY[24] = y;
		crosshairX[25] = x+5;
		crosshairY[25] = y;
		crosshairX[26] = x+6;
		crosshairY[26] = y;
		
		crosshairX[27] = x;
		crosshairY[27] = y-6;
		crosshairX[28] = x;
		crosshairY[28] = y-5;
		crosshairX[29] = x;
		crosshairY[29] = y+5;
		crosshairX[30] = x;
		crosshairY[30] = y+6;
		//left middle
		for(int i = 0; i<3; i++){
			crosshairX[i+31] = x-3+i;
			crosshairY[i+31] = y;
		}
		//right middle
		for(int i = 0; i<3; i++){
			crosshairX[i+34] = x-i+3;
			crosshairY[i+34] = y;
		}
		//topp middle
		for(int i = 0; i<3; i++){
			crosshairX[i+37] = x;
			crosshairY[i+37] = y-3+i;
		}
		//bottom middle
		for(int i = 0; i<3; i++){
			crosshairX[i+40] = x;
			crosshairY[i+40] = y-i+3;
		}
		//weird diagonals
		crosshairX[43] = x-3;
		crosshairY[43] = y-3;
		crosshairX[44] = x-3;
		crosshairY[44] = y+3;
		crosshairX[45] = x+3;
		crosshairY[45] = y-3;
		crosshairX[46] = x+3;
		crosshairY[46] = y+3;
		
		
	
		
	
		
		FixEdges();
	}
	
	void UpdateEnemy(int x, int y){
		//x-9, y+9
		bitmapX = x-9; bitmapY = y+9;
		ST7735_DrawBitmap(bitmapX, bitmapY, enemyTarget, 19, 19);
		//x and y are the center of the enemies
		//top row 
		/*for(int i = 0; i<3; i++){
			enemyX[i] = x-1+i;
			enemyY[i] = y-4;
		}
		//2+49	indexes 3-51 MIddle square
		for(int i = 0; i<7; i++){
			for(int j = 0; j<7; j++){
			enemyX[j*7+i+3] = x-3+i;
			enemyY[j*7+i+3] = y-3+j;
			}
		}
		//left column
		for(int i = 0; i<3; i++){
			enemyX[i+52] = x-4;
			enemyY[i+52] = y-1+i;
		}
		//right column
		for(int i = 0; i<3; i++){
			enemyX[i+55] = x+4;
			enemyY[i+55] = y-1+i;
		}
		//bottom row
		for(int i = 0; i<3; i++){
			enemyX[i+58] = x-1+i;
			enemyY[i+58] = y+4;
		}*/
	}
	void DeleteEnemy(int x, int y){
		bitmapX = x-9; bitmapY = y+9;
		ST7735_DrawBitmap(bitmapX, bitmapY, black, 19, 19);
	}
	void FixEdges(void){
		for(int i = 0; i<48; i++){
			if(crosshairY[i]<7){
				crosshairY[i]=159;
			}
			if(crosshairY[i]>159){
					crosshairY[i] = 7;
				}
			if(crosshairX[i]<0){
				crosshairX[i] = 128;
			}
		}
	}
	/*Draws enemies but doesn't if the state is 1, meaning they're dead. 
		
	*/
	//
	/*Problem: Need to find out how enemy looks and plug that into a data structure. */
	/*/Go through each center, check if it's -1, color black if it is. If it's blue don't mess with it. 
	//1 target pops up. 3 secs to click. Where you click determines score. Repeat 10 times for ten targets. 
	/*/
	void CollisionAvoidance(int j){
		deltaX = 0; deltaY = 0;
		//Collision avoidance
		for(int k = 0; k<3; k++){
			if(pointerArray[k]!=j){
				//check if there is collision
				deltaX = abs(enemyCenterX[pointerArray[k]] - enemyCenterX[j]);
				deltaY = abs(enemyCenterY[pointerArray[k]] - enemyCenterY[j]);
				if(deltaX<=25 && deltaY<=25){
					enemyCenterX[j] = rand()%110 + 9;		//9- 119
					enemyCenterY[j] = rand()%120+19;	//so that the whole enemy stays in the board//19-140
					//Check if new value collides
					deltaX = abs(enemyCenterX[pointerArray[k]] - enemyCenterX[j]);
					deltaY = abs(enemyCenterY[pointerArray[k]] - enemyCenterY[j]);
					if(deltaX<=25 && deltaY<=25){
						k--;
					}
					
				}
			}
		}
	}
	void DrawEnemies(void){
		if(timeMode == 1){
			DrawEnemiesTime();
		}
		else if(precisionMode == 1){
			DrawEnemiesPrec();
		}
	}
	void DrawEnemiesTime(){
		for(int e = 0; e<3; e++){
			//UpdateEnemy(enemyCenterX[pointerArray[e]],enemyCenterY[pointerArray[e]]);
			/*If enemy state is 1, draw a blue enemy*/
			if(enemyState[pointerArray[e]] == 1){
				/*for(int i = 0; i<61; i++){
					ST7735_DrawPixel(enemyX[i],enemyY[i],ST7735_Color565(0,0,255));
				}*/
				UpdateEnemy(enemyCenterX[pointerArray[e]],enemyCenterY[pointerArray[e]]);
			}
			/*If it's 2, draw a black circle and set state to 3. 3 is dead and don't draw. */
			else if(enemyState[pointerArray[e]] == 2) {
				/*for(int i = 0; i<61; i++){
					ST7735_DrawPixel(enemyX[i],enemyY[i],ST7735_Color565(0,0,0));
				}*/
				DeleteEnemy(enemyCenterX[pointerArray[e]],enemyCenterY[pointerArray[e]]);
				enemyState[pointerArray[e]] = 3;	//3 means dead and don't draw. 2 is dead and draw. 
				//Move pointer of killed enemy to the next available one
				for(int j = (pointerArray[e]+1); j<ENEMIES; j++){
					if(enemyState[j] == 0){
						pointerArray[e] = j;
						enemyState[j] = 1;
						CollisionAvoidance(j);
						break;
					}
					
				}
				
				
			}
		}
	}
	//Target pops up, 3 secs to click it, After 3 seconds new target pops up
	void DrawEnemiesPrec(){
		/*State: 0=black, not shot	1=blue and alive	2=black and shot
			I want to put this first if statement in the Timer3 Handler, but it wouldn't update enemyState there*/
		if(enemyState[enemyIdx]==0){
			enemyState[enemyIdx]=1;
		}
		if(enemyIdx>0){
			//Delete old enemy if you ran out of time
			if(enemyState[enemyIdx-1] == 1){
				enemyState[enemyIdx-1] =0;
				DeleteEnemy(enemyCenterX[enemyIdx-1],enemyCenterY[enemyIdx-1]);
				/*UpdateEnemy(enemyCenterX[enemyIdx-1],enemyCenterY[enemyIdx-1]);
				for(int i = 0; i<61; i++){
					ST7735_DrawPixel(enemyX[i],enemyY[i],ST7735_Color565(0,0,0));
				}*/
			}
		}
		//Draw new enemy or shot enemy
		//UpdateEnemy(enemyCenterX[enemyIdx],enemyCenterY[enemyIdx]);
		if(enemyState[enemyIdx] == 1){
			UpdateEnemy(enemyCenterX[enemyIdx],enemyCenterY[enemyIdx]);
			/*for(int i = 0; i<61; i++){
				ST7735_DrawPixel(enemyX[i],enemyY[i],ST7735_Color565(0,0,255));
			}*/
		}
		else if(enemyState[enemyIdx] == 2){
			DeleteEnemy(enemyCenterX[enemyIdx],enemyCenterY[enemyIdx]);
			/*for(int i = 0; i<61; i++){
				ST7735_DrawPixel(enemyX[i],enemyY[i],ST7735_Color565(0,0,0));
			}*/
			timer = PRECTIME;
			enemyIdx++;
		}
		
	}
	void DrawText(void){
		if(timeMode == 1){
			DrawTextTime();
		}
		else if(precisionMode == 1){
			DrawTextPrec();
		}
	}
	void DrawTextTime(){
		ST7735_SetCursor(0,0);
		ST7735_OutString("Score: ");
		ST7735_OutUDec(enemiesKilled);
		ST7735_OutString(" Time: ");
		if(timer <= 0){
			ST7735_OutString(" ");
			ST7735_OutUDec(0);
		}
		else{
			if(timer<10){
				ST7735_OutString(" ");
			}
			ST7735_OutUDec(timer);
		}
	}
	void DrawTextPrec(){
		ST7735_SetCursor(0,0);
		ST7735_OutString("Score: ");
		if(precScore<10){
			ST7735_OutString(" ");
		}
		ST7735_OutUDec(precScore);
		ST7735_OutString(" Time: ");
		if(timer>=0){
			ST7735_OutUDec(timer);
		}
	}
		
	void DrawCrosshair(void){
		if(xPrev!=x || yPrev!=y){
			ST7735_SetCursor(xPrev,yPrev);
			UpdateCrosshair(xPrev,yPrev);
			for(int i = 0; i<47; i++){
				ST7735_DrawPixel(crosshairX[i],crosshairY[i],ST7735_Color565(0,0,0));	//Color black previous joystick spot
			}
		}
		ST7735_SetCursor(x,y);
		UpdateCrosshair(x,y);
		for(int i = 0; i<47; i++){
			ST7735_DrawPixel(crosshairX[i],crosshairY[i],ST7735_Color565(255,0,0));	//Color black previous joystick spot
		}
	}
	void OutputState(void){
		DrawText();
		DrawEnemies();
		DrawCrosshair();
	}
	
	uint32_t Gameover(void){
		return score;
	}