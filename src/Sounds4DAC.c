
#include "Sounds4DAC.h"

void Sound_Play(const unsigned char *pt, unsigned long count){
  Wave = pt;
  Index = 0;
  Count = count;
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
}