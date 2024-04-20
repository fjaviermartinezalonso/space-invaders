
// *************************** Space Invaders ***************************
#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "DAC.h"
#include "Switch.h"
#include "Sprites.h"
#include "Sound.h"
#include "Timer3.h"

#define enemyNumber 8
#define bunkerNumber 3
#define missileNumber 2

unsigned char special = 0;
unsigned char laser = 0;

typedef struct State {
  long x;      // x coordinate
  long y;      // y coordinate
	unsigned char width;
	unsigned char height;
  const unsigned char *image[2]; // two pointers to images
  unsigned long life;            // 0 = dead, 1+ = alive
	long points;									 // score points
} STyp;         

STyp Enemy[enemyNumber];
STyp EnemyGun[enemyNumber];
STyp EExplosion[enemyNumber];		 // entity for enemy explosion
STyp Bunker[3];
STyp PlayerShip;
STyp PlayerExplosion;
STyp Gun;
STyp Missile[2];

//unsigned long TimerCount;
unsigned long Semaphore;
unsigned char enemyDirection = 0x00;
unsigned long FrameCount = 0;
unsigned long playerScore = 0;
unsigned long lose = 0;		// you lose one life
unsigned long win = 0;		// all enemy destroyed! next level
unsigned long nextScreen = 0;
unsigned long level = 1;	// TODO: random, enemy fires more frequently
unsigned long enemyDead = 0; // how many enemies are dead?

void ParameterInit(unsigned long lives);
void MovePlayer(void);
void MoveShots(long speed);
void MoveEnemy(long speed);
void Draw(void);
void NewScreen(unsigned long lives, unsigned long score);
void Game_Over(unsigned long score);
//************  createPlayerShot  ******************
// First check if a shot is already in the game.
// Second check which button was pressed and create
// that normal/special shot
//**************************************************
void createPlayerShot(void);
void createEnemyShot(void);
unsigned char damaged(STyp* entity, STyp* shot);
void damageBunker(int number);
void damageEnemy(int number);
void checkGun(void);
void checkMissile(void);
void checkEnemyGun(void);
void checkEntities(void);
void updatePlayerExplosion(void);
void updateEExplosion(int number);

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Timer2_Init(unsigned long period);
void Delay100ms(unsigned long count); // time delay in 0.1 seconds
