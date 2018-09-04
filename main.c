#include "stm32f10x.h"                  // Device header
#include "type.h"
#include "clock.h"
#include "timer.h"
#include "GPIO.h"
#include "in.h"
#include "IR.h"

void TIM2_IRQHandler(void); 
tIR_DATA d ;

int brightness = 0x1;
#define ever ;;

int main()
{
	
  // CLOCK_setCoreClock8MHZ();
   TIMER_initTickTimer();
	IR_init();
	TIMER_initPwmTimer();
	TIMER_initIRsendingTimer();
	
//TIMER_startTimer(USPERTICK ); //50 uS timer interrupt, OS



	
	for(ever)
	{
			/*DO NOTHING*/
		
		IR_sendNECCode(0xA659FF00);
		IR_sendNECCode(0xA659FF00);
		TIMER_delay(1000);
		TIMER_delay(1000);
	}
	return 0;
}

void TIM2_IRQHandler(void)
{
	TIMER_updateTimer();
	
	/*DO YOUR STUFF HERE*/
	IR_recvUpdate();

	if(IR_validCodeDetected())
	{
 		 d = IR_getRecievedCode();
		//if(d == 0xA659FF00)
		if(d == 0xAE51FF00)
		{
			//TIM3->CCR1 = --brightness;
			TIMER_disablePWM();
		} 
		else if(d == 0xA659FF00)
		{
			//TIM3->CCR1 = ++brightness;
			TIMER_enablePWM();
		}

	}
}
