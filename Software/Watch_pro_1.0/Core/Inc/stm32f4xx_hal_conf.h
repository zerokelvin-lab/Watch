/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_hal_conf.h
  * @brief   STM32F4 HAL库配置文件——选择使能的外设模块、时钟源参数、系统配置
  *         本项目启用的模块：ADC、RTC、SPI、TIM、UART、GPIO、EXTI、DMA、RCC、FLASH、PWR、CORTEX
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
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
#ifndef __STM32F4xx_HAL_CONF_H
#define __STM32F4xx_HAL_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* ########################## Module Selection ############################## */
/**
  * @brief HAL库模块选择——定义需要启用的外设驱动模块
  *        注释掉的模块不参与编译，可减小固件体积
  */
// HAL总模块使能（必须启用）
#define HAL_MODULE_ENABLED

  // ====== 启用的外设模块 ======
  #define HAL_ADC_MODULE_ENABLED       // ADC模数转换模块（电池电压采集）
/* #define HAL_CRYP_MODULE_ENABLED   */
/* #define HAL_CAN_MODULE_ENABLED   */
/* #define HAL_CRC_MODULE_ENABLED   */
/* #define HAL_CAN_LEGACY_MODULE_ENABLED   */
/* #define HAL_CRYP_MODULE_ENABLED   */
/* #define HAL_DAC_MODULE_ENABLED   */
/* #define HAL_DCMI_MODULE_ENABLED   */
/* #define HAL_DMA2D_MODULE_ENABLED   */
/* #define HAL_ETH_MODULE_ENABLED   */
/* #define HAL_NAND_MODULE_ENABLED   */
/* #define HAL_NOR_MODULE_ENABLED   */
/* #define HAL_PCCARD_MODULE_ENABLED   */
/* #define HAL_SRAM_MODULE_ENABLED   */
/* #define HAL_SDRAM_MODULE_ENABLED   */
/* #define HAL_HASH_MODULE_ENABLED   */
/* #define HAL_I2C_MODULE_ENABLED   */
/* #define HAL_I2S_MODULE_ENABLED   */
/* #define HAL_IWDG_MODULE_ENABLED   */
/* #define HAL_LTDC_MODULE_ENABLED   */
/* #define HAL_RNG_MODULE_ENABLED   */
  #define HAL_RTC_MODULE_ENABLED       // RTC实时时钟模块
/* #define HAL_SAI_MODULE_ENABLED   */
/* #define HAL_SD_MODULE_ENABLED   */
/* #define HAL_MMC_MODULE_ENABLED   */
  #define HAL_SPI_MODULE_ENABLED       // SPI串行外设接口模块（显示屏驱动）
  #define HAL_TIM_MODULE_ENABLED       // TIM定时器模块（PWM背光 + HAL时基）
  #define HAL_UART_MODULE_ENABLED      // UART串口模块（调试输出 + 通信）
/* #define HAL_USART_MODULE_ENABLED   */
/* #define HAL_IRDA_MODULE_ENABLED   */
/* #define HAL_SMARTCARD_MODULE_ENABLED   */
/* #define HAL_SMBUS_MODULE_ENABLED   */
/* #define HAL_WWDG_MODULE_ENABLED   */
/* #define HAL_PCD_MODULE_ENABLED   */
/* #define HAL_HCD_MODULE_ENABLED   */
/* #define HAL_DSI_MODULE_ENABLED   */
/* #define HAL_QSPI_MODULE_ENABLED   */
/* #define HAL_QSPI_MODULE_ENABLED   */
/* #define HAL_CEC_MODULE_ENABLED   */
/* #define HAL_FMPI2C_MODULE_ENABLED   */
/* #define HAL_FMPSMBUS_MODULE_ENABLED   */
/* #define HAL_SPDIFRX_MODULE_ENABLED   */
/* #define HAL_DFSDM_MODULE_ENABLED   */
/* #define HAL_LPTIM_MODULE_ENABLED   */
  // ====== 核心基础模块（必须使能） ======
  #define HAL_GPIO_MODULE_ENABLED      // GPIO通用输入输出模块
  #define HAL_EXTI_MODULE_ENABLED      // EXTI外部中断模块（按键、充电、MPU中断）
  #define HAL_DMA_MODULE_ENABLED       // DMA直接存储器访问模块
  #define HAL_RCC_MODULE_ENABLED       // RCC复位与时钟控制模块
  #define HAL_FLASH_MODULE_ENABLED     // Flash闪存模块
  #define HAL_PWR_MODULE_ENABLED       // PWR电源控制模块
  #define HAL_CORTEX_MODULE_ENABLED    // Cortex-M4内核模块（NVIC/SysTick等）

