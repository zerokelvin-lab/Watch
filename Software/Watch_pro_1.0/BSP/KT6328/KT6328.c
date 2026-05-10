/** @file KT6328.c @brief KT6328 BLE/NFC芯片驱动实现，通过PA8引脚电平控制芯片电源使能 */
#include "KT6328.h"

/**
  * @brief 初始化KT6328控制引脚PA8为推挽输出(默认低电平)
  */
void KT6328_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();    // 使能GPIOA时钟

  /*Configure GPIO pin Output Level: PA8默认输出低电平(芯片关闭) */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

  /* PA8配置: 推挽输出, 上拉, 低速 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;    // 推挽输出模式
  GPIO_InitStruct.Pull = GPIO_PULLUP;             // 内部上拉
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;   // 低速(控制信号)
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}


/**
  * @brief 使能KT6328 BLE/NFC芯片(PA8置高)
  */
void KT6328_Enable(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // PA8输出高电平 -> 芯片上电使能
}


/**
  * @brief 关闭KT6328 BLE/NFC芯片(PA8置低)
  */
void KT6328_Disable(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); // PA8输出低电平 -> 芯片断电关闭
}
