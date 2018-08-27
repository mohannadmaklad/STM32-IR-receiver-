#ifndef IR_H_INCLUDED
#define IR_H_INCLUDED

/*Includes*/
#include "type.h"

#define USPERTICK 	2


/*Public Interface*/
void        IR_init(void);
void        IR_update(void);                //periodic task
tBOOL       IR_validCodeDetected(void);
tIR_DATA    IR_getRecievedCode(void);



#endif // IR_H_INCLUDED


//A659FF00
//A659FF00
//AE51FF00