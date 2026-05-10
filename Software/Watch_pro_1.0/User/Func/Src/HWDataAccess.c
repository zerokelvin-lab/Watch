/** @file HWDataAccess.c
 *  @brief 硬件中间层实现——封装BSP和STM32 HAL库的硬件访问接口，为UI和应用层提供统一的硬件操作函数
 */

/**
 *  @addtogroup  Hareware Middle Layer
 *  @brief       Hardware Middle Layer, to access data from BSP and STM32 HAL Library
 *
 *  @{
 *      @file       HWDataAccess.c
 *      @brief      middleware, for UI and APP Layer to get the hardware data
 *      @details    you can enable or disable in .h file.
 * 					加这个文件就是为了方便UI移植, 比如你要把工程移植到PC仿真,
 *   				直接把MidFunc中的文件和UI文件都复制过去,
 * 					然后直接把.h文件中的HW_USE_HARDWARE变成0就行了.
 */

#include "../Inc/HWDataAccess.h"


/* ====== RTC 实时时钟功能 ====== */

/**
 * @brief  从硬件获取当前日期和时间
 * @param  nowdatetime 存储日期时间的结构体指针
 * @return 无
 */
void HW_RTC_Get_TimeDate(HW_DateTimeTypeDef * nowdatetime)
{

	#if HW_USE_RTC
		if (nowdatetime != NULL)
		{
            RTC_DateTypeDef nowdate;
            RTC_TimeTypeDef nowtime;
			HAL_RTC_GetTime(&hrtc, &nowtime, RTC_FORMAT_BIN);    // 从硬件RTC读取当前时间
			HAL_RTC_GetDate(&hrtc, &nowdate, RTC_FORMAT_BIN);    // 从硬件RTC读取当前日期
            nowdatetime->Year = nowdate.Year;
            nowdatetime->Month = nowdate.Month;
            nowdatetime->Date = nowdate.Date;
			nowdatetime->WeekDay = weekday_calculate(nowdatetime->Year, nowdatetime->Month, nowdatetime->Date, 20); // 根据日期计算星期几
            nowdatetime->Hours = nowtime.Hours;
            nowdatetime->Minutes = nowtime.Minutes;
            nowdatetime->Seconds = nowtime.Seconds;
		}
	#else
		// 不使用硬件时，返回固定的模拟时间（用于PC仿真测试）
		nowdatetime->Year = 24;
		nowdatetime->Month = 6;
		nowdatetime->Date = 23;
		nowdatetime->WeekDay = 7;
		nowdatetime->Hours = 11;
		nowdatetime->Minutes = 59;
		nowdatetime->Seconds = 55;
	#endif
}


/**
 * @brief  设置RTC硬件日期
 * @param  year   年份（0-99）
 * @param  month  月份（1-12）
 * @param  date   日期（1-31）
 * @return 无
 */
void HW_RTC_Set_Date(uint8_t year, uint8_t month, uint8_t date)
{
	#if HW_USE_RTC
		RTC_SetDate(year, month, date);
	#endif
}

/**
 * @brief  设置RTC硬件时间
 * @param  hours   小时（0-23）
 * @param  minutes 分钟（0-59）
 * @param  seconds 秒钟（0-59）
 * @return 无
 */
void HW_RTC_Set_Time(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	#if HW_USE_RTC
		RTC_SetTime(hours, minutes, seconds);
	#endif
}

/**
 * @brief  计算给定日期是星期几（蔡勒公式变体）
 * @param  setyear  年份后两位
 * @param  setmonth 月份
 * @param  setday   日期
 * @param  century  世纪数（如2024年传20）
 * @return 星期数（1-7，1=周一，7=周日）
 */
uint8_t HW_weekday_calculate(uint8_t setyear, uint8_t setmonth, uint8_t setday, uint8_t century)
{
	int w;
	// 1月和2月要当作上一年的13月和14月来计算
	if (setmonth == 1 || setmonth == 2)
	{setyear--, setmonth += 12;}
	w = setyear + setyear / 4 + century / 4  + 26*(setmonth + 1)/10 + setday - 1 - 2 * century;
	while(w<0)
		w+=7;          // 处理负数情况
	w%=7;              // 取模得到0-6
	w=(w==0)?7:w;      // 0转换为7（周日）
	return w;
}



/* ====== 电源管理功能 ====== */
/**
 * @brief  初始化电源管理模块
 * @param  无
 * @return 无
 */
