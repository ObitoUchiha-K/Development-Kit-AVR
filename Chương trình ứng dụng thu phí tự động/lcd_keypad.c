#include "lcd_keypad.h"


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







