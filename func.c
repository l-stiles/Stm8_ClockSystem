#include "func.h"

uchar Min_Uni;
uchar Min_Dec;
uchar Hour_Uni;
uchar Hour_Dec; 
const uchar Min_Uni_Addr  = 0x00;
const uchar Min_Dec_Addr  = 0x02;
const uchar Hour_Uni_Addr = 0x04;
const uchar Hour_Dec_Addr = 0x06;

extern uchar Num_Addr[4];
extern uchar Num_Data[10]; 
uchar num_temp = 0;
uchar num_pos  = 0;
uchar set_mode = 0;
uchar *Data_Pos;
uchar set_pos  = 0;
uchar buff[255];
uchar Receive_Count = 0;
uchar data_limit = 0;
uchar Time_Seconds = 0;

extern unsigned char time_flag;
extern unsigned char key_time;
extern unsigned char begin_count;
extern unsigned char mov;
extern unsigned char set_quit;
extern unsigned char Receive_Flag;

/*******************************************************************************
****Name:
****Func:��ʱ����
****Version:V1.0
****date:22-8-2017
****��ڲ���:��
****���ڲ���:��
****˵��:
********************************************************************************/
void Delay(uint16_t nCount)
{
  while (nCount != 0)
  {   
    nCount--;
  }  
}

void Delay_4_Test()
{
  uchar i;
  for(i=0; i<3; i++)
    Delay(65000);

}
/*******************************************************************************
****Name:
****Func:�ⲿ�жϹ��ܳ�ʼ��
****Version:V1.0
****date:22-8-2017
****��ڲ���:��
****���ڲ���:��
****˵��:
********************************************************************************/

void ExTiKey_init()
{
  EXTI_DeInit();
  
  GPIO_Init(GPIOB, GPIO_PIN_3, GPIO_MODE_IN_PU_IT);
  GPIO_Init(GPIOD, GPIO_PIN_3|GPIO_PIN_7, GPIO_MODE_IN_PU_IT);
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOB, EXTI_SENSITIVITY_FALL_ONLY);
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_FALL_ONLY);
  
 
  enableInterrupts();
  
}
/*******************************************************************************
****Name:
****Func:timer4 ���ܳ�ʼ��
****Version:V1.0
****date:22-8-2017
****��ڲ���:��
****���ڲ���:��
****˵��:
********************************************************************************/
void Timer4_init()
{
    TIM4_DeInit();
    TIM4_TimeBaseInit(TIM4_PRESCALER_128, 0xff);
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
    TIM4_Cmd(ENABLE);

}

/*******************************************************************************
****Name:
****Func:д��eeprom
****Version:V1.0
****date:22-8-2017
****��ڲ���:��
****���ڲ���:��
****˵��:��ʱ������д��eeprom
********************************************************************************/
void Write_EEPROM()
{
  EEPROM_WriteData(&Hour_Dec, Hour_Dec_Addr, 1);
  EEPROM_WriteData(&Hour_Uni, Hour_Uni_Addr, 1);
  EEPROM_WriteData(&Min_Dec,  Min_Dec_Addr,  1);
  EEPROM_WriteData(&Min_Uni,  Min_Uni_Addr,  1);


}
/*******************************************************************************
****Name:
****Func:��ȡeeprom
****Version:V1.0
****date:22-8-2017
****��ڲ���:��
****���ڲ���:��
****˵��:��ʱ�����ݴ�eeprom��ȡ����
********************************************************************************/

void Read_EEPROM()
{
  EEPROM_ReadData(&Hour_Dec, Hour_Dec_Addr, 1);
  EEPROM_ReadData(&Hour_Uni, Hour_Uni_Addr, 1);
  EEPROM_ReadData(&Min_Dec, Min_Dec_Addr, 1);
  EEPROM_ReadData(&Min_Uni, Min_Uni_Addr, 1);

}

/*******************************************************************************
****Name:
****Func:ʱ��
****Version:V1.0
****date:22-8-2017
****��ڲ���:��
****���ڲ���:��
****˵��:����ʱ�䣬����ʱ����ʾ��lcd�ϡ�
********************************************************************************/

