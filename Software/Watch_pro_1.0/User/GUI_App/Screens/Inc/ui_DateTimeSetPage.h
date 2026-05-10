/** @file ui_DateTimeSetPage.h
 *  @brief 日期时间设置页面 - 支持APP同步开关、日期设置（年/月/日滚轮）和时间设置（时/分/秒滚轮）
 */
#ifndef _DATEDAYSETPAGE_UI_H
#define _DATEDAYSETPAGE_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"
#include "../../ui.h"

extern lv_obj_t * ui_DateTimeSetPage; // 日期时间设置主页
extern lv_obj_t * ui_APPSyPanel; // APP同步面板
extern lv_obj_t * ui_APPSySwitch; // APP同步开关
extern lv_obj_t * ui_APPSyLabel; // APP同步标签
extern lv_obj_t * ui_DateSetPanel; // 日期设置入口面板
extern lv_obj_t * ui_DateSetLabel; // 日期设置标签
extern lv_obj_t * ui_DateSetLabel1; // 日期设置装饰点1
extern lv_obj_t * ui_DateSetLabel2; // 日期设置装饰点2
extern lv_obj_t * ui_DateSetLabel3; // 日期设置装饰点3
extern lv_obj_t * ui_TimeSetPanel; // 时间设置入口面板
extern lv_obj_t * ui_TimeSetLabel; // 时间设置标签
extern lv_obj_t * ui_TimeSetLabel1; // 时间设置装饰点1
extern lv_obj_t * ui_TimeSetLabel2; // 时间设置装饰点2
extern lv_obj_t * ui_TimeSetLabel3; // 时间设置装饰点3
extern lv_obj_t * ui_DateSetPage; // 日期设置子页面
extern lv_obj_t * ui_YearSetRoller; // 年份滚轮
extern lv_obj_t * ui_MonthSetRoller; // 月份滚轮
extern lv_obj_t * ui_DaySetRoller; // 日期滚轮
extern lv_obj_t * ui_DateSetOKButton; // 日期设置确认按钮
extern lv_obj_t * ui_DateSetOKicon; // 日期设置确认图标
extern lv_obj_t * ui_TimeSetPage; // 时间设置子页面
extern lv_obj_t * ui_HourSetRoller; // 小时滚轮
extern lv_obj_t * ui_TimePoint; // 时间冒号1
extern lv_obj_t * ui_MinSetRoller; // 分钟滚轮
extern lv_obj_t * ui_TimePoint1; // 时间冒号2
extern lv_obj_t * ui_SecSetRoller; // 秒滚轮
extern lv_obj_t * ui_TimeSetOKButton; // 时间设置确认按钮
extern lv_obj_t * ui_TimeSetOKicon; // 时间设置确认图标

extern Page_t Page_DateTimeSet; // 日期时间设置主页管理器
extern Page_t Page_DateSet; // 日期设置子页面管理器
extern Page_t Page_TimeSet; // 时间设置子页面管理器

void ui_DateTimeSetPage_screen_init(void); // 日期时间设置主页初始化
void ui_DateSetPage_screen_init(void); // 日期设置页面初始化
void ui_TimeSetPage_screen_init(void); // 时间设置页面初始化

void ui_DateTimeSetPage_screen_deinit(void); // 日期时间设置主页注销
void ui_DateSetPage_screen_deinit(void); // 日期设置页面注销
void ui_TimeSetPage_screen_deinit(void); // 时间设置页面注销

extern uint8_t ui_APPSy_EN; // APP同步使能标志

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
