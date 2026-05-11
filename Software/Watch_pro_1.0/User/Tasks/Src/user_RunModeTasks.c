/** @file user_RunModeTasks.c
 *  @brief 运行模式管理任务 - 管理空闲息屏、STOP低功耗模式的进入与唤醒恢复
 */

/* ====== 头文件包含 ====== */
/* Private includes -----------------------------------------------------------*/
//includes
#include "user_TasksInit.h"

#include "ui.h"
#include "ui_HomePage.h"
#include "ui_OffTimePage.h"
#include "ui_TimerPage.h"

#include "main.h"
#include "stm32f4xx_it.h"
#include "usart.h"
#include "lcd_init.h"
#include "power.h"
#include "CST816.h"
#include "MPU6050.h"
#include "key.h"
#include "WDOG.h"

#include "HWDataAccess.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/** @brief 空闲计时计数器 - 记录自上次用户操作以来的毫秒数，用于判断是否进入息屏/休眠 */
uint32_t IdleTimerCount = 0;

/* Private function prototypes -----------------------------------------------*/

/* Tasks ---------------------------------------------------------------------*/

/**
	* @brief  Enter Idle state  // 进入空闲（息屏）状态
	* @param  argument: Not used
	* @retval None
	*/
void IdleEnterTask(void *argument)
{
	uint8_t Idlestr=0;        // 息屏消息
	uint8_t IdleBreakstr=0;   // 空闲打断消息
	uint8_t light_off_sent = 0;  // 背光已关闭标志
	while(1)
	{
		//light get dark  // 收到息屏请求，降低背光
		if(osMessageQueueGet(Idle_MessageQueue,&Idlestr,NULL,1)==osOK)
		{
			LCD_Set_Light(5);  // 背光降至最低（5%）
			light_off_sent = 1;
		}
		//resume light if light got dark and idle state breaked by key pressing or screen touching  // 被按键或触摸打断，恢复背光
		if(osMessageQueueGet(IdleBreak_MessageQueue,&IdleBreakstr,NULL,1)==osOK)
		{
			IdleTimerCount = 0;  // 重置空闲计时
			light_off_sent = 0;
			LCD_Set_Light(ui_LightSliderValue);  // 恢复用户设置的亮度
		}

		// 空闲时间超过用户设置的息屏时间，发送息屏命令
		if((!light_off_sent) && (IdleTimerCount >= ((uint32_t)ui_LTimeValue * 1000U)))
		{
			uint8_t IdleMsg = 0;
			osMessageQueuePut(Idle_MessageQueue, &IdleMsg, 0, 0);
		}

		// 空闲时间超过用户设置的休眠时间，发送休眠命令
		if(IdleTimerCount >= ((uint32_t)ui_TTimeValue * 1000U))
		{
			uint8_t Stopstr = 1;
			IdleTimerCount = 0;
			light_off_sent = 0;
			osMessageQueuePut(Stop_MessageQueue, &Stopstr, 0, 0);
		}

		osDelay(10);  // 10ms检测周期
	}
}

/**
  * @brief  enter the stop mode and resume  // 进入STOP模式并恢复
  * @param  argument: Not used
  * @retval None
  */
void StopEnterTask(void *argument)
{
	uint8_t Stopstr;          // 休眠消息
	uint8_t HomeUpdataStr;    // 首页更新消息
	uint8_t Wrist_Flag=0;     // 抬手唤醒标志
	while(1)
	{
		if(osMessageQueueGet(Stop_MessageQueue,&Stopstr,NULL,0)==osOK)  // 收到休眠请求
		{

			/*************************** your operations before sleep***************************/  // 休眠前操作
			sleep:
			IdleTimerCount = 0;  // 重置空闲计时

			//sensors  // 传感器已在此前休眠

			//usart  // 关闭串口以省电
			HAL_UART_MspDeInit(&huart1);
			HAL_UART_MspDeInit(&huart2);

			//lcd  // 关闭显示屏
			LCD_RES_Clr();
			LCD_Close_Light();
			//touch  // 触摸屏进入休眠
			CST816_Sleep();

			/***********************************************************************************/

			/****************************** enter wakeup operations *****************************/  // 进入休眠

			vTaskSuspendAll();  // 挂起所有任务
			//Disnable Watch Dog  // 关闭看门狗
			WDOG_Disnable();
			//systick int  // 关闭SysTick中断
			CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);
			//enter stop mode  // 进入STOP低功耗模式
			HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON,PWR_STOPENTRY_WFI);

			//here is the sleep period  // 此处为休眠期间（CPU停止）

			/***********************************************************************************/

			/****************************** quit wakeup operations *****************************/  // 唤醒后恢复

			//resume run mode and reset the sysclk  // 恢复运行模式，重新配置系统时钟
			SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);  // 恢复SysTick中断
			HAL_SYSTICK_Config(SystemCoreClock / (1000U / uwTickFreq));
			SystemClock_Config();
			WDOG_Feed();  // 喂狗
			xTaskResumeAll();  // 恢复所有任务

			/***********************************************************************************/

			/****************************** your wakeup operations *****************************/  // 唤醒后操作

			//MPU Check  // 检查手腕姿态
			if(HWInterface.IMU.wrist_is_enabled)  // 抬手亮屏功能已启用
			{
				uint8_t hor;
				hor = MPU_isHorizontal();  // 检测是否为水平姿态（抬手）
				if(hor && HWInterface.IMU.wrist_state == WRIST_DOWN)  // 检测到抬手动作
				{
					HWInterface.IMU.wrist_state = WRIST_UP;
					Wrist_Flag = 1;
					//resume, go on  // 唤醒，继续执行
				}
				else if(!hor && HWInterface.IMU.wrist_state == WRIST_UP)  // 放下手腕
				{
					HWInterface.IMU.wrist_state = WRIST_DOWN;
					IdleTimerCount  = 0;
					goto sleep;  // 重新进入休眠
				}
			}

			// 判断唤醒源：按键按下 || 充电插入 || 抬手亮屏
			if(!KEY1 || KEY2 || ChargeCheck() || Wrist_Flag)
			{
				Wrist_Flag = 0;
				//resume, go on  // 有效唤醒，继续
			}
			else  // 非预期唤醒，重新休眠
			{
				IdleTimerCount  = 0;
				goto sleep;
			}

			//usart  // 恢复串口
			HAL_UART_MspInit(&huart1);
			HAL_UART_MspInit(&huart2);
			// 重新启动UART2 DMA接收
			HAL_UART_Receive_DMA(&huart2,(uint8_t*)HardInt_receive2_str,25);
			__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
			//lcd  // 恢复显示屏
			LCD_Init();
			LCD_Set_Light(ui_LightSliderValue);
			//touch  // 唤醒触摸屏
			CST816_Wakeup();
			//check if is Charging  // 检查是否在充电
			if(ChargeCheck())
			{osEventFlagsSet(HardIntEventHandle, HARDINT_EVENT_CHARG);}  // 通知充电事件
			//send the Home Updata message  // 发送首页数据更新消息
			osMessageQueuePut(HomeUpdata_MessageQueue, &HomeUpdataStr, 0, 1);

			/**************************************************************************************/

		}
		osDelay(100);  // 100ms检测周期
	}
}

