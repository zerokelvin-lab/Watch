/** @file user_ScrRenewTask.h
 *  @brief 屏幕刷新任务 - 处理按键触发的页面返回/切换，并管理传感器的休眠
 */

#ifndef __USER_SCRRENEWTASK_H__
#define __USER_SCRRENEWTASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "PageManager.h"

/**
 * @brief  屏幕刷新任务 - 根据按键键值执行页面返回或回到底部，并在返回菜单时休眠传感器
 * @param  argument 任务参数（未使用）
 */
void ScrRenewTask(void *argument);


#ifdef __cplusplus
}
#endif

#endif

