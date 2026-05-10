/** @file ui.h @brief LVGL GUI主程序头文件，声明外部字体和初始化接口 */

// LVGL VERSION: 8.2.0

// 头文件保护宏，防止重复包含
#ifndef _UI_H
#define _UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"                 // LVGL图形库头文件

#include "stdio.h"                     // 标准输入输出

#include "../Func/Inc/PageManager.h"   // 页面管理器

// 声明外部仓颉字体（不同字号）
LV_FONT_DECLARE(ui_font_Cuyuan18);     // 仓颉字体 18号
LV_FONT_DECLARE(ui_font_Cuyuan20);     // 仓颉字体 20号
LV_FONT_DECLARE(ui_font_Cuyuan24);     // 仓颉字体 24号
LV_FONT_DECLARE(ui_font_Cuyuan30);     // 仓颉字体 30号
LV_FONT_DECLARE(ui_font_Cuyuan38);     // 仓颉字体 38号
LV_FONT_DECLARE(ui_font_Cuyuan48);     // 仓颉字体 48号
LV_FONT_DECLARE(ui_font_Cuyuan80);     // 仓颉字体 80号
LV_FONT_DECLARE(ui_font_Cuyuan100);    // 仓颉字体 100号

// 声明外部图标字体（不同字号）
LV_FONT_DECLARE(ui_font_iconfont16);   // 图标字体 16号
LV_FONT_DECLARE(ui_font_iconfont24);   // 图标字体 24号
LV_FONT_DECLARE(ui_font_iconfont28);   // 图标字体 28号
LV_FONT_DECLARE(ui_font_iconfont30);   // 图标字体 30号
LV_FONT_DECLARE(ui_font_iconfont32);   // 图标字体 32号
LV_FONT_DECLARE(ui_font_iconfont34);   // 图标字体 34号
LV_FONT_DECLARE(ui_font_iconfont45);   // 图标字体 45号

/**
 * @brief GUI初始化函数
 *
 * 初始化LVGL默认主题、所有页面和主刷新定时器
 */
void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
