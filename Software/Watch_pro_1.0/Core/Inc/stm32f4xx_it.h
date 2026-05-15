/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.h
  * @brief   中断服务函数头文件——声明Cortex-M4系统异常和外设中断的ISR函数原型
  *         包括NMI、HardFault、BusFault等系统异常以及RTC、TIM1、USART1等外设中断
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
#ifndef __STM32F4xx_IT_H
#define __STM32F4xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
// UART硬件中断接收缓冲区（520字节），在其他模块中可extern引用（支持二进制帧协议）
extern uint8_t HardInt_receive_str[520];
// UART接收实际长度（ISR中在重启DMA前设置，任务直接使用）
extern volatile uint16_t HardInt_rx_len;
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
// ====== Cortex-M4 系统异常处理函数声明 ======
void NMI_Handler(void);                      // 不可屏蔽中断
void HardFault_Handler(void);                // 硬件错误
void MemManage_Handler(void);                // 内存管理错误
void BusFault_Handler(void);                 // 总线错误
void UsageFault_Handler(void);               // 用法错误
void DebugMon_Handler(void);                 // 调试监视器
// ====== STM32F4 外设中断处理函数声明 ======
void RTC_WKUP_IRQHandler(void);              // RTC唤醒定时器中断
void TIM1_UP_TIM10_IRQHandler(void);         // TIM1更新中断（HAL时基 + KEY1长按检测）
void USART1_IRQHandler(void);                // USART1全局中断（IDLE空闲中断+DMA接收）
void USART2_IRQHandler(void);                // USART2全局中断（MKS_142字节中断接收）
void DMA2_Stream2_IRQHandler(void);          // DMA2 Stream2中断（SPI1_TX）
void DMA2_Stream5_IRQHandler(void);          // DMA2 Stream5中断（USART1_RX）
void DMA2_Stream7_IRQHandler(void);          // DMA2 Stream7中断（USART1_TX）
/* USER CODE BEGIN EFP */
// ====== 用户自定义EXTI外部中断处理函数声明 ======
void EXTI0_IRQHandler(void);                 // EXTI0中断（PA0充电检测）
void EXTI4_IRQHandler(void);                 // EXTI4中断（KEY2按键）
void EXTI9_5_IRQHandler(void);               // EXTI9_5中断（KEY1按键）
void EXTI15_10_IRQHandler(void);             // EXTI15_10中断（MPU运动传感器）
/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_IT_H */
