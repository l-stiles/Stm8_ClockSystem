#include "stm8s_i2c.h"
#include "24c02.h"


/*******************************************************************************
****Name:
****Func:初始化IIC
****Version:V1.0
****date:22-8-2017
****入口参数:无
****出口参数:无
****说明:
********************************************************************************/
void Init_IIc(void)
{
  I2C_DeInit();
  
  I2C_Init(IICSPEED, EEPROM_Addr, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 2);
  
  I2C_Cmd(ENABLE);


}
/*******************************************************************************
****Name:
****Func:延时函数
****Version:V1.0
****date:22-8-2017
****入口参数:无
****出口参数:无
****说明:
********************************************************************************/
void WaitI2C_SDA_Idle(void)
{

  unsigned int nCount = 0xfff;
  while(nCount-- != 0);

}
/*******************************************************************************
****Name:
****Func:按页将数据写入外部eeprom
****Version:V1.0
****date:22-8-2017
****入口参数:buffer：要写入的数据，addr:写入地址，num写入数据的数量
****出口参数:无
****说明:
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
****Func:将数据写入外部eeprom
****Version:V1.0
****date:22-8-2017
****入口参数:buffer：要写入的数据，addr:写入地址，num写入数据的数量
****出口参数:无
****说明:
********************************************************************************/

void EEPROM_WriteData(unsigned char *buffer, unsigned char addr, unsigned char num)
{

  unsigned char NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
  
  
  Addr  = addr % EEPROM_PAGE_SIZE; //确定写入地址是否为页的首地址
  count = EEPROM_PAGE_SIZE - Addr; //写入地址离该页尾还有多少字节
  
  NumOfPage   = num / EEPROM_PAGE_SIZE; //写入的数量是否大于1页
  NumOfSingle = num % EEPROM_PAGE_SIZE; //写完完整页，剩余的数据量
  
  if(Addr == 0) //是页的首地址
  {
      if(NumOfPage == 0)  //写入的数量不足一页
          EEPROM_WriteOnePage(buffer, addr, NumOfSingle);
      else               //超过一页
      {
          while(NumOfPage--) //写完整页
          {
              EEPROM_WriteOnePage(buffer, addr, EEPROM_PAGE_SIZE);
              addr   += EEPROM_PAGE_SIZE;
              buffer += EEPROM_PAGE_SIZE;
              
              WaitI2C_SDA_Idle();
          }
          
          if(NumOfSingle != 0) //剩余数据量
              EEPROM_WriteOnePage(buffer, addr, NumOfSingle);
      
      }
  
  
  }
  
  else //不在页的首地址中
  {
    if(NumOfPage == 0) //写入的数量不足一页
    {
        if(NumOfSingle > count) //写完完整页剩余的容量大于不满一页的地址剩余容量
        {
          temp =  NumOfSingle - count;
          EEPROM_WriteOnePage(buffer, addr, count);
          WaitI2C_SDA_Idle();
          addr   += count;
          buffer += count;
          EEPROM_WriteOnePage(buffer, addr, temp);
               
        } 
        else  //写完完整页剩余的容量小于不满一页的地址剩余容量
          EEPROM_WriteOnePage(buffer, addr, num);
    }
    else //写入的数量大于一页
    {
        num -= count;                               //减掉（写入地址离写入页尾的数据量）所剩下的数据量
        NumOfPage =  num / EEPROM_PAGE_SIZE;  //剩余量是否大于1页
        NumOfSingle = num % EEPROM_PAGE_SIZE; //剩余量写完完整页后的数据量
  
        EEPROM_WriteOnePage(buffer, addr, count);  //先写写入地址离写入页尾的数据量 
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
          EEPROM_WriteOnePage(buffer, addr, NumOfSingle); //写完完整页后有数据量剩余
        }
        
    
    }

    
  
  }
  
  
  WaitI2C_SDA_Idle();
  
  
  
  

}
/*******************************************************************************
****Name:
****Func:从eeprom读取数据
****Version:V1.0
****date:22-8-2017
****入口参数:buffer：要写入的数据，addr:写入地址，num写入数据的数量
****出口参数:无
****说明:
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