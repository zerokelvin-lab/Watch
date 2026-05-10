/** @file user_SensUpdateTask.h
 *  @brief 传感器数据更新任务 - 定时刷新心率、环境温湿度、电子罗盘、气压高度等传感器数据
 */

#ifndef __USER_SENSUPDATETASK_H__
#define __USER_SENSUPDATETASK_H__

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 心率检测时间计数 - 配合HR算法库，用于心率计算的时间累积变量 */
extern uint32_t user_HR_timecount;

/**
 * @brief  传感器数据更新任务 - 定时采集电池、计步、温湿度数据，并根据当前页面更新对应传感器
 * @param  argument 任务参数（未使用）
 */
void SensorDataUpdateTask(void *argument);

/**
 * @brief  心率数据更新任务 - 在心率页面显示时持续采集心率并更新显示值
 * @param  argument 任务参数（未使用）
 */
void HRDataUpdateTask(void *argument);

/**
 * @brief  MPU6050姿态检测任务 - 监测手表姿态（抬起/放下），实现抬手亮屏和放下息屏
 * @param  argument 任务参数（未使用）
 */
void MPUCheckTask(void *argument);

#ifdef __cplusplus
}
#endif

#endif

