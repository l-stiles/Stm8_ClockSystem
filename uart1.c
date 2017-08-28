#include "uart1.h"
#include "stm8s.h"

/*******************************************************************************
****Name:
****Func:��ʼ��UART
****Version:V1.0
****date:22-8-2017
****��ڲ���:��
****���ڲ���:��
****˵��:
********************************************************************************/
void Uart1_Init()
{
  UART1_DeInit();
  
  UART1_Init((u32)19200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, 
                UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
  
  UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
  
  UART1_Cmd(ENABLE);
   
}

/*******************************************************************************
****Name:
****Func:����8λ����
****Version:V1.0
****date:22-8-2017
****��ڲ���:Ҫ���͵�����
****���ڲ���:��
****˵��:
********************************************************************************/
void Send_Dat(unsigned char dat)
{
   while(UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
   UART1_SendData8(dat);
}

/*******************************************************************************
****Name:
****Func:�����ַ���
****Version:V1.0
****date:22-8-2017
****��ڲ���:Ҫ���͵�����
****���ڲ���:��
****˵��:
********************************************************************************/
void Send_Str(unsigned char* dat)
{
  
  while(*dat != '\0')
  {
    Send_Dat(*dat);

    dat++;
  
  }

}
