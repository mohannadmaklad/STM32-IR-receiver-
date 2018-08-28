/* 
if pin number < 8 : Port configuration register low
if pin number > 8 : Port configuration register high

each pin is configured by 4 bits, so the first bit to be written for pin x is : x*4
first two bits : 00 input , 01 output 10MHZ, 10 output 2MHZ, 11 output 50MHZ
secon two bits In output mode:
00: General purpose output push-pull, 01: General purpose output Open-drain
10: Alternate function output Push-pull , 11: Alternate function output Open-drain : 00 
In input mode (MODE[1:0]=00):
00: Analog mode
01: Floating input (reset state)
10: Input with pull-up / pull-down
11: Reserved
 */
 
 
 #ifndef _GPIO_H_
 #define _GPIO_H_

#include "stm32f10x.h"                  // Device header
 
#define AnalogInput    ((uint32_t) 0x0)
#define floatingInput  ((uint32_t) 0x4)
#define pulledInput    ((uint32_t) 0x8)

#define pushPullOutput 		((uint32_t) 0x1)
#define AF_pushPullOutput ((uint32_t) 0x9)


 /*-----------------------------------------------------------------------------------------------------*/
 /*--------------------------------------------- C L O C K ---------------------------------------------*/
 /*-----------------------------------------------------------------------------------------------------*/
enum {A=2, B=3, C=4};
#define enablePort(n) 	RCC->APB2ENR |= 1 << n ;


 
 /*-----------------------------------------------------------------------------------------------------*/
 /*------------------------------------------- Configuration -------------------------------------------*/
 /*-----------------------------------------------------------------------------------------------------*/
#define resetGpioH(GPIO,pinNumber)     (GPIO->CRH &= ~(((uint32_t) 0xf)  << ((pinNumber%8)*4))) 
#define resetGpioL(GPIO,pinNumber)     (GPIO->CRL &= ~(((uint32_t) 0xf)  << ((pinNumber*4)))) 
#define confGpioH(GPIO,pinNumber,mode) (GPIO->CRH |= (mode << ((pinNumber%8)*4)))
#define confGpioL(GPIO,pinNumber,mode) (GPIO->CRL |= (mode << (pinNumber*4)))

 /*-----------------------------------------------------------------------------------------------------*/
 /*----------------------------------------------- Output ----------------------------------------------*/
 /*-----------------------------------------------------------------------------------------------------*/
#define setPin(GPIO,pinNumber)    (GPIO->BSRR |= ((uint32_t)0x1 << pinNumber))  //output 1 or pulled up input
#define resetPin(GPIO,pinNumber)  (GPIO->BRR  |= ((uint32_t)0x1 << pinNumber))  //output 0 or pulled down input
#define togglePin(GPIO,pinNumber) (GPIO->ODR  ^= ((uint32_t)0x1 << pinNumber))  //output 0 or pulled down input

 /*-----------------------------------------------------------------------------------------------------*/
 /*----------------------------------------------- input -----------------------------------------------*/
 /*-----------------------------------------------------------------------------------------------------*/
#define PullPinUp(GPIO,pinNumber) 	(GPIO->ODR |= (1<<pinNumber))
#define PullPinDown(GPIO,pinNumber) (GPIO->ODR &= !(1<<pinNumber))
#define getInput(GPIO,pinNumber)  	((GPIO->IDR & ((uint32_t)0x0001 << pinNumber)) >> pinNumber)
 
 #endif
 

  
 
 
 
 
 
 