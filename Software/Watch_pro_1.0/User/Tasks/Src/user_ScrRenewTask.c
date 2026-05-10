/** @file user_ScrRenewTask.c
 *  @brief 屏幕刷新任务 - 根据按键键值执行页面返回/切换，并管理传感器的唤醒与休眠
 */

/* ====== 头文件包含 ====== */
/* Private includes -----------------------------------------------------------*/
//includes
#include "user_TasksInit.h"
#include "user_ScrRenewTask.h"
#include "main.h"
#include "lvgl.h"
#include "ui_HomePage.h"
#include "ui_MenuPage.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
extern osMessageQueueId_t Key_MessageQueue;  // 按键消息队列

/* Private function prototypes -----------------------------------------------*/


/**
  * @brief  Screen renew task  // 屏幕刷新（页面切换）任务
  * @param  argument: Not used
  * @retval None
  */
void ScrRenewTask(void *argument)
{
	uint8_t keystr=0;
	while(1)
	{
		if(osMessageQueueGet(Key_MessageQueue,&keystr,NULL,0)==osOK)  // 收到按键消息
		{
			//key1 pressed  // 按键1（返回键）按下
			if(keystr == 1)
			{

				Page_Back();  // 返回上一页
				if(Page_Get_NowPage()->page_obj == &ui_MenuPage)  // 已返回到菜单页
				{
					//HR sensor sleep  // 休眠心率传感器
					EM7028_hrs_DisEnable();
					//sensor sleep  // 休眠电子罗盘
					LSM303DLH_Sleep();
					// SPL_Sleep();  // 休眠气压计
				}
			}
			//key2 pressed  // 按键2（底部返回键）按下
			else if(keystr == 2)
			{
				Page_Back_Bottom();  // 返回底部页面
				//HR sensor sleep  // 休眠心率传感器
				EM7028_hrs_DisEnable();
  				//sensor sleep  // 休眠电子罗盘
				LSM303DLH_Sleep();
				// SPL_Sleep();  // 休眠气压计
			}
		}
		osDelay(10);  // 10ms检测周期
	}
}
