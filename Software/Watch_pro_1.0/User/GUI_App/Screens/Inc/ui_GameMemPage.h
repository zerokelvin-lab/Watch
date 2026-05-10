/** @file ui_GameMemPage.h
 *  @brief 记忆方块游戏页面 - 翻牌记忆配对游戏，找出所有相同数字的方块对
 */
#ifndef _UI_GAMEMEMPAGE_H
#define _UI_GAMEMEMPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ui.h"

extern lv_obj_t * ui_GameMem_Page; // 记忆方块游戏页面对象
extern Page_t Page_GameMem; // 记忆方块游戏页面管理器

void ui_GameMemPage_screen_init(void); // 记忆方块游戏页面初始化
void ui_GameMemPage_screen_deinit(void); // 记忆方块游戏页面注销

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
