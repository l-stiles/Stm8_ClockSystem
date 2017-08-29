#ifndef __24C02_H
#define __24C02_H


#define I2C_Slave_Addr   0xA0
#define EEPROM_BASE_Addr 0x01

#define IICSPEED 100000
#define EEPROM_PAGE_SIZE 8
#define EEPROM_Addr      0xA0




extern void vfnTogetdata();
extern void EEPROM_ReadData(unsigned char *buffer, unsigned char addr, unsigned char num);
extern void EEPROM_WriteData(unsigned char *buffer, unsigned char addr, unsigned char num);
extern void EEPROM_WriteOnePage(unsigned char *buffer, unsigned char addr, unsigned char num);
extern void Init_IIc(void);
void WaitI2C_SDA_Idle(void);


#endif