int Show_Time()
{
    if(Time_Seconds == 120)
    {
      Time_Seconds = 0;
      if(Min_Uni == 9)
      {
          Min_Uni = 0;
          if(Min_Dec == 5)
          {
              Min_Dec  = 0;
              if(Hour_Dec == 2)
              {
                if(Hour_Uni == 3)
                {
                    Min_Uni  = 0;
                    Min_Dec  = 0;
                    Hour_Uni = 0;
                    Hour_Dec = 0;
                }                     
                else
                  Hour_Uni++;
              }            
              else
              {
                if(Hour_Uni == 9)
                {
                   Hour_Uni = 0;
                   Hour_Dec++;
                
                }
                else
                  Hour_Uni++;
              }
          
          }
          else
            Min_Dec++;
      
      }
      else
        Min_Uni++;
    
    }  
    

      if(time_flag)
      {  
        num_temp = Num_Data[Hour_Dec] + 0x10;  
        Ht1621WrOneNum(Num_Addr[0], num_temp);
      }
      else
      { 
        num_temp = Num_Data[Hour_Dec] + 0x00;  
        Ht1621WrOneNum(Num_Addr[0], num_temp);
      }
     
      Ht1621WrOneNum(Num_Addr[1], Num_Data[Hour_Uni]);
      Ht1621WrOneNum(Num_Addr[2], Num_Data[Min_Dec]);
      Ht1621WrOneNum(Num_Addr[3], Num_Data[Min_Uni ]);
      
      Write_EEPROM(); //��������
      
      return 0;

}
/*******************************************************************************
****Name:
****Func:ͨ���ƶ�Э���޸�ʱ��ֵ
****Version:V1.0
****date:22-8-2017
****��ڲ���:��
****���ڲ���:��
****˵��:�ɹ�ͨ�����ڷ���0x55 01 0xAA�� ʧ��ͨ������set time failed
********************************************************************************/
int Uart1_Set_Time()
{
  uchar i = 0, Dec, Uni;
  uchar temp_data[6];
    
    while(1)
    {
      if(Receive_Flag)
      {
        Delay_4_Test();
        if(Receive_Count >= 19)
        {
            while(buff[i++] != 0x20);
            Dec = i + 2;
            Uni = i + 3;
            temp_data[0] = buff[Dec] - '0';
            temp_data[1] = buff[Uni] - '0';            
            temp_data[2] = (temp_data[0] << 4) | (temp_data[1] & 0x0f);
            
            while(buff[i++] != 0x20);
            Dec = i + 2;
            Uni = i + 3;
            temp_data[3] = buff[Dec] - '0';
            temp_data[4] = buff[Uni] - '0';            
            temp_data[5] = (temp_data[3] << 4) | (temp_data[4] & 0x0f);
        }
        Receive_Flag  = 0;
        Receive_Count = 0;
        break;
      }
    }
/*    
    if((temp_data[2] > 24) || (temp_data[5] > 59))
    {  
      Send_Str("set time failed \r\n");
      Time_Seconds = 0;
      return -1;
    }
    if((temp_data[2] == 24) && (temp_data[5] > 0x00))
    {  
      Send_Str("set time failed \r\n");
      Time_Seconds = 0;
      return -1;
    }
*/    
    Hour_Dec = temp_data[0];
    Hour_Uni = temp_data[1]; 
    Min_Dec  = temp_data[3];
    Min_Uni  = temp_data[4]; 
    Show_Time();    
    Send_Str("0x55 01 0xAA \r\n");
    
    Time_Seconds = 0;
    Write_EEPROM(); //��������
    return 0;
    
    
    

}
/*******************************************************************************
****Name:
****Func:ͨ�������޸�ʱ��
****Version:V1.0
****date:22-8-2017
****��ڲ���:��
****���ڲ���:��
****˵��:����1:����2����� ����ģʽ����ģʽ�£��ٰ��˼��ı� �޸�λ����˸��ʾ�޸�λ����
         ����2:����
         ����3������
         
********************************************************************************/

