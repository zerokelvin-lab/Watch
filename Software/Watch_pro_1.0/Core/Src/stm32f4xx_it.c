/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   中断服务函数集合——包含Cortex-M4系统异常处理和STM32F4外设中断处理
  *         含用户自定义EXTI中断（按键、充电检测、MPU中断等）
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
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// 用户自定义模块头文件
#include "key.h"      // 按键驱动
#include "mks_142.h"  // MKS_142健康模块驱动
#include "user_TasksInit.h"  // 用户任务初始化
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
// 外部外设句柄引用（在各模块源文件中定义）
extern RTC_HandleTypeDef hrtc;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim1;

/* USER CODE BEGIN EV */
// UART硬件中断接收缓冲区（520字节，通过IDLE中断+DMA接收变长数据，支持二进制帧协议最大帧长）
uint8_t HardInt_receive_str[520];
// UART接收状态: 0=空闲(等待数据), 1=数据就绪(等待任务处理)
volatile uint8_t  HardInt_rx_ready = 0;
// UART接收实际长度（ISR中DMA停止后设置，任务处理后重启DMA）
volatile uint16_t HardInt_rx_len = 0;

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief  NMI不可屏蔽中断处理函数
  *         发生不可屏蔽中断时进入死循环（通常由硬件严重错误触发）
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief  HardFault硬件错误中断处理函数
  *         发生硬件错误（如非法内存访问、总线错误升级等）时进入死循环
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief  内存管理错误中断处理函数（MemManage Fault）
  *         当MPU访问违规或未定义内存区域访问时触发
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief  总线错误中断处理函数（Bus Fault）
  *         当指令预取错误或数据访问总线错误时触发
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief  用法错误中断处理函数（Usage Fault）
  *         当未定义指令、非法状态、除零等操作时触发
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief  调试监视器中断处理函数
  *         用于调试器连接时的断点和监视功能
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief  RTC唤醒定时器中断处理函数
  *         通过EXTI线22触发，调用HAL库RTC唤醒中断处理
  *         用于定时（约1秒）唤醒以刷新时间显示
  */
void RTC_WKUP_IRQHandler(void)
{
  /* USER CODE BEGIN RTC_WKUP_IRQn 0 */
  /* USER CODE END RTC_WKUP_IRQn 0 */
  // 调用HAL库RTC唤醒定时器中断处理
  HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);
  /* USER CODE BEGIN RTC_WKUP_IRQn 1 */

  /* USER CODE END RTC_WKUP_IRQn 1 */
}

/**
  * @brief  TIM1更新中断和TIM10全局中断处理函数
  *         同时处理HAL时基（TIM1）和KEY1长按检测计数
  */
void TIM1_UP_TIM10_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 0 */
  // KEY1长按检测计数（在TIM1的1ms中断中累加）
  static uint16_t key1_long_press;
  if(!KEY1)
  {
    // KEY1按下（低电平有效），计数器累加
    key1_long_press += 1;
  }
  else
  {
    // KEY1释放，清零计数器
    key1_long_press = 0;
  }
  /* USER CODE END TIM1_UP_TIM10_IRQn 0 */
  // 调用HAL库TIM中断处理（驱动HAL时基）
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 1 */

  /* USER CODE END TIM1_UP_TIM10_IRQn 1 */
}

