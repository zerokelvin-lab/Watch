/** @file WDOG.c @brief 外部看门狗驱动 - TPS3839/SGM803 看门狗芯片的GPIO控制和喂狗操作 */

#include "WDOG.h"
#include "main.h"

/* ====== 看门狗引脚定义 ====== */
//WDOG_EN：看门狗使能引脚
#define WDOG_EN_PORT	GPIOB
#define WDOG_EN_PIN		GPIO_PIN_1
//WDI：看门狗输入（喂狗引脚）
#define WDI_PORT	GPIOB
#define WDI_PIN		GPIO_PIN_2

/**
 * @brief  初始化看门狗控制引脚
 * @retval 无
 * @note   配置WDOG_EN(PB1)和WDI(PB2)为推挽输出
 *         WDOG_EN: 低速输出，内部上拉
 *         WDI: 高速输出，用于快速翻转产生喂狗脉冲
 */
void WDOG_Port_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();                     // 使能GPIOB时钟

  GPIO_InitStructure.Pin = WDOG_EN_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;    // 推挽输出
  GPIO_InitStructure.Pull = GPIO_PULLUP;            // 内部上拉（默认禁用看门狗）
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;   // 低速即可（使能信号）
  HAL_GPIO_Init(WDOG_EN_PORT, &GPIO_InitStructure); // 初始化WDOG_EN引脚
  GPIO_InitStructure.Pin = WDI_PIN;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;  // 高速输出（喂狗需要快速翻转）
  HAL_GPIO_Init(WDI_PORT, &GPIO_InitStructure);     // 初始化WDI喂狗引脚
}

/**
 * @brief  使能外部看门狗
 * @retval 无
 * @note   WDOG_EN拉低使能看门狗（低电平有效）
 */
void WDOG_Enable(void)
{
  HAL_GPIO_WritePin(WDOG_EN_PORT,WDOG_EN_PIN,GPIO_PIN_RESET); // WDOG_EN=0，使能看门狗
}

/**
 * @brief  禁用外部看门狗
 * @retval 无
 * @note   WDOG_EN拉高禁用看门狗
 */
void WDOG_Disnable(void)
{
  HAL_GPIO_WritePin(WDOG_EN_PORT,WDOG_EN_PIN,GPIO_PIN_SET);   // WDOG_EN=1，禁用看门狗
}

/**
 * @brief  喂看门狗（翻转WDI引脚电平）
 * @retval 无
 * @note   通过翻转WDI引脚产生边沿信号来清除看门狗定时器，防止系统复位
 */
void WDOG_Feed(void)
{
  HAL_GPIO_TogglePin(WDI_PORT,WDI_PIN);             // 翻转WDI引脚，产生看门狗喂狗信号
}
