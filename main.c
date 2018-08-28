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
	TIMER_startTimer(USPERTICK ); //50 uS timer interrupt, OS
	
	for(ever)
	{
		//do nothing
	}
	return 0;
}

void TIM2_IRQHandler(void)
{
	TIMER_updateTimer();
	
	/*DO YOUR STUFF HERE*/
	IR_recvUpdate(); //updates the state machine

	if(IR_validCodeDetected()) //check for a new code
	{
 		 d = IR_getRecievedCode(); //if there, get it
	}
}
