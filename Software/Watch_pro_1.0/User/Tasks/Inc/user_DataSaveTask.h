/** @file user_DataSaveTask.h
 *  @brief 数据保存任务 - 将用户设置和计步数据保存到EEPROM
 */

#ifndef __USER_DATASAVETASK_H__
#define __USER_DATASAVETASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "user_TasksInit.h"

/**
 * @brief  数据保存任务 - 将用户设置、日期、步数等数据周期性地保存至EEPROM
 * @param  argument 任务参数（未使用）
 */
void DataSaveTask(void *argument);

	
#ifdef __cplusplus
}
#endif

#endif