/* ########################## HSE/HSI Values adaptation ##################### */
/**
  * @brief 外部高速振荡器/内部高速振荡器频率配置
  *        本项目使用HSI(16MHz)作为主时钟源，HSE(25MHz)未使用但保留默认值
  */
#if !defined  (HSE_VALUE)
  #define HSE_VALUE    25000000U /*!< 外部高速振荡器频率（Hz），默认25MHz */
#endif /* HSE_VALUE */

#if !defined  (HSE_STARTUP_TIMEOUT)
  #define HSE_STARTUP_TIMEOUT    100U   /*!< HSE启动超时时间（ms） */
#endif /* HSE_STARTUP_TIMEOUT */

/**
  * @brief Internal High Speed oscillator (HSI) value.
  *        This value is used by the RCC HAL module to compute the system frequency
  *        (when HSI is used as system clock source, directly or through the PLL).
  */
#if !defined  (HSI_VALUE)
  #define HSI_VALUE    ((uint32_t)16000000U) /*!< 内部高速振荡器频率（Hz），16MHz */
#endif /* HSI_VALUE */

/**
  * @brief Internal Low Speed oscillator (LSI) value.
  */
#if !defined  (LSI_VALUE)
 #define LSI_VALUE  32000U       /*!< 内部低速振荡器典型值（Hz），约32kHz */
#endif /* LSI_VALUE */                      /*!< 实际值受电压和温度影响 */
/**
  * @brief External Low Speed oscillator (LSE) value.
  */
#if !defined  (LSE_VALUE)
 #define LSE_VALUE  32768U    /*!< 外部低速振荡器频率（Hz），32.768kHz（用于RTC） */
#endif /* LSE_VALUE */

#if !defined  (LSE_STARTUP_TIMEOUT)
  #define LSE_STARTUP_TIMEOUT    5000U   /*!< LSE启动超时时间（ms） */
#endif /* LSE_STARTUP_TIMEOUT */

/**
  * @brief External clock source for I2S peripheral
  *        This value is used by the I2S HAL module to compute the I2S clock source
  *        frequency, this source is inserted directly through I2S_CKIN pad.
  */
#if !defined  (EXTERNAL_CLOCK_VALUE)
  #define EXTERNAL_CLOCK_VALUE    12288000U /*!< 外部音频时钟频率（Hz） */
#endif /* EXTERNAL_CLOCK_VALUE */

/* ########################### System Configuration ######################### */
/**
  * @brief HAL系统配置——内核电压、滴答优先级、RTOS开关、缓存使能
  */
// 系统电压配置
#define  VDD_VALUE		      3300U        /*!< VDD供电电压（mV），3.3V */
#define  TICK_INT_PRIORITY            15U   /*!< 滴答中断优先级（最低），TIM1替代SysTick后此值仍保留 */
#define  USE_RTOS                     0U    /*!< RTOS使用标志（0 = 未使用RTOS时基，实际使用FreeRTOS） */
#define  PREFETCH_ENABLE              1U    /*!< 使能Flash预取 */  
#define  INSTRUCTION_CACHE_ENABLE     1U    /*!< 使能指令缓存 */
#define  DATA_CACHE_ENABLE            1U    /*!< 使能数据缓存 */

