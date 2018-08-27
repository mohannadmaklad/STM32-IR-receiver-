#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

enum EXTI_AFIO{PORTB,PORTC};

//Enable an interrupt using its number
#define enableInterrupt(irq)      (NVIC->ISER[((uint32_t)(irq) >> 5)] = (1 << ((uint32_t)(irq) & 0x1F))) 
//To determine the source of the interrupt: use it only for PORT B & C, PORT A is the default
#define EXTIsource(EXTInum,PORT)  (AFIO->EXTICR[EXTInum/4] |= ((uint32_t)0x1 << (PORT+(EXTInum*4))))

#define unmaskEXTI(EXTInum)       (EXTI->IMR  |= ((uint32_t)0x1 << EXTInum))
#define risingEdgeEXTI(EXTInum)   (EXTI->RTSR |= ((uint32_t)0x1 << EXTInum))
#define fallingEdgeEXTI(EXTInum)  (EXTI->FTSR |= ((uint32_t)0x1 << EXTInum))
#define resetPendingBit(EXTInum)  (EXTI->PR   |= ((uint32_t)0x1 << EXTInum))






#endif

/*
EXTI6
EXTI7
EXTI8
EXTI9
9.4.3 External interrupt configuration register 1 (AFIO_EXTICR1)

1- write to 9.4.4 External interrupt configuration register 2 (AFIO_EXTICR2)
2- interrupt mask register 10.3.1 Interrupt mask register (EXTI_IMR)
3- edge detecting : 10.3.3 Rising trigger selection register (EXTI_RTSR), 10.3.4 Falling trigger selection register (EXTI_FTSR)
4- pending request register: 10.3.6 Pending register (EXTI_PR) "This bit is set when the selected edge event arrives on the external interrupt line. This bit is
cleared by writing a ‘1’ into the bit."

*/