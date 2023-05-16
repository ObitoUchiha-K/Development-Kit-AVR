/*
 * I2C_DS1307.h
 *
 * Created: 12/12/2022 2:16:10 PM
 *  Author: obito.uchiha
 */ 


#ifndef I2C_DS1307_H_
#define I2C_DS1307_H_
#define F_CPU 8000000UL
#include <util/delay.h>
#include <avr/io.h>
extern void I2C_START();
extern void I2C_INIT();
extern void I2C_STOP();
extern void I2C_WRITE(uint8_t data);
extern uint8_t I2C_READ_ACK();
extern uint8_t I2C_READ_NACK();
extern uint8_t Get_Switch();
extern void Write_DS1307();
extern void Read_DS1307();
extern void I2C_DS1307();
#endif /* I2C_DS1307_H_ */