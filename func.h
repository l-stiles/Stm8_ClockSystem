#ifndef __FUNC_H
#define __FUNC_H


#include "stm8s.h"
#include "stm8s_exti.h"
#include "24c02.h"
#include "ht1621b.h"
#include "uart1.h"
#include "stm8s_it.h"

#define NULL 0


extern void Delay(uint16_t nCount);
extern void Delay_4_Test();
extern void ExTiKey_init();
extern void Timer4_init();
extern void Write_EEPROM();
extern void Read_EEPROM();
extern int Show_Time();
extern int Uart1_Set_Time();
extern int Set_Time_Mode();
extern void TimeClockSys_Init();

#endif