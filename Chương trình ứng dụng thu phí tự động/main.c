
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd_keypad.h"
#include "mfrc522.h"
//______for key pad_________//
uint8_t count_key=0;
unsigned char scan_code_row[4]={0X70,0XB0,0XD0,0XE0};
unsigned char scan_code_colum[4]={0x0E,0x0D,0x0B,0x07};
unsigned char key[4][4]=
{ 'C','D','E','F',
	'8','9','A','B',
	'4','5','6','7',
'0','1','2','3'};

// ____variable for MFRC522 and ESP8266________
uint8_t key_money[6]; // data for key money.
uint8_t key_money_temp[8]; // data luu tru
uint8_t money_encryp[4]; // data for check key money.
uint8_t buff[100];
uint8_t CardID[5];
uint8_t	sodu[16];
uint8_t	name[16];
uint8_t leng_data;
uint8_t data_encrypted[16]; // data for block 18.
uint8_t check_block18[16]; // data from block 18
uint8_t str[16];
uint8_t status;
uint8_t sectorKeyA[16][16] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},};
void Display_user(uint8_t c);///
uint8_t Get_Switch() ;
uint8_t get_key();// get_keypad4x4
//___FOR UART_AND_ESP8266s01__//
void USART_Init();
void USART_Transmit( unsigned char data );
unsigned char USART_Receive(uint8_t check);
uint8_t Find_Oke();
uint8_t Wait_Oke(uint8_t x,uint8_t y);
void UART_puts(char* s);
uint8_t caculate_leng();
void ESP8266_init();
void Senddata_to_webserver();
/////___ MFRC522 function____/////////
void lcd_putstr1();
void lcd_putstr2();
void MFRC522_ESP8266();
void Encryption ();
uint8_t Compare_two_block();
uint8_t Trutien();
uint8_t write_two_block(uint8_t block1,uint8_t block2,uint8_t *data1,uint8_t *data2);
uint8_t read_three_block(uint8_t block1,uint8_t block2,uint8_t block3,uint8_t *data1,uint8_t *data2,uint8_t *data3);
void Open_gate();
void Get_keymoney();
void convert_key();
uint8_t kiemtra_naptien();
void cong_tien();
uint8_t main(void)
{
	uint8_t get_sw1;
	// configure
	DDRA = 0xFF; //LCD
	DDRD|=(1<<PIND2)|(1<<PIND3);//Output for motor
	DDRB&=~(1<<PINB3);// Input for sensor
	PORTB|=(1<<PINB3);
	PORTD&=~((1<<PIND2)|(1<<PIND3));
	// for key pad4x4
	DDRC |=(1<<PINC4)|(1<<PINC5)|(1<<PINC6)|(1<<PINC7);// PC4-7 ->H0-H3 OUT  / PC0-PC3 ->C0-C3
	PORTC = 0XFF;
	// for sw1,sw2,sw3
	DDRD&=~((1<<PIND6)|(1<<PIND7)|(1<<PIND5));
	PORTD|=(1<<PIND5)|(1<<PIND6)|(1<<PIND7);
	///
	lcd_putstr2();
	spi_init();
	USART_Init();
	ESP8266_init();
	lcd_putstr1();
	//
	while(1){
		get_sw1=Get_Switch();
		if (get_sw1==1) Get_keymoney(); // nap tien
		//________tac vu tru tien va mo cong_______//
		status = MFRC522_Request(PICC_REQIDL, str);
		if (status == MI_OK)
		{
			read_three_block(16,17,18,name,sodu,check_block18);
			if(kiemtra_naptien()){
						lcd_command(0x01); // clear
						_delay_ms(3);
						goto_xy(1,1);
						lcd_print("   Nap tien");
						goto_xy(2,1);
						lcd_print("  Thanh cong.");
						_delay_ms(2000);
						lcd_putstr1();
			}
			else{
			Encryption();
			if(Compare_two_block()==1){
				if (((sodu[3]<0x35)&&(sodu[2]==0x30)&&(sodu[1]==0x30)&&(sodu[0]==0x30)))
				Display_user(0); // thong bao khong du tien
				else {
					Trutien();
					Encryption();
					write_two_block(17,18,sodu,data_encrypted);
					Display_user(1); // thong bao tien&so du
					Open_gate();
					Senddata_to_webserver();
				}
				lcd_putstr1();
			}
			else if(Compare_two_block()==2)
			Display_user(2);// thong bao ko hop le
		  }
		}
		//____________________________//
		
	}
}
void cong_tien(){
	uint16_t x=0,y=0;
	x=(sodu[0]-0x30)*1000;
	x=x+(sodu[1]-0x30)*100;
	x=x+(sodu[2]-0x30)*10;
	x=x+(sodu[3]-0x30);
	y=(key_money[2]-0x30)*1000;
	y=y+(key_money[3]-0x30)*100;
	y=y+(key_money[4]-0x30)*10;
	y=y+(key_money[5]-0x30);
	x=x+y;
	y=x;
	sodu[0]=(x/1000)+0x30;
	y=x%1000;
	sodu[1]=(y/100)+0x30;
	x=y%100;
	sodu[2]=(x/10)+0x30;
	sodu[3]=x%10+0x30;
}
//__________kiem tra va nap tien________//
uint8_t kiemtra_naptien(){
	uint8_t ck[2]={0,0};
	ck[0]=sodu[14]^CardID[0];
	ck[1]=sodu[15]^CardID[1];
	ck[1]=ck[1]+1;
	if((key_money[0]==ck[0])&&(key_money[1]==ck[1])){
		sodu[14]=ck[0];
		sodu[15]=ck[1];
		cong_tien();
		for(uint8_t i=0;i<6;i++) key_money[i]=0;
		Encryption();
		write_two_block(17,18,sodu,data_encrypted);
		return 1;
	}
	else return 0;
}


