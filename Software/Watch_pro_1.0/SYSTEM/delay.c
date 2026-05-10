/** @file delay.c @brief 基于SysTick的微秒/毫秒延时函数实现（含OS支持版本） */

#include "delay.h"
#include "sys.h"
 
/* 
//roughly delay
void delay_us(u32 nus)
{		
	u32 i;
	u16 temp;
	temp = nus*SYS_CLK/15;
	for(i=0;i<temp;i++);
}

//��ʱnms
//nms:Ҫ��ʱ��ms��
void delay_ms(u16 nms)
{
	u32 i;
	for(i=0;i<nms;i++) delay_us(1000);
}
*/

/** @brief 初始化延时系统，配置SysTick定时器为1ms中断周期
 *  @return 无 */
void delay_init(void)
{
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	/* Configure the SysTick to have interrupt in 1ms time basis*/
	HAL_SYSTICK_Config(SystemCoreClock / (1000U / uwTickFreq));
}

/* ====== OS支持版本的延时函数 ====== */
#if OS_SUPPORT 						    								   

/** @brief 微秒级延时（OS版本，含任务调度锁保护）
 *  @param nus 延时的微秒数
 *  @return 无 */
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				   	 
	ticks=nus*SYS_CLK; 						 
	delay_osschedlock();					
	told=SysTick->VAL;        				
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.	
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			
		}  
	};
	delay_osschedunlock();															    
}  

/** @brief 毫秒级延时（OS版本，支持任务级延时调度）
 *  @param nms 延时的毫秒数
 *  @return 无 */
void delay_ms(u16 nms)
{	
	if(delay_osrunning&&delay_osintnesting==0)    
	{		 
		if(nms>=fac_ms)						
		{ 
   			delay_ostimedly(nms/fac_ms);	
		}
		nms%=fac_ms;						 
	}
	delay_us((u32)(nms*1000));			
}
#else  

/* ====== 裸机版本的延时函数 ====== */	 
/** @brief 微秒级延时（裸机版本，直接轮询SysTick计数器）
 *  @param nus 延时的微秒数
 *  @return 无 */
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				    	 
	ticks=nus*SYS_CLK; 						
	told=SysTick->VAL;        				
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			
		}  
	}
}

/** @brief 毫秒级延时（裸机版本，循环调用微秒延时实现）
 *  @param nms 延时的毫秒数
 *  @return 无 */
void delay_ms(u16 nms)
{
	u32 i;
	for(i=0;i<nms;i++) delay_us(1000);
}
#endif
