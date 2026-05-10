/** @file power.h @brief 电源管理驱动头文件 - 引脚定义、API声明 */

#ifndef _POWER_H_
#define _POWER_H_

#include "stm32f4xx_hal.h"

/* ====== 电源相关引脚定义 ====== */
#define BAT_CHECK_PORT	GPIOA                      // 电池电压ADC检测端口
#define BAT_CHECK_PIN		GPIO_PIN_1               // 电池电压ADC检测引脚：PA1

#define CHARGE_PORT			GPIOA                      // 充电状态检测端口
#define CHARGE_PIN			GPIO_PIN_0                  // 充电状态检测引脚：PA0

#define POWER_PORT			GPIOA                      // 电源开关控制端口
#define POWER_PIN				GPIO_PIN_3               // 电源开关控制引脚：PA3

/* ====== 电源管理API函数声明 ====== */
void Power_Pins_Init(void);          // 初始化电源控制引脚
void Power_Enable(void);             // 开启系统电源
void Power_DisEnable(void);          // 关闭系统电源
float BatCheck(void);                // 单次采集电池电压
float BatCheck_8times(void);         // 8次平均采集电池电压
uint8_t ChargeCheck(void);           // 检测充电状态（1=充电中）
uint8_t PowerCalculate(void);        // 计算电池电量百分比
void Power_Init(void);               // 电源模块初始化

#endif
