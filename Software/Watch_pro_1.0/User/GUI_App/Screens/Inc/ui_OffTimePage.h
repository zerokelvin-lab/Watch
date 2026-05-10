/** @file ui_OffTimePage.h
 *  @brief 熄屏/常亮时间设置页面 - 设置屏幕常亮时间和自动熄屏时间，通过滚轮选择预设时长
 */
#ifndef _UI_OFFTIMEPAGE_H
#define _UI_OFFTIMEPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

// 常亮时间设置页面
extern lv_obj_t * ui_LOffTimeSetPage; // 常亮时间设置页面
extern lv_obj_t * ui_LTimeSetRoller; // 常亮时间滚轮
extern lv_obj_t * ui_LTimeSetLabel; // 常亮时间标签
extern lv_obj_t * ui_LTimeSetOKButton; // 常亮时间确认按钮
extern lv_obj_t * ui_LTimeSetOKLabel; // 常亮时间确认标签
extern lv_obj_t * ui_LTimeSeticon; // 常亮时间图标

// 熄屏时间设置页面
extern lv_obj_t * ui_TOffTimeSetPage; // 熄屏时间设置页面
extern lv_obj_t * ui_TTimeSetRoller; // 熄屏时间滚轮
extern lv_obj_t * ui_TTimeSetLabel; // 熄屏时间标签
extern lv_obj_t * ui_TTimeSetOKButton; // 熄屏时间确认按钮
extern lv_obj_t * ui_TTimeSetOKLabel; // 熄屏时间确认标签
extern lv_obj_t * ui_TTimeSeticon; // 熄屏时间图标

extern const uint8_t ui_LTimeOptions[6]; // 常亮时间选项（秒）
extern const uint8_t ui_TTimeOptions[6]; // 熄屏时间选项（秒）
extern uint8_t ui_LTimeValue; // 当前常亮时间值
extern uint8_t ui_TTimeValue; // 当前熄屏时间值

extern Page_t Page_LOffTimeSet; // 常亮时间页面管理器
extern Page_t Page_TOffTimeSet; // 熄屏时间页面管理器

void ui_LOffTimeSetPage_screen_init(void); // 常亮时间页面初始化
void ui_TOffTimeSetPage_screen_init(void); // 熄屏时间页面初始化

void ui_LOffTimeSetPage_screen_deinit(void); // 常亮时间页面注销
void ui_TOffTimeSetPage_screen_deinit(void); // 熄屏时间页面注销

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
