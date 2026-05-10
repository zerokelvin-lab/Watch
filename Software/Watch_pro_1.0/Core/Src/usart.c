/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   USART1串口配置——115200波特率/8N1，DMA收发（Stream5 RX/Stream7 TX）+
  *         IDLE空闲中断实现变长数据接收，重定向printf到串口（fputc）
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
#include "usart.h"

/* USER CODE BEGIN 0 */
// 包含stdio.h用于printf重定向
#include "stdio.h"
/* USER CODE END 0 */

// USART1外设句柄和DMA RX/TX句柄（全局变量）
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

/* USART1 init function */

/**
  * @brief  USART1 UART初始化函数
  *         配置为115200波特率、8位数据位、1位停止位、无校验、无流控
  *         收发模式，16倍过采样
  * @retval None
  */
void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  // 配置USART1参数
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;                    // 波特率115200bps
  huart1.Init.WordLength = UART_WORDLENGTH_8B;      // 8位数据位
  huart1.Init.StopBits = UART_STOPBITS_1;           // 1位停止位
  huart1.Init.Parity = UART_PARITY_NONE;            // 无校验位
  huart1.Init.Mode = UART_MODE_TX_RX;               // 收发模式
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;      // 无硬件流控
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;  // 16倍过采样
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief  UART外设MSP初始化回调函数
  *         配置USART1时钟、GPIO引脚（PA9=TX, PA10=RX）
  *         配置DMA接收通道（Stream5）和发送通道（Stream7），使能USART1中断
  * @param  uartHandle UART句柄指针
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    // 使能USART1外设时钟
    __HAL_RCC_USART1_CLK_ENABLE();

    // 使能GPIOA时钟（USART1引脚所在端口）
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    // 配置PA9(TX)和PA10(RX)为复用推挽输出，高速，AF7(USART1)
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    // 配置USART1_RX DMA通道：DMA2_Stream5，通道4，外设到内存，字节对齐
    hdma_usart1_rx.Instance = DMA2_Stream5;
    hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    // 将DMA RX句柄关联到UART句柄
    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    // 配置USART1_TX DMA通道：DMA2_Stream7，通道4，内存到外设，字节对齐
    hdma_usart1_tx.Instance = DMA2_Stream7;
    hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    // 将DMA TX句柄关联到UART句柄
    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    // 使能USART1全局中断，优先级5
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

/**
  * @brief  UART外设MSP反初始化回调函数
  *         关闭USART1时钟、恢复GPIO、反初始化DMA、禁用中断
  * @param  uartHandle UART句柄指针
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    // 禁用USART1外设时钟
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    // 恢复PA9和PA10到默认状态
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    // 反初始化USART1的DMA接收和发送通道
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    // 禁用USART1中断
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/**
  * @brief  printf重定向——将标准输出重定向到USART1串口
  *         通过重写fputc函数，使printf输出通过HAL_UART_Transmit发送
  * @param  ch 要发送的字符
  * @param  f 文件指针（未使用）
  * @return 发送的字符
  */
int fputc(int ch, FILE *f)
{
  // 通过USART1阻塞发送单个字符
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}
/* USER CODE END 1 */
