/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.h
  * @brief   RTC实时时钟头文件——声明RTC句柄hrtc、初始化函数和自定义时间/日期设置接口
  *         提供RTC_SetTime()、RTC_SetDate()和weekday_calculate()用户API
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

// RTC外设句柄（全局变量，在rtc.c中定义）
extern RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

// RTC初始化函数声明（CubeMX生成）
void MX_RTC_Init(void);

/* USER CODE BEGIN Prototypes */
// 用户自定义RTC接口函数声明
void RTC_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);  // 设置RTC时间
void RTC_SetDate(uint8_t year, uint8_t month, uint8_t date);        // 设置RTC日期（自动计算星期）
uint8_t weekday_calculate(int y,int m,int d,int c);                  // 星期计算函数（蔡勒公式）
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __RTC_H__ */
