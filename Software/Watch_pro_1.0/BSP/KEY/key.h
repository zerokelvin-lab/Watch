/** @file key.h @brief 按键驱动头文件，定义按键引脚、中断和扫描函数 */
#ifndef __KEY_H__
#define __KEY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* ====== KEY1 按键定义(PA5, 下降沿触发) ====== */
#define KEY1_PORT	GPIOA           /** @brief KEY1按键GPIO端口 */
#define KEY1_PIN	GPIO_PIN_5      /** @brief KEY1按键引脚 */
#define KEY1 HAL_GPIO_ReadPin(KEY1_PORT,KEY1_PIN)  /** @brief 读取KEY1引脚电平 */

/* ====== KEY2 唤醒按键定义(PA4, 上升沿触发) ====== */
#define KEY2_PORT	GPIOA           /** @brief KEY2/Wake按键GPIO端口 */
#define KEY2_PIN	GPIO_PIN_4      /** @brief KEY2/Wake按键引脚 */
#define KEY2 HAL_GPIO_ReadPin(KEY2_PORT,KEY2_PIN)  /** @brief 读取KEY2引脚电平 */
	
void Key_Port_Init(void);              /** @brief 初始化按键GPIO和外部中断 */
void Key_Interrupt_Callback(void);     /** @brief 按键中断回调函数 */
uint8_t KeyScan(uint8_t mode);         /** @brief 按键扫描(带消抖) @param mode 1=连续扫描, 0=单次触发 @return 按键键值(0=无按键, 1=KEY1, 2=KEY2) */


#ifdef __cplusplus
}
#endif
#endif
