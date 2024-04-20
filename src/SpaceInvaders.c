// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the edX Lab 15
// In order for other students to play your game
// 1) You must leave the hardware configuration as defined
// 2) You must not add/remove any files from the project
// 3) You must add your code only this this C file
// I.e., if you wish to use code from sprite.c or sound.c, move that code in this file
// 4) It must compile with the 32k limit of the free Keil

// This virtual Nokia project only runs on the real board, this project cannot be simulated
// Instead of having a real Nokia, this driver sends Nokia
//   commands out the UART to TExaSdisplay
// The Nokia5110 is 48x84 black and white
// pixel LCD to display text, images, or other information.

// ******* Required Hardware I/O connections*******************
// PA1, PA0 UART0 connected to PC through USB cable
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

/*
*		****************			SPACE			INVADERS			*****************
*		Javier Martinez Alonso                               2017 05 23
*		
*		This game has 4 threads:
*		- main: initializes the game. After that, this controls the game flow
*						i.e. changes of screens if the player loses, wins or game over
*		- SysTick timer: controls all game variables: position, animations, I/O
*						switches and screen drawing. 30 Hz, but actualy it runs a bit
*						slower to compensate the higher speed of the simulated screen
*		- Timer2: exclusively sound output using the DAC, which is connected
*						to the phones. It runs at 11.025 kHz because of the frequencies
*						of the sound. 4-bit DAC
*		- Timer3: exclusively for LED output. This interrupt only executes the
*						"checkButtons" function at high speed to avoid Timer2 high freq
*						to disrupt LEDs (they can shine too weak)
*/

#include "SpaceInvaders.h"

void SysTick_Init(unsigned long period){
  NVIC_ST_CTRL_R = 0;           		// disable SysTick during setup
  NVIC_ST_RELOAD_R = period - 1;    // set reload register
  NVIC_ST_CURRENT_R = 0;        		// any write to current clears it
  NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R&0x00FFFFFF; // priority 0                
  NVIC_ST_CTRL_R |= 0x0007;  		    // enable with core clock and interrupts
}

void SysTick_Handler(void){ 
	
	special = GPIO_PORTE_DATA_R & 0x02;		// check buttons
	laser = GPIO_PORTE_DATA_R & 0x01;
	createPlayerShot(); // start a player shot (if needed)
	createEnemyShot();  // start a random enemy shot
	checkEntities();		// check if any entity is damaged or not
	
	MovePlayer();				// move player with potenciometer
  MoveShots(5);				// move shots with "speed"
	MoveEnemy(0);				// move enemy with "speed"
	Semaphore = 1; 			// ready for draw
}

