/** @file user_ChargCheckTask.c
 *  @brief 充电检测与充电页面切换任务 - 监测充电状态并自动进入/退出充电界面
 */

/* ====== 头文件包含 ====== */
/* Private includes -----------------------------------------------------------*/
//includes
#include "user_TasksInit.h"
#include "user_ScrRenewTask.h"
#include "user_RunModeTasks.h"
#include "ui_HomePage.h"
#include "ui_ChargPage.h"
#include "main.h"
#include "HWDataAccess.h"
#include "stm32f4xx_it.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/


/**
  * @brief  charg page enter task  // 充电页面进入任务
  * @param  argument: Not used
  * @retval None
  */
void ChargPageEnterTask(void *argument)
{
	while(1)
	{
		uint32_t hardint_flags = osEventFlagsWait(HardIntEventHandle, HARDINT_EVENT_CHARG, osFlagsWaitAny, osWaitForever);
		if((hardint_flags & HARDINT_EVENT_CHARG) != 0U)  // 充电事件发生
		{
			uint8_t IdleBreakstr = 0;
			osMessageQueuePut(IdleBreak_MessageQueue,&IdleBreakstr,NULL,1);  // 打断空闲状态
			if((ChargeCheck()) && (Page_Get_NowPage()->page_obj != &ui_ChargPage))  // 正在充电且不在充电页
			{
				Page_Load(&Page_Charg);  // 加载充电页面
			}
			else if((!ChargeCheck()) && (Page_Get_NowPage()->page_obj == &ui_ChargPage))  // 已拔出且当前在充电页
			{
				Page_Back();  // 返回上一页
			}
		}
	}
}

