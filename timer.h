#include "interrupt.h"
#include "stm32f10x.h"                  // Device header
#include "type.h"

#define TIMER_FREQ								SystemCoreClock
#define TICK_TIMER								TIM2
#define TICK_IRQn									TIM2_IRQn



/*Public interface*/
void TIMER_initTickTimer(void);
void TIMER_updateTimer(void);
void TIMER_startTimer(unsigned int usTime);
void TIMER_startTimerMS(unsigned int msTime);
void TIMER_stopTimer();
unsigned int TIMER_getTimerValue(void);

