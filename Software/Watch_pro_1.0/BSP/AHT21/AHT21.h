/** @file AHT21.h @brief AHT21温湿度传感器驱动头文件，定义I2C通信接口函数 */
#ifndef __AHT21_H
#define __AHT21_H

#include "stm32f4xx_hal.h"
#include "iic_hal.h"
#include "delay.h"

uint8_t AHT_Read_Status(void);          /** @brief 读取AHT21状态寄存器 */
void AHT_Reset(void);                   /** @brief 软复位AHT21传感器 */
uint8_t AHT_Init(void);                 /** @brief 初始化AHT21传感器 */
uint8_t AHT_Read(float *humi, float *temp); /** @brief 读取温湿度数据 @param humi[out] 湿度值(%) @param temp[out] 温度值(℃) @return 0成功, 1失败 */

#endif
