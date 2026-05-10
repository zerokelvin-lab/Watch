/** @file ui_SetPage.h
 *  @brief 设置页面 - 提供常亮时间、熄屏时间、日期时间设置、抬腕亮屏、密码设置等系统配置入口
 */
#ifndef _UI_SETPAGE_H
#define _UI_SETPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

// 常亮时间设置
extern lv_obj_t * ui_SetPage; // 设置页面对象
extern lv_obj_t * ui_LightTimePanel; // 常亮时间面板
extern lv_obj_t * ui_LightTimeButton; // 常亮时间按钮
extern lv_obj_t * ui_LightTimeicon; // 常亮时间图标
extern lv_obj_t * ui_LightTimeLabel; // 常亮时间标签

// 熄屏时间设置
extern lv_obj_t * ui_TurnOffTimePanel; // 熄屏时间面板
extern lv_obj_t * ui_TOffTimeButton; // 熄屏时间按钮
extern lv_obj_t * ui_TOffTimeicon; // 熄屏时间图标
extern lv_obj_t * ui_TOffTimeLabel; // 熄屏时间标签

// 日期时间设置
extern lv_obj_t * ui_DateTimeSetPanel; // 日期时间设置面板
extern lv_obj_t * ui_DateTimeSetButton; // 日期时间设置按钮
extern lv_obj_t * ui_LOffTimeicon; // 日期时间设置图标
extern lv_obj_t * ui_DateTimeSetLabel; // 日期时间设置标签

// 抬腕亮屏
extern lv_obj_t * ui_WristPanel; // 抬腕亮屏面板
extern lv_obj_t * ui_WristButton; // 抬腕亮屏按钮
extern lv_obj_t * ui_Wristicon; // 抬腕亮屏图标
extern lv_obj_t * ui_WristLabel; // 抬腕亮屏标签
extern lv_obj_t * ui_WristSwitch; // 抬腕亮屏开关

// 密码设置
extern lv_obj_t * ui_PasswordPanel; // 密码设置面板
extern lv_obj_t * ui_PasswordButton; // 密码设置按钮
extern lv_obj_t * ui_Passwordicon; // 密码设置图标
extern lv_obj_t * ui_PasswordLabel; // 密码设置标签

extern Page_t Page_Set; // 设置页面管理器

void ui_SetPage_screen_init(void); // 设置页面初始化
void ui_SetPage_screen_deinit(void); // 设置页面注销


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
