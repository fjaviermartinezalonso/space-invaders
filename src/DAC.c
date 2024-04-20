// DAC.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Implementation of the 4-bit digital to analog converter
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC 
// (actually Port B bits 5-0 have a 6-bit DAC)

#include "DAC.h"
#include "..//tm4c123gh6pm.h"

// **************DAC_Init*********************
// Initialize 4-bit DAC (actually 6-bit DAC)
// Input: none
// Output: none
void DAC_Init(void){
	unsigned long delay;
	
  SYSCTL_RCGC2_R |= 0x00000002;     // activate clock for Port B
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
	GPIO_PORTB_DR8R_R |= 0x3F;    		// can drive up to 8mA out
  GPIO_PORTB_AMSEL_R &= ~0x3F;      // disable analog on PB
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // PCTL GPIO on PB5-0
  GPIO_PORTB_DIR_R |= 0x3F;         // PB5-0 out
  GPIO_PORTB_AFSEL_R &= ~0x3F;      // disable alt funct on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;         // enable digital I/O on PB5-0
}


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15  (actually, 6-bit data, 0 to 64
// Output: none
void DAC_Out(unsigned long data){
  GPIO_PORTB_DATA_R = data;
}
