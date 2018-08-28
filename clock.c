#include "clock.h"

void CLOCK_setCoreClock8MHZ(void)
{
	RCC->CFGR &= !(0x3); //reset first 2 bits
	RCC->CFGR |= RCC_CFGR_SW_HSE;
	SystemCoreClockUpdate(); //updates SystemCoreClock variable
}
