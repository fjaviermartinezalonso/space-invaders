
#include "Switch.h"


void Switch_Init(void){ unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000010;     // activate clock for Port E
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTE_AMSEL_R &= ~0x0F;      // disable analog on PE
  GPIO_PORTE_PCTL_R &= ~0x000000FF; // PCTL GPIO on PE1-0
  GPIO_PORTE_DIR_R &= ~0x03;        // PE1-0 in
  GPIO_PORTE_AFSEL_R &= ~0x03;      // disable alt funct on PE1-0
  GPIO_PORTE_DEN_R |= 0x03;         // enable digital I/O on PE1-0
}

void LED_Init(void){ unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000002;     // activate clock for Port B
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTB_AMSEL_R &= ~0x0F;      // disable analog on PB
  GPIO_PORTB_PCTL_R &= ~0x00FF0000; // PCTL GPIO on PB5-4
  GPIO_PORTB_DIR_R |= 0x30;         // PB5-4 out
  GPIO_PORTB_AFSEL_R &= ~0x30;      // disable alt funct on PB5-4
  GPIO_PORTB_DEN_R |= 0x30;         // enable digital I/O on PB5-4
}

unsigned char wasPressedPE1(void) {
	unsigned char input = GPIO_PORTE_DATA_R & 0x02;
	
	if(input) {
		GPIO_PORTB_DATA_R |= 0x20;			// turn on LED PB5
		return 1;
	}
	else {
		GPIO_PORTB_DATA_R &= ~0x20;			// turn off LED PB5
		return 0;
	}
}

unsigned char wasPressedPE0(void) {
	unsigned char input = GPIO_PORTE_DATA_R & 0x01;
	
	if(input) {
		GPIO_PORTB_DATA_R |= 0x10;			// turn on LED PB4
		return 1;
	}
	else {
		GPIO_PORTB_DATA_R &= ~0x10;			// turn off LED PB4
		return 0;
	}
}

void turnOffLED(int number) {
	if(number == 5) {
		GPIO_PORTB_DATA_R &= ~0x20;			// turn off LED PB5
	}
	else if(number == 4) {
		GPIO_PORTB_DATA_R &= ~0x10;			// turn off LED PB4
	}
}

void turnOnLED(int number) {
	if(number == 5) {
		GPIO_PORTB_DATA_R |= 0x20;			// turn on LED PB5
	}
	else if(number == 4) {
		GPIO_PORTB_DATA_R |= 0x10;			// turn on LED PB4
	}
}
