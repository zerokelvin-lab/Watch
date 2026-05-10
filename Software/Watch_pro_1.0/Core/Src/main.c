/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : 主程序入口——智能手表固件主函数，负责系统时钟配置、外设初始化、FreeRTOS调度器启动
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
#include "cmsis_os.h"
#include "adc.h"
#include "dma.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

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
// 系统时钟配置函数声明
void SystemClock_Config(void);
// FreeRTOS初始化函数声明
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  应用程序入口函数——主函数
  *         执行流程：HAL初始化 → 时钟配置 → 外设初始化 → FreeRTOS内核启动
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	// 矢量表重映射到Flash地址0x0000C000（与Keil设置保持一致，用于IAP/Bootloader场景）
	SCB->VTOR = 0x00000000U;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  // 复位所有外设，初始化Flash接口和SysTick定时器
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  // 配置系统时钟（HSI+PLL → 100MHz）
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	// 使能全局中断
	__enable_irq();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  // 初始化所有配置的外设模块
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_RTC_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  // 初始化FreeRTOS内核，创建任务和内核对象
  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  // 启动FreeRTOS调度器（此后控制权由调度器接管，不再返回）
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief  系统时钟配置函数
  *         时钟源：HSI (16MHz) → PLL (×100/8/2) → SYSCLK = 100MHz
  *         AHB = 100MHz, APB1 = 50MHz, APB2 = 100MHz
  *         同时使能LSE (32.768kHz) 用于RTC
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  // 使能PWR时钟，配置内核电压为Scale1模式（最高频率）
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  // 配置振荡器：HSI开启（16MHz内部高速时钟），LSE开启（32.768kHz外部低速时钟用于RTC）
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;      // PLL分频因子M：16MHz/8 = 2MHz VCO输入
  RCC_OscInitStruct.PLL.PLLN = 100;    // PLL倍频因子N：2MHz * 100 = 200MHz VCO
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;  // PLL分频因子P：200MHz/2 = 100MHz SYSCLK
  RCC_OscInitStruct.PLL.PLLQ = 4;      // PLL分频因子Q：200MHz/4 = 50MHz (USB/OTG FS等)
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  // 配置系统时钟总线：SYSCLK=PLLCLK, AHB不分频, APB1二分频, APB2不分频
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  定时器周期溢出回调函数（非阻塞模式）
  *         当TIM1产生更新中断时被HAL_TIM_IRQHandler()调用，
  *         以此驱动HAL库的SysTick时基（替代默认的SysTick中断）
  * @param  htim 定时器句柄指针
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  // 检查是否为TIM1的中断，若是则递增HAL系统滴答计数器uwTick
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  错误处理函数——当HAL库函数返回错误时调用
  *         关闭全局中断后进入死循环，阻止程序继续执行
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  // 关闭所有中断，进入死循环等待看门狗复位或调试器干预
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  断言失败处理函数——报告发生断言错误的源文件名和行号
  * @param  file 源文件名指针
  * @param  line 断言错误所在行号
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
