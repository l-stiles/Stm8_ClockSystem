#include "func.h"


extern unsigned char begin_count;
extern unsigned char key_time;
extern unsigned char Receive_Flag;


void main(void)
{ 
    Uart1_Init();   
    ExTiKey_init();
    Init_IIc();
    port_init();   //初始化ht1621端口
    Ht1621_Init(); 
    Timer4_init();
    
    TimeClockSys_Init();
    
    while(1)
    {
     
      if(GPIO_ReadInputPin(GPIOB, GPIO_PIN_3))
        begin_count = 0;
      
      Show_Time();
           
      if(key_time >= 4)   //按键，按2秒，进入设置模式
      {
          begin_count = 0;
          key_time    = 0;
          Set_Time_Mode();
      }

      if(Receive_Flag)    
        Uart1_Set_Time();

    }
  
}





#ifdef USE_FULL_ASSERT

void assert_failed(u8* file, u32 line)
{ 
  while (1)
  {
  }
}
#endif