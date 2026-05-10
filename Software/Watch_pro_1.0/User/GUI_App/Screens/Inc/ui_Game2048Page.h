/** @file ui_Game2048Page.h
 *  @brief 2048游戏页面 - 经典2048数字合并游戏，支持手势滑动操作和得分统计
 */
#ifndef _UI_GAME2048PAGE_H
#define _UI_GAME2048PAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ui.h"

extern lv_obj_t * ui_Game2048Page; // 2048游戏页面对象

extern Page_t Page_Game_2048; // 2048游戏页面管理器

void ui_Game2048Page_screen_init(void); // 2048游戏页面初始化
void ui_Game2048Page_screen_deinit(void); // 2048游戏页面注销

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
