/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   TIM3定时器PWM配置——通道3（PB0）输出PWM信号，用于智能手表显示屏背光亮度调节
  *         PWM周期300，预分频99，计数器时钟=APB1/100=500kHz，PWM频率=500kHz/300≈1667Hz
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
#include "tim.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

// TIM3定时器句柄（全局变量）
TIM_HandleTypeDef htim3;

/* TIM3 init function */
/**
  * @brief  TIM3 PWM初始化函数
  *         配置为PWM模式1输出，向上计数，周期300，预分频99
  *         PWM频率 = 50MHz(APB1×2) / (99+1) / 300 ≈ 1667Hz
  *         通道3（TIM3_CH3 → PB0），初始占空比0%
  * @retval None
  */
void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  // 主从模式配置和输出比较配置结构体
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  // 配置TIM3基本参数
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 99;                         // 预分频值：APB1时钟/100 = 500kHz
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;       // 向上计数模式
  htim3.Init.Period = 300;                           // 自动重装载值：PWM周期
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // 时钟不分频
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;  // 禁用自动重装载预装载
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  // 配置主从模式：无主输出触发，禁用主从模式
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  // 配置PWM通道3：PWM模式1，初始占空比0%，极性高，禁用快速模式
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;                               // 初始占空比为0（背光关闭）
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;        // 输出极性高
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;         // 禁用快速输出比较
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  // GPIO初始化延后处理（因为GPIO时钟可能尚未就绪）
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief  TIM PWM外设MSP初始化回调函数
  *         使能TIM3外设时钟
  * @param  tim_pwmHandle TIM句柄指针
  * @retval None
  */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* tim_pwmHandle)
{

  if(tim_pwmHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* TIM3 clock enable */
    // 使能TIM3外设时钟
    __HAL_RCC_TIM3_CLK_ENABLE();
  /* USER CODE BEGIN TIM3_MspInit 1 */

  /* USER CODE END TIM3_MspInit 1 */
  }
}

/**
  * @brief  TIM MSP延后初始化回调函数
  *         在外设初始化完成后配置GPIO引脚复用功能
  *         TIM3_CH3 → PB0，复用推挽输出（AF2_TIM3）
  * @param  timHandle TIM句柄指针
  * @retval None
  */
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(timHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspPostInit 0 */

  /* USER CODE END TIM3_MspPostInit 0 */

    // 使能GPIOB时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM3 GPIO Configuration
    PB0     ------> TIM3_CH3
    */
    // 配置PB0为复用推挽输出，AF2（TIM3_CH3），无上下拉，低速
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM3_MspPostInit 1 */

  /* USER CODE END TIM3_MspPostInit 1 */
  }

}

/**
  * @brief  TIM PWM外设MSP反初始化回调函数
  *         关闭TIM3外设时钟
  * @param  tim_pwmHandle TIM句柄指针
  * @retval None
  */
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* tim_pwmHandle)
{

  if(tim_pwmHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    // 禁用TIM3外设时钟
    __HAL_RCC_TIM3_CLK_DISABLE();
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
