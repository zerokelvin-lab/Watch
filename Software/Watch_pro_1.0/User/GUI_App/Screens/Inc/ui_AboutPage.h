/** @file ui_AboutPage.h
 *  @brief 关于页面 - 显示手表型号、固件版本、主控芯片、操作系统、图形界面及开发者信息
 */
#ifndef _UI_ABOUTPAGE_H
#define _UI_ABOUTPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

extern lv_obj_t * ui_AboutPage; // 关于页面对象

extern Page_t Page_About; // 关于页面管理器

void ui_AboutPage_screen_init(void); // 关于页面初始化
void ui_AboutPage_screen_deinit(void); // 关于页面注销

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
