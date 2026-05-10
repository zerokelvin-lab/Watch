/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.h
  * @brief   SPI外设头文件——声明SPI1句柄hspi1、DMA发送句柄hdma_spi1_tx和初始化函数MX_SPI1_Init()
  *         SPI1用于驱动智能手表显示屏，支持DMA高速发送
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
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

// SPI1外设句柄（全局变量，在spi.c中定义）
extern SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN Private defines */
// SPI1_TX DMA句柄（全局变量，在spi.c中定义）
extern DMA_HandleTypeDef hdma_spi1_tx;
/* USER CODE END Private defines */

// SPI1初始化函数声明
void MX_SPI1_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */
