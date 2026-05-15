/** @file user_CommTask.h
 *  @brief ESP32通信任务 - UART1二进制帧协议收发、定时数据上报、双击语音唤醒
 */
#ifndef __USER_COMMTASK_H__
#define __USER_COMMTASK_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  ESP32通信主任务 - 接收ESP32帧、定时上报运动/环境/健康数据、双击唤醒语音
 * @param  argument 任务参数（未使用）
 */
void CommTask(void *argument);

#ifdef __cplusplus
}
#endif

#endif
