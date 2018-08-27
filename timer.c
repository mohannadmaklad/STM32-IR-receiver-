#include "timer.h"
#include "stm32f10x.h"                  // Device header


#define TIMER2_IRQN 28
#define ENABLE_TIMER						  TIM2->CR1 |= (TIM_CR1_CEN)
#define DISABLE_TIMER						  TIM2->CR1 &= !(TIM_CR1_CEN)
#define UEV_ENALBE						 		TIM2->CR1 &= !(TIM_CR1_UDIS)
#define ENABLE_TIMER_CLOCK				RCC->APB1ENR |= RCC_APB1ENR_TIM2EN
#define ENABLE_ONE_PULSE_MODE			TIM2->CR1 |= (TIM_CR1_OPM)
#define DISABLE_ONE_PULSE_MODE		TIM2->CR1 &= !(TIM_CR1_OPM)
#define COUNTER_UP_MODE						TIM2->CR1 &= !(TIM_CR1_DIR)
#define COUNTER_DOWN_MODE					TIM2->CR1 |= (TIM_CR1_DIR)
#define SET_PRESCALER(VALUE)		  TIM2->PSC = VALUE-1;
#define LOAD_TIMER(VALUE)					TIM2->ARR = VALUE;
#define ENABLE_TIMER_INTERRUPT    TIM2->DIER |= TIM_DIER_UIE

#define TIMER_FREQ								SystemCoreClock

/*Private interface*/
static unsigned int getTimerFreq(void);

/*Public interface*/
void TIMER_initTimer(void)
{

	/*enable timer clock*/
	ENABLE_TIMER_CLOCK;
	/*config the timer*/
	UEV_ENALBE;
	DISABLE_ONE_PULSE_MODE;
	COUNTER_DOWN_MODE;
	/*enable timer interrupt*/
	NVIC_EnableIRQ(TIM2_IRQn);
	ENABLE_TIMER_INTERRUPT;
}

void TIMER_updateTimer(void)
{
	/*reset pending bit*/
	//resetPendingBit(TIMER2_IRQN);
	TIM2->SR &= (!TIM_SR_UIF);
	
}

void TIMER_startTimer(unsigned int usTime)
{
	DISABLE_TIMER;
	//SET_PRESCALER(1);
	double usTick = 1000000/(double)TIMER_FREQ;
	LOAD_TIMER((usTime/usTick)-1);
	ENABLE_TIMER;
}

void TIMER_startTimerMS(unsigned int msTime)
{
	DISABLE_TIMER;
	SET_PRESCALER(1000);
	double usTick = 1000000/(double)TIMER_FREQ;
	LOAD_TIMER(msTime/usTick);
	ENABLE_TIMER;
}

void TIMER_stopTimer()
{
	DISABLE_TIMER;
}


void TIMER_resetTimer(void)
{

	
}	


unsigned int TIMER_getTimerValue(void)
{
	return TIM2->CNT;
	
}





/**/
/*
static unsigned int getTimerFreq(void) //not tested
{
	unsigned int AHB_prescaler,APB1_prescaler;
	
	AHB_prescaler  = (RCC->CFGR & (RCC_CFGR_HPRE))  >> 4 ;
	APB1_prescaler = (RCC->CFGR & (RCC_CFGR_PPRE1)) >> 8 ;
	
	return (SystemCoreClock/(AHB_prescaler*APB1_prescaler));
}
*/
