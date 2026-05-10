/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   RTC实时时钟配置——LSE时钟源(32.768kHz)，24小时制，带备份寄存器判断首次初始化
  *         提供RTC_SetTime/RTC_SetDate自定义接口和星期计算函数
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
#include "rtc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

// RTC外设句柄（全局变量）
RTC_HandleTypeDef hrtc;

/* RTC init function */
/**
  * @brief  RTC初始化函数
  *         使用LSE作为时钟源，配置24小时制、异步预分频127、同步预分频255
  *         通过备份寄存器RTC_BKP_DR0判断是否首次上电，首次上电时设置初始时间
  *         并使能RTC唤醒定时器（2秒周期，用于定时更新时间显示）
  * @retval None
  */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  // 时间和日期配置结构体
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  // 配置RTC基本参数：24小时制，异步预分频值127，同步预分频值255
  // RTC时钟频率 = LSE / (127+1) / (255+1) = 32768 / 128 / 256 = 1Hz
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
	// 检查备份寄存器是否为0x5050标记值，判断是否已经初始化过RTC
	if(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR0)!=0X5050)//是否第一次配置
  {
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  // 首次上电：设置初始时间 11:59:55
  sTime.Hours = 0x11;
  sTime.Minutes = 0x59;
  sTime.Seconds = 0x55;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  // 设置初始日期：2023年5月21日 星期日
  sDate.WeekDay = RTC_WEEKDAY_SUNDAY;
  sDate.Month = RTC_MONTH_MAY;
  sDate.Date = 0x21;
  sDate.Year = 0x23;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the WakeUp
  */
  // 使能RTC唤醒定时器，周期2000个RTC时钟分频后 ≈ 1秒（用于定时刷新时间显示）
  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 2000, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
	// 写入备份寄存器标记0x5050，表示RTC已初始化，下次上电不再重新设置时间
	HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0,0X5050);//标记已经初始化过
  }
  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief  RTC外设MSP初始化回调函数
  *         配置RTC时钟源为LSE（32.768kHz外部晶振），使能RTC唤醒中断
  * @param  rtcHandle RTC句柄指针
  * @retval None
  */
void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */

  /** Initializes the peripherals clock
  */
    // 配置RTC外设时钟源为LSE（外部32.768kHz低速晶振）
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* RTC clock enable */
    // 使能RTC时钟
    __HAL_RCC_RTC_ENABLE();

    /* RTC interrupt Init */
    // 配置RTC唤醒中断，优先级5
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

/**
  * @brief  RTC外设MSP反初始化回调函数
  *         关闭RTC时钟和唤醒中断
  * @param  rtcHandle RTC句柄指针
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    // 禁用RTC外设时钟
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    // 禁用RTC唤醒中断
    HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/**
  * @brief  星期计算函数（蔡勒公式变体）
  *         根据年、月、日、世纪计算对应的星期几
  * @param  y 年份（后两位，如2023年传入23）
  * @param  m 月份（1-12）
  * @param  d 日期（1-31）
  * @param  c 世纪（年份前两位，如2023年传入20）
  * @return 星期值（1=星期一 ... 7=星期日）
  */
uint8_t weekday_calculate(int y,int m,int d,int c)
{
	int w;
	if (m == 1 || m == 2)
	{y--, m += 12;}
	w = y + y / 4 + c / 4  + 26*(m + 1)/10 + d - 1 - 2 * c;
	while(w<0)
		w+=7;
	w%=7;
	w=(w==0)?7:w;
	return w;
}

/**
  * @brief  设置RTC时间（二进制格式）
  * @param  hours 小时（0-23）
  * @param  minutes 分钟（0-59）
  * @param  seconds 秒（0-59）
  * @retval None
  */
void RTC_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	RTC_TimeTypeDef time={0};//结构体初始化为{0}时，部分字段会使用默认值
	time.Hours=hours;
	time.Minutes=minutes;
	time.Seconds=seconds;
	if (HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  设置RTC日期（二进制格式），自动计算星期
  * @param  year 年份（后两位，如2023年传入23）
  * @param  month 月份（1-12）
  * @param  date 日期（1-31）
  * @retval None
  */
void RTC_SetDate(uint8_t year, uint8_t month, uint8_t date)
{
	RTC_DateTypeDef setdate={0};//结构体初始化为{0}时，部分字段会使用默认值
	setdate.Date=date;
	setdate.Month=month;
	setdate.Year=year;

	//culculat the weekday
	// 根据年月日自动计算星期几
	setdate.WeekDay = weekday_calculate(year,month,date,20);

	if (HAL_RTC_SetDate(&hrtc, &setdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
}
/* USER CODE END 1 */
