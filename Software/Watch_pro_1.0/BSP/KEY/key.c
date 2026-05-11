/** @file key.c @brief 按键驱动实现，包含GPIO中断初始化、带消抖的按键扫描和双击检测 */
#include "key.h"
#include "delay.h"
#include "user_TasksInit.h"

/**
  * @brief 初始化按键GPIO引脚和外部中断
  * @note KEY1(PA5): 下降沿触发，上拉输入，EXTI9_5中断
  * @note KEY2(PA4): 上升沿触发，浮空输入，EXTI4中断
  */
void Key_Port_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();    // 使能GPIOA时钟

  /* KEY1配置: PA5, 下降沿触发, 上拉输入 */
  GPIO_InitStruct.Pin = KEY1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;   // 下降沿中断模式
  GPIO_InitStruct.Pull = GPIO_PULLUP;             // 内部上拉(默认高电平,按下接地)
  HAL_GPIO_Init(KEY1_PORT, &GPIO_InitStruct);

  /* KEY2/Wake配置: PA4, 上升沿触发, 浮空输入 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;    // 上升沿中断模式
  GPIO_InitStruct.Pull = GPIO_NOPULL;             // 无上下拉
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI external interrupt init（外部中断初始化） */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);       // KEY1(PA5)中断优先级=5, 子优先级=0
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);         // KEY2(PA4)中断优先级=5, 子优先级=0
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

/**
  * @brief 按键扫描函数(带软件消抖+双击检测)
  * @param mode 扫描模式: 0=单次触发(按下只返回一次), 1=连续触发(按下持续返回)
  * @return 按键键值: 0=无按键, 1=KEY1单击, 2=KEY2单击, 3=KEY1双击, 4=KEY2双击
  * @note 消抖延迟3ms，双击检测阈值KEY_DOUBLE_CLICK_THRESHOLD(ms)
  */
uint8_t KeyScan(uint8_t mode)
{
	static uint8_t key_up = 1;              // 按键释放标志(1=已释放, 0=已按下)
	static uint8_t key_down = 0;            // 按键按下暂存值(0=无, 1=KEY1, 2=KEY2)
	static uint8_t pending_click = 0;       // 待确认的单击键值(等待双击窗口)
	static uint32_t first_click_tick = 0;   // 第一次点击的时间戳
	uint8_t keyvalue = 0;                   // 本次扫描返回值

	if(mode)                                // 连续模式: 清零状态,允许重复触发
	{
		key_up = 1;
		key_down = 0;
		pending_click = 0;
	}

	// ====== 双击检测：等待第二次点击 ======
	if(pending_click)
	{
		uint32_t elapsed = osKernelGetTickCount() - first_click_tick;
		if(elapsed < KEY_DOUBLE_CLICK_THRESHOLD)
		{
			// 在阈值内，检测是否有第二次点击
			if(key_up && ((!KEY1) || KEY2))
			{
				osDelay(3);  // 消抖
				if(!KEY1)
					key_down = 1;
				if(KEY2)
					key_down = 2;
				if(key_down)
					key_up = 0;
			}

			// 第二次按键释放，确认双击
			if(key_down && (KEY1 && (!KEY2)))
			{
				osDelay(3);
				if(KEY1 && (!KEY2))
				{
					key_up = 1;
					if(key_down == pending_click)
					{
						keyvalue = pending_click + 2;  // 双击: 1->3, 2->4
						pending_click = 0;
						key_down = 0;
						return keyvalue;
					}
					key_down = 0;
				}
			}
			return 0;  // 还在等待窗口内，暂不返回
		}
		else
		{
			// 超时，返回第一次单击
			keyvalue = pending_click;
			pending_click = 0;
			return keyvalue;
		}
	}

	// ====== 正常按键扫描（单击检测） ======
	// 状态1: 等待按键按下(key_up==1 且 检测到按键)
	if( key_up && ((!KEY1) || KEY2))
	{
		osDelay(3);//ensure the key（消抖延迟3ms）
		if(!KEY1)          // KEY1按下(低电平)
			key_down = 1;
		if(KEY2)           // KEY2按下(高电平)
			key_down = 2;
		if(key_down)       // 确认有键按下，标记已处理
			key_up = 0;
	}

	// 状态2: 等待按键释放(key_down非0 且 按键已释放)
	if ( key_down && (KEY1 && (!KEY2)) )
	{
		osDelay(3);//ensure the key（消抖延迟3ms）
		if(KEY1 && (!KEY2))  // 确认按键已释放
		{
			key_up = 1;        // 恢复释放状态
			// 缓存点击，启动双击等待窗口
			pending_click = key_down;
			first_click_tick = osKernelGetTickCount();
			key_down = 0;
			return 0;  // 暂不返回，等待可能的双击
		}
	}

	return keyvalue;
}