void checkButtons(void) {
	// if player gun is alive, red LED shines
	if(Gun.life > 0) turnOnLED(4);
	else turnOffLED(4);
	
	// if player missiles are alive, yellow LED shines
	if(Missile[0].life == 1) turnOnLED(5);
	else if(Missile[1].life == 1) turnOnLED(5);
	else turnOffLED(5);
}
void ParameterInit(unsigned long lives) { int i, row;
	// player set up
	PlayerShip.x = 34;			
	PlayerShip.y = 47;
	PlayerShip.width = PLAYERW;
	PlayerShip.height = PLAYERH;
	PlayerShip.image[0] = PlayerShip0;
	PlayerShip.image[1] = PlayerShip0;
	PlayerShip.life = lives;		// player starts with 3 lifes
	
	// initialize enemy explosion (dont show on screen)
	PlayerExplosion.x = PlayerShip.x; 		// irrelevant
	PlayerExplosion.y = PlayerShip.y;
	PlayerExplosion.width = 0;					// irrelevant
	PlayerExplosion.height = 0;					// irrelevant
	PlayerExplosion.image[0] = BigExplosion0;
	PlayerExplosion.image[1] = BigExplosion0;
	PlayerExplosion.life = 0;
	
	// initialize player gun (dont show on screen)
	Gun.x = PlayerShip.x;
	Gun.y = PlayerShip.y;
	Gun.width = LASERW;
	Gun.height = LASERH;
	Gun.image[0] = Laser0;
	Gun.image[1] = Laser0;
	Gun.life = 0;
	
	// initialize player missiles (dont show on screen)
	for(i=0;i<missileNumber;i++) {
		Missile[i].x = PlayerShip.x;
		Missile[i].y = PlayerShip.y;
		Missile[i].width = MISSILEW;
		Missile[i].height = MISSILEH;
		Missile[i].image[0] = Missile0;
		Missile[i].image[1] = Missile0;
		Missile[i].life = 0;
	}
	
	// initialize all bunkers with full life
	for(i=0;i<bunkerNumber;i++) {
		Bunker[i].x = 4+(30*i);
		Bunker[i].y = 47 - PLAYERH;
		Bunker[i].width = BUNKERW;
		Bunker[i].height = BUNKERH;
		Bunker[i].image[0] = Bunker0;
		Bunker[i].image[1] = Bunker0;
		Bunker[i].life = 3;						// 2 states (new and damaged)
	}
	
	// initialize all 8 enemy
	row = 0;
  for(i=0;i<enemyNumber;i++){
		if(!(i%4)) row++;	// each 4 enemy, another row is added
    Enemy[i].x = 10+(16*(i%4));
    Enemy[i].y = 10 + (row-1)*ENEMY20H;
		if(row-1 == 0) { // top enemy has more points
			Enemy[i].width = ENEMY20W;
			Enemy[i].height = ENEMY20H;
			Enemy[i].image[0] = SmallEnemy20PointA;
			Enemy[i].image[1] = SmallEnemy20PointB;
			Enemy[i].points = 20;
		}
		else {
			Enemy[i].width = ENEMY10W;
			Enemy[i].height = ENEMY10H;
			Enemy[i].image[0] = SmallEnemy10PointA;
			Enemy[i].image[1] = SmallEnemy10PointB;
			Enemy[i].points = 10;
    }
		Enemy[i].life = 1;
		
		// initialize enemy laser
		EnemyGun[i].x = Enemy[i].x;
		EnemyGun[i].y = Enemy[i].y;
		EnemyGun[i].width = LASERW;
		EnemyGun[i].height = LASERH;
		EnemyGun[i].image[0] = Laser0;
		EnemyGun[i].image[1] = Laser0;
		EnemyGun[i].life = 0;
		
		// initialize enemy explosion
		EExplosion[i].x = Enemy[i].x; 		// irrelevant
		EExplosion[i].y = Enemy[i].y;
		EExplosion[i].width = 0;					// irrelevant
		EExplosion[i].height = 0;					// irrelevant
		EExplosion[i].image[0] = SmallExplosion0;
		EExplosion[i].image[1] = SmallExplosion0;
		EExplosion[i].life = 0;
  }
}

void MovePlayer(void) {
	// Player movement
	unsigned long constant = 4095/(80-PLAYERW); // screen limit - player width
	PlayerShip.x = ADC0_In()/constant;
}

void MoveShots(long speed) { int i;
	// Laser movement
	if(Gun.y <= 10) Gun.life = 0;		// if out of the screen, kill Gun
	if(Gun.life) Gun.y -= speed; 		// player gun goes UP
	
	// Missile movement
	if((Missile[0].y <= 10) || (Missile[0].x < 6)) {
			Missile[0].life = 0; // if out, kill Missile
	}
	if(Missile[0].life) {
		Missile[0].y -= speed;	
		Missile[0].x -= speed/3;	// missile two moves to left
	}

	if((Missile[1].y <= 10) || (Missile[1].x >= 80-Missile[1].width)) {
		Missile[1].life = 0; // if out, kill Missile
	}	
	if(Missile[1].life) {
		Missile[1].y -= speed;	
		Missile[1].x += speed/3; // missile two moves to right
	}
	
	// Enemy Laser movement
	for(i=0;i<enemyNumber;i++) {
		if(EnemyGun[i].y >= 50) EnemyGun[i].life = 0;
		if(EnemyGun[i].life) EnemyGun[i].y += speed;
	}
}

void MoveEnemy(long speed) { int i;
	// Enemy movement (right or left)
	if(!enemyDirection) {
		for(i=0;i<enemyNumber;i++){
			Enemy[i].x = Enemy[i].x + 1 + speed;
			if(Enemy[i].x >= 83-Enemy[i].width) {	
				enemyDirection = 0x01; 		// update movement variable
			}
		}
	}
	else {
		for(i=0;i<enemyNumber;i++){
			Enemy[i].x = Enemy[i].x - 1 - speed;
			if(Enemy[i].x <= 1) {	// enemy must go down
				enemyDirection = 0x00; 		// update movement variable
			}
		}
	}
}

