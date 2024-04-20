
// *************************** Switch ***************************
#include "..//tm4c123gh6pm.h"

unsigned char wasPressedPE1(void); 
unsigned char wasPressedPE0(void);
void Switch_Init(void); // initializes PA1, PA0 as switches
void LED_Init(void);		// initializes PB5, PB4 as "shooting" LEDs
void turnOffLED(int number);	// turn off both LEDs
void turnOnLED(int number);
