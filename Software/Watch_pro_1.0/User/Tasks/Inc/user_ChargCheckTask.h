/** @file user_ChargCheckTask.h
 *  @brief 充电检测与充电页面切换任务 - 监测充电状态并自动进入/退出充电界面
 */

#ifndef __USER_CHARGCHECKTASK_H__
#define __USER_CHARGCHECKTASK_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  充电页面进入任务 - 检测充电插拔事件，自动切换充电界面
 * @param  argument 任务参数（未使用）
 */
void ChargPageEnterTask(void *argument);

#ifdef __cplusplus
}
#endif

#endif

