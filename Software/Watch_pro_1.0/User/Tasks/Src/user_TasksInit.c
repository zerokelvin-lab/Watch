/** @file user_TasksInit.c
 *  @brief 任务初始化 - 创建所有FreeRTOS任务、消息队列、事件标志，以及LVGL和看门狗服务任务
 */

/* ====== 头文件包含 ====== */
/* Private includes -----------------------------------------------------------*/
//includes
#include "user_TasksInit.h"
//sys
#include "sys.h"
#include "stdio.h"
#include "lcd.h"
#include "WDOG.h"
//gui
#include "lvgl.h"
#include "ui_TimerPage.h"
//tasks  // 各任务模块
#include "user_HardwareInitTask.h"
#include "user_RunModeTasks.h"
#include "user_KeyTask.h"
#include "user_ScrRenewTask.h"
#include "user_SensUpdateTask.h"
#include "user_ChargCheckTask.h"
#include "user_MessageSendTask.h"
#include "user_DataSaveTask.h"

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


/* Tasks ---------------------------------------------------------------------*/
// Hardwares initialization  // 硬件初始化任务
osThreadId_t HardwareInitTaskHandle;
const osThreadAttr_t HardwareInitTask_attributes = {
  .name = "HardwareInitTask",
  .stack_size = 128 * 10,
  .priority = (osPriority_t) osPriorityHigh3,  // 高优先级，优先完成初始化
};

//LVGL Handler task  // LVGL图形库处理任务
osThreadId_t LvHandlerTaskHandle;
const osThreadAttr_t LvHandlerTask_attributes = {
  .name = "LvHandlerTask",
  .stack_size = 128 * 24,
  .priority = (osPriority_t) osPriorityLow,  // 低优先级
};

//WDOG Feed task  // 看门狗喂狗任务
osThreadId_t WDOGFeedTaskHandle;
const osThreadAttr_t WDOGFeedTask_attributes = {
  .name = "WDOGFeedTask",
  .stack_size = 128 * 1,
  .priority = (osPriority_t) osPriorityHigh2,  // 高优先级，防止系统复位
};

//Idle Enter Task  // 空闲（息屏）进入任务
osThreadId_t IdleEnterTaskHandle;
const osThreadAttr_t IdleEnterTask_attributes = {
  .name = "IdleEnterTask",
  .stack_size = 128 * 1,
  .priority = (osPriority_t) osPriorityHigh,
};

//Stop Enter Task  // STOP低功耗进入任务
osThreadId_t StopEnterTaskHandle;
const osThreadAttr_t StopEnterTask_attributes = {
  .name = "StopEnterTask",
  .stack_size = 128 * 16,  // 需要较大栈空间（包含唤醒恢复逻辑）
  .priority = (osPriority_t) osPriorityHigh1,
};

//Key task  // 按键扫描任务
osThreadId_t KeyTaskHandle;
const osThreadAttr_t KeyTask_attributes = {
  .name = "KeyTask",
  .stack_size = 128 * 1,
  .priority = (osPriority_t) osPriorityNormal,
};

//ScrRenew task  // 屏幕刷新（页面切换）任务
osThreadId_t ScrRenewTaskHandle;
const osThreadAttr_t ScrRenewTask_attributes = {
  .name = "ScrRenewTask",
  .stack_size = 128 * 10,
  .priority = (osPriority_t) osPriorityLow1,
};

//SensorDataRenew task  // 传感器数据更新任务
osThreadId_t SensorDataTaskHandle;
const osThreadAttr_t SensorDataTask_attributes = {
  .name = "SensorDataTask",
  .stack_size = 128 * 5,
  .priority = (osPriority_t) osPriorityLow1,
};

//HRDataRenew task  // 心率数据更新任务
osThreadId_t HRDataTaskHandle;
const osThreadAttr_t HRDataTask_attributes = {
  .name = "HRDataTask",
  .stack_size = 128 * 5,
  .priority = (osPriority_t) osPriorityLow1,
};

//ChargPageEnterTask  // 充电页面切换任务
osThreadId_t ChargPageEnterTaskHandle;
const osThreadAttr_t ChargPageEnterTask_attributes = {
  .name = "ChargPageEnterTask",
  .stack_size = 128 * 10,
  .priority = (osPriority_t) osPriorityLow1,
};

//messagesendtask  // BLE消息收发任务
osThreadId_t MessageSendTaskHandle;
const osThreadAttr_t MessageSendTask_attributes = {
  .name = "MessageSendTask",
  .stack_size = 128 * 5,
  .priority = (osPriority_t) osPriorityLow1,
};

//MPUCheckTask  // MPU6050姿态检测任务
osThreadId_t MPUCheckTaskHandle;
const osThreadAttr_t MPUCheckTask_attributes = {
  .name = "MPUCheckTask",
  .stack_size = 128 * 3,
  .priority = (osPriority_t) osPriorityLow2,
};

//DataSaveTask  // 数据保存任务
osThreadId_t DataSaveTaskHandle;
const osThreadAttr_t DataSaveTask_attributes = {
  .name = "DataSaveTask",
  .stack_size = 128 * 5,
  .priority = (osPriority_t) osPriorityLow2,
};


/* Message queues ------------------------------------------------------------*/
//Key message  // 按键消息队列
osMessageQueueId_t Key_MessageQueue;
/** @brief 空闲（息屏）消息队列 */
osMessageQueueId_t Idle_MessageQueue;
/** @brief STOP休眠消息队列 */
osMessageQueueId_t Stop_MessageQueue;
/** @brief 空闲打断消息队列 */
osMessageQueueId_t IdleBreak_MessageQueue;
/** @brief 首页数据更新消息队列 */
osMessageQueueId_t HomeUpdata_MessageQueue;
/** @brief 数据保存消息队列 */
osMessageQueueId_t DataSave_MessageQueue;
/** @brief SOS消息队列 */
osMessageQueueId_t SOS_MessageQueue;
/** @brief 硬件中断事件句柄 */
osEventFlagsId_t HardIntEventHandle;