/**
  * @brief  USART1全局中断处理函数
  *         使用UART IDLE空闲中断检测一帧数据接收完成
  *         检测到IDLE标志后发送事件标志通知HardIntEvent任务处理接收数据
  *         停止当前DMA接收并重新启动下一轮DMA接收
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
  // 检查UART IDLE中断标志（一帧数据接收完成后的空闲状态）
  if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE)!=RESET)
  {
    // 读SR+DR清除IDLE和RXNE标志（STM32F4要求先读SR再读DR）
    (void)huart1.Instance->SR;
    (void)huart1.Instance->DR;
    // 仅在DMA活跃时处理（避免DMA已停止时读到错误的NDTR值）
    if(!HardInt_rx_ready)
    {
      // 先读取接收长度（NDTR是剩余计数），再停止DMA
      HardInt_rx_len = 520 - __HAL_DMA_GET_COUNTER(huart1.hdmarx);
      // 在HAL_UART_IRQHandler之前停止DMA，防止HAL看到RXNE后重启DMA
      HAL_UART_DMAStop(&huart1);
      // 手动将UART状态重置为READY（HAL_UART_DMAStop不改State，导致后续HAL_UART_Transmit死等BUSY）
      huart1.gState = HAL_UART_STATE_READY;
      huart1.RxState = HAL_UART_STATE_READY;
      HardInt_rx_ready = 1;
      // 通知硬件中断处理任务有新数据到达
      if(HardIntEventHandle != NULL)
      {
        osEventFlagsSet(HardIntEventHandle, HARDINT_EVENT_UART);
      }
    }
  }
  /* USER CODE END USART1_IRQn 0 */
  // 调用HAL库UART中断处理（此时DMA已停止、RXNE已清除，HAL不会重启DMA）
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief  USART2全局中断处理函数
  *         字节中断接收MKS_142健康模块88字节帧数据
  *         帧格式: [0]=0xFF帧头, [1..64]=PPG波形, [65]=心率, [66]=血氧, [68]=疲劳, ...
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
  static uint8_t mks142_rx_flag = 0;
  static uint8_t mks142_rx_idx = 0;

  if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET)
  {
    uint8_t res = (uint8_t)(huart2.Instance->DR & 0xFF);

    if(res == 0xFF)
    {
      mks142_rx_flag = 1;
      mks142_rx_idx = 0;
    }
    if(mks142_rx_flag)
    {
      mks142_frame_buf[mks142_rx_idx] = res;
      mks142_rx_idx++;
      if(mks142_rx_idx >= MKS142_FRAME_LEN)
      {
        mks142_rx_flag = 0;
        mks142_rx_idx = 0;
        MKS142_ParseFrame(mks142_frame_buf);
        if(HardIntEventHandle != NULL)
        {
          osEventFlagsSet(HardIntEventHandle, HARDINT_EVENT_UART2);
        }
      }
    }
  }
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief  DMA2 Stream2全局中断处理函数
  *         处理SPI1_TX DMA传输完成中断
  */
void DMA2_Stream2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream2_IRQn 0 */

  /* USER CODE END DMA2_Stream2_IRQn 0 */
  // 调用HAL库DMA中断处理（SPI1_TX）
  HAL_DMA_IRQHandler(&hdma_spi1_tx);
  /* USER CODE BEGIN DMA2_Stream2_IRQn 1 */

  /* USER CODE END DMA2_Stream2_IRQn 1 */
}

/**
  * @brief  DMA2 Stream5全局中断处理函数
  *         处理USART1_RX DMA接收中断
  */
void DMA2_Stream5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream5_IRQn 0 */

  /* USER CODE END DMA2_Stream5_IRQn 0 */
  // 调用HAL库DMA中断处理（USART1_RX）
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA2_Stream5_IRQn 1 */

  /* USER CODE END DMA2_Stream5_IRQn 1 */
}

/**
  * @brief  DMA2 Stream7全局中断处理函数
  *         处理USART1_TX DMA发送中断
  */
void DMA2_Stream7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream7_IRQn 0 */

  /* USER CODE END DMA2_Stream7_IRQn 0 */
  // 调用HAL库DMA中断处理（USART1_TX）
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  /* USER CODE BEGIN DMA2_Stream7_IRQn 1 */

  /* USER CODE END DMA2_Stream7_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/**
  * @brief  EXTI0外部中断处理函数
  *         充电检测引脚中断（PA0等），检测到充电器插入/拔出时通知HardIntEvent任务
  */
void EXTI0_IRQHandler(void)
{
  if(HardIntEventHandle != NULL)
  {
    // 发送充电检测事件标志
    osEventFlagsSet(HardIntEventHandle, HARDINT_EVENT_CHARG);
  }

  // 调用HAL库EXTI中断处理，清除中断标志
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);

}

/**
  * @brief  EXTI4外部中断处理函数
  *         KEY2按键中断（PA4等），调用HAL库处理
  */
void EXTI4_IRQHandler(void)
{

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);

}


/**
  * @brief  EXTI线[9:5]外部中断处理函数
  *         KEY1按键中断（PA5等），调用HAL库处理
  */
void EXTI9_5_IRQHandler(void)
{

  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);

}

/**
  * @brief  EXTI线[15:10]外部中断处理函数
  *         MPU（运动传感器）中断（PB12等），检测到运动事件时通知HardIntEvent任务
  */
void EXTI15_10_IRQHandler(void)
{
  if(HardIntEventHandle != NULL)
  {
    // 发送MPU运动检测事件标志
    osEventFlagsSet(HardIntEventHandle, HARDINT_EVENT_MPU);
  }
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);

}

/* USER CODE END 1 */
