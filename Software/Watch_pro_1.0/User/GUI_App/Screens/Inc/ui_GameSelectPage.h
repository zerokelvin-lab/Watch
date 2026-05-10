/** @file ui_GameSelectPage.h
 *  @brief 游戏选择页面 - 游戏菜单入口，提供2048、记忆方块、反弹球等游戏的选择
 */
#ifndef _UI_GAMESELECTPAGE_H
#define _UI_GAMESELECTPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ui.h"

extern lv_obj_t * ui_GameSelectPage; // 游戏选择页面对象
extern Page_t Page_GameSelect; // 游戏选择页面管理器

void ui_GameSelectPage_screen_init(void); // 游戏选择页面初始化
void ui_GameSelectPage_screen_deinit(void); // 游戏选择页面注销

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