// 外设回调函数注册功能统一禁用（减小代码体积，使用默认弱回调）
#define  USE_HAL_ADC_REGISTER_CALLBACKS         0U /* ADC回调注册禁用 */
#define  USE_HAL_CAN_REGISTER_CALLBACKS         0U /* CAN回调注册禁用 */
#define  USE_HAL_CEC_REGISTER_CALLBACKS         0U /* CEC回调注册禁用 */
#define  USE_HAL_CRYP_REGISTER_CALLBACKS        0U /* CRYP回调注册禁用 */
#define  USE_HAL_DAC_REGISTER_CALLBACKS         0U /* DAC回调注册禁用 */
#define  USE_HAL_DCMI_REGISTER_CALLBACKS        0U /* DCMI回调注册禁用 */
#define  USE_HAL_DFSDM_REGISTER_CALLBACKS       0U /* DFSDM回调注册禁用 */
#define  USE_HAL_DMA2D_REGISTER_CALLBACKS       0U /* DMA2D回调注册禁用 */
#define  USE_HAL_DSI_REGISTER_CALLBACKS         0U /* DSI回调注册禁用 */
#define  USE_HAL_ETH_REGISTER_CALLBACKS         0U /* ETH回调注册禁用 */
#define  USE_HAL_HASH_REGISTER_CALLBACKS        0U /* HASH回调注册禁用 */
#define  USE_HAL_HCD_REGISTER_CALLBACKS         0U /* HCD回调注册禁用 */
#define  USE_HAL_I2C_REGISTER_CALLBACKS         0U /* I2C回调注册禁用 */
#define  USE_HAL_FMPI2C_REGISTER_CALLBACKS      0U /* FMPI2C回调注册禁用 */
#define  USE_HAL_FMPSMBUS_REGISTER_CALLBACKS    0U /* FMPSMBUS回调注册禁用 */
#define  USE_HAL_I2S_REGISTER_CALLBACKS         0U /* I2S回调注册禁用 */
#define  USE_HAL_IRDA_REGISTER_CALLBACKS        0U /* IRDA回调注册禁用 */
#define  USE_HAL_LPTIM_REGISTER_CALLBACKS       0U /* LPTIM回调注册禁用 */
#define  USE_HAL_LTDC_REGISTER_CALLBACKS        0U /* LTDC回调注册禁用 */
#define  USE_HAL_MMC_REGISTER_CALLBACKS         0U /* MMC回调注册禁用 */
#define  USE_HAL_NAND_REGISTER_CALLBACKS        0U /* NAND回调注册禁用 */
#define  USE_HAL_NOR_REGISTER_CALLBACKS         0U /* NOR回调注册禁用 */
#define  USE_HAL_PCCARD_REGISTER_CALLBACKS      0U /* PCCARD回调注册禁用 */
#define  USE_HAL_PCD_REGISTER_CALLBACKS         0U /* PCD回调注册禁用 */
#define  USE_HAL_QSPI_REGISTER_CALLBACKS        0U /* QSPI回调注册禁用 */
#define  USE_HAL_RNG_REGISTER_CALLBACKS         0U /* RNG回调注册禁用 */
#define  USE_HAL_RTC_REGISTER_CALLBACKS         0U /* RTC回调注册禁用 */
#define  USE_HAL_SAI_REGISTER_CALLBACKS         0U /* SAI回调注册禁用 */
#define  USE_HAL_SD_REGISTER_CALLBACKS          0U /* SD回调注册禁用 */
#define  USE_HAL_SMARTCARD_REGISTER_CALLBACKS   0U /* SMARTCARD回调注册禁用 */
#define  USE_HAL_SDRAM_REGISTER_CALLBACKS       0U /* SDRAM回调注册禁用 */
#define  USE_HAL_SRAM_REGISTER_CALLBACKS        0U /* SRAM回调注册禁用 */
#define  USE_HAL_SPDIFRX_REGISTER_CALLBACKS     0U /* SPDIFRX回调注册禁用 */
#define  USE_HAL_SMBUS_REGISTER_CALLBACKS       0U /* SMBUS回调注册禁用 */
#define  USE_HAL_SPI_REGISTER_CALLBACKS         0U /* SPI回调注册禁用 */
#define  USE_HAL_TIM_REGISTER_CALLBACKS         0U /* TIM回调注册禁用 */
#define  USE_HAL_UART_REGISTER_CALLBACKS        0U /* UART回调注册禁用 */
#define  USE_HAL_USART_REGISTER_CALLBACKS       0U /* USART回调注册禁用 */
#define  USE_HAL_WWDG_REGISTER_CALLBACKS        0U /* WWDG回调注册禁用 */

/* ########################## Assert Selection ############################## */
/**
  * @brief 断言功能选择——取消注释以启用HAL库内部参数检查（调试用）
  */
/* #define USE_FULL_ASSERT    1U */

/* ################## Ethernet peripheral configuration ##################### */

/* Section 1 : Ethernet peripheral configuration */
// 以太网配置（本项目未使用）

