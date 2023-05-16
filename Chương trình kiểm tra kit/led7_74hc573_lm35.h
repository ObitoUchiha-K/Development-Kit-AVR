/*
 *
 * Created: 11/30/2022 10:05:59 PM
 *  Author: obito.uchiha
 */ 


#ifndef LED7_74HC573_LM35_H_
#define LED7_74HC573_LM35_H_


#define F_CPU 8000000UL
#include <util/delay.h>
#include <avr/io.h>

//extern void shift_out(uint8_t dat);
extern void led7_lm35(); 

#endif /* LED7_74HC595_H_ */