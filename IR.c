#include "IR.h"
#include "in.h"
#include "gpio.h"
#include "timer.h"

#define MARK        0
#define SPACE       1



/*Availabe protocols*/
#define AVAILABLE_PROTOCOLS 2
typedef enum
{
	NEC = 0,
	SONY
}tPROTOCOL;


/*General parameters*/
#define TOLERANCE 			 0.25f
#define GapPeriod   		 5000
#define GapPeriodTICKS   GapPeriod/USPERTICK


/*Sony protocol Parameters*/
#define SONY_numOfBits          50
#define SONY_HeaderPeriod       2400
#define SONY_OneMarkPeriod      1200
#define SONY_ZeroMarkPeriod     600
#define SONY_SpacePeriod        600
#define SONY_BufferSize         (2*SONY_numOfBits)+3


/*NEC protocol Parameters*/
#define NEC_numOfBits          	32
#define NEC_HeaderMarkPeriod    9000
#define NEC_HeaderSpacePeriod   4500
#define NEC_MarkPeriod          560
#define NEC_ZeroSpacePeriod     560
#define NEC_OneSpacePeriod      1690
#define NEC_BufferSize         (2*NEC_numOfBits)+3


/*State Machine definitions for SONY protocol*/
typedef enum
{
    IR_IDLE = 0,
    IR_MARK,
    IR_SPACE
}tIR_STATE;

tIR_STATE currentState;


/*Private global variables*/
/*------------------------------------------------------------------------------------*/
/*------------------------------- Receiving Paramaters -------------------------------*/
/*------------------------------------------------------------------------------------*/
tBOOL newCodeFlag; //1 If a key is pressed and decoded successfully
tIR_DATA receivedData; //the received data in hexa
unsigned int recvTimer;
static char index;
unsigned int periods[NEC_BufferSize+2]; //2 more places to handle overflow cases
volatile tBOOL IRinput; //polling the IR input pin
volatile tBOOL deb;


/*------------------------------------------------------------------------------------*/
/*-------------------------------- Sending Paramaters --------------------------------*/
/*------------------------------------------------------------------------------------*/
unsigned int sendTimer;
unsigned char currentBit;



/*Private Interface*/

/*
    INPUTS: A POINTER TO THE ARRAY OF MEASURED PERIODS
    OUTPUTS: THE RESULTING CODE (1s AND 0s)
*/
void        IR_resetRecieveData(void);
tBOOL       IR_checkOverflow(void);
tBOOL 			IR_NECmatchHeaderMark(char index);
tBOOL 			IR_NECmatchHeaderSpace(char index);
tBOOL 			IR_NECmatchMark(char index);
tBOOL 			IR_NECmatchOneSpace(char index);
tBOOL 			IR_NECmatchZeroSpace(char index);
tBOOL   	  IR_NECdecode(unsigned int* periods);

void 				IR_sendHeader();
void 				IR_sendBit(tBOOL bit);


/*Private interface definitions*/

void        IR_resetRecieveData(void)
{
    newCodeFlag  = 0;        //no received code or not complete
    receivedData = 0;   //no decoded data
    recvTimer  = 0;          //reset the timer
    index = 0;
}

tBOOL       IR_checkOverflow(void)
{
    return (index > (NEC_BufferSize+1));
}

/**/
tBOOL 			IR_NECmatchHeaderMark(char index)
{
	float Lmark,Umark;
	
 	Lmark = (1.0-TOLERANCE) * (float)NEC_HeaderMarkPeriod;
	Umark = (1.0+TOLERANCE) * (float)NEC_HeaderMarkPeriod;
	
	return (((periods[index]*USPERTICK)) >= Lmark && ((periods[index]*USPERTICK) <= Umark));
}

tBOOL 			IR_NECmatchHeaderSpace(char index)
{
	float Lmark,Umark;
	Lmark = (1.0-TOLERANCE) * (float)NEC_HeaderSpacePeriod;
	Umark = (1.0+TOLERANCE) * (float)NEC_HeaderSpacePeriod;
	
	return (((periods[index]*USPERTICK)) >= Lmark && ((periods[index]*USPERTICK) <= Umark));
}

tBOOL 			IR_NECmatchMark(char index)
{
	float Lmark,Umark;
	Lmark = (1.0-TOLERANCE) * (float)NEC_MarkPeriod;
	Umark = (1.0+TOLERANCE) * (float)NEC_MarkPeriod;

	return (((periods[index]*USPERTICK)) >= Lmark && ((periods[index]*USPERTICK) <= Umark));
}

tBOOL 			IR_NECmatchOneSpace(char index)
{
	float Lspace,Uspace;
	Lspace = (1.0-TOLERANCE) * (float)NEC_OneSpacePeriod;
	Uspace = (1.0+TOLERANCE) * (float)NEC_OneSpacePeriod;
	
	return (((periods[index]*USPERTICK) >= Lspace) && ((periods[index]*USPERTICK) <= Uspace));
}

tBOOL 			IR_NECmatchZeroSpace(char index)
{
	float Lspace,Uspace;
	Lspace = (1.0-TOLERANCE) * (float)NEC_ZeroSpacePeriod;
	Uspace = (1.0+TOLERANCE) * (float)NEC_ZeroSpacePeriod;
	
	return (((periods[index]*USPERTICK) >= Lspace) && ((periods[index]*USPERTICK) <= Uspace));
}