/* MAC ADDRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
#define MAC_ADDR0   2U
#define MAC_ADDR1   0U
#define MAC_ADDR2   0U
#define MAC_ADDR3   0U
#define MAC_ADDR4   0U
#define MAC_ADDR5   0U

/* Definition of the Ethernet driver buffers size and count */
#define ETH_RX_BUF_SIZE                ETH_MAX_PACKET_SIZE /* buffer size for receive               */
#define ETH_TX_BUF_SIZE                ETH_MAX_PACKET_SIZE /* buffer size for transmit              */
#define ETH_RXBUFNB                    4U       /* 4 Rx buffers of size ETH_RX_BUF_SIZE  */
#define ETH_TXBUFNB                    4U       /* 4 Tx buffers of size ETH_TX_BUF_SIZE  */

/* Section 2: PHY configuration section */

/* DP83848_PHY_ADDRESS Address*/
#define DP83848_PHY_ADDRESS           0x01U
/* PHY Reset delay these values are based on a 1 ms Systick interrupt*/
#define PHY_RESET_DELAY                 0x000000FFU
/* PHY Configuration delay */
#define PHY_CONFIG_DELAY                0x00000FFFU

#define PHY_READ_TO                     0x0000FFFFU
#define PHY_WRITE_TO                    0x0000FFFFU

/* Section 3: Common PHY Registers */

#define PHY_BCR                         ((uint16_t)0x0000U)    /*!< Transceiver Basic Control Register   */
#define PHY_BSR                         ((uint16_t)0x0001U)    /*!< Transceiver Basic Status Register    */

#define PHY_RESET                       ((uint16_t)0x8000U)  /*!< PHY Reset */
#define PHY_LOOPBACK                    ((uint16_t)0x4000U)  /*!< Select loop-back mode */
#define PHY_FULLDUPLEX_100M             ((uint16_t)0x2100U)  /*!< Set the full-duplex mode at 100 Mb/s */
#define PHY_HALFDUPLEX_100M             ((uint16_t)0x2000U)  /*!< Set the half-duplex mode at 100 Mb/s */
#define PHY_FULLDUPLEX_10M              ((uint16_t)0x0100U)  /*!< Set the full-duplex mode at 10 Mb/s  */
#define PHY_HALFDUPLEX_10M              ((uint16_t)0x0000U)  /*!< Set the half-duplex mode at 10 Mb/s  */
#define PHY_AUTONEGOTIATION             ((uint16_t)0x1000U)  /*!< Enable auto-negotiation function     */
#define PHY_RESTART_AUTONEGOTIATION     ((uint16_t)0x0200U)  /*!< Restart auto-negotiation function    */
#define PHY_POWERDOWN                   ((uint16_t)0x0800U)  /*!< Select the power down mode           */
#define PHY_ISOLATE                     ((uint16_t)0x0400U)  /*!< Isolate PHY from MII                 */

#define PHY_AUTONEGO_COMPLETE           ((uint16_t)0x0020U)  /*!< Auto-Negotiation process completed   */
#define PHY_LINKED_STATUS               ((uint16_t)0x0004U)  /*!< Valid link established               */
#define PHY_JABBER_DETECTION            ((uint16_t)0x0002U)  /*!< Jabber condition detected            */

/* Section 4: Extended PHY Registers */
#define PHY_SR                          ((uint16_t)0x10U)    /*!< PHY status register Offset                      */

#define PHY_SPEED_STATUS                ((uint16_t)0x0002U)  /*!< PHY Speed mask                                  */
#define PHY_DUPLEX_STATUS               ((uint16_t)0x0004U)  /*!< PHY Duplex mask                                 */

/* ################## SPI peripheral configuration ########################## */

/* CRC FEATURE: Use to activate CRC feature inside HAL SPI Driver
* Activated: CRC code is present inside driver
* Deactivated: CRC code cleaned from driver
*/
// SPI CRC功能禁用（本项目不使用SPI CRC校验）
#define USE_SPI_CRC                     0U

/* Includes ------------------------------------------------------------------*/
/**
  * @brief 根据模块使能条件包含对应的HAL驱动头文件
  */

#ifdef HAL_RCC_MODULE_ENABLED
  #include "stm32f4xx_hal_rcc.h"
#endif /* HAL_RCC_MODULE_ENABLED */

#ifdef HAL_GPIO_MODULE_ENABLED
  #include "stm32f4xx_hal_gpio.h"
#endif /* HAL_GPIO_MODULE_ENABLED */

#ifdef HAL_EXTI_MODULE_ENABLED
  #include "stm32f4xx_hal_exti.h"