void Draw(void){ int i;
  Nokia5110_ClearBuffer();
	
	if(lose == 0) {
		Nokia5110_PrintBMP(PlayerShip.x, PlayerShip.y, PlayerShip.image[FrameCount], 0);
	}
	
	if(PlayerExplosion.life > 0){
			Nokia5110_PrintBMP(PlayerExplosion.x, PlayerExplosion.y, PlayerExplosion.image[FrameCount], 0);
	}
	
	if(Gun.life) {
		Nokia5110_PrintBMP(Gun.x, Gun.y, Gun.image[FrameCount], 0);
	}
	
	for(i=0;i<missileNumber;i++) {
		if(Missile[i].life) {
			Nokia5110_PrintBMP(Missile[i].x, Missile[i].y, Missile[i].image[FrameCount], 0);
		}
	}
	
  for(i=0;i<bunkerNumber;i++) {
		if(Bunker[i].life > 0) {
			Nokia5110_PrintBMP(Bunker[i].x, Bunker[i].y, Bunker[i].image[FrameCount], 0);
		}
	}
	
	for(i=0;i<enemyNumber;i++){
    if(Enemy[i].life){
			Nokia5110_PrintBMP(Enemy[i].x, Enemy[i].y, Enemy[i].image[FrameCount], 0);
    }
		if(EnemyGun[i].life) {
			Nokia5110_PrintBMP(EnemyGun[i].x, EnemyGun[i].y, EnemyGun[i].image[FrameCount], 0);
		}
		if(EExplosion[i].life > 0){
			Nokia5110_PrintBMP(EExplosion[i].x, EExplosion[i].y, EExplosion[i].image[FrameCount], 0);
		}
  }
	
  Nokia5110_DisplayBuffer();      // draw buffer
  FrameCount = (FrameCount+1)&0x01; // 0,1,0,1,...
}

void NewScreen(unsigned long lives, unsigned long score) {
	lose = 0;
	win = 0;
	enemyDead = 0;
	
	// kill player shots in order to turn off both LEDs (Timer3A)
	Gun.life = 0;
	Missile[0].life = 0;
	Missile[1].life = 0;
	
	NVIC_ST_CTRL_R = 0;	// disable SysTick
	Delay100ms(20);			// stop and watch how PlayerShip/last enemy is destroyed!!
	// show this before starting to shoot
	Nokia5110_Clear();
  Nokia5110_SetCursor(1, 1);
  Nokia5110_OutString("Round");
	Nokia5110_OutUDec(level);
	Nokia5110_SetCursor(1, 3);
  Nokia5110_OutString("Lives");
	Nokia5110_OutUDec(PlayerShip.life);
	Nokia5110_SetCursor(1, 4);
  Nokia5110_OutString("Score");
  Nokia5110_SetCursor(1, 5);
  Nokia5110_OutUDec(score);
  Nokia5110_SetCursor(0, 0); // renders screen
	
	Delay100ms(20);										 // delay before start shooting
	while(!(GPIO_PORTE_DATA_R & 0x01)) {}; 				 // wait until you push laser button
	ParameterInit(PlayerShip.life);
	Draw();                    				 // draw starting screen
	Delay100ms(20);										 // delay before start shooting
	NVIC_ST_CTRL_R |= 0x0007;  		     // enable with core clock and interrupts
}

void Game_Over(unsigned long score) {
	lose = 0;
	win = 0;
	enemyDead = 0;
	
	// kill player shots in order to turn off both LEDs (Timer3A)
	Gun.life = 0;
	Missile[0].life = 0;
	Missile[1].life = 0;
	
	NVIC_ST_CTRL_R = 0;	// disable SysTick
	Delay100ms(20);			// stop and watch how PlayerShip/last enemy is destroyed!!
	// explode las enemy/player
	Nokia5110_Clear();
  Nokia5110_SetCursor(1, 1);
  Nokia5110_OutString("GAME OVER");
	Nokia5110_SetCursor(1, 3);
	Delay100ms(10);
  Nokia5110_OutString("Score");
	Nokia5110_SetCursor(1, 4);
	Delay100ms(10);
  Nokia5110_OutUDec(score);
  Nokia5110_SetCursor(0, 0); // renders screen
	
	Delay100ms(20);										 // delay before start shooting
	while(!(GPIO_PORTE_DATA_R & 0x01)) {}; 				 // wait until you push laser button
	PlayerShip.life = 3;
	playerScore = 0;
	level = 1;
	NewScreen(PlayerShip.life, playerScore);
}

