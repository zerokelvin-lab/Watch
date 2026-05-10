/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   TIM定时器头文件——声明TIM3句柄htim3、PWM参数宏和初始化函数
  *         TIM3_CH3(PB0)用于PWM输出（显示屏背光亮度控制）
  *         PWM周期=300（对应0-100%占空比范围），预分频=99（PWM频率≈1667Hz）
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
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
// PWM参数宏定义（供其他模块使用）
#define PWM_PERIOD 300   // PWM周期值（ARR寄存器），取值范围0~300
#define PWM_COMPARE 0    // PWM比较值初始值（CCR寄存器），0=0%占空比
/* USER CODE END Includes */

// TIM3定时器句柄（全局变量，在tim.c中定义）
extern TIM_HandleTypeDef htim3;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

// TIM3 PWM初始化函数声明
void MX_TIM3_Init(void);

// TIM MSP延后初始化函数声明（GPIO复用配置在时钟使能后执行）
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */
