/** @file ui_TimerPage.h
 *  @brief 秒表页面 - 高精度秒表计时器，支持开始/暂停/复位，显示分:秒.毫秒
 */
#ifndef _UI_TIMERPAGE_H
#define _UI_TIMERPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ui.h"

extern lv_obj_t * ui_TimerPage; // 秒表页面对象

extern uint8_t ui_TimerPageFlag; // 秒表运行标志
extern uint32_t ui_TimerPage_elapsed_ms; // 秒表累计毫秒数

extern Page_t Page_Timer; // 秒表页面管理器

void ui_TimerPage_screen_init(void); // 秒表页面初始化
void ui_TimerPage_screen_deinit(void); // 秒表页面注销

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
