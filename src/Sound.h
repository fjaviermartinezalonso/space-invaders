// Sound.h
// Runs on TM4C123 or LM4F120
// Prototypes for basic functions to play sounds from the
// original Space Invaders.
// Jonathan Valvano
// November 19, 2012

#include "..//tm4c123gh6pm.h"

void Sound_Init(void);
void Sound_Play(const unsigned char *pt, unsigned long count);

void Sound_Shoot(void);
void Sound_Killed(void);
void Sound_Explosion(void);

void Sound_Fastinvader1(void);
void Sound_Fastinvader2(void);
void Sound_Fastinvader3(void);
void Sound_Fastinvader4(void);
void Sound_Highpitch(void);

void Timer2_Init(unsigned long period);
void Timer2A_Stop(void);
void Timer2A_Start(void);
