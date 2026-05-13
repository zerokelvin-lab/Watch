/** @file user_MKS142AutoTask.h
 *  @brief MKS142后台自动测量任务 - 非健康页面时每10分钟自动测量，稳定后更新保存数据
 */
#ifndef __USER_MKS142AUTOTASK_H__
#define __USER_MKS142AUTOTASK_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  MKS142后台自动测量任务 - 每10分钟自动测量心率血氧等健康数据
 *         仅在非心率/疲劳页面时运行，测量到稳定数据后自动停止
 * @param  argument 任务参数（未使用）
 */
void MKS142AutoMeasureTask(void *argument);

#ifdef __cplusplus
}
#endif

#endif