void createPlayerShot(void) { int i;
	if(laser) {
		if(Gun.life == 0) {	// shoot only if there are no player shots in the screen
			Gun.x = (PlayerShip.x + PLAYERW/2 - 1) & 0xFF; // center in the middle of player sprite
			Gun.y = 47;
			Gun.image[0] = Laser0;
			Gun.image[1] = Laser0;
			Gun.life = 1;
			Sound_Shoot(); // Timer2
		}
	}
	if(special) { // shoot only if all missiles are off
		if(!Missile[0].life && !Missile[1].life) {
			for(i=0;i<missileNumber;i++) {
				Missile[i].x = (PlayerShip.x + PLAYERW/2 - 1) & 0xFF;
				Missile[i].y = 47;
				Missile[i].image[0] = Missile0;
				Missile[i].image[1] = Missile0;
				Missile[i].life = 1;
				Sound_Shoot(); // Timer2
			}
		}
	}
}

void createEnemyShot(void) { int i;
	unsigned long enemyShoots = Random()%20;		// one enemy will shoot with 5% each iteration
	
	if(enemyShoots == 0) {	// one enemy will shoot
		unsigned long n = Random()%(8-enemyDead);	// choose one random enemy to shoot
		for(i=0;i<enemyNumber;i++) {
			if(Enemy[i].life) {
				if(n == 0) { // this enemy will shoot
					EnemyGun[i].x = (Enemy[i].x + Enemy[i].width/2 - 1) & 0xFF;
					EnemyGun[i].y = Enemy[i].y;
					EnemyGun[i].image[0] = Laser0;
					EnemyGun[i].image[1] = Laser0;
					EnemyGun[i].life = 1;
					n = 1000;	// no more shots!
				}
				else n--;					
			}
		}
	}
	
	
}

void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}

unsigned char damaged(STyp* entity, STyp* shot) {
	// first compare if minimum Y axis is respected
	long operandA;
	long operandB;
	
	if(entity->y > shot->y) {
		operandA = entity->y + entity->height;
		operandB = shot->y;
	}
	else {
		operandA = shot->y + shot->height;
		operandB = entity->y;
	}
	// real distance is less than minimum distance? (8 white spaces)
	if((operandA - operandB) < (entity->height + shot->height - 4)) {
		// now compare X axis
		if(entity->x > shot->x) {
			operandA = entity->x + entity->width;
			operandB = shot->x;
		}
		else {
			operandA = shot->x + shot->width;
			operandB = entity->x;
		}
		//real distance is less than minimum distance?
		if((operandA-operandB) < (entity->width + shot->width)) {
			entity->life--;
			shot->life--;
			return 1;
		}
	}
	return	0;
}

void damagePlayer(void) {
	// update player explosion (because he is dead)
	PlayerExplosion.y = PlayerShip.y;
	PlayerExplosion.x = PlayerShip.x + (PlayerShip.width - 4) / 2;
	PlayerExplosion.life = 4;
	Sound_Explosion(); // Timer2
	lose = 1;
}

void damageBunker(int number) {
	// update bunker life and image
	Bunker[number].life -= 1;
	if(Bunker[number].life == 2) {
		Bunker[number].image[0] = Bunker1;
		Bunker[number].image[1] = Bunker1;
	}
	if(Bunker[number].life == 1) {
		Bunker[number].image[0] = Bunker2;
		Bunker[number].image[1] = Bunker2;
	}
	if(Bunker[number].life > 3) {
		Bunker[number].life = 0;
	}
}

void damageEnemy(int number) {
	// destroy an enemy and count it as dead for calculate when the player wins
	Enemy[number].life = 0;
	enemyDead++;
	playerScore += Enemy[number].points; // update player score
	EExplosion[number].y = Enemy[number].y;
	EExplosion[number].x = Enemy[number].x + (Enemy[number].width - 4) / 2;
	EExplosion[number].life = 4;
	Sound_Killed(); // Timer2
}

void checkGun(void) { int i;
	// check if the gun hitted something
	if(Gun.life) {
		for(i=0;i<bunkerNumber;i++) {
			if(Bunker[i].life > 0) {
				if(damaged(&Bunker[i],&Gun)) {
					Gun.life = 0;
					damageBunker(i);				
				}					
			}
		}
		
		for(i=0;i<enemyNumber;i++) {
			if(Enemy[i].life > 0) {
				if(damaged(&Enemy[i],&Gun)) {
					Gun.life = 0;
					damageEnemy(i);
				}
			}
		}
	}
}