int Set_Time_Mode()
{
   unsigned char modify_pos;
   set_mode = 1;
   while(1)
   { 
     if(set_quit == 20)
     {
        set_mode = 0;
        set_quit = 0;
        mov      = 0;
        Time_Seconds = 0;
        Write_EEPROM(); //��������
        return 0;

     }
     
     if(Hour_Dec == 2)
       data_limit = 1;
     else
       data_limit = 0;
   
     switch(mov)
     {
       case 0:
             modify_pos = Hour_Dec;
             Data_Pos = &Hour_Dec;
             set_pos  = 0;
              break;
       case 1:
              modify_pos = Hour_Uni;
              Data_Pos = &Hour_Uni;
              set_pos  = 1;
              break;
       case 2:
              modify_pos = Min_Dec;
              Data_Pos = &Min_Dec;
              set_pos  = 2;
              break;
       case 3:
              modify_pos = Min_Uni;
              Data_Pos = &Min_Uni;
              set_pos  = 3;
              break; 
     
     }

     if(mov == 0)
     {
        if(time_flag)
        {  
          num_temp = Num_Data[modify_pos] + 0x10;  
          Ht1621WrOneNum(Num_Addr[0], num_temp);
           
        }
        else
        { 
          num_temp = 0x00;  
          Ht1621WrOneNum(Num_Addr[0], num_temp);
        }

        Ht1621WrOneNum(Num_Addr[1], Num_Data[Hour_Uni]);
        Ht1621WrOneNum(Num_Addr[2], Num_Data[Min_Dec]);
        Ht1621WrOneNum(Num_Addr[3], Num_Data[Min_Uni ]);		  
		
     }	 
     else if(mov == 1)
     {

        if(time_flag)
        {  
          num_temp = Num_Data[Hour_Dec] + 0x10;  
          Ht1621WrOneNum(Num_Addr[0], num_temp);

           Ht1621WrOneNum(Num_Addr[1], Num_Data[modify_pos] );

           
        }
        else
        { 
          num_temp = Num_Data[Hour_Dec] + 0x00;  
          Ht1621WrOneNum(Num_Addr[0], num_temp);

           Ht1621WrOneNum(Num_Addr[1], 0x00);

           
        }

        
        Ht1621WrOneNum(Num_Addr[2], Num_Data[Min_Dec]);
        Ht1621WrOneNum(Num_Addr[3], Num_Data[Min_Uni ]);	

     }
     else if(mov == 2)
     {

        if(time_flag)
        {  
          num_temp = Num_Data[Hour_Dec] + 0x10;  
          Ht1621WrOneNum(Num_Addr[0], num_temp);
  
           Ht1621WrOneNum(Num_Addr[2], Num_Data[modify_pos]);
  
           
        }
        else
        { 
          num_temp = Num_Data[Hour_Dec] + 0x00;  
          Ht1621WrOneNum(Num_Addr[0], num_temp);
  
           Ht1621WrOneNum(Num_Addr[2], 0x00);
  
           
        }
  
        
        Ht1621WrOneNum(Num_Addr[1], Num_Data[Hour_Uni]);
        Ht1621WrOneNum(Num_Addr[3], Num_Data[Min_Uni]);	

     }
     else if(mov == 3)
     {

        if(time_flag)
        {  
          num_temp = Num_Data[Hour_Dec] + 0x10;  
          Ht1621WrOneNum(Num_Addr[0], num_temp);
        
          Ht1621WrOneNum(Num_Addr[3], Num_Data[modify_pos] );
        
        
        }
        else
        { 
          num_temp = Num_Data[Hour_Dec] + 0x00;  
          Ht1621WrOneNum(Num_Addr[0], num_temp);
          
          Ht1621WrOneNum(Num_Addr[3], 0x00);
          
        
        }

        
        Ht1621WrOneNum(Num_Addr[1], Num_Data[Hour_Uni]);
        Ht1621WrOneNum(Num_Addr[2], Num_Data[Min_Dec]);	

     }

	  
   }



}

/*******************************************************************************
****Name:
****Func:ʱ�ӱ�־λ��ʼ��
****Version:V1.0
****date:22-8-2017
****��ڲ���:��
****���ڲ���:��
****˵��:��ʱ��ϵͳ��Ӧ�߼��ı�־��ʼ��Ϊԭʼ״̬������ȡ��һ�ε�eeprom���ʱ������
********************************************************************************/
void TimeClockSys_Init()
{
    time_flag = 0; //ð����˸��־λ
    
    key_time  = 0; //�����ֶ�����ʱ����� ���ñ�־λ
    begin_count = 0;
    
    
    set_quit = 0; //10�����޲����˳� ���ñ�־
    num_temp = 0; //��ʱ�洢��
    num_pos  = 0;  
    set_mode = 0;
    Data_Pos = NULL;
    set_pos  = 0; 
    Receive_Count = 0; //���ڽ��ܵ��ֽ�����
    data_limit = 0;    
    Time_Seconds = 0;  //����
    mov = 0;           //ȷ���޸�ʱ���޸�λ��λ��
    Receive_Flag = 0; //��ʼ�����������
      
      
    Read_EEPROM(); //��ȡ����ǰ������
  
}
