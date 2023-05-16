#ifndef MFRC522_H_
#define MFRC522_H_

#define F_CPU 8000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>

#define MI_OK									0
#define MI_ERR									2

// MFRC522 Commands
#define PCD_IDLE								0x00	// NO action; Cancel the current command
#define PCD_AUTHENT								0x0E  	// Authentication Key
#define PCD_RECEIVE								0x08   	// Receive Data
#define PCD_TRANSMIT							0x04   	// Transmit data
#define PCD_TRANSCEIVE							0x0C   	// Transmit and receive data,
#define PCD_RESETPHASE							0x0F   	// Reset
#define PCD_CALCCRC								0x03   	// CRC Calculate

// Mifare_One card command word
#define PICC_REQIDL								0x26   	// find the antenna area does not enter hibernation
#define PICC_REQALL								0x52   	// find all the cards antenna area
#define PICC_ANTICOLL							0x93   	// anti-collision
#define PICC_SElECTTAG							0x93   	// election card
#define PICC_AUTHENT1A							0x60   	// authentication key A
#define PICC_AUTHENT1B							0x61   	// authentication key B
#define PICC_READ								0x30   	// Read Block
#define PICC_WRITE								0xA0   	// write block
#define PICC_HALT								0x50   	// 

// MFRC522 Registers
// Page 0: Command and Status
#define CommandReg						0x01
#define ComIrqReg					    0x04
#define DivIrqReg						0x05
#define ErrorReg						0x06
#define Status1Reg						0x07
#define Status2Reg						0x08
#define FIFODataReg					    0x09
#define FIFOLevelReg				    0x0A
#define ControlReg						0x0C
#define BitFramingReg				    0x0D
// Page 1: Command
#define ModeReg						    0x11
#define TxModeReg						0x12
#define RxModeReg						0x13
#define TxControlReg				    0x14
#define TxASKReg						0x15
// Page 2: CFG
#define CRCResultRegH		    	    0x21
#define CRCResultRegL		            0x22
#define RFCfgReg						0x26
#define TModeReg						0x2A
#define TPrescalerReg				    0x2B
#define TReloadRegH			    	    0x2C
#define TReloadRegL			    	    0x2D

#define MFRC522_MAX_LEN							16			// Buf len byte

// RC522
 void spi_init();
 void SendSPI(uint8_t data);
 uint8_t TransferSPI(uint8_t data);
void MFRC522_WriteRegister(uint8_t addr, uint8_t val);
uint8_t MFRC522_ReadRegister(uint8_t addr);
void MFRC522_SetBitMask(uint8_t reg, uint8_t mask);
void MFRC522_ClearBitMask(uint8_t reg, uint8_t mask);
uint8_t MFRC522_Request(uint8_t reqMode, uint8_t* TagType);
uint8_t MFRC522_ToCard(uint8_t command, uint8_t* sendData, uint8_t sendLen, uint8_t* backData, uint16_t* backLen);
uint8_t MFRC522_Anticoll(uint8_t* serNum);
void MFRC522_CalulateCRC(uint8_t* pIndata, uint8_t len, uint8_t* pOutData);
uint8_t MFRC522_SelectTag(uint8_t* serNum);
uint8_t MFRC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t* Sectorkey, uint8_t* serNum);
uint8_t MFRC522_Read(uint8_t blockAddr, uint8_t* recvData);
uint8_t MFRC522_Write(uint8_t blockAddr, uint8_t* writeData);
void MFRC522_Init(void);
void MFRC522_Reset(void);
void MFRC522_Hard_Reset(void);
void MFRC522_AntennaOn(void);
void MFRC522_AntennaOff(void);
void MFRC522_Halt(void);

#endif