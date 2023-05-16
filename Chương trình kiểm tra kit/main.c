
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "buzzer_rgb.h"
#include "lcd_keypad.h"
#include "led7_74hc573_lm35.h"
#include "I2C_DS1307.h"

uint8_t Check=0;
uint8_t main(void)
{

	//_____Thiet lap chung_____//
	DDRD  &= 0xC3; //PD2-5 input
	PORTD|=(1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD5); // dien tro keo len
	EICRA=(1<<ISC21) | (0<<ISC20) | (0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);// falling edge
	EIMSK=(1<<INT2) | (0<<INT1) | (0<<INT0); // enable interrup 2
	sei();
	while (1)
	{
		Check = (~((PIND&0x3C)>>2))&0x0F;

		if((Check==1)&&(PIND&0x20!=0)){
			buzzer_led();
		}
		else if((Check==2)&&(PIND&0x20!=0)){
			led7_lm35();
		}
		else if((Check==3)&&(PIND&0x20!=0)){
			lcd_keypad_xung();
		}
		else if((Check==4)&&(PIND&0x20!=0)){
			I2C_DS1307();
		}		
	}
}

void shift_out(uint8_t dat)
{
	DDRB|=(1<<PINB0)|(1<<PINB1)|(1<<PINB3);
	unsigned char tam,i,x;
	x=dat;
	tam=0x80 ;// xét bit 7
	for (i=1;i<=8;i++) // vòng lap gui lan luot 8 bit ra
	{
		if((x&tam)==tam) //bit xét=1
		{
			PORTB|=(1<<PINA3);
			PORTB|=(1<<PINA0);
			_delay_us(10);
			PORTB &=~((1<<PINA0)|(1<<PINA1)|(1<<PINA3));
		}
		else // bit xét =0
		{
			PORTB&=(1<<PINA3);
			PORTB|=(1<<PINA0);
			_delay_us(10);
			PORTB &=~((1<<PINA0)|(1<<PINA1)|(1<<PINA3));
		}
		tam=tam>>1; //xét bit ke tiep
		
	}
	PORTB|=(1<<PINA1); //xuat xung  xuat data
	PORTB &=~((1<<PINA0)|(1<<PINA1)|(1<<PINA3));

}
void USART_Transmit( uint8_t data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) )
	;
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

ISR(INT2_vect)
{
 	UBRR0H = 0;
 	UBRR0L = 51; // 9600
 	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0); // cho phép ngat nhan data
 	/* Set frame format: 8data, 2stop bit */
 	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

ISR(USART0_RX_vect) {
	uint8_t data;
	data= UDR0;
    for(uint8_t i=0;i<5;i++){
	shift_out(0);
		shift_out(data);
	}
	USART_Transmit(data);
}