void HW_Power_Init(void)
{
	#if HW_USE_BAT
		Power_Init();
	#endif
}

/**
 * @brief  执行关机操作，断开电源
 * @param  无
 * @return 无
 */
void HW_Power_Shutdown(void)
{
	#if HW_USE_BAT
		Power_DisEnable();
	#endif
}

/**
 * @brief  计算电池剩余电量百分比
 * @param  无
 * @return 电池剩余电量百分比（0-100）
 */
uint8_t HW_Power_BatCalculate(void)
{
	#if HW_USE_BAT
		return PowerCalculate();
	#endif
		return 0; // 不使用电池时返回0
}


/* ====== 蓝牙 BLE 功能 ====== */

/**
 * @brief  初始化蓝牙BLE模块
 * @param  无
 * @return 无
 */
void HW_BLE_Init(void)
{
	#if HW_USE_BLE
		KT6328_GPIO_Init(); // 初始化KT6328蓝牙芯片GPIO
	#endif
}

/**
 * @brief  使能蓝牙BLE
 * @param  无
 * @return 无
 */
void HW_BLE_Enable(void)
{
	#if HW_USE_BLE
		KT6328_Enable();
	#endif
}


/**
 * @brief  禁用蓝牙BLE
 * @param  无
 * @return 无
 */
void HW_BLE_Disable(void)
{
	#if HW_USE_BLE
		KT6328_Disable();
	#endif
}



/* ====== LCD 屏幕功能 ====== */

/**
 * @brief  设置LCD屏幕背光亮度
 * @param  dc 背光占空比（0-255），值越大越亮
 * @return 无
 */
void HW_LCD_Set_Light(uint8_t dc)
{
	#if HW_USE_LCD
		LCD_Set_Light(dc);
	#endif
}


/* ====== IMU（MPU6050）姿态传感器功能 ====== */

/**
 * @brief  初始化MPU6050六轴传感器及DMP运动引擎
 * @param  无
 * @return 0表示初始化成功，-1表示未启用硬件
 */
uint8_t HW_MPU_Init(void)
{
	#if HW_USE_IMU
		return mpu_dmp_init();
	#endif
	return -1;
}


/**
 * @brief  启用抬腕唤醒功能
 * @param  无
 * @return 无
 */
void HW_MPU_Wrist_Enable(void)
{
	#if HW_USE_IMU
		HWInterface.IMU.wrist_is_enabled = 1; // 设置抬腕唤醒标志为启用
	#endif
}


/**
 * @brief  禁用抬腕唤醒功能
 * @param  无
 * @return 无
 */
void HW_MPU_Wrist_Disable(void)
{
	#if HW_USE_IMU
		HWInterface.IMU.wrist_is_enabled = 0; // 设置抬腕唤醒标志为禁用
	#endif
}


/**
 * @brief  获取计步器当前步数
 * @param  无
 * @return 当前累计步数
 */
uint16_t HW_MPU_Get_Steps(void)
{
	#if HW_USE_IMU
		unsigned long STEPS = 0;
		if(!HWInterface.IMU.ConnectionError)              // 确保传感器连接正常
			dmp_get_pedometer_step_count(&STEPS);          // 从DMP读取计步数据
		return (uint16_t)STEPS;
	#endif
		return 0;
}

/**
 * @brief  设置计步器步数（用于同步或校准）
 * @param  count 要设置的步数值
 * @return 0表示成功，-1表示失败或未启用硬件
 */
int HW_MPU_Set_Steps(unsigned long count)
{
	#if HW_USE_IMU
		if(!HWInterface.IMU.ConnectionError)
			return dmp_set_pedometer_step_count(count);
	#endif
		return -1;
}

/* ====== AHT21 温湿度传感器功能 ====== */

/**
 * @brief  初始化AHT21温湿度传感器
 * @param  无
 * @return 0表示成功，-1表示未启用硬件
 */
uint8_t HW_AHT21_Init(void)
{
	#if HW_USE_AHT21
		return AHT_Init();
	#endif
	return -1;
}

/**
 * @brief  读取温湿度数据
 * @param  humi 输出参数，存储湿度值
 * @param  temp 输出参数，存储温度值
 * @return 无
 */
