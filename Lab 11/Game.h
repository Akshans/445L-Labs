#include <stdlib.h>
#include <stdint.h>
void GameInit(void);
void Gameplay(void);					//calls all the gameplay methods.
void Timer3Code(void);
void TimeModeInit(void);
void PrecisionModeInit(void);
void JoystickCapture(void);
void JoystickControl(void);		// self explanatory
void SpriteInit(void);					//specifies the initial coordinates of the enemies
void CheckEnemyDefeat(void);	//updates enemy state
void CheckEnemyDefeatTime(void);
void CheckEnemyDefeatPrec(void);
void UpdateCrosshair(int x, int y/*, int* crosshairX, int* crosshairY*/);
void UpdateEnemy(int enemycenterX, int enemycenterY);
void DeleteEnemy(int x, int y);
void FixEdges(void);
void DrawCrosshair(void);
void DrawText(void);
void DrawTextTime(void);
void DrawTextPrec(void);
void CollisionAvoidance(int j);
void DrawEnemies(void);
void DrawEnemiesTime(void);
void DrawEnemiesPrec(void);
void OutputState(void);				
//void PortFInit(void);
void TimeTasks(void);
void SetTimer(void);
void CheckGameEnd();
void CheckGameEndTime(void);
void CheckGameEndPrec(void);

uint32_t Gameover(void);



