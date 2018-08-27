#include "in.h"

#define INPUT_PORT 		GPIOA
#define INPUT_PIN  		0



void IN_initInputPort()
{
	/*Enable PORT A CLOCK*/
	enablePort(A);
	/*Reset Input Pin*/
	resetGpioL(INPUT_PORT,INPUT_PIN);
	/*Config As Digital Input*/
	confGpioL(INPUT_PORT,INPUT_PIN,pulledInput);
	/*Pull pin A0 down*/
	PullPinDown(INPUT_PORT,INPUT_PIN);
}



tBOOL IN_readIRinput(void)
{
	return getInput(INPUT_PORT,INPUT_PIN);
}
