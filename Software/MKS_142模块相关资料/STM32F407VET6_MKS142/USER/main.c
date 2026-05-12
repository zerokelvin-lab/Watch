#include "sys.h"
#include "usart.h"
#include "usart2.h"
#include "delay.h"
#include "key.h"
#include "lcd.h"
#include "timer.h"
#include "GX310.h"
#include "iic.h"
#include "iic2.h"
#include "function.h"



//STM32F4工程模板-库函数版本
//genbotter.com


int main(void)
{
	

	
	sys_init();						//系统初始化
		
	LCD_Clear(WHITE);			//清屏，默认白底
	TIM2_Init(125-1,83);  //定时器2 初始化 125us中断
	TIM3_Init(10000-1,8400);//定时器3，初始化1s
	LCD_InitUI();
	
	Beep_Set(1,2,0,0);			//蜂鸣器响一声
	USART1_SendByte(0x88);
	
  while(1)
		{
			if(Timer_Structure.bT2ms)
			{
				Timer_Structure.bT2ms = 0;
				Key_Handle();						//按键扫描

			}
			if(Timer_Structure.bT10ms)
			{
				Timer_Structure.bT10ms = 0;
				Beep_Drive();						//蜂鸣器驱动
				Display_update();				//更新显示
				LED_Handle();						//LED闪烁
				Sensor_Handle();			 //数据异常报警检测
				Usart_Handle();	
				
			}	
		}
}



