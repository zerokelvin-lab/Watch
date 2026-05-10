/** @file delay.h @brief 延时函数头文件，声明微秒/毫秒级延时及初始化函数 */

#ifndef _DELAY_H
#define _DELAY_H
#include "sys.h"	  

void delay_init(void);    // 延时系统初始化，配置SysTick
void delay_ms(u16 nms);   // 毫秒级延时
void delay_us(u32 nus);   // 微秒级延时
#endif