//__________end________________//
//______Lay ma switch______________//
uint8_t Get_Switch() { //____lay ma switch_______//
	uint8_t sw=0,i=0;
	while(i<50) // chong rung phim bang quet 50 lan
	{
		sw=PIND&0xE0;
		if (sw!=0xE0) i++;
		else {
			i=51;
		}
	}
	while((PIND&0xE0)!=0xE0); // cho nha phim

	if (sw==0xE0) return 0; // ko nhan
	else if (sw==0x60) return 1; // sw1
	else if (sw==0xA0) return 2; // sw2
	else if (sw==0xC0) return 3; //sw3
}
//_____________lay key pad______________________//
uint8_t get_key(){
	unsigned int i,j,key_pad;
	for(i=0;i<4;i++){
		PORTC &=0x0F;
		PORTC |=scan_code_row[i];
		_delay_ms(10);
		key_pad = 0x0F&PINC;
		if( key_pad != 0x0F) {
			for(j=0;j<4;j++){
				if(key_pad == scan_code_colum[j]) {
					key_money_temp[count_key]=key[i][j];
					_delay_ms(200);
					if (count_key==15) count_key=0;
					else count_key++;
					return key[i][j];
				}
			}
		}
	}
}
//___________end_)_____________________//
void convert_key(){
	uint8_t i;
	for( i=0;i<4;i++)
	{
		if(key_money_temp[i]>0x39) key_money_temp[i]=key_money_temp[i]-55;
		else key_money_temp[i]=key_money_temp[i]-48;
	}
	for( i=0;i<4;i=i+2)  key_money_temp[i]=key_money_temp[i]<<4;
	
	for( i=0;i<4;i=i+2)  key_money_temp[i]=key_money_temp[i]|key_money_temp[i+1];
	
	for (i=0;i<2;i++) key_money[i]=key_money_temp[i*2];
	for (i=2;i<6;i++) key_money[i]=key_money_temp[i+2];
}
//____________Lay ma key money_____________________________//
void Get_keymoney(){
	uint8_t s_w1=0;
	count_key = 0;
	for (uint8_t i=0;i<8;i++) key_money[i]=0; // xoa key
	for (uint8_t i=0;i<16;i++) key_money_temp[i]=0;
	lcd_command(0x01); // clear
	_delay_ms(3);
	goto_xy(1,1);
	lcd_print("Enter key:");
	s_w1=Get_Switch();
	while (s_w1!=2)
	{
		s_w1=Get_Switch();
		if(s_w1==3) {
			 count_key--;
			 key_money_temp[count_key]=0x20;
		}
		
		get_key();
		goto_xy(2,1);
		lcd_print(&key_money_temp[0]);
	}
	convert_key();
	lcd_putstr1();
	
}

//___________Hien thi thong tin/ thong bao_________________//
void Display_user(uint8_t c){
	if(c==1){
		lcd_command(0x01); // clear
		_delay_ms(3);
		goto_xy(1,1);
		lcd_print("Xin chao -->");
		goto_xy(2,1);
		for(uint8_t j = 0;j<16;j++){
			if(name[j] != 0) lcd_data(name[j]);
		}
		_delay_ms(1000);
		lcd_command(0x01); // clear
		_delay_ms(3);
		goto_xy(1,1);
		lcd_print("So du con lai:");
		goto_xy(2,2);
		if((sodu[0]==0x30)&&(sodu[1]!=0x30)&&(sodu[2]!=0x30))
		lcd_print(&sodu[1]);
		else if((sodu[0]==0x30)&&(sodu[1]==0x30)&&(sodu[2]!=0x30))
		lcd_print(&sodu[2]);
		else if((sodu[0]==0x30)&&(sodu[1]==0x30)&&(sodu[2]==0x30))
		lcd_print(&sodu[3]);
		else {
			lcd_data(sodu[0]);
			lcd_print(".");
			lcd_print(&sodu[1]);
		}
		lcd_print(".000 VND");
	}
	else if(c==0){
		lcd_command(0x01); // clear
		_delay_ms(3);
		goto_xy(1,1);
		lcd_print("So du cua ban");
		goto_xy(2,1);
		lcd_print(" Khong du.");
		_delay_ms(1000);
	}
	else if(c==2){
		lcd_command(0x01); // clear
		_delay_ms(3);
		goto_xy(1,1);
		lcd_print("The chua cua ban");
		goto_xy(2,1);
		lcd_print(" Chua dang ky");
		_delay_ms(1000);
		lcd_putstr1();
	}
}

//__________for MFRC522________//
void lcd_putstr1(){
	lcd_command(0x01); // clear
	_delay_ms(3);
	goto_xy(1,1);
	lcd_print("He thong thu phi");
	goto_xy(2,1);
	lcd_print("   Tu dong");
}
void lcd_putstr2(){
	lcd_init();
	//lcd_command(0x01); // clear
	_delay_ms(3);
	goto_xy(1,1);
	lcd_print("Connecting ESP");
	goto_xy(2,1);
	lcd_print("...");
}
void Encryption (){
	for(uint8_t i = 0;i<16;i++){
		data_encrypted[i] = name[i]^sodu[i];
	}
}
uint8_t Compare_two_block(){
	if(name[0]==0) return 2;
	else{
		for(uint8_t i = 0;i<16;i++){
			if(check_block18[i]!=data_encrypted[i]) return 2;
		}
		return 1;
	}
}
uint8_t Trutien()
{
	for(uint8_t i=0;i<5;i++){
		if (sodu[3]!=0x30) sodu[3]=sodu[3] - 0x01;
		else {
			sodu[3]=0x39;
			if(sodu[2]!=0x30) sodu[2]=sodu[2] - 0x01;
			else {
				sodu[2]=0x39;
				if(sodu[1]!=0x30) sodu[1]=sodu[1] - 0x01;
				else  sodu[0]=sodu[0] - 0x01;
			}
		}
	}
}



