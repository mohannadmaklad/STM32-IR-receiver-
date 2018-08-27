#include "IR.h"
#include "in.h"
#include "gpio.h"

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
    SONY_IDLE = 0,
    SONY_MARK,
    SONY_SPACE,
    SONY_STOP,
    SONY_OVERFLOW,
}tIR_STATE;

tIR_STATE currentState;


/*Private global variables*/
tBOOL newCodeFlag;
tIR_DATA receivedData;
unsigned int timer;
static char index;
unsigned int periods[SONY_BufferSize];
volatile tBOOL IRinput;
volatile tBOOL deb;


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

/*Private interface definitions*/

void        IR_resetRecieveData(void)
{
    newCodeFlag  = 0;        //no received code or not complete
    receivedData = 0;   //no decoded data
    timer = 0;          //reset the timer
    index = 0;
}

tBOOL       IR_checkOverflow(void)
{
    return (index >= SONY_BufferSize);
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


/*Public interface definitions*/

void        IR_init(void)
{
    /*config GPIO input pin*/
		IN_initInputPort(); 
    /*start in IDLE state*/
    tIR_STATE currentState = SONY_IDLE;
    IR_resetRecieveData();
}


/*
 *  DESCRIPTION : Checks the current state and Updates the received periods array if it's in any of the receiving states
 *                if the receiving state is IDLE, it just increases the timer to measure the gap
*/
void        IR_update(void)
{
	IRinput = IN_readIRinput() ;

	
/*	
		if(IRinput == 0 && deb == 0 ) IRinput = 0;
		else IRinput = 1;
*/
    /*Increment the ticks counter*/
    timer++;

    /*State Machine*/
    switch(currentState)
    {
        case SONY_IDLE:
            /*get input and check for a mark && the end of the current gap*/
						if(timer < GapPeriodTICKS)
						{
							timer++;
						}
            else if(IRinput == MARK)
            {
                periods[index++] = timer; //store the period of the gap
                if(IR_checkOverflow())
                {
                    //shouldn't be here ever
                    //add debug code
                    currentState = SONY_OVERFLOW;
                }
                else
                {
                    timer = 0;
                    currentState = SONY_MARK;
                }
            }
            break;

        case SONY_MARK:
             if(IRinput == SPACE)
            {
                periods[index++] = timer;
                if(IR_checkOverflow())
                {
                    currentState = SONY_OVERFLOW;
                }
                else
                {
                    timer = 0;
                    currentState = SONY_SPACE;
                }
            }
            break;

        case SONY_SPACE:
            if(IRinput == MARK)
            {
                periods[index++] = timer;
                if(IR_checkOverflow())
                {
                    currentState = SONY_OVERFLOW;
                }
                else
                {
                    timer = 0;
                    currentState = SONY_MARK;
                }
            }
            else
            {
                if(timer >= GapPeriodTICKS ) //received full packet or stopped in the middle of packet
                {
 									IR_NECdecode(periods);
									if(index < NEC_BufferSize) {IR_resetRecieveData();currentState = SONY_IDLE;}
                  else currentState = SONY_STOP;
									
                }
            }
            break;

        case SONY_STOP: //a blocking state
            if(IRinput == MARK) timer = 0;
            break;

        case SONY_OVERFLOW:
            IR_resetRecieveData();
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
	
	/*No more data to return*/
	/*Reset the routine protocol and start again*/
	IR_resetRecieveData();
	currentState = SONY_IDLE;
	newCodeFlag = 0;
	return receivedData;
}