void HW_AHT21_Get_Humi_Temp(float *humi, float *temp)
{
	#if HW_USE_AHT21
		//temp and humi messure
		if(!HWInterface.AHT21.ConnectionError)             // 确保传感器连接正常
			AHT_Read(humi,temp);                            // 从AHT21读取温度和湿度
	#endif
}


/* ====== SPL06-001 气压计功能 ====== */

/**
 * @brief  初始化SPL06-001气压计传感器
 * @param  无
 * @return 0表示成功，-1表示未启用硬件
 */
uint8_t HW_Barometer_Init(void)
{
	#if HW_USE_SPL06
		return SPL_init();
	#endif

	return -1;
}


/* ====== LSM303 电子罗盘功能 ====== */

/**
 * @brief  初始化LSM303电子罗盘传感器
 * @param  无
 * @return 0表示成功，-1表示未启用硬件
 */
uint8_t HW_Ecompass_Init(void)
{
	#if HW_USE_LSM303
		return LSM303DLH_Init();
	#endif

	return -1;
}

/**
 * @brief  将LSM303电子罗盘设为休眠模式以节省功耗
 * @param  无
 * @return 无
 */
void HW_Ecompass_Sleep(void)
{
	#if HW_USE_LSM303
		LSM303DLH_Sleep();
	#endif
}

/* ====== EM7028 心率血氧传感器功能 ====== */

/**
 * @brief  初始化EM7028心率血氧传感器
 * @param  无
 * @return 0表示成功，-1表示未启用硬件
 */
uint8_t HW_HRmeter_Init(void)
{
	#if HW_USE_EM7028
		return EM7028_hrs_init();
	#endif

	return -1;

}

/**
 * @brief  将EM7028心率传感器设为休眠模式以节省功耗
 * @param  无
 * @return 无
 */
void HW_HRmeter_Sleep(void)
{
	#if HW_USE_EM7028
		EM7028_hrs_DisEnable();
	#endif
}





/* ====== 外部变量定义 ====== */

// 全局硬件接口实例，通过函数指针将各硬件模块的具体实现注册到统一结构体中
HW_InterfaceTypeDef HWInterface = {
    .RealTimeClock = {
        .GetTimeDate = HW_RTC_Get_TimeDate,
        .SetDate = HW_RTC_Set_Date,
        .SetTime = HW_RTC_Set_Time,
        .CalculateWeekday = HW_weekday_calculate
    },
    .BLE = {
		.Init = HW_BLE_Init,
        .Enable = HW_BLE_Enable,
        .Disable = HW_BLE_Disable
    },
    .Power = {
		.power_remain = 0,
		.Init = HW_Power_Init,
        .Shutdown = HW_Power_Shutdown,
		.BatCalculate = HW_Power_BatCalculate
    },
    .LCD = {
        .SetLight = HW_LCD_Set_Light
    },

	.IMU = {
		.ConnectionError = 1,  // 初始状态为连接异常，待初始化后更新
		.Steps = 0,
		.wrist_is_enabled = 0,
		.wrist_state = WRIST_UP,
		.Init = HW_MPU_Init,
        .WristEnable = HW_MPU_Wrist_Enable,
        .WristDisable = HW_MPU_Wrist_Disable,
        .GetSteps = HW_MPU_Get_Steps,
		.SetSteps = HW_MPU_Set_Steps
    },
	.AHT21 = {
		.ConnectionError = 1, // 初始状态为连接异常
		.humidity = 67,       // 默认湿度值（用于仿真模式）
		.temperature = 26,    // 默认温度值（用于仿真模式）
		.Init = HW_AHT21_Init,
		.GetHumiTemp = HW_AHT21_Get_Humi_Temp
	},
	.Barometer = {
		.ConnectionError = 1, // 初始状态为连接异常
		.altitude = 19,       // 默认海拔值（用于仿真模式）
		.Init = HW_Barometer_Init,
	},
	.Ecompass = {
		.ConnectionError = 1, // 初始状态为连接异常
		.direction = 45,      // 默认方位角度（用于仿真模式）
		.Init = HW_Ecompass_Init,
		.Sleep = HW_Ecompass_Sleep
	},
	.HR_meter = {
		.ConnectionError = 1, // 初始状态为连接异常
		.HrRate = 0,          // 心率初始为0
		.SPO2 = 99,           // 默认血氧值（用于仿真模式）
		.Init = HW_HRmeter_Init,
		.Sleep = HW_HRmeter_Sleep
	}
};
