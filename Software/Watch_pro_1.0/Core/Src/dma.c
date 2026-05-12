/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dma.c
  * @brief   DMA2控制器配置——使能DMA2时钟并配置SPI1_TX/USART1_RX/USART1_TX三个DMA流的中断优先级
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

/* Includes ------------------------------------------------------------------*/
#include "dma.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure DMA                                                              */
/*----------------------------------------------------------------------------*/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @brief  DMA初始化函数——使能DMA2控制器时钟并配置DMA流中断
  *         DMA2_Stream2 → SPI1_TX（SPI发送DMA通道）
  *         DMA2_Stream5 → USART1_RX（串口接收DMA通道）
  *         DMA2_Stream7 → USART1_TX（串口发送DMA通道）
  *         所有DMA中断优先级设为5（适合在FreeRTOS中断安全API范围内使用）
  * @retval None
  */
void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  // 使能DMA2控制器时钟
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  // 配置DMA2各Stream的中断优先级和使能
  /* DMA2_Stream2_IRQn interrupt configuration */
  // DMA2 Stream2中断：用于SPI1_TX，优先级5
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  /* DMA2_Stream5_IRQn interrupt configuration */
  // DMA2 Stream5中断：用于USART1_RX，优先级5
  HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
  /* DMA2_Stream7_IRQn interrupt configuration */
  // DMA2 Stream7中断：用于USART1_TX，优先级5
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