uint8_t write_two_block(uint8_t block1,uint8_t block2,uint8_t *data1,uint8_t *data2){
	status = MFRC522_Request(PICC_REQIDL, str);
	if (status == MI_OK) {
		status = MFRC522_Anticoll(CardID);
		if(status == MI_OK) {
			MFRC522_SelectTag(CardID);
			status = MFRC522_Auth(PICC_AUTHENT1B, 19, sectorKeyA[2], CardID);
			if(status==MI_OK){
				status = MFRC522_Write(block1, data1);
				status = MFRC522_Write(block2, data2);
				return status;
			}
		}
	}
}
uint8_t read_three_block(uint8_t block1,uint8_t block2,uint8_t block3,uint8_t *data1,uint8_t *data2,uint8_t *data3){
	status = MFRC522_Request(PICC_REQIDL, str);
	if (status == MI_OK) {
		status = MFRC522_Anticoll(CardID);
		if(status == MI_OK) {
			MFRC522_SelectTag(CardID);
			status = MFRC522_Auth(PICC_AUTHENT1B, 19, sectorKeyA[2], CardID);
			status = MFRC522_Read(block1, data1);
			status = MFRC522_Read(block2, data2);
			status = MFRC522_Read(block3, data3);
			return status;
		}
	}
}
void Open_gate(){
	uint8_t y;
	PORTD|=(1<<PINC2);
	PORTD&=~(1<<PINC3);
	_delay_ms(1000);
	PORTD&=~((1<<PINC2)|(1<<PINC3));
	y=PINB&0x08;
	while(y!=0){
		y=PINB&0x08;
	}
	_delay_ms(500);
	while(y==0){
		y=PINB&0x08;
	}
	_delay_ms(1000);
	PORTD|=(1<<PINC3);
	PORTD&=~(1<<PINC2);
	_delay_ms(1000);
	PORTD&=~((1<<PINC2)|(1<<PINC3));
}

