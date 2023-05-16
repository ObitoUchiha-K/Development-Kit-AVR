/*
 * led7_74hc595.c
 *
 * Created: 11/30/2022 10:05:42 PM
 *  Author: obito.uchiha
 */ 
#include "led7_74hc573_lm35.h"
float temp;
uint8_t integral,decimal,integral1; //for lm35
uint8_t led_code[4],j;
uint16_t k,tram,chuc;


//adc_lm35
unsigned int read_adc(unsigned char adc_input)
{
	ADMUX|=(0<<REFS1)|(1<<REFS0)|(0<<ADLAR)|adc_input;
	// Delay needed for the stabilization of the ADC input voltage
	_delay_us(50);
	// Start the AD conversion
	ADCSRA|=(1<<ADSC);
	while ((ADCSRA & (1<<ADIF))==0);
	ADCSRA|=(1<<ADIF);
	return ADCW;
}
void adc(void){
	temp= read_adc(7)*5.0/1024.0;
	temp = temp*100.0;
	integral = temp/1;
	decimal = (temp - integral)*100;
}


void led7_lm35() 
{
//_delay_ms(2);// 
uint8_t tam3;
uint8_t tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,
	0x83,0xc6,0xa1,0x86,0x8e}; 
uint8_t scan_a =0x70	;       
DDRC=0xFF; // for 74hc573
ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS0); // adc
DDRA|=(1<<PINA0)|(1<<PINA1);
     temp= read_adc(7)*5.0/1024.0;
     temp = temp*100.0;
     integral = temp/1;
     decimal = (temp - integral)*100;
     decimal=decimal/10;
     k=integral*10+decimal;
     led_code[0]=k/1000;
     tram = k%1000;
     led_code[1]=tram/100;
     chuc = k%100;
     led_code[2]=chuc/10;
     led_code[3]=k%10;
	for (j=0;j<4;j++)
	{
		PORTC=0xff;
		PORTA|=(1<<PINA1);//mo le2
		PORTA&=~(1<<PINA1);	//khóa  le2
		if(j==2) PORTC=tab[led_code[j]] & 0x7F;
		else PORTC=tab[led_code[j]];
		PORTA|=(1<<PINA0);//mo le1
		PORTA&=~(1<<PINA0);	;//khóa le1
		PORTC=scan_a;// mã lái anode
		PORTA|=(1<<PINA1);//mo le2
		PORTA&=~(1<<PINA1);	;//khóa  le2
		_delay_ms(5)	;
		scan_a=(scan_a>>1) | 0x80;//quét led ke tiep
	}

//___ dua moi thu ve gia tri ban dau___//
DDRA=0;
DDRC=0;
PORTA=0;
PORTC=0;

}




