
#include "Timer2A.h"

unsigned long TimerCount;
void Timer2_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  TIMER2_CTL_R = 0x00000000;   // 1) disable timer2A
  TIMER2_CFG_R = 0x00000000;   // 2) 32-bit mode
  TIMER2_TAMR_R = 0x00000002;  // 3) periodic mode
  TIMER2_TAILR_R = period-1;   // 4) reload value
  TIMER2_TAPR_R = 0;           // 5) clock resolution
  TIMER2_ICR_R = 0x00000001;   // 6) clear timeout flag
  TIMER2_IMR_R = 0x00000001;   // 7) arm timeout
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; 
// 8) priority 4
  NVIC_EN0_R = 1<<23;          // 9) enable IRQ 23 in
  TIMER2_CTL_R = 0x00000001;   // 10) enable timer2A
}
// trigger is Timer2A Time-Out Interrupt
// set periodically TATORIS set on rollover
void Timer2A_Handler(void){
  TIMER2_ICR_R = 0x00000001;  // acknowledge
  TimerCount++;
// run some background stuff here
}
void Timer2A_Stop(void){ 
  TIMER2_CTL_R &= ~0x00000001; // disable
}
void Timer2A_Start(void){ 
  TIMER2_CTL_R |= 0x00000001;   // enable
}
