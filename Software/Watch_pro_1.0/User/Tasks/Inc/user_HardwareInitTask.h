/** @file user_HardwareInitTask.h
 *  @brief 硬件初始化任务 - 上电后初始化所有外设（传感器、显示屏、触摸、BLE等）
 */

#ifndef __USER_HARDWAREINITTASK_H__
#define __USER_HARDWAREINITTASK_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  硬件初始化任务 - 依次初始化RTC、串口、PWM、传感器、EEPROM、BLE、触摸屏、LCD和LVGL
 * @param  argument 任务参数（未使用）
 */
void HardwareInitTask(void *argument);

#ifdef __cplusplus
}
#endif

#endif

