/** @file power.c @brief 电源管理驱动 - 电池电压ADC采集、充电检测、电量百分比计算、电源开关控制 */

#include "power.h"
#include "adc.h"
#include "delay.h"

/* ====== 电池参数常量 ====== */
#define INTERNAL_RES 0.128                         // 电池内阻估算值（单位：欧姆），用于充电时电压补偿
#define CHARGING_CUR 1                             // 充电电流（单位：安培），用于内阻压降补偿计算

/* ====== 电源控制引脚初始化 ====== */

/**
 * @brief  初始化电源控制相关引脚
 * @retval 无
 * @note   POWER_PIN(PA3): 电源开关控制引脚，推挽输出
 *         CHARGE_PIN(PA0): 充电状态检测引脚，上升/下降沿中断
 *         EXTI0中断优先级: 抢占5，子优先级0
 */
void Power_Pins_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();                    // 使能GPIOA时钟

//   /*Configure GPIO pin Output Level */
//   HAL_GPIO_WritePin(POWER_PORT, POWER_PIN, GPIO_PIN_RESET); // 初始状态关闭电源

//   /*Configure GPIO pin : PA3（电源开关控制引脚，推挽输出） */
//   GPIO_InitStruct.Pin = POWER_PIN;
//   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;       // 推挽输出模式
//   GPIO_InitStruct.Pull = GPIO_NOPULL;               // 无上下拉
//   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;      // 低速输出
//   HAL_GPIO_Init(POWER_PORT, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0（充电状态检测引脚，双边沿中断） */
  GPIO_InitStruct.Pin = CHARGE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING; // 上升沿和下降沿均触发中断
  GPIO_InitStruct.Pull = GPIO_NOPULL;               // 无上下拉
  HAL_GPIO_Init(CHARGE_PORT, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);           // 设置EXTI0中断优先级
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);                   // 使能EXTI0中断
}

/* ====== 电源开关控制 ====== */

/**
 * @brief  开启系统电源
 * @retval 无
 */
void Power_Enable()
{
	//HAL_GPIO_WritePin(POWER_PORT,POWER_PIN,GPIO_PIN_SET); // 电源使能引脚拉高
}

/**
 * @brief  关闭系统电源
 * @retval 无
 */
void Power_DisEnable()
{
	//HAL_GPIO_WritePin(POWER_PORT,POWER_PIN,GPIO_PIN_RESET); // 电源使能引脚拉低
}

/* ====== 充电检测 ====== */

/**
 * @brief  检测充电状态
 * @retval 1=正在充电, 0=未充电
 */
uint8_t ChargeCheck()//1:charging
{
	return HAL_GPIO_ReadPin(CHARGE_PORT,CHARGE_PIN); // 读取充电检测引脚电平
}

/* ====== 电池电压采集 ====== */

/**
 * @brief  单次采集电池电压（通过ADC）
 * @retval 电池电压值（单位：伏特）
 * @note   ADC采集值换算公式: 电压 = ADC值 * 2 * 3.3 / 4096
 *         其中系数2是因为硬件分压电路将电池电压分压1/2后送入ADC（3.3V参考电压，12位分辨率）
 */
float BatCheck()
{
	uint16_t dat;
	float BatVoltage;
	HAL_ADC_Start(&hadc1);                          // 启动ADC1
	HAL_ADC_PollForConversion(&hadc1,5);            // 等待转换完成（超时5ms）
	dat = HAL_ADC_GetValue(&hadc1);                 // 读取ADC转换值
	HAL_ADC_Stop(&hadc1);                           // 停止ADC
	BatVoltage = dat *2 *3.3 /4096;                 // 换算为实际电压：ADC值 * 分压比2 * 参考电压3.3V / 2^12
	return BatVoltage;
}

/**
 * @brief  8次采集取平均值获取电池电压（提高精度）
 * @retval 电池电压值（8次平均值，单位：伏特）
 */
float BatCheck_8times()
{
	uint32_t dat=0;
	uint8_t i;
	float BatVoltage;
	for(i=0;i<8;i++)
	{
		HAL_ADC_Start(&hadc1);                       // 启动ADC
		HAL_ADC_PollForConversion(&hadc1,5);         // 等待转换完成
		dat += HAL_ADC_GetValue(&hadc1);             // 累加8次ADC值
		HAL_ADC_Stop(&hadc1);                        // 停止ADC
		delay_ms(1);                                 // 延时1ms等待ADC稳定
	}
	dat = dat>>3;                                    // 除以8求平均值（右移3位）
	BatVoltage = dat *2 *3.3 /4096;                  // 换算为实际电压
	return BatVoltage;
}

/* ====== 电量百分比计算 ====== */

/**
 * @brief  计算电池电量百分比
 * @retval 电量百分比（0~100）
 * @note   根据电池电压查表法估算电量：
 *         充电时需要减去内阻产生的压降（电压 -= 内阻 * 充电电流）
 *         电压→电量映射表（锂电池典型放电曲线）：
 *         ≥4.20V = 100%, 4.06~4.20 = 90%, 3.98~4.06 = 80%, 3.92~3.98 = 70%,
 *         3.87~3.92 = 60%, 3.82~3.87 = 50%, 3.79~3.82 = 40%, 3.77~3.79 = 30%,
 *         3.74~3.77 = 20%, 3.68~3.74 = 10%, 3.45~3.68 = 5%
 */
uint8_t PowerCalculate()
{
	uint8_t power;
	float voltage;
	voltage = BatCheck_8times();                     // 8次平均获取电池电压

	if(ChargeCheck())                                // 如果正在充电，补偿内阻压降
	{voltage -= INTERNAL_RES * CHARGING_CUR;}        // 减去充电内阻压降 = 内阻 * 充电电流

	if((voltage >= 4.2))
	{power = 100;}
	else if(voltage >= 4.06 && voltage <4.2)
	{power = 90;}
	else if(voltage >= 3.98 && voltage <4.06)
	{power = 80;}
	else if(voltage >= 3.92 && voltage <3.98)
	{power = 70;}
	else if(voltage >= 3.87 && voltage <3.92)
	{power = 60;}
	else if(voltage >= 3.82 && voltage <3.87)
	{power = 50;}
	else if(voltage >= 3.79 && voltage <3.82)
	{power = 40;}
	else if(voltage >= 3.77 && voltage <3.79)
	{power = 30;}
	else if(voltage >= 3.74 && voltage <3.77)
	{power = 20;}
	else if(voltage >= 3.68 && voltage <3.74)
	{power = 10;}
	else if(voltage >= 3.45 && voltage <3.68)
	{power = 5;}
	return power;
}

/* ====== 电源模块初始化 ====== */

/**
 * @brief  电源管理模块初始化
 * @retval 无
 * @note   初始化引脚并开启电源
 */
void Power_Init(void)
{
	Power_Pins_Init();                               // 初始化GPIO引脚
	//Power_Enable();                                  // 开启系统电源
}
