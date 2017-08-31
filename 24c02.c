#include "stm8s_i2c.h"
#include "24c02.h"


/*******************************************************************************
****Name:
****Func:��ʼ��IIC
****Version:V1.0
****date:22-8-2017
****��ڲ���:��
****���ڲ���:��
****˵��:
********************************************************************************/
void Init_IIc(void)
{
  I2C_DeInit();
  
  I2C_Init(IICSPEED, EEPROM_Addr, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 2);
  
  I2C_Cmd(ENABLE);


}
/*******************************************************************************
****Name:
****Func:��ʱ����
****Version:V1.0
****date:22-8-2017
****��ڲ���:��
****���ڲ���:��
****˵��:
********************************************************************************/
void WaitI2C_SDA_Idle(void)
{

  unsigned int nCount = 0xfff;
  while(nCount-- != 0);

}
/*******************************************************************************
****Name:
****Func:��ҳ������д���ⲿeeprom
****Version:V1.0
****date:22-8-2017
****��ڲ���:buffer��Ҫд������ݣ�addr:д���ַ��numд�����ݵ�����
****���ڲ���:��
****˵��:
********************************************************************************/
void EEPROM_WriteOnePage(unsigned char *buffer, unsigned char addr, unsigned char num)
{

  while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));  //wait idle
  
  I2C_GenerateSTART(ENABLE);                   //start signal
  
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));  //wait ack
  
  I2C_Send7bitAddress(EEPROM_Addr, I2C_DIRECTION_TX);    //send slave_addr
  
  while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));  //wait ack
  
  
  I2C_SendData(addr);   //send write addr
  
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING));   //wait ack
  
  while(num--)
  {
    I2C_SendData(*buffer);  // sendata
    
    buffer++;
    
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)); //wait ack
    I2C_AcknowledgeConfig(I2C_ACK_CURR);
  
  }
  
  
  I2C_GenerateSTOP(ENABLE); // send end signal


}
/*******************************************************************************
****Name:
****Func:������д���ⲿeeprom
****Version:V1.0
****date:22-8-2017
****��ڲ���:buffer��Ҫд������ݣ�addr:д���ַ��numд�����ݵ�����
****���ڲ���:��
****˵��:
********************************************************************************/

void EEPROM_WriteData(unsigned char *buffer, unsigned char addr, unsigned char num)
{

  unsigned char NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
  
  
  Addr  = addr % EEPROM_PAGE_SIZE; //ȷ��д���ַ�Ƿ�Ϊҳ���׵�ַ
  count = EEPROM_PAGE_SIZE - Addr; //д���ַ���ҳβ���ж����ֽ�
  
  NumOfPage   = num / EEPROM_PAGE_SIZE; //д��������Ƿ����1ҳ
  NumOfSingle = num % EEPROM_PAGE_SIZE; //д������ҳ��ʣ���������
  
  if(Addr == 0) //��ҳ���׵�ַ
  {
      if(NumOfPage == 0)  //д�����������һҳ
          EEPROM_WriteOnePage(buffer, addr, NumOfSingle);
      else               //����һҳ
      {
          while(NumOfPage--) //д����ҳ
          {
              EEPROM_WriteOnePage(buffer, addr, EEPROM_PAGE_SIZE);
              addr   += EEPROM_PAGE_SIZE;
              buffer += EEPROM_PAGE_SIZE;
              
              WaitI2C_SDA_Idle();
          }
          
          if(NumOfSingle != 0) //ʣ��������
              EEPROM_WriteOnePage(buffer, addr, NumOfSingle);
      
      }
  
  
  }
  
  else //����ҳ���׵�ַ��
  {
    if(NumOfPage == 0) //д�����������һҳ
    {
        if(NumOfSingle > count) //д������ҳʣ����������ڲ���һҳ�ĵ�ַʣ������
        {
          temp =  NumOfSingle - count;
          EEPROM_WriteOnePage(buffer, addr, count);
          WaitI2C_SDA_Idle();
          addr   += count;
          buffer += count;
          EEPROM_WriteOnePage(buffer, addr, temp);
               
        } 
        else  //д������ҳʣ�������С�ڲ���һҳ�ĵ�ַʣ������
          EEPROM_WriteOnePage(buffer, addr, num);
    }
    else //д�����������һҳ
    {
        num -= count;                               //������д���ַ��д��ҳβ������������ʣ�µ�������
        NumOfPage =  num / EEPROM_PAGE_SIZE;  //ʣ�����Ƿ����1ҳ
        NumOfSingle = num % EEPROM_PAGE_SIZE; //ʣ����д������ҳ���������
  
        EEPROM_WriteOnePage(buffer, addr, count);  //��дд���ַ��д��ҳβ�������� 
        WaitI2C_SDA_Idle();
        addr +=  count;
        buffer += count;
  
        while (NumOfPage--)
        {
          EEPROM_WriteOnePage(buffer, addr, EEPROM_PAGE_SIZE);
          WaitI2C_SDA_Idle();
          addr   += EEPROM_PAGE_SIZE;
          buffer += EEPROM_PAGE_SIZE;
        }
  
        if (NumOfSingle != 0)
        {
          EEPROM_WriteOnePage(buffer, addr, NumOfSingle); //д������ҳ����������ʣ��
        }
        
    
    }

    
  
  }
  
  
  WaitI2C_SDA_Idle();
  
  
  
  

}
/*******************************************************************************
****Name:
****Func:��eeprom��ȡ����
****Version:V1.0
****date:22-8-2017
****��ڲ���:buffer��Ҫд������ݣ�addr:д���ַ��numд�����ݵ�����
****���ڲ���:��
****˵��:
********************************************************************************/
void EEPROM_ReadData(unsigned char *buffer, unsigned char addr, unsigned char num)
{

    while(I2C_GetFlagStatus(I2C_FLAG_BUSBUSY));  //wait idle
    
    I2C_GenerateSTART(ENABLE);                   //start signal
    
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));  //wait ack
    
    I2C_Send7bitAddress(EEPROM_Addr, I2C_DIRECTION_TX);    //send slave_addr
    
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));  //wait ack
    
    
    I2C_SendData(addr);   //send write addr
    
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING));   //wait ack  
  
     
    I2C_GenerateSTART(ENABLE);                     //start signal
   
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));         //wait ack  
  
    
    I2C_Send7bitAddress(EEPROM_Addr, I2C_DIRECTION_RX);        //read mode
    
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)); //wait ack  
   
    while(num)  
    {
    
       
      if(I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)) //wait ack 
      {      
       
        *buffer = I2C_ReceiveData();                     
        
        buffer++;  
        
        if(num == 1)
        {
        
          I2C_AcknowledgeConfig(I2C_ACK_NONE);
        
          I2C_GenerateSTOP(ENABLE);  
        }
        else      
          I2C_AcknowledgeConfig(I2C_ACK_CURR);
        num--;
      }   
    }
    

}


/*****************************/
void vfnTogetdata()
{
	unsigned char ucCount;
	
	//printf("hello world~~\n");
	//this is a test for boring~
	
	ucCount = 500;
	while(ucCount--);	
	
	
	
}



/****************************/


/***************/

int nfnBroadring(void)
{
	
	u8 ucTemp;
	
	for(;;);
				
	ucTemp = 213; 	
	
	return ucTemp;
	
}


/*************/