#endif /* HAL_EXTI_MODULE_ENABLED */

#ifdef HAL_DMA_MODULE_ENABLED
  #include "stm32f4xx_hal_dma.h"
#endif /* HAL_DMA_MODULE_ENABLED */

#ifdef HAL_CORTEX_MODULE_ENABLED
  #include "stm32f4xx_hal_cortex.h"
#endif /* HAL_CORTEX_MODULE_ENABLED */

#ifdef HAL_ADC_MODULE_ENABLED
  #include "stm32f4xx_hal_adc.h"
#endif /* HAL_ADC_MODULE_ENABLED */

#ifdef HAL_CAN_MODULE_ENABLED
  #include "stm32f4xx_hal_can.h"
#endif /* HAL_CAN_MODULE_ENABLED */

#ifdef HAL_CAN_LEGACY_MODULE_ENABLED
  #include "stm32f4xx_hal_can_legacy.h"
#endif /* HAL_CAN_LEGACY_MODULE_ENABLED */

#ifdef HAL_CRC_MODULE_ENABLED
  #include "stm32f4xx_hal_crc.h"
#endif /* HAL_CRC_MODULE_ENABLED */

#ifdef HAL_CRYP_MODULE_ENABLED
  #include "stm32f4xx_hal_cryp.h"
#endif /* HAL_CRYP_MODULE_ENABLED */

#ifdef HAL_DMA2D_MODULE_ENABLED
  #include "stm32f4xx_hal_dma2d.h"
#endif /* HAL_DMA2D_MODULE_ENABLED */

#ifdef HAL_DAC_MODULE_ENABLED
  #include "stm32f4xx_hal_dac.h"
#endif /* HAL_DAC_MODULE_ENABLED */

#ifdef HAL_DCMI_MODULE_ENABLED
  #include "stm32f4xx_hal_dcmi.h"
#endif /* HAL_DCMI_MODULE_ENABLED */

#ifdef HAL_ETH_MODULE_ENABLED
  #include "stm32f4xx_hal_eth.h"
#endif /* HAL_ETH_MODULE_ENABLED */

#ifdef HAL_FLASH_MODULE_ENABLED
  #include "stm32f4xx_hal_flash.h"
#endif /* HAL_FLASH_MODULE_ENABLED */

#ifdef HAL_SRAM_MODULE_ENABLED
  #include "stm32f4xx_hal_sram.h"
#endif /* HAL_SRAM_MODULE_ENABLED */

#ifdef HAL_NOR_MODULE_ENABLED
  #include "stm32f4xx_hal_nor.h"
#endif /* HAL_NOR_MODULE_ENABLED */

#ifdef HAL_NAND_MODULE_ENABLED
  #include "stm32f4xx_hal_nand.h"
#endif /* HAL_NAND_MODULE_ENABLED */

#ifdef HAL_PCCARD_MODULE_ENABLED
  #include "stm32f4xx_hal_pccard.h"
#endif /* HAL_PCCARD_MODULE_ENABLED */

#ifdef HAL_SDRAM_MODULE_ENABLED
  #include "stm32f4xx_hal_sdram.h"
#endif /* HAL_SDRAM_MODULE_ENABLED */

#ifdef HAL_HASH_MODULE_ENABLED
 #include "stm32f4xx_hal_hash.h"
#endif /* HAL_HASH_MODULE_ENABLED */

#ifdef HAL_I2C_MODULE_ENABLED
 #include "stm32f4xx_hal_i2c.h"
#endif /* HAL_I2C_MODULE_ENABLED */

#ifdef HAL_SMBUS_MODULE_ENABLED
 #include "stm32f4xx_hal_smbus.h"
#endif /* HAL_SMBUS_MODULE_ENABLED */

#ifdef HAL_I2S_MODULE_ENABLED
 #include "stm32f4xx_hal_i2s.h"
#endif /* HAL_I2S_MODULE_ENABLED */

#ifdef HAL_IWDG_MODULE_ENABLED
 #include "stm32f4xx_hal_iwdg.h"
#endif /* HAL_IWDG_MODULE_ENABLED */

#ifdef HAL_LTDC_MODULE_ENABLED
 #include "stm32f4xx_hal_ltdc.h"
#endif /* HAL_LTDC_MODULE_ENABLED */