/**/
tBOOL    IR_NECdecode(unsigned int* periods)
{
	
	unsigned char i;
	unsigned char currentBit = 0;
	
	/*Check the header mark*/
	if(!IR_NECmatchHeaderMark(1)) 	 return 0;  //faulty header MARK
	if(!IR_NECmatchHeaderSpace(2))	 return 0;	//faulty header SPACE
	
	/*Loop over the recieved periods, check for a 1 or a 0, then increment by 2*/
	for(i=3 ; i<NEC_BufferSize ; i+= 2)
	{
		/*Check for a prober '1' mark and '1' space*/
		/*if found, mark the current bit as 1*/
		if(IR_NECmatchMark(i) && IR_NECmatchOneSpace(i+1))
		{
			receivedData |= (1 << currentBit++); 
			continue;
		}
		/*Check for a prober '0' mark and '0' space*/
		/*if found, mark the current bit as 0*/
		if(IR_NECmatchMark(i) && IR_NECmatchZeroSpace(i+1))
		{
			currentBit++;
			continue;
		}
		
		return 0; //not a valid 1 or a zero, failed decoding
	}
	
	newCodeFlag = 1;
	return 1; //success
}
/**/
void 				IR_sendHeader()
{
	/*Send the Mark*/
	TIMER_enablePWM();
 	TIMER_delay(NEC_HeaderMarkPeriod);
		
	/*Send the Space*/
	TIMER_disablePWM();
	TIMER_delay(NEC_HeaderSpacePeriod);
	
}

void 				IR_sendBit(tBOOL bit)
{
	/*Send the Mark*/
	TIMER_enablePWM();
	TIMER_delay(NEC_MarkPeriod);
	
	/*Send the Space*/
	TIMER_disablePWM();
	if(bit & 0x1) TIMER_delay(NEC_OneSpacePeriod);
	else TIMER_delay(NEC_ZeroSpacePeriod);

}

/*Public interface definitions*/

void        IR_init(void)
{
    /*config GPIO input pin*/
		IN_initInputPort(); 
    /*start in IDLE state*/
    tIR_STATE currentState = IR_IDLE;
    IR_resetRecieveData();
}


/*
 *  DESCRIPTION : Checks the current state and Updates the received periods array if it's in any of the receiving states
 *                if the receiving state is IDLE, it just increases the timer to measure the gap
*/
void        IR_recvUpdate(void)
 {
		IRinput = IN_readIRinput() ;

    /*Increment the ticks counter*/
    recvTimer++;

    /*State Machine*/
    switch(currentState)
    {
        case IR_IDLE:
						
						/*get input and check for a mark && the end of the current gap*/
						if(IRinput == MARK)
						{ 
							/*Check for a long gap && old clicks are received*/
							if(recvTimer < GapPeriodTICKS || (newCodeFlag == 1))
							{
								recvTimer = 0;
							}
							else
							{
								periods[index++] = recvTimer; //store the period of the gap
								if(IR_checkOverflow())
								{
										//shouldn't be here ever
										//add debug code
										IR_resetRecieveData();
										currentState = IR_IDLE;
								}
								else
								{
										recvTimer = 0;
										currentState = IR_MARK;
								}
							}
						}
					
            break;

        case IR_MARK:
             if(IRinput == SPACE)
            {
                periods[index++] = recvTimer;
                if(IR_checkOverflow())
                {
                    IR_resetRecieveData();
										currentState = IR_IDLE;
                }
                else
                {
                    recvTimer = 0;
                    currentState = IR_SPACE;
                }
            }
            break;

        case IR_SPACE:
            if(IRinput == MARK)
            {
                periods[index++] = recvTimer;
                if(IR_checkOverflow())
                {
                    IR_resetRecieveData();
										currentState = IR_IDLE;
                }
                else
                {
                    recvTimer = 0;
                    currentState = IR_MARK;
                }
            }
            else //check for a long gap
            {
                if(recvTimer >= GapPeriodTICKS ) //received full packet or stopped in the middle of packet
                {
									
									if(index == (NEC_BufferSize+1))//success
									{
										/*decode the received data, update receivedData variable and raise newCode flag*/
										IR_NECdecode(periods); 
									}
									else //failed
									{
										/*reset and start over again*/
										IR_resetRecieveData();
									}
									currentState = IR_IDLE;
                }
            }
            break;
    }
}

/*Returns true if a new code(a code that didn't got accessed) is found*/
tBOOL       IR_validCodeDetected(void)
{
    return newCodeFlag;
}


tIR_DATA    IR_getRecievedCode(void)
{
	/*Reset the routine protocol and start again*/
	tIR_DATA ret = receivedData;
	IR_resetRecieveData();
	currentState = IR_IDLE;
	/*No more data to return*/
	newCodeFlag = 0;
	return ret;
}
/**/


void	IR_sendNECCode(tIR_DATA hexData)
{
	TIMER_disablePWM();
	
 	IR_sendHeader();
	for(currentBit = 0 ; currentBit < NEC_numOfBits ; currentBit++)
	{
		IR_sendBit((hexData & (0x1 << currentBit)) >> currentBit) ;
	}
}
