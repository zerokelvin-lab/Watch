/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.c
  * @brief   SPI1外设配置——主机模式，8位数据，CPOL=1/CPHA=1（模式3），波特率分频2
  *         通过DMA2_Stream2实现SPI TX DMA发送，用于驱动智能手表显示屏等SPI设备
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
#include "spi.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

// SPI1外设句柄和DMA TX句柄（全局变量）
SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

/* SPI1 init function */
/**
  * @brief  SPI1初始化函数
  *         配置为主机模式、双线全双工、8位数据帧
  *         时钟极性高(CPOL=1)/时钟相位第2边沿(CPHA=1)对应SPI模式3
  *         软件NSS管理，MSB先行，波特率=APB2时钟/2
  * @retval None
  */
void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  // 配置SPI1参数
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;              // 主机模式
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;    // 双线全双工
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;         // 8位数据帧
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;     // 时钟空闲时为高电平
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;           // 第2个时钟边沿采样
  hspi1.Init.NSS = SPI_NSS_SOFT;                   // 软件管理片选
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;  // 波特率 = fPCLK/2
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;          // 高位在前
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;          // 禁用TI模式
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;  // 禁用CRC校验
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief  SPI外设MSP初始化回调函数
  *         配置SPI1时钟、GPIO引脚(PB3=SCK, PB5=MOSI)以及DMA发送通道
  * @param  spiHandle SPI句柄指针
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    // 使能SPI1外设时钟
    __HAL_RCC_SPI1_CLK_ENABLE();

    // 使能GPIOB时钟（SPI1引脚所在端口）
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PB3     ------> SPI1_SCK
    PB5     ------> SPI1_MOSI
    */
    // 配置PB3(SCK)和PB5(MOSI)为复用推挽输出，高速，复用功能AF5(SPI1)
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* SPI1 DMA Init */
    /* SPI1_TX Init */
    // 配置SPI1_TX DMA通道：DMA2_Stream2，通道2，内存到外设，字节对齐，普通模式
    hdma_spi1_tx.Instance = DMA2_Stream2;
    hdma_spi1_tx.Init.Channel = DMA_CHANNEL_2;
    hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi1_tx.Init.Mode = DMA_NORMAL;
    hdma_spi1_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_spi1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_spi1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    // 将DMA TX句柄关联到SPI句柄
    __HAL_LINKDMA(spiHandle,hdmatx,hdma_spi1_tx);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
}

/**
  * @brief  SPI外设MSP反初始化回调函数
  *         关闭SPI1时钟，恢复GPIO引脚默认状态，反初始化DMA
  * @param  spiHandle SPI句柄指针
  * @retval None
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    // 禁用SPI1外设时钟
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PB3     ------> SPI1_SCK
    PB5     ------> SPI1_MOSI
    */
    // 恢复PB3和PB5到默认状态
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_5);

    /* SPI1 DMA DeInit */
    // 反初始化SPI1_TX的DMA通道
    HAL_DMA_DeInit(spiHandle->hdmatx);
  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
