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


void TIMER_resetTimer(void)
{

	
}	


unsigned int TIMER_getTimerValue(void)
{
	return TICK_TIMER->CNT;
	
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


void TIMER_initPwmTimer(void)
{
	/*config PA6 as AF output*/
	enablePort(A);
	resetGpioL(GPIOA,6);
	confGpioL(GPIOA,6,AF_pushPullOutput);
	

	/*enable timer clock*/
	ENABLE_TIMER3_CLOCK;
	/*config the timer*/
	DISABLE_ONE_PULSE_MODE(PWM_TIMER);
	COUNTER_UP_MODE(PWM_TIMER);
	
	/*PWM mode 1*/
	PWM_TIMER->CCMR1 &= !TIM_CCMR1_OC1M_0;
	PWM_TIMER->CCMR1 |= TIM_CCMR1_OC1M_1; 
	PWM_TIMER->CCMR1 |= TIM_CCMR1_OC1M_2; 
	
	SET_PRESCALER(PWM_TIMER,90);
	LOAD_TIMER(PWM_TIMER,20);
	PWM_TIMER->CCR1 = 0x4; //.25 PWM
 	
}

void TIMER_disablePWM()
{
	PWM_TIMER->CCER &= !TIM_CCER_CC1E;
	DISABLE_TIMER(PWM_TIMER);

}

void TIMER_enablePWM()
{
	PWM_TIMER->CNT = 20;
	PWM_TIMER->CCER |= TIM_CCER_CC1E;
	ENABLE_TIMER(PWM_TIMER);
}

void TIMER_initIRsendingTimer(void)
{
	/*enable timer clock*/
	ENABLE_TIMER4_CLOCK;
	/*config the timer*/
	COUNTER_UP_MODE(TICK_TIMER);
}

void TIMER_startIRsendingTimer(unsigned int usTime)
{
	DISABLE_TIMER(IR_TIMER);
	IR_TIMER -> CNT = 0;
	//SET_PRESCALER(1);
 	double usTick = 1000000/(double)TIMER_FREQ;
	LOAD_TIMER(IR_TIMER,(usTime/usTick)-1);
	ENABLE_TIMER(IR_TIMER);
}

void TIMER_stopIRsendingTimer(void)
{
	DISABLE_TIMER(IR_TIMER);
}

tBOOL TIMER_IRtimerFinished(void)
{
	return (IR_TIMER->SR & TIM_SR_UIF);
}
tBOOL TIMER_IRtimerUpdate(void)
{
	IR_TIMER ->EGR |= TIM_EGR_UG;
}