/** @file user_RunModeTasks.h
 *  @brief 运行模式管理任务 - 管理空闲/息屏和STOP低功耗模式的进入与唤醒
 */

#ifndef __USER_RUNMODETASKS_H__
#define __USER_RUNMODETASKS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "user_TasksInit.h"

/**
 * @brief  空闲（息屏）进入任务 - 检测空闲超时，降低屏幕亮度/进入息屏状态
 * @param  argument 任务参数（未使用）
 */
void IdleEnterTask(void *argument);

/**
 * @brief  STOP模式进入任务 - 进入STOP低功耗模式，处理唤醒源检测和外设恢复
 * @param  argument 任务参数（未使用）
 */
void StopEnterTask(void *argument);

/** @brief 空闲计时计数器 - 记录自上次用户操作以来的毫秒数，用于判断是否进入息屏/休眠 */
extern uint32_t IdleTimerCount;

#ifdef __cplusplus
}
#endif

#endif

