/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_hal_timebase_TIM.c
  * @brief   使用TIM1替代SysTick作为HAL库时基——提供1ms精确定时基准
  *         TIM1时钟源为APB2（100MHz），预分频后计数器频率1MHz，周期1000实现1ms中断
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
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// TIM1定时器句柄，用于HAL系统时基
TIM_HandleTypeDef        htim1;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  配置TIM1作为HAL库时基源，替代SysTick
  *         预分频后时钟=1MHz，自动重装载值=999，产生1ms周期中断
  *         此函数在HAL_Init()中自动调用，或在时钟配置变更时调用
  * @param  TickPriority 滴答中断优先级
  * @retval HAL状态 (HAL_OK/HAL_ERROR)
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  RCC_ClkInitTypeDef    clkconfig;
  uint32_t              uwTimclock = 0U;

  uint32_t              uwPrescalerValue = 0U;
  uint32_t              pFLatency;
  HAL_StatusTypeDef     status;

  /* Enable TIM1 clock */
  // 使能TIM1时钟
  __HAL_RCC_TIM1_CLK_ENABLE();

  /* Get clock configuration */
  // 获取当前时钟配置
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  /* Compute TIM1 clock */
  // 获取APB2定时器时钟频率（TIM1挂载在APB2总线上）
  uwTimclock = HAL_RCC_GetPCLK2Freq();

  /* Compute the prescaler value to have TIM1 counter clock equal to 1MHz */
  // 计算预分频值使得计数器频率 = 1MHz (uwTimclock / 1000000 - 1)
  uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000U) - 1U);

  /* Initialize TIM1 */
  htim1.Instance = TIM1;

  /* Initialize TIMx peripheral as follow:
  + Period = [(TIM1CLK/1000) - 1]. to have a (1/1000) s time base.
  + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
  + ClockDivision = 0
  + Counter direction = Up
  */
  // 配置TIM1：向上计数模式，不自动重装载预装载
  // 周期 = 1MHz/1000 - 1 = 999，即每1ms溢出一次
  htim1.Init.Period = (1000000U / 1000U) - 1U;
  htim1.Init.Prescaler = uwPrescalerValue;
  htim1.Init.ClockDivision = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  status = HAL_TIM_Base_Init(&htim1);
  if (status == HAL_OK)
  {
    /* Start the TIM time Base generation in interrupt mode */
    // 以中断模式启动TIM1时基
    status = HAL_TIM_Base_Start_IT(&htim1);
    if (status == HAL_OK)
    {
    /* Enable the TIM1 global Interrupt */
        // 使能TIM1更新中断（在NVIC中）
        HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
      /* Configure the SysTick IRQ priority */
      if (TickPriority < (1UL << __NVIC_PRIO_BITS))
      {
        /* Configure the TIM IRQ priority */
        // 配置TIM1中断优先级
        HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, TickPriority, 0U);
        uwTickPrio = TickPriority;
      }
      else
      {
        status = HAL_ERROR;
      }
    }
  }

 /* Return function status */
  return status;
}

/**
  * @brief  暂停滴答增量——通过禁用TIM1更新中断来暂停HAL时基计数
  *         用于临界区保护或低功耗模式前的时序暂停
  * @param  None
  * @retval None
  */
void HAL_SuspendTick(void)
{
  /* Disable TIM1 update Interrupt */
  // 禁用TIM1更新中断，暂停uwTick递增
  __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);
}

/**
  * @brief  恢复滴答增量——重新使能TIM1更新中断以恢复HAL时基计数
  * @param  None
  * @retval None
  */
void HAL_ResumeTick(void)
{
  /* Enable TIM1 Update interrupt */
  // 使能TIM1更新中断，恢复uwTick递增
  __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
}
