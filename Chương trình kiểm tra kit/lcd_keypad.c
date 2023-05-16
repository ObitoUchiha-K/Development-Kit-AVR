#include "lcd_keypad.h"
uint8_t count_key = 0;
uint8_t tram,chuc,donvi;
uint8_t BCD_BUF[5];
unsigned char scan_code_row[4]={0X70,0XB0,0XD0,0XE0};
unsigned char scan_code_colum[4]={0x0E,0x0D,0x0B,0x07};
unsigned char key[4][4]=
{ 'C','D','E','F',
	'8','9','A','B',
	'4','5','6','7',
'0','1','2','3'};

void lcd_command(unsigned char cmd){ //rs =PB.0,Rw = PB.1,E=PB.2
	PORTA =(PORTA&0x0F)|(cmd&0xF0); //
	PORTA&=~(1<<0); // rs =0
	PORTA&=~(1<<1); // rw=0
	PORTA|=(1<<2); //e=1
	_delay_us(1);
	PORTA&=~(1<<2);//e=0
	_delay_us(100);
	PORTA =(PORTA&0x0F)|(cmd<<4); //
	PORTA|=(1<<2); //e=1
	_delay_us(1);
	PORTA&=~(1<<2);//e=0
	_delay_us(100);
}
void lcd_data(unsigned char data){
	PORTA =(PORTA&0x0F)|(data&0xF0);
	PORTA &= ~(1<<1); //rw = 0
	PORTA|=1;         //rs = 1
	PORTA|=(1<<2); // e =1
	_delay_us(1);
	PORTA&=~(1<<2); //e= 0
	PORTA =(PORTA&0x0F)|(data<<4); //
	PORTA|=(1<<2); //e=1
	_delay_us(1);
	PORTA&=~(1<<2);//e=0
	_delay_us(100);
}
void lcd_init(){
	DDRA = 0xff;
	PORTA&=~(1<<2); //e= 0
	_delay_ms(20);
	lcd_command(0x33);
	lcd_command(0x32);
	lcd_command(0x28);//mode 8
	lcd_command(0x0C); // on display off cursor
	lcd_command(0x06); // auto move cursor
	lcd_command(0x01); // clear
	_delay_ms(2);
	lcd_command(0x80); // fist line
	_delay_ms(100);
}
void goto_xy(unsigned int x,unsigned int y){
	unsigned char t;
	t= (x==1?0x80:0xC0)+y-1;
	lcd_command(t);
}
void lcd_print(char *p){
	while(*p){
		lcd_data(*p++);
	}
}

void xoa_kytu(){
	goto_xy(1,1);
	lcd_print("                ");
	goto_xy(2,1);
	lcd_print("                ");
}
// put strings when begin
//

void lcd_putstr(){
	lcd_init();
	lcd_print("Xungvuong:   (%)");
	goto_xy(2,1);
	lcd_print("T(uS):");
	_delay_ms(10);
}


void BIN_BCD16(uint16_t x){
	uint8_t i;
	for( i=0;i<=4;i++) //xóa buffer BCD 5 ô nh?
	BCD_BUF[i]=0 ;
	i=4 ; //kh?i ??ng ô nh? th? 5
	do
	{
		BCD_BUF[i]=x%10 ;//c?t d? s? phép chia 10 vào buffer
		BCD_BUF[i]+=0x30;
		i-- ; //gi?m ??a ch? buffer
		x=x/10 ; //l?y th??ng s? phép chia
	}
	while(i!=0) ; //thoát khi th??ng s?=0
}



uint8_t get_key(){
	unsigned int i,j,key_pad;
	for(i=0;i<4;i++){
		PORTC &=0x0F;
		PORTC |=scan_code_row[i];
		_delay_ms(6);
		key_pad = 0x0F&PINC;
		if( key_pad != 0x0F) {
			for(j=0;j<4;j++){
				if(key_pad == scan_code_colum[j]) {
					if (count_key == 0){
						tram=key[i][j]-0x30;
						goto_xy(1,11);
						lcd_data(key[i][j]);
						_delay_ms(200);
					}
					else if (count_key == 1){
						chuc=key[i][j]-0x30;
						goto_xy(1,12);
						lcd_data(key[i][j]);
						_delay_ms(200);
					}
					else if (count_key == 2){
						donvi=key[i][j]-0x30;
						goto_xy(1,13);
						lcd_data(key[i][j]);
						_delay_ms(200);
					}
					count_key++;
					return key[i][j];
				}
			}
		}
	}
}

void taoxung_T2ms(){
	uint16_t dorong;
	count_key = 0;

	while((count_key<3)&&((PIND&0x3C)!=0x3C)){
		get_key();
	}
	dorong=tram*100+chuc*10+donvi;
	dorong=dorong*64;
	dorong=dorong/25;
	DDRD |=(1<<PORTD7);
	TCCR2A |= (1<<WGM21) | (1<<WGM20); // top = 0xff
	TCCR2B &= ~(1<<WGM22);
	// Do PWM on pin OC2A
	TCCR2A = (TCCR2A | (1<<COM2A1)) & ~(1<<COM2A0);  // clear pin OC2A on compare match
	// set at bottom
	// prescaler x64
	TCCR2B = (TCCR2B & ~((1<<CS20) | (1<<CS21))) | (1<<CS22);
	OCR2A = dorong;
}
uint8_t Doxung_us(){
	uint16_t temp0,temp1;
	TIFR1 |=(1<<ICF1) ;//xóa co ICF1 chu?n b? ?o
	while(!(TIFR1&(1<<ICF1))) ;//c? ICF1=1?
	TIFR1 |=(1<<ICF1) ;//xóa c? ICF1
	temp0=ICR1 ;//th?i ?i?m c?nh lên xung
	TCCR1B=0x02 ;//Timer1 mode NOR,N=1,xung kích c?nh lên
	while(!(TIFR1&(1<<ICF1))) ;//c? ICR1=1?
	TIFR1 |=(1<<ICF1) ;//xóa c? ICR1
	temp1=ICR1 ;//th?i ?i?m c?nh xu?ng xung k? ti?p
	TCCR1B=0x42 ;//Timer1 mode NOR,N=8,xung kích c?nh lên
	temp1=temp1-temp0 ;//tính chu k?
	BIN_BCD16(temp1);
	goto_xy(2,7);
	lcd_data(BCD_BUF[1]);
	lcd_data(BCD_BUF[2]);
	lcd_data(BCD_BUF[3]);
	lcd_data(BCD_BUF[4]);

}

void lcd_keypad_xung(){
	uint8_t tam3;
	DDRA|=(1<<PINA0)|(1<<PINA1)|(1<<PINA2)|(1<<PINA4)|(1<<PINA5)|(1<<PINA6)|(1<<PINA7);
	DDRC |=(1<<PINC4)|(1<<PINC5)|(1<<PINC6)|(1<<PINC7);// PC4-7 ->H0-H3 OUT  / PC0-PC3 ->C0-C3
	PORTC = 0XFF;
	lcd_putstr();
	TCCR1A=0x00 ;//Timer1 mode Normal
	TCCR1B=0x42;
	taoxung_T2ms();
	tam3 = PIND&0x3C;
	while(tam3!=0x3C){
		tam3 = PIND&0x3C;
		Doxung_us();
	}
	//___dung timer va dua moi thu ve gia tri ban dau___//
	xoa_kytu();
	DDRA =0;
	DDRC=0;
	PORTA=0;
	PORTC=0;
	TCCR1B=0;
	TCCR2A=0;
	TCCR2B=0;
	
}