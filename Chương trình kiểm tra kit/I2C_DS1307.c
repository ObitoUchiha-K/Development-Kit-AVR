/*
 * I2C_DS1307.c
 *
 * Created: 12/12/2022 2:15:55 PM
 *  Author: obito.uchiha
 */ 
#include "I2C_DS1307.h"
#include "lcd_keypad.h"
 uint8_t giay,phut,gio,thu,ngay,thang,nam; // cac bi?n th?t
 uint8_t giay1,phut1,gio1,thu1,ngay1,thang1,nam1; // các bi?n t?m ?? l?u tr? 
 
//??t t?c ?? tuy?n=100Khz
void I2C_INIT(){
	TWBR=8;
	TWSR=1;
	TWCR=(1<<TWEN);
}

void I2C_START(){
	TWCR=(1<<TWEN)|(1<<TWSTA)|(1<<TWINT);//cho phép TWI,START,xóa TWINT
	while (!(TWCR & (1<<TWINT)));
}

void I2C_STOP(){
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

void I2C_WRITE(uint8_t data){
	TWDR = data;
	TWCR=(1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
}

uint8_t I2C_READ_ACK(){
	TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}
uint8_t I2C_READ_NACK(){
	TWCR=(1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}

uint8_t Get_Switch() { //____lay ma switch_______//

uint8_t sw=0,i=0;

while(i<50) // chong rung phim bang quet 50 lan
{
	sw=PINC&0xF0;
	if (sw!=0xF0) i++;
	else {
		i=51;
	}
}
while((PINC&0xF0)!=0xF0); // cho nha phím

if (sw==0xF0) return 0; // ko nhan
else if (sw==0xE0) return 1; // sw1
else if (sw==0xD0) return 2; // sw2
else if (sw==0xB0) return 3; // sw3
else if (sw==0x70) return 4; // sw4
else return 0;
}

void Write_DS1307(){//__________viet data vào ds1307______
	I2C_START();
	I2C_WRITE(0XD0);
	I2C_WRITE(0);
	I2C_WRITE(giay);
	I2C_WRITE(phut);
	I2C_WRITE(gio);
    I2C_WRITE(thu);
	I2C_WRITE(ngay);
	I2C_WRITE(thang);
	I2C_WRITE(nam);
	I2C_STOP();
}
void Read_DS1307(){//__________doc data tu ds1307______
	I2C_START();
	I2C_WRITE(0XD0);
	I2C_WRITE(0);
	I2C_START();
	I2C_WRITE(0XD1);
	giay=I2C_READ_ACK();
	phut=I2C_READ_ACK();
	gio=I2C_READ_ACK();
	thu=I2C_READ_ACK();
	ngay=I2C_READ_ACK();
	thang=I2C_READ_ACK();
	nam=I2C_READ_NACK();
	I2C_STOP();
}
void lcd_putstr_ds1307(){
	lcd_init();
	lcd_print("Ngay:  /  /");
	goto_xy(2,1);
	lcd_print("T:   Hr:  :  :");
}
void Display_DS1307(){ //______hi?n th? lên lcd_______//
	goto_xy(1,6);
	lcd_data((ngay>>4)+0x30);
	lcd_data((ngay&0x0F)+0x30);
	goto_xy(1,9);
	lcd_data((thang>>4)+0x30);
	lcd_data((thang&0x0F)+0x30);
	goto_xy(1,12);
	lcd_data(0x32);
	lcd_data(0x30);
	lcd_data((nam>>4)+0x30);
	lcd_data((nam&0x0F)+0x30);
	//_____________________
	goto_xy(2,3);
	if (thu==0x01) lcd_print("CN");
	else {
		lcd_data(thu+0x30);
		lcd_data(0x20);
	}
	goto_xy(2,9);
	lcd_data((gio>>4)+0x30);
	lcd_data((gio&0x0F)+0x30);
	_delay_ms(10);
	goto_xy(2,12);
	lcd_data((phut>>4)+0x30);
	lcd_data((phut&0x0F)+0x30);
	goto_xy(2,15);
	lcd_data((giay>>4)+0x30);
	lcd_data((giay&0x0F)+0x30);
}
void Store_data(uint8_t x){//_______hàm luu tru vao và lay ra tu bien tam_______//
	if(x==0){
	ngay1=(ngay>>4)*10+(ngay&0x0F);
	thang1=(thang>>4)*10+(thang&0x0F);
	nam1=(nam>>4)*10+(nam&0x0F);
	thu1=thu;
	gio1=(gio>>4)*10+(gio&0x0F);
	phut1=(gio>>4)*10+(gio&0x0F);
	giay1=(gio>>4)*10+(gio&0x0F);
	}
	else if(x==1){
	 ngay=((ngay1/10)<<4)+ngay1%10;
	 thang=((thang1/10)<<4)+thang1%10;
	 nam=((nam1/10)<<4)+nam1%10;
	 gio=((gio1/10)<<4)+gio1%10;
	 phut=((phut1/10)<<4)+phut1%10;
	 giay=((giay1/10)<<4)+giay1%10;
	 thu=thu1;
	}
}
void Setdata_ds1307(){//______ thiet lap data cho ds1307______//
    uint8_t tam4=0,i=0;
	Store_data(0);
	while(tam4!=4){
	  tam4=Get_Switch();
	  if (tam4 == 3) 
	  {
		  if (i==6) i=0;
		  else i++;
	  }
      switch(i){
	     case 0: {
			 if (tam4==1){
				 if (ngay1==1) ngay1=31;
				 else ngay1--; 
			 }
			 else if(tam4==2){
				 if (ngay1==31) ngay1=1;
				 else ngay1++;
			 }
			 Store_data(1);
			 Display_DS1307();
			 break;
		 }
		 case 1:{
		     if (tam4==1){
			     if (thang1==1) thang1=12;
			     else thang1--;
		     }
		     else if(tam4==2){
			     if (thang1==12) thang1=1;
			     else thang1++;
		      }
		     Store_data(1);
		     Display_DS1307();
		     break;
		 }
		 case 2:{
			 if (tam4==1){
				 if (nam1==0) nam1=99;
				 else nam1--;
			 }
			 else if(tam4==2){
				 if (nam1==99) nam1=0;
				 else nam1++;
			 }
			 Store_data(1);
			 Display_DS1307();
			 break;
		 }
		 case 3:{
			 if (tam4==1){
				 if (thu1==1) thu1=7;
				 else thu1--;
			 }
			 else if(tam4==2){
				 if (thu1==7) thu1=1;
				 else thu1++;
			 }
			 Store_data(1);
			 Display_DS1307();
			 break;
	     }
		 case 4:{
			 if (tam4==1){
				 if (gio1==0) gio1=23;
				 else gio1--;
			 }
			 else if(tam4==2){
				 if (gio1==23) gio1=0;
				 else gio1++;
			 }
			 Store_data(1);
			 Display_DS1307();
			 break;
		 }
		 case 5:{
			 if (tam4==1){
				 if (phut1==0) phut1=59;
				 else phut1--;
			 }
			 else if(tam4==2){
				 if (phut1==59) phut1=0;
				 else phut1++;
			 }
			 Store_data(1);
			 Display_DS1307();
			 break;
	     }
		 case 6:{
			 if (tam4==1){
				 if (giay1==0) giay1=59;
				 else giay1--;
			 }
			 else if(tam4==2){
				 if (giay1==59) giay1=0;
				 else giay1++;
			 }
			 Store_data(1);
			 Display_DS1307();
			 break;
		 }
       }
	}
  Write_DS1307();
}

void I2C_DS1307(){
	uint8_t tam3,s_w;
	DDRA=0xff;
	DDRC=0;
	PORTC|=0xF0;
	lcd_putstr_ds1307();
	I2C_INIT();
	tam3 = PIND&0x3C;
	while(tam3!=0x3C){
		tam3 = PIND&0x3C;
		Read_DS1307();
		Display_DS1307();
		s_w=Get_Switch();
		if(s_w==3) Setdata_ds1307();
	}
	xoa_kytu();
	PORTA=0;
	PORTC=0;
	DDRA=0;
	DDRC=0;
}
