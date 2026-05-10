/** @file user_KeyTask.c
 *  @brief 按键检测任务 - 轮询扫描物理按键，区分短按/长按并发送消息
 */

/* ====== 头文件包含 ====== */
/* Private includes -----------------------------------------------------------*/
//includes
#include "user_TasksInit.h"
#include "ui_HomePage.h"
#include "main.h"
#include "key.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/


/**
  * @brief  Key press check task  // 按键扫描任务
  * @param  argument: Not used
  * @retval None
  */
void KeyTask(void *argument)
{
	uint8_t keystr=0;         // 按键键值（1=短按, 2=长按）
	uint8_t Stopstr=0;        // 休眠消息
	uint8_t IdleBreakstr=0;   // 空闲打断消息
	while(1)
	{
		switch(KeyScan(0))  // 扫描按键状态
		{
			case 1:  // 短按（KEY1按下）
				keystr = 1;
				osMessageQueuePut(Key_MessageQueue, &keystr, 0, 1);      // 发送按键消息
				osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);  // 打断空闲计时
				break;

			case 2:  // 长按（KEY2按下）
				if(Page_Get_NowPage()->page_obj == &ui_HomePage)  // 在首页时长按=进入休眠
				{
					osMessageQueuePut(Stop_MessageQueue, &Stopstr, 0, 1);  // 发送休眠消息
				}
				else  // 非首页时长按=返回操作
				{
					keystr = 2;
					osMessageQueuePut(Key_MessageQueue, &keystr, 0, 1);
					osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);
				}
				break;
		}
		osDelay(1);  // 1ms扫描周期
	}
}