//____________Function for ESP8266___________//
void USART_Init()
{
	/*Set baud rate */
	UBRR0H = 0;
	UBRR0L = 51; // 9600
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(0<<RXCIE0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void USART_Transmit( uint8_t data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) )
	;
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void UART_puts(char* s)
{
	// transmit character until NULL is reached
	while(*s > 0) USART_Transmit(*s++);
}
uint8_t caculate_leng(){
	leng_data = 0;
	for(uint8_t i=0;i<16;i++){
		if (name[i]==0) break;
		else leng_data++;
	}
	for(uint8_t j=0;j<16;j++){
		if (sodu[j]==0) break;
		else leng_data++;
	}
	leng_data=leng_data+49;
	if((sodu[0]==0x30)&&(sodu[1]!=0x30)&&(sodu[2]!=0x30))
	leng_data=leng_data-2;
	else if((sodu[0]==0x30)&&(sodu[1]==0x30)&&(sodu[2]!=0x30))
	leng_data=leng_data-3;
	else if((sodu[0]==0x30)&&(sodu[1]==0x30)&&(sodu[2]==0x30))
	leng_data=leng_data-4;
}
uint8_t Find_Oke(){
	uint8_t count=0;
	for(uint8_t i=0;i<100;i++){
		if ((buff[i]=='O')&&(buff[i+1]=='K')) count++;
	}
	return count;
}
unsigned char USART_Receive( uint8_t check )
{
	uint32_t timeout;
	/* Wait for data to be received */
	if(check == 1) timeout=8000000; //  ~1s
	else if(check == 0) timeout = 40000; // ~5ms
	do
	{
		timeout--;
	} while ((!(UCSR0A & (1<<RXC0)))&&(!(timeout==0)));
	/* Get and return received data from buffer */
	return UDR0;

}
uint8_t Wait_Oke(uint8_t x,uint8_t y){
	for(uint8_t i =0;i<100;i++) buff[i]=0; // xoa buff
	if(y==0)
	for(uint8_t j=0;j<100;j++) {
		buff[j]=USART_Receive(x);
		if(Find_Oke()==1) return 1;
	}
	else
	for(uint8_t j=0;j<100;j++) {
		buff[j]=USART_Receive(x);
		if(Find_Oke()==2) return 2;
	}
	return 0;
}

void ESP8266_init(){
	uint8_t kiemtra=0;
	UART_puts("AT");
	UART_puts("\r\n");
	kiemtra=Wait_Oke(0,0);
	if (kiemtra==0)
	{
		lcd_print("ESP8266 fail");
		_delay_ms(2000);
		asm("jmp 0"); // reset
	}
	else if(kiemtra==1){
		lcd_print("ESP8266 OKE");
	}
	UART_puts("AT+CWMODE=1");
	UART_puts("\r\n");
	_delay_ms(100);
	
	UART_puts("AT+CWJAP=");
	USART_Transmit(0x22); // 0x22 = "
	UART_puts("WIFI 03");
	USART_Transmit(0x22); // 0x22 = "
	USART_Transmit(0x2C); //0x2C= ,
	USART_Transmit(0x22); // 0x22 = "
	UART_puts("vt290102");
	USART_Transmit(0x22); // 0x22 = "
	UART_puts("\r\n");
	kiemtra=Wait_Oke(1,0);
	if (kiemtra==0){
		goto_xy(2,4);
		lcd_print("Wifi Fail  ");
		_delay_ms(2000);
		asm("jmp 0"); // reset
	}
	else if(kiemtra==1){
		goto_xy(2,4);
		lcd_print("Wifi Oke   ");
		_delay_ms(1000);
	}
	
	UART_puts("AT+CIPMUX=1");
	UART_puts("\r\n");
	_delay_ms(100);
}
void Senddata_to_webserver(){
	uint8_t kiemtra=0;
	again:
	UART_puts("AT+CIPSTART=1");//,"TCP","192.168.0.123",80");
	USART_Transmit(0x2C); //0x2C= ,
	USART_Transmit(0x22); // 0x22 = "
	UART_puts("TCP");
	USART_Transmit(0x22); // 0x22 = "
	USART_Transmit(0x2C); //0x2C= ,
	USART_Transmit(0x22); // 0x22 = "
	UART_puts("192.168.0.123");
	USART_Transmit(0x22); // 0x22 = "
	USART_Transmit(0x2C); //0x2C= ,
	UART_puts("80");
	UART_puts("\r\n");
	_delay_ms(500);
	
	caculate_leng();
	UART_puts("AT+CIPSEND=1,"); //54
	USART_Transmit((leng_data/10)+0x30);
	USART_Transmit((leng_data%10)+0x30);
	UART_puts("\r\n");
	_delay_ms(500);
	
	UART_puts("GET /webserver/getdemo.php?HOTEN=");
	UART_puts(&name[0]);
	UART_puts("&SODU=");
	if((sodu[0]==0x30)&&(sodu[1]!=0x30)&&(sodu[2]!=0x30))
	UART_puts(&sodu[1]);
	else if((sodu[0]==0x30)&&(sodu[1]==0x30)&&(sodu[2]!=0x30))
	UART_puts(&sodu[2]);
	else if((sodu[0]==0x30)&&(sodu[1]==0x30)&&(sodu[2]==0x30))
	UART_puts(&sodu[3]);
	else{
		USART_Transmit(sodu[0]);
		UART_puts(".");
		UART_puts(&sodu[1]);
	}
	UART_puts(".000");
	UART_puts("VND");
	UART_puts("\r\n");
	kiemtra=Wait_Oke(0,1);
	if (kiemtra==0)
	{
		lcd_command(0x01); // clear
		_delay_ms(3);
		goto_xy(1,1);
		lcd_print(" Send Data fail");
		goto again;
	}
	else if(kiemtra==2) {
		lcd_command(0x01); // clear
		_delay_ms(3);
		goto_xy(1,1);
		lcd_print(" Send Data Oke");
	}
}
