#ifndef __HT1621B_H
#define __HT1621B_H


#include "stm8s.h"

#define uchar unsigned char
#define uint  unsigned int

#define BIAS   0x52            //0b1000 0101 0010  1/3duty 4com
#define SYSDIS 0X00           //0b1000 0000 0000  关振系统荡器和LCD偏压发生器
#define SYSEN  0X02           //0b1000 0000 0010 打开系统 振荡器
#define LCDOFF 0X04           //0b1000 0000 0100  关LCD偏压
#define LCDON  0X06           //0b1000 0000 0110  打开LCD偏 压
#define XTAL   0x28           //0b1000 0010 1000 外部接时钟
#define RC256  0X30           //0b1000 0011 0000  内部时钟RC256K
#define TONEON  0X12           //0b1000 0001 0010  打开声音 输出
#define TONEOFF 0X10           //0b1000 0001 0000 关闭声音输出
#define WDTDIS  0X0A        //0b1000 0000 1010  禁止看门狗
#define TONE_4K 0x80        //4k声音输出频率
#define TONE_2K 0xC0   

#define CLS_HT_CS  GPIO_WriteLow(GPIOF, GPIO_PIN_4)   //cs
#define SET_HT_CS  GPIO_WriteHigh(GPIOF, GPIO_PIN_4)
#define CLS_HT_WR  GPIO_WriteLow(GPIOB, GPIO_PIN_7)   //wr
#define SET_HT_WR  GPIO_WriteHigh(GPIOB, GPIO_PIN_7)
#define CLS_HT_DAT GPIO_WriteLow(GPIOB, GPIO_PIN_6)   //data
#define SET_HT_DAT GPIO_WriteHigh(GPIOB, GPIO_PIN_6)


void Delay_lcd(uchar us);
void DelayMS(uint iMs);
extern void port_init(void);
extern void Ht1621Wr_Data(uchar Data,uchar cnt);
extern void Ht1621WrOneData(uchar Addr,uchar Data);
extern void Ht1621WrOneNum(uchar Addr,uchar num);
extern void Ht1621WrCmd(uchar Cmd);
extern void Ht1621_Init(void);
extern void Ht1621WrAllData(void);

extern void Clear_Screen();



#endif