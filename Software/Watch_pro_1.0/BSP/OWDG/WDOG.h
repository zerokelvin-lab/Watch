/** @file WDOG.h @brief 外部看门狗驱动头文件 - WDOG控制API声明 */

#ifndef __WDOG_H__
#define __WDOG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ====== 看门狗API函数声明 ====== */
void WDOG_Port_Init(void);    // 初始化看门狗GPIO引脚
void WDOG_Enable(void);       // 使能外部看门狗
void WDOG_Disnable(void);     // 禁用外部看门狗
void WDOG_Feed(void);         // 喂狗（翻转WDI引脚产生复位信号）

#ifdef __cplusplus
}
#endif
#endif
