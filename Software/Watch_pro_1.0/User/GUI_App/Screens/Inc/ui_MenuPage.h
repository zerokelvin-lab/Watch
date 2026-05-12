/** @file ui_MenuPage.h
 *  @brief 菜单页面 - 应用功能列表，包含日历、计算器、秒表、卡包、运动、心率、血氧、环境、指南针、游戏、设置、关于等入口
 */
#ifndef _UI_MENUPAGE_H
#define _UI_MENUPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ui.h"

// 功能页面使能配置
#define CALENDAR_PAGE_EN		1 // 日历页面使能
#define COMPUTER_PAGE_EN		1 // 计算器页面使能
#define TIMER_PAGE_EN			1 // 秒表页面使能
#define CARD_PAGE_EN            1 // NFC卡包页面使能
#define SPROTS_PAGE_EN			0 // 运动页面使能（暂未实现）
#define HR_PAGE_EN				1 // 心率页面使能
#define O2_PAGE_EN				1 // 血氧页面使能
#define ENVIRONMENT_PAGE_EN	    1 // 环境页面使能
#define COMPASS_PAGE_EN			1 // 指南针页面使能
#define GAME_PAGE_EN            0 // 游戏页面使能
#define SET_PAGE_EN				1 // 设置页面使能
#define ABOUT_PAGE_EN			1 // 关于页面使能
#define MKS142_PAGE_EN			1 // MKS_142健康监测页面使能
#define MKS142_DETAIL_PAGE_EN	1 // MKS_142健康详情页面使能

extern lv_obj_t * ui_MenuPage; // 菜单页面对象

// 日历按钮
extern lv_obj_t * ui_MenuCalPanel;
extern lv_obj_t * ui_MenuCalButton;
extern lv_obj_t * ui_MenuCalicon;
extern lv_obj_t * ui_MenuCalLabel;

// 计算器按钮
extern lv_obj_t * ui_MenuComPanel;
extern lv_obj_t * ui_MenuComButton;
extern lv_obj_t * ui_MenuComicon;
extern lv_obj_t * ui_MenuComLabel;

// 秒表按钮
extern lv_obj_t * ui_MenuTimPanel;
extern lv_obj_t * ui_MenuTimButton;
extern lv_obj_t * ui_MenuTimicon;
extern lv_obj_t * ui_MenuTimLabel;

// 卡包按钮
extern lv_obj_t * ui_MenuCardPanel;
extern lv_obj_t * ui_MenuCardButton;
extern lv_obj_t * ui_MenuCardicon;
extern lv_obj_t * ui_MenuCardLabel;

// 运动按钮
extern lv_obj_t * ui_MenuSprPanel;
extern lv_obj_t * ui_MenuSprButton;
extern lv_obj_t * ui_MenuSpricon;
extern lv_obj_t * ui_MenuSprLabel;

// 心率按钮
extern lv_obj_t * ui_MenuHRPanel;
extern lv_obj_t * ui_MenuHRButton;
extern lv_obj_t * ui_MenuHRicon;
extern lv_obj_t * ui_MenuHRLabel;

// 血氧按钮
extern lv_obj_t * ui_MenuO2Panel;
extern lv_obj_t * ui_MenuO2Button;
extern lv_obj_t * ui_MenuO2icon;
extern lv_obj_t * ui_MenuO2Label;

// 环境按钮
extern lv_obj_t * ui_MenuEnvPanel;
extern lv_obj_t * ui_MenuEnvButton;
extern lv_obj_t * ui_MenuEnvicon;
extern lv_obj_t * ui_MenuEnvLabel;

// 指南针按钮
extern lv_obj_t * ui_MenuCPPanel;
extern lv_obj_t * ui_MenuCPButton;
extern lv_obj_t * ui_MenuCPicon;
extern lv_obj_t * ui_MenuCPLabel;

// 设置按钮
extern lv_obj_t * ui_MenuSetPanel;
extern lv_obj_t * ui_MenuSetButton;
extern lv_obj_t * ui_MenuSeticon;
extern lv_obj_t * ui_MenuSetLabel;

// 关于按钮
extern lv_obj_t * ui_MenuAbPanel;
extern lv_obj_t * ui_MenuAbButton;
extern lv_obj_t * ui_MenuAbicon;
extern lv_obj_t * ui_MenuAbLabel;

// MKS_142健康监测按钮
extern lv_obj_t * ui_MenuMKS142Panel;
extern lv_obj_t * ui_MenuMKS142Button;
extern lv_obj_t * ui_MenuMKS142icon;
extern lv_obj_t * ui_MenuMKS142Label;

// MKS_142健康详情按钮
extern lv_obj_t * ui_MenuMKS142DetailPanel;
extern lv_obj_t * ui_MenuMKS142DetailButton;
extern lv_obj_t * ui_MenuMKS142Detailicon;
extern lv_obj_t * ui_MenuMKS142DetailLabel;

extern int16_t ui_MenuScrollY; // 菜单滚动Y轴偏移

extern Page_t Page_Menu; // 菜单页面管理器

void ui_MenuPage_screen_init(void); // 菜单页面初始化
void ui_MenuPage_screen_deinit(void); // 菜单页面注销

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
