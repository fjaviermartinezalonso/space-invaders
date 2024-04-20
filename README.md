Space Invaders game for a Tiva Launchpad based board

Program starts at SpaceInvaders, where the board is initialised: system clock, interrupts, ADC and DAC, etc. This program runs four threads:
* main: initializes the game. After that, this controls the game flow
				i.e. changes of screens if the player loses, wins or game over
* SysTick timer: controls all game variables: position, animations, I/O
				switches and screen drawing. 30 Hz, but actualy it runs a bit
				slower to compensate the higher speed of the simulated screen
* Timer2: exclusively sound output using the DAC, which is connected
				to the phones. It runs at 11.025 kHz because of the frequencies
				of the sound. 4-bit DAC
* Timer3: exclusively for LED output. This interrupt only executes the
				"checkButtons" function at high speed to avoid Timer2 high freq
				to disrupt LEDs (they can shine too weak)

The project is compiled using Keil software and runs on Tiva Launchpad. In order to get the things work I prepared a board with two buttons for the shoot inputs (normal and special shoots) and a sliding potenciometer as a movement controller. The game produces sound effects hearable by headphones via a minijack socket. The screen used for this project was Nokia5110, which is 48x84 black and white pixel LCD.

The program draws constantly the screen to update all the items in the game: player, enemies, bunkers and shoots. After a round the game pauses and displays the remaining lives, round and score.