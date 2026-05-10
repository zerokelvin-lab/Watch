/** @file KT6328.h @brief KT6328 BLE/NFC芯片驱动头文件，通过GPIO控制芯片使能/关闭 */
#ifndef __KT6328_H__
#define __KT6328_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void KT6328_GPIO_Init(void);     /** @brief 初始化KT6328控制GPIO(PA8, 推挽输出) */
void KT6328_Enable(void);        /** @brief 使能KT6328芯片(PA8输出高电平) */
void KT6328_Disable(void);       /** @brief 关闭KT6328芯片(PA8输出低电平) */


#ifdef __cplusplus
}
#endif
#endif
