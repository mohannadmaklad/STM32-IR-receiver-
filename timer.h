#include "interrupt.h"
#include "stm32f10x.h"                  // Device header
#include "type.h"

#define TIMER_FREQ								SystemCoreClock
#define TICK_TIMER								TIM2
#define TICK_IRQn									TIM2_IRQn

#define PWM_TIMER									TIM3
#define IR_TIMER									TIM4


/*Public interface*/
void TIMER_initTickTimer(void);
void TIMER_updateTimer(void);
void TIMER_startTimer(unsigned int usTime);
void TIMER_startTimerMS(unsigned int msTime);
void TIMER_stopTimer();
void TIMER_resetTimer(void);
unsigned int TIMER_getTimerValue(void);

/*Timer 3 is used to generate PWM on pin PA6*/
void TIMER_initPwmTimer(void);
void TIMER_enablePWM(); //pwm output at pin A6
void TIMER_disablePWM(); //pwm output at pin A6



/*Timer 4 is used to send IR codes on pin PA6*/

void TIMER_initIRsendingTimer(void);
void TIMER_startIRsendingTimer(unsigned int usTime);
void TIMER_stopIRsendingTimer(void);
tBOOL TIMER_IRtimerFinished(void);
tBOOL TIMER_IRtimerUpdate(void);
