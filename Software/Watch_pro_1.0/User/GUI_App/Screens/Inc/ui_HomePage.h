/** @file ui_HomePage.h
 *  @brief 主页（表盘页面）- 显示时间、日期、电池、步数、温度/湿度/心率及下拉快捷面板
 */
#ifndef _UI_HOMEPAGE_H
#define _UI_HOMEPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ui.h"

// 主页元素
extern lv_obj_t * ui_HomePage; // 主页对象
extern lv_obj_t * ui_TimeHourLabel; // 小时标签
extern lv_obj_t * ui_TimeColonLabel; // 时间冒号
extern lv_obj_t * ui_TimeMinuteLabel; // 分钟标签
extern lv_obj_t * ui_BatArc; // 电池电量弧线
extern lv_obj_t * ui_BaticonLabel; // 电池图标
extern lv_obj_t * ui_BatNumLabel; // 电池百分比数字
extern lv_obj_t * ui_DateLabel; // 日期标签
extern lv_obj_t * ui_DayLabel; // 星期标签
extern lv_obj_t * ui_StepiconLabel; // 步数图标
extern lv_obj_t * ui_StepCnLabel; // 步数中文标签
extern lv_obj_t * ui_StepNumLabel; // 步数数字标签
extern lv_obj_t * ui_StepNumBar; // 步数进度条
extern lv_obj_t * ui_TempArc; // 温度弧形
extern lv_obj_t * ui_TempiconLabel; // 温度图标
extern lv_obj_t * ui_TempNumLabel; // 温度数字
extern lv_obj_t * ui_HumiiconLabel; // 湿度图标
extern lv_obj_t * ui_HumiArc; // 湿度弧形
extern lv_obj_t * ui_HumiNumLabel; // 湿度数字
extern lv_obj_t * ui_HRiconLabel; // 心率图标
extern lv_obj_t * ui_HRSpinner; // 心率旋转动画
extern lv_obj_t * ui_HRNumLabel; // 心率数字

// 下拉面板元素
extern lv_obj_t * ui_DropDownPanel; // 下拉面板
extern lv_obj_t * ui_UpBGPanel; // 上背景面板
extern lv_obj_t * ui_NFCButton; // NFC开关按钮
extern lv_obj_t * ui_NFCLabel; // NFC标签
extern lv_obj_t * ui_BLEButton; // 蓝牙开关按钮
extern lv_obj_t * ui_BLELabel; // 蓝牙标签
extern lv_obj_t * ui_PowerButton; // 电源按钮
extern lv_obj_t * ui_PowerLabel; // 电源标签
extern lv_obj_t * ui_SetButton; // 设置按钮
extern lv_obj_t * ui_SetLabel; // 设置标签
extern lv_obj_t * ui_LightSlider; // 亮度滑块
extern lv_obj_t * ui_LightLabel; // 亮度标签
extern lv_obj_t * ui_DownBGPanel; // 下背景面板

// 时间日期变量
extern uint8_t ui_TimeHourValue; // 当前小时值
extern uint8_t ui_TimeMinuteValue; // 当前分钟值
extern uint8_t ui_DateMonthValue; // 当前月份值
extern uint8_t ui_DateDayValue; // 当前日期值
extern uint8_t ui_DataWeekdayValue; // 当前星期值
extern const char * ui_Days[7]; // 星期字符串数组

// 传感器与状态变量
extern uint8_t ui_BatArcValue; // 电池电量值
extern uint16_t ui_StepNumValue; // 步数值
extern uint8_t ui_LightSliderValue; // 亮度滑块值

extern Page_t Page_Home; // 主页管理器

// 事件回调函数
void ui_event_HomePage_DropDownPanel(lv_event_t * e); // 下拉面板事件
void ui_event_NFCButton(lv_event_t * e); // NFC按钮事件
void ui_event_BLEButton(lv_event_t * e); // 蓝牙按钮事件
void ui_event_PowerButton(lv_event_t * e); // 电源按钮事件
void ui_event_SetButton(lv_event_t * e); // 设置按钮事件

void ui_HomePage_screen_init(void); // 主页初始化
void ui_PowerPage_screen_init(void); // 电源页面初始化

void ui_HomePage_screen_deinit(void); // 主页注销
void ui_PowerPage_screen_deinit(void); // 电源页面注销

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
