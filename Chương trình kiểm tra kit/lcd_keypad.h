/*
 * lcd_keypad.h
 *
 * Created: 11/29/2022 9:56:06 PM
 *  Author: obito.uchiha
 */ 

#ifndef LCD_KEPAD_H_
#define LCD_KEPAD_H_

#define F_CPU 8000000UL
#include <util/delay.h>
#include <avr/io.h>

extern void lcd_command(unsigned char cmd);
extern void lcd_data(unsigned char data);
extern void lcd_init();
extern void goto_xy(unsigned int x,unsigned int y);
extern void lcd_print(char *p);
extern void lcd_putstr();
extern uint8_t get_key();
extern void lcd_keypad_xung();
extern void taoxung_T2ms();
extern uint8_t Doxung_us();
extern void xoa_kytu();

#endif /* LCD_KEPAD_H_ */