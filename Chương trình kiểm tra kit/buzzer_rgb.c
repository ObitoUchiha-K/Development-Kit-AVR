/*
 * buzzer_rgb.c
 *
 * Created: 11/29/2022 6:45:16 PM
 *  Author: obito.uchiha
 */ 
#include "buzzer_rgb.h"

void buzzer_led(){
uint8_t tam3 = PIND&0x3C; ;
DDRA=0xFF;
DDRC|=(1<<PORTC7);	
PORTC |=0x80;// Pin buzzer PC7
_delay_ms(200);
PORTC&=~(1<<PORTC7);
	while(tam3!=0x3C){
	tam3 = PIND&0x3C;
	
	PORTA =0x01;
	_delay_ms(250);
	PORTA =0; 
	_delay_ms(250);

	PORTA =0x02;
	_delay_ms(500);
	PORTA =0;
	_delay_ms(250);
	
	// red , green,blue , red-green, red-blue,green blu ,trang
		PORTA =0x04;
		_delay_ms(570);
		PORTA = 0x08;
		_delay_ms(570);
		PORTA = 0x10;
		_delay_ms(570);
		PORTA= 0x0C;
		_delay_ms(570);
		PORTA= 0x14;
		_delay_ms(570);
		PORTA= 0x18;
		_delay_ms(570);
		PORTA = 0;
		_delay_ms(250);

	PORTA =0x20;
	_delay_ms(570);
	PORTA = 0x40;
	_delay_ms(570);
	PORTA = 0x80;
	_delay_ms(570);
	PORTA= 0x60;
	_delay_ms(570);
	PORTA= 0xA0;
	_delay_ms(570);
	PORTA= 0xC0;
	_delay_ms(570);
	PORTA = 0;
	_delay_ms(250);
	tam3 = PIND&0x3C;
	}
PORTC |=0x80;// Pin buzzer PC7
_delay_ms(200);
PORTC&=~(1<<PORTC7);
//___dua moi thu ve gia tri ban dau___//
DDRA=0;
PORTA=0;
DDRC=0;
PORTC=0;
}
	
