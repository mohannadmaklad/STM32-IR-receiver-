#include "timer.h"
#include  "GPIO.h"

extern int brightness;

#define ENABLE_TIMER2_CLOCK							RCC->APB1ENR |= RCC_APB1ENR_TIM2EN
#define ENABLE_TIMER3_CLOCK							RCC->APB1ENR |= RCC_APB1ENR_TIM3EN
#define ENABLE_TIMER4_CLOCK							RCC->APB1ENR |= RCC_APB1ENR_TIM4EN

#define ENABLE_TIMER(TIM)						  	TIM->CR1 |= (TIM_CR1_CEN)
#define DISABLE_TIMER(TIM)							TIM->CR1 &= !(TIM_CR1_CEN)
#define UEV_ENALBE(TIM)							 		TIM->CR1 &= !(TIM_CR1_UDIS)
#define ENABLE_ONE_PULSE_MODE(TIM)			TIM->CR1 |= (TIM_CR1_OPM)
#define DISABLE_ONE_PULSE_MODE(TIM)			TIM->CR1 &= !(TIM_CR1_OPM)
#define COUNTER_UP_MODE(TIM)						TIM->CR1 &= !(TIM_CR1_DIR)
#define COUNTER_DOWN_MODE(TIM)					TIM->CR1 |= (TIM_CR1_DIR)
#define SET_PRESCALER(TIM,VALUE)		    TIM->PSC = VALUE-1;
#define LOAD_TIMER(TIM,VALUE)						TIM->ARR = VALUE;
#define ENABLE_TIMER_INTERRUPT(TIM)	    TIM->DIER |= TIM_DIER_UIE


/*Private interface*/
static unsigned int getTimerFreq(void);

/*Public interface*/
void TIMER_initTickTimer(void)
{
	/*enable timer clock*/
	ENABLE_TIMER2_CLOCK;
	/*config the timer*/
	UEV_ENALBE(TICK_TIMER);
	DISABLE_ONE_PULSE_MODE(TICK_TIMER);
	COUNTER_UP_MODE(TICK_TIMER);
	/*enable timer interrupt*/
	NVIC_EnableIRQ(TICK_IRQn);
	ENABLE_TIMER_INTERRUPT(TICK_TIMER);
}

void TIMER_updateTimer(void)
{
	/*reset pending bit*/
	//resetPendingBit(TIMER2_IRQN);
	TICK_TIMER->SR &= (!TIM_SR_UIF);
	
}

void TIMER_startTimer(unsigned int usTime)
{
	DISABLE_TIMER(TICK_TIMER);
	//SET_PRESCALER(1);
	double usTick = 1000000/(double)TIMER_FREQ;
	LOAD_TIMER(TICK_TIMER,(usTime/usTick)-1);
	ENABLE_TIMER(TICK_TIMER);
}

void TIMER_startTimerMS(unsigned int msTime)
{
	DISABLE_TIMER(TICK_TIMER);
	SET_PRESCALER(TICK_TIMER,1000);
	double usTick = 1000000/(double)TIMER_FREQ;
	LOAD_TIMER(TICK_TIMER,msTime/usTick);
	ENABLE_TIMER(TICK_TIMER);
}

void TIMER_stopTimer()
{
	DISABLE_TIMER(TICK_TIMER);
}


unsigned int TIMER_getTimerValue(void)
{
	return TICK_TIMER->CNT;
	
}