#ifdef HAL_PWR_MODULE_ENABLED
 #include "stm32f4xx_hal_pwr.h"
#endif /* HAL_PWR_MODULE_ENABLED */

#ifdef HAL_RNG_MODULE_ENABLED
 #include "stm32f4xx_hal_rng.h"
#endif /* HAL_RNG_MODULE_ENABLED */

#ifdef HAL_RTC_MODULE_ENABLED
 #include "stm32f4xx_hal_rtc.h"
#endif /* HAL_RTC_MODULE_ENABLED */

#ifdef HAL_SAI_MODULE_ENABLED
 #include "stm32f4xx_hal_sai.h"
#endif /* HAL_SAI_MODULE_ENABLED */

#ifdef HAL_SD_MODULE_ENABLED
 #include "stm32f4xx_hal_sd.h"
#endif /* HAL_SD_MODULE_ENABLED */

#ifdef HAL_SPI_MODULE_ENABLED
 #include "stm32f4xx_hal_spi.h"
#endif /* HAL_SPI_MODULE_ENABLED */

#ifdef HAL_TIM_MODULE_ENABLED
 #include "stm32f4xx_hal_tim.h"
#endif /* HAL_TIM_MODULE_ENABLED */

#ifdef HAL_UART_MODULE_ENABLED
 #include "stm32f4xx_hal_uart.h"
#endif /* HAL_UART_MODULE_ENABLED */

#ifdef HAL_USART_MODULE_ENABLED
 #include "stm32f4xx_hal_usart.h"
#endif /* HAL_USART_MODULE_ENABLED */

#ifdef HAL_IRDA_MODULE_ENABLED
 #include "stm32f4xx_hal_irda.h"
#endif /* HAL_IRDA_MODULE_ENABLED */

#ifdef HAL_SMARTCARD_MODULE_ENABLED
 #include "stm32f4xx_hal_smartcard.h"
#endif /* HAL_SMARTCARD_MODULE_ENABLED */

#ifdef HAL_WWDG_MODULE_ENABLED
 #include "stm32f4xx_hal_wwdg.h"
#endif /* HAL_WWDG_MODULE_ENABLED */

#ifdef HAL_PCD_MODULE_ENABLED
 #include "stm32f4xx_hal_pcd.h"
#endif /* HAL_PCD_MODULE_ENABLED */

#ifdef HAL_HCD_MODULE_ENABLED
 #include "stm32f4xx_hal_hcd.h"
#endif /* HAL_HCD_MODULE_ENABLED */

#ifdef HAL_DSI_MODULE_ENABLED
 #include "stm32f4xx_hal_dsi.h"
#endif /* HAL_DSI_MODULE_ENABLED */

#ifdef HAL_QSPI_MODULE_ENABLED
 #include "stm32f4xx_hal_qspi.h"
#endif /* HAL_QSPI_MODULE_ENABLED */

#ifdef HAL_CEC_MODULE_ENABLED
 #include "stm32f4xx_hal_cec.h"
#endif /* HAL_CEC_MODULE_ENABLED */

#ifdef HAL_FMPI2C_MODULE_ENABLED
 #include "stm32f4xx_hal_fmpi2c.h"
#endif /* HAL_FMPI2C_MODULE_ENABLED */

#ifdef HAL_FMPSMBUS_MODULE_ENABLED
 #include "stm32f4xx_hal_fmpsmbus.h"
#endif /* HAL_FMPSMBUS_MODULE_ENABLED */

#ifdef HAL_SPDIFRX_MODULE_ENABLED
 #include "stm32f4xx_hal_spdifrx.h"
#endif /* HAL_SPDIFRX_MODULE_ENABLED */

#ifdef HAL_DFSDM_MODULE_ENABLED
 #include "stm32f4xx_hal_dfsdm.h"
#endif /* HAL_DFSDM_MODULE_ENABLED */

#ifdef HAL_LPTIM_MODULE_ENABLED
 #include "stm32f4xx_hal_lptim.h"
#endif /* HAL_LPTIM_MODULE_ENABLED */

#ifdef HAL_MMC_MODULE_ENABLED
 #include "stm32f4xx_hal_mmc.h"
#endif /* HAL_MMC_MODULE_ENABLED */

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
/**
  * @brief  assert_param宏用于函数参数检查
  * @param  expr 若表达式为false，调用assert_failed报告源文件名和行号
  *         若为true，无返回值
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_CONF_H */