/* Private function prototypes -----------------------------------------------*/
void LvHandlerTask(void *argument);
void WDOGFeedTask(void *argument);

/**
  * @brief  FreeRTOS initialization  // FreeRTOS任务初始化
  * @param  None
  * @retval None
  */
void User_Tasks_Init(void)
{
  /* add mutexes, ... */

  /* add semaphores, ... */

  /* start timers, add new ones, ... */

  /* add queues, ... */  // 创建消息队列
	Key_MessageQueue  = osMessageQueueNew(1, 1, NULL);       // 按键消息队列
	Idle_MessageQueue = osMessageQueueNew(1, 1, NULL);       // 空闲消息队列
	Stop_MessageQueue = osMessageQueueNew(1, 1, NULL);       // 休眠消息队列
	IdleBreak_MessageQueue = osMessageQueueNew(1, 1, NULL);  // 空闲打断消息队列
	HomeUpdata_MessageQueue = osMessageQueueNew(1, 1, NULL); // 首页更新消息队列
  SOS_MessageQueue = osMessageQueueNew(1, 1, NULL);         // SOS消息队列
	DataSave_MessageQueue = osMessageQueueNew(2, 1, NULL);   // 数据保存消息队列
  HardIntEventHandle = osEventFlagsNew(NULL);               // 硬件中断事件标志组

	/* add threads, ... */  // 创建各任务线程
  HardwareInitTaskHandle  = osThreadNew(HardwareInitTask, NULL, &HardwareInitTask_attributes);
  LvHandlerTaskHandle  = osThreadNew(LvHandlerTask, NULL, &LvHandlerTask_attributes);
  WDOGFeedTaskHandle   = osThreadNew(WDOGFeedTask, NULL, &WDOGFeedTask_attributes);
	IdleEnterTaskHandle  = osThreadNew(IdleEnterTask, NULL, &IdleEnterTask_attributes);
	StopEnterTaskHandle  = osThreadNew(StopEnterTask, NULL, &StopEnterTask_attributes);
	KeyTaskHandle 			 = osThreadNew(KeyTask, NULL, &KeyTask_attributes);
	ScrRenewTaskHandle   = osThreadNew(ScrRenewTask, NULL, &ScrRenewTask_attributes);
	SensorDataTaskHandle = osThreadNew(SensorDataUpdateTask, NULL, &SensorDataTask_attributes);
	HRDataTaskHandle		 = osThreadNew(HRDataUpdateTask, NULL, &HRDataTask_attributes);
	ChargPageEnterTaskHandle = osThreadNew(ChargPageEnterTask, NULL, &ChargPageEnterTask_attributes);
  MessageSendTaskHandle = osThreadNew(MessageSendTask, NULL, &MessageSendTask_attributes);
	MPUCheckTaskHandle		= osThreadNew(MPUCheckTask, NULL, &MPUCheckTask_attributes);
	DataSaveTaskHandle		= osThreadNew(DataSaveTask, NULL, &DataSaveTask_attributes);

  /* add events, ... */


	/* add  others ... */  // 发送首次首页数据更新
	uint8_t HomeUpdataStr;
	osMessageQueuePut(HomeUpdata_MessageQueue, &HomeUpdataStr, 0, 1);

}


/**
  * @brief  FreeRTOS Tick Hook, to increase the LVGL tick  // 系统Tick钩子，用于驱动LVGL时钟和空闲计时
  * @param  None
  * @retval None
  */
void TaskTickHook(void)
{
	//to increase the LVGL tick  // 递增LVGL时钟
	lv_tick_inc(1);
	//to increase the timerpage's timer(put in here is to ensure the Real Time)  // 计时器页面计时（放在此处保证实时性）
	if(ui_TimerPageFlag)  // 计时器页面激活中
	{
    IdleTimerCount = 0;  // 计时器运行时不进入空闲
    ui_TimerPage_elapsed_ms += 1;  // 递增计时器毫秒数
	}
  else  // 非计时器页面，递增空闲计时
  {
    IdleTimerCount += 1;
  }
	user_HR_timecount+=1;  // 心率计时递增
}


/**
  * @brief  LVGL Handler task, to run the lvgl  // LVGL处理任务
  * @param  argument: Not used
  * @retval None
  */
void LvHandlerTask(void *argument)
{
	uint8_t IdleBreakstr=0;
  while(1)
  {
		if(lv_disp_get_inactive_time(NULL)<1000)  // 屏幕在1秒内有刷新活动，说明用户正在操作
		{
			//Idle time break, set to 0  // 打断空闲计时
			osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 0);
		}
    uint32_t wait = lv_task_handler();  // 执行LVGL任务
    if(wait < 1U) wait = 1U;  // 最小延时1ms
    if(wait > 30U) wait = 30U;  // 最大延时30ms
    osDelay(wait);
	}
}


/**
  * @brief  Watch Dog Feed task  // 看门狗喂狗任务
  * @param  argument: Not used
  * @retval None
  */
void WDOGFeedTask(void *argument)
{
	//owdg  // 独立看门狗
	WDOG_Port_Init();  // 初始化看门狗引脚
  while(1)
  {
		WDOG_Feed();    // 喂狗（清除计数器）
		WDOG_Enable();  // 使能看门狗
    osDelay(100);   // 100ms喂狗周期
  }
}


