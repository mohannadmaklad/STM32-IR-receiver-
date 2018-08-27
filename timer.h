#include "interrupt.h"

/*Public interface*/
void TIMER_initTimer(void);
void TIMER_updateTimer(void);
void TIMER_startTimer(unsigned int usTime);
void TIMER_startTimerMS(unsigned int msTime);
void TIMER_stopTimer();
void TIMER_resetTimer(void);
unsigned int TIMER_getTimerValue(void);
