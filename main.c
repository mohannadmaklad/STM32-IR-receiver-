#include "stm32f10x.h"                  // Device header
#include "type.h"
#include "clock.h"
#include "timer.h"
#include "GPIO.h"
#include "in.h"
#include "IR.h"

void TIM2_IRQHandler(void); 
 		tIR_DATA d ;


#define ever ;;

int main()
{
	
  // CLOCK_setCoreClock8MHZ();
	TIMER_initTimer();
	IR_init();
	TIMER_startTimer(USPERTICK ); //50 uS timer interrupt
	
	
	for(ever)
	{
			/*DO NOTHING*/

	}
	return 0;
}

void TIM2_IRQHandler(void)
{
	TIMER_updateTimer();
	
	/*DO YOUR STUFF HERE*/
	IR_update();

	if(IR_validCodeDetected())
	{
 		 d = IR_getRecievedCode();
	}
}