void checkMissile(void) { int i, j;
	// check if some missile hitted some enemy
	for(j=0;j<missileNumber;j++) {
		if(Missile[j].life) {
			for(i=0;i<enemyNumber;i++) {
				if(Enemy[i].life > 0) {
					if(damaged(&Enemy[i],&Missile[j])) {
						damageEnemy(i);
						Missile[j].life = 0;
					}
				}
			}
		}
	}
}

void checkEnemyGun(void) { int i,j;
	// check if the enemy gun hitted something
	for(j=0;j<enemyNumber;j++) {
		if(EnemyGun[j].life) {
			for(i=0;i<bunkerNumber;i++) {
				if(Bunker[i].life > 0) {
					if(damaged(&Bunker[i],&EnemyGun[j])) { //funciona??? quizas sea "damage"
						EnemyGun[j].life = 0;
						damageBunker(i);				
					}					
				}
			}
		
			if(damaged(&PlayerShip,&EnemyGun[j])) damagePlayer();
		}
	}
}

void checkEntities(void) { int i;
	// check all things of screen' state
	for(i=0;i<enemyNumber;i++) {		
		if(EExplosion[i].life > 0) {
			updateEExplosion(i);
		}
	}
	updatePlayerExplosion();
	checkGun();
	checkMissile();
	checkEnemyGun();
}

void updatePlayerExplosion(void) {
	// update player dead animation sprite (4 phases)
	PlayerExplosion.life -= 1;
	if(PlayerExplosion.life > 2) {
		PlayerExplosion.image[0] = BigExplosion0;
		PlayerExplosion.image[1] = BigExplosion0;
	}
	else {
		PlayerExplosion.image[0] = BigExplosion1;
		PlayerExplosion.image[1] = BigExplosion1;
	}
	if(PlayerExplosion.life == 1) nextScreen = 1;
	if(PlayerExplosion.life > 4) PlayerExplosion.life = 0;
}

void updateEExplosion(int number) {
	// update enemy dead animation sprite (4 phases)
	EExplosion[number].life -= 1;
	if(EExplosion[number].life > 2) {
		EExplosion[number].image[0] = SmallExplosion0;
		EExplosion[number].image[1] = SmallExplosion0;
	}
	else {
		EExplosion[number].image[0] = SmallExplosion1;
		EExplosion[number].image[1] = SmallExplosion1;
	}
	if(EExplosion[number].life > 4) EExplosion[number].life = 0;
}

int main(void){
  TExaS_Init(NoLCD_NoScope);      // set system clock to 80 MHz
  // you cannot use both the Scope and the virtual Nokia (both need UART0)
  Random_Init(1);
  Nokia5110_Init();
  EnableInterrupts();             // virtual Nokia uses UART0 interrupts
  Nokia5110_ClearBuffer();
	Nokia5110_DisplayBuffer();      // draw buffer
	
	ADC0_Init();                    // initialize ADC (potenciometer)
	DAC_Init();                     // initialize DAC (sound)
	Switch_Init();                  // initialize siwtches (PE1, PE0)
  LED_Init();											// initialize LEDs (PB5, PB4)
	ParameterInit(3);               // initialize entity parameters with 3 lives
	SysTick_Init(80000000/15);			// 30 Hz (movement calculation)
	Timer2_Init(80000000/11025); 		// 11.025 kHz (sound)
	Timer3_Init(checkButtons, 80000000/12000); // 12kHz (LEDs)
	NewScreen(PlayerShip.life, playerScore);   // present the first level
  
	while(1) {											// repeat forever
		while(PlayerShip.life > 0){		// repeat until you run out of life
			
			while(1) { 									// repeat until you lose life or win
				while(Semaphore == 0){};	// wait until all calculations are done
				Draw();										// paint screen
				if(enemyDead == 8) {			// all enemy are dead and you win
					win = 1;
					nextScreen = 1;
				}
				if(nextScreen == 1) break;    // the level finished
				Semaphore = 0; 								// runs at 30 Hz    
			}
			
			nextScreen = 0;
			if(win == 1) level++;						// change to the next level
			// show lives and score. Re-start the screen (and turn off LEDs)
			if(PlayerShip.life > 0)	{
				turnOffLED(4);
				turnOffLED(5);
				NewScreen(PlayerShip.life, playerScore); 
			}
		}
		Game_Over(playerScore);	// final screen
	}
}
