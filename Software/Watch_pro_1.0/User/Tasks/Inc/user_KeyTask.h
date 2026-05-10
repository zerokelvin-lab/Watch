/** @file user_KeyTask.h
 *  @brief 按键检测任务 - 扫描物理按键状态并发送消息通知其他任务
 */

#ifndef __USER_KEYTASK_H__
#define __USER_KEYTASK_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  按键检测任务 - 轮询扫描按键状态，区分短按/长按，将键值放入消息队列
 * @param  argument 任务参数（未使用）
 */
void KeyTask(void *argument);


#ifdef __cplusplus
}
#endif

#endif

