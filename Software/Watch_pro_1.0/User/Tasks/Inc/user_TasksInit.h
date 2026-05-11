/** @file user_TasksInit.h
 *  @brief 任务初始化头文件 - 定义所有FreeRTOS任务的消息队列、事件标志和任务函数原型
 */

#ifndef __USER_TASKSINIT_H__
#define __USER_TASKSINIT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "cmsis_os.h"

/* ====== 宏定义：事件标志位 ====== */
/** @brief 屏幕刷新深度 - 控制页面返回的最大层数 */
#define SCRRENEW_DEPTH	5
/** @brief UART中断事件标志 - 蓝牙串口收到数据时置位 */
#define HARDINT_EVENT_UART  (1U << 0)
/** @brief 充电检测事件标志 - 充电器插入/拔出时置位 */
#define HARDINT_EVENT_CHARG (1U << 1)
/** @brief MPU中断事件标志 - MPU6050运动检测中断 */
#define HARDINT_EVENT_MPU   (1U << 2)
/** @brief SOS中断事件标志 - SOS报告检测中断 */
#define HARDINT_EVENT_SOS   (1U << 3)
/** @brief UART2中断事件标志 - UART2串口收到数据时置位 */
#define HARDINT_EVENT_UART2 (1U << 4)

/* ====== 消息队列：任务间通信 ====== */
/** @brief 按键消息队列 - 传递按键键值（1=短按, 2=长按） */
extern osMessageQueueId_t Key_MessageQueue;
/** @brief 空闲（息屏）消息队列 - 通知进入息屏状态 */
extern osMessageQueueId_t Idle_MessageQueue;
/** @brief STOP休眠消息队列 - 通知进入STOP低功耗模式 */
extern osMessageQueueId_t Stop_MessageQueue;
/** @brief 空闲打断消息队列 - 用户操作打断空闲计时 */
extern osMessageQueueId_t IdleBreak_MessageQueue;
/** @brief 首页数据更新消息队列 - 通知更新首页显示的传感器数据 */
extern osMessageQueueId_t HomeUpdata_MessageQueue;
/** @brief 数据保存消息队列 - 通知将数据写入EEPROM */
extern osMessageQueueId_t DataSave_MessageQueue;
/** @brief SOS消息队列 */
extern osMessageQueueId_t SOS_MessageQueue;
/** @brief 硬件中断事件句柄 - 用于UART和充电器的中断事件通知 */
extern osEventFlagsId_t HardIntEventHandle;

/**
 * @brief  用户任务初始化 - 创建所有FreeRTOS任务、消息队列和事件标志
 */
void User_Tasks_Init(void);

/**
 * @brief  系统Tick钩子函数 - 每个SysTick中断调用，用于LVGL心跳和空闲计时
 */
void TaskTickHook(void);

#ifdef __cplusplus
}
#endif

#endif

