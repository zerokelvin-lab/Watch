/** @file user_MessageSendTask.h
 *  @brief BLE消息收发任务 - 通过UART接收蓝牙指令并回复传感器数据或设置时间
 */

#ifndef __USER_MESSAGESENDTASK_H__
#define __USER_MESSAGESENDTASK_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  BLE消息收发任务 - 解析从蓝牙串口收到的AT指令，回复设备信息、传感器数据或设置系统时间
 * @param  argument 任务参数（未使用）
 */
void MessageSendTask(void *argument);


#ifdef __cplusplus
}
#endif

#endif

