/** @file ui_MenuPage.c
 *  @brief 菜单页面实现 - 功能菜单列表，包含12个可配置使能的应用入口，支持滚动和右滑返回
 */
#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_HomePage.h"
#include "../Inc/ui_MenuPage.h"
#include "../Inc/ui_Megboxes.h"
#include "../Inc/ui_CalendarPage.h"
#include "../Inc/ui_ComputerPage.h"
#include "../Inc/ui_TimerPage.h"
#include "../Inc/ui_NFCCardPage.h"
#include "../Inc/ui_HRPage.h"
#include "../Inc/ui_SPO2Page.h"
#include "../Inc/ui_EnvPage.h"
#include "../Inc/ui_CompassPage.h"
#include "../Inc/ui_GameSelectPage.h"
#include "../Inc/ui_SetPage.h"
#include "../Inc/ui_AboutPage.h"


/* ====== 页面管理器 ====== */
Page_t Page_Menu = {ui_MenuPage_screen_init, ui_MenuPage_screen_deinit, &ui_MenuPage};

/* ====== 变量定义 ====== */
lv_obj_t * ui_MenuPage; // 菜单页面对象

// 日历入口
lv_obj_t * ui_MenuCalPanel;
lv_obj_t * ui_MenuCalButton;
lv_obj_t * ui_MenuCalicon;
lv_obj_t * ui_MenuCalLabel;

// 计算器入口
lv_obj_t * ui_MenuComPanel;
lv_obj_t * ui_MenuComButton;
lv_obj_t * ui_MenuComicon;
lv_obj_t * ui_MenuComLabel;

// 秒表入口
lv_obj_t * ui_MenuTimPanel;
lv_obj_t * ui_MenuTimButton;
lv_obj_t * ui_MenuTimicon;
lv_obj_t * ui_MenuTimLabel;

// NFC卡包入口
lv_obj_t * ui_MenuCardPanel;
lv_obj_t * ui_MenuCardButton;
lv_obj_t * ui_MenuCardicon;
lv_obj_t * ui_MenuCardLabel;

// 运动入口
lv_obj_t * ui_MenuSprPanel;
lv_obj_t * ui_MenuSprButton;
lv_obj_t * ui_MenuSpricon;
lv_obj_t * ui_MenuSprLabel;

// 心率入口
lv_obj_t * ui_MenuHRPanel;
lv_obj_t * ui_MenuHRButton;
lv_obj_t * ui_MenuHRicon;
lv_obj_t * ui_MenuHRLabel;

// 血氧入口
lv_obj_t * ui_MenuO2Panel;
lv_obj_t * ui_MenuO2Button;
lv_obj_t * ui_MenuO2icon;
lv_obj_t * ui_MenuO2Label;

// 环境入口
lv_obj_t * ui_MenuEnvPanel;
lv_obj_t * ui_MenuEnvButton;
lv_obj_t * ui_MenuEnvicon;
lv_obj_t * ui_MenuEnvLabel;

// 指南针入口
lv_obj_t * ui_MenuCPPanel;
lv_obj_t * ui_MenuCPButton;
lv_obj_t * ui_MenuCPicon;
lv_obj_t * ui_MenuCPLabel;

// 游戏入口
lv_obj_t * ui_MenuGamePanel;
lv_obj_t * ui_MenuGameButton;
lv_obj_t * ui_MenuGameicon;
lv_obj_t * ui_MenuGameLabel;

// 设置入口
lv_obj_t * ui_MenuSetPanel;
lv_obj_t * ui_MenuSetButton;
lv_obj_t * ui_MenuSeticon;
lv_obj_t * ui_MenuSetLabel;

// 关于入口
lv_obj_t * ui_MenuAbPanel;
lv_obj_t * ui_MenuAbButton;
lv_obj_t * ui_MenuAbicon;
lv_obj_t * ui_MenuAbLabel;

// 滚动位置记录
int16_t ui_MenuScrollY = 0; // 菜单Y轴滚动偏移


/* ====== 事件处理函数 ====== */

/**
 * @brief  菜单页面手势及滚动事件 - 右滑返回，滚动结束时记录位置
 * @param  e LVGL事件对象
 */
void ui_event_MenuPage(lv_event_t * e)
{
   lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);

    if(event_code == LV_EVENT_GESTURE)
    {
        if(lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) // 右滑返回
        {
		    Page_Back();
        }
    }
		if(event_code == LV_EVENT_SCROLL_END) // 记录滚动位置
    {
        ui_MenuScrollY = lv_obj_get_scroll_y(ui_MenuPage);
    }
}

/**
 * @brief  日历入口点击 - 跳转日历页面
 */
void ui_event_MenuCalPanel(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t * target = lv_event_get_target(e);
	if(event_code == LV_EVENT_CLICKED)
	{
		#if CALENDAR_PAGE_EN
			Page_Load(&Page_Calender);
		#else


		#endif
	}
}

/**
 * @brief  计算器入口点击 - 跳转计算器页面
 */
void ui_event_MenuComPanel(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t * target = lv_event_get_target(e);
	if(event_code == LV_EVENT_CLICKED)
	{
			#if COMPUTER_PAGE_EN
					Page_Load(&Page_Computer);
			#else


			#endif

	}
}

/**
 * @brief  秒表入口点击 - 跳转秒表页面
 */
void ui_event_MenuTimPanel(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
	lv_obj_t * target = lv_event_get_target(e);
	if(event_code == LV_EVENT_CLICKED)
	{
			#if TIMER_PAGE_EN
					Page_Load(&Page_Timer);
			#else


			#endif
	}
}

/**
 * @brief  卡包入口点击 - 跳转NFC卡包页面
 */
void ui_event_MenuCardPanel(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
        #if CARD_PAGE_EN
            Page_Load(&Page_NFCCard);
        #else


        #endif

    }
}

/**
 * @brief  运动入口点击 - 跳转运动页面（暂未实现）
 */
void ui_event_MenuSprPanel(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
        #if SPROTS_PAGE_EN

        #else


        #endif
    }
}

/**
 * @brief  心率入口点击 - 跳转心率页面
 */
void ui_event_MenuHRPanel(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
        #if HR_PAGE_EN
					Page_Load(&Page_HR);

        #else


        #endif
    }
}

/**
 * @brief  血氧入口点击 - 跳转血氧页面
 */
void ui_event_MenuO2Panel(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
        #if O2_PAGE_EN
            Page_Load(&Page_SPO2);
        #else


        #endif
    }
}

/**
 * @brief  环境入口点击 - 跳转环境监测页面
 */
void ui_event_MenuEnvPanel(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
        #if ENVIRONMENT_PAGE_EN
            Page_Load(&Page_Env);
        #else


        #endif
    }
}

/**
 * @brief  指南针入口点击 - 跳转指南针页面
 */
void ui_event_MenuCPPanel(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
        #if COMPASS_PAGE_EN
			Page_Load(&Page_Compass);
        #else


        #endif
    }
}

/**
 * @brief  游戏入口点击 - 跳转游戏选择页面
 */
void ui_event_MenuGamePanel(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
        #if GAME_PAGE_EN
			Page_Load(&Page_GameSelect);
        #else


        #endif

    }
}

/**
 * @brief  设置入口点击 - 跳转设置页面
 */
void ui_event_MenuSetPanel(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
        #if SET_PAGE_EN
            Page_Load(&Page_Set);
        #else


        #endif

    }
}

/**
 * @brief  关于入口点击 - 跳转关于页面
 */
void ui_event_MenuAbPanel(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
        #if ABOUT_PAGE_EN
            Page_Load(&Page_About);
        #else


        #endif
    }
}

/* ====== 页面初始化 ====== */

/**
 * @brief  菜单页面初始化 - 创建所有功能入口面板（日历、计算器、秒表、卡包、运动、心率、血氧、环境、指南针、游戏、设置、关于）
 */
void ui_MenuPage_screen_init(void)
{
    ui_MenuPage = lv_obj_create(NULL);

    /* ====== 1. 日历入口 ====== */
    ui_MenuCalPanel = lv_obj_create(ui_MenuPage);
    lv_obj_set_width(ui_MenuCalPanel, 240);
    lv_obj_set_height(ui_MenuCalPanel, 70);
    lv_obj_set_align(ui_MenuCalPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_MenuCalPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuCalPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuCalPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_MenuCalPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_MenuCalPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_MenuCalPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuCalPanel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_MenuCalPanel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_MenuCalButton = lv_btn_create(ui_MenuCalPanel);
    lv_obj_set_width(ui_MenuCalButton, 40);
    lv_obj_set_height(ui_MenuCalButton, 40);
    lv_obj_set_align(ui_MenuCalButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_MenuCalButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_MenuCalButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuCalButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuCalButton, lv_color_hex(0xFF8080), LV_PART_MAIN | LV_STATE_DEFAULT); // 粉红色
    lv_obj_set_style_bg_opa(ui_MenuCalButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuCalicon = lv_label_create(ui_MenuCalButton);
    lv_obj_set_width(ui_MenuCalicon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuCalicon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_MenuCalicon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_MenuCalicon, "\xEE\x98\x81"); // 日历图标
    lv_obj_set_style_text_font(ui_MenuCalicon, &ui_font_iconfont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuCalLabel = lv_label_create(ui_MenuCalPanel);
    lv_obj_set_width(ui_MenuCalLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuCalLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_MenuCalLabel, 60);
    lv_obj_set_y(ui_MenuCalLabel, 0);
    lv_obj_set_align(ui_MenuCalLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_MenuCalLabel, "\xE6\x97\xA5 \xE5\x8E\x86"); // 日历
    lv_obj_set_style_text_font(ui_MenuCalLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 2. 计算器入口 ====== */
    ui_MenuComPanel = lv_obj_create(ui_MenuPage);
    lv_obj_set_width(ui_MenuComPanel, 240);
    lv_obj_set_height(ui_MenuComPanel, 70);
    lv_obj_set_x(ui_MenuComPanel, 0);
    lv_obj_set_y(ui_MenuComPanel, 70);
    lv_obj_set_align(ui_MenuComPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_MenuComPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuComPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuComPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_MenuComPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_MenuComPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_MenuComPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuComPanel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_MenuComPanel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_MenuComButton = lv_btn_create(ui_MenuComPanel);
    lv_obj_set_width(ui_MenuComButton, 40);
    lv_obj_set_height(ui_MenuComButton, 40);
    lv_obj_set_align(ui_MenuComButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_MenuComButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_MenuComButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuComButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuComButton, lv_color_hex(0xC86400), LV_PART_MAIN | LV_STATE_DEFAULT); // 橙色
    lv_obj_set_style_bg_opa(ui_MenuComButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuComicon = lv_label_create(ui_MenuComButton);
    lv_obj_set_width(ui_MenuComicon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuComicon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_MenuComicon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_MenuComicon, "\xEE\xA2\x9B"); // 计算器图标
    lv_obj_set_style_text_font(ui_MenuComicon, &ui_font_iconfont28, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuComLabel = lv_label_create(ui_MenuComPanel);
    lv_obj_set_width(ui_MenuComLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuComLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_MenuComLabel, 60);
    lv_obj_set_y(ui_MenuComLabel, 0);
    lv_obj_set_align(ui_MenuComLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_MenuComLabel, "\xE8\xAE\xA1 \xE7\xAE\x97 \xE5\x99\xA8"); // 计算器
    lv_obj_set_style_text_font(ui_MenuComLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 3. 秒表入口 ====== */
    ui_MenuTimPanel = lv_obj_create(ui_MenuPage);
    lv_obj_set_width(ui_MenuTimPanel, 240);
    lv_obj_set_height(ui_MenuTimPanel, 70);
    lv_obj_set_x(ui_MenuTimPanel, 0);
    lv_obj_set_y(ui_MenuTimPanel, 140);
    lv_obj_set_align(ui_MenuTimPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_MenuTimPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuTimPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuTimPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_MenuTimPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_MenuTimPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_MenuTimPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuTimPanel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_MenuTimPanel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_MenuTimButton = lv_btn_create(ui_MenuTimPanel);
    lv_obj_set_width(ui_MenuTimButton, 40);
    lv_obj_set_height(ui_MenuTimButton, 40);
    lv_obj_set_align(ui_MenuTimButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_MenuTimButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_MenuTimButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuTimButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuTimButton, lv_color_hex(0xDC80E6), LV_PART_MAIN | LV_STATE_DEFAULT); // 紫色
    lv_obj_set_style_bg_opa(ui_MenuTimButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuTimicon = lv_label_create(ui_MenuTimButton);
    lv_obj_set_width(ui_MenuTimicon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuTimicon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_MenuTimicon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_MenuTimicon, "\xEE\x98\xB3"); // 秒表图标
    lv_obj_set_style_text_font(ui_MenuTimicon, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuTimLabel = lv_label_create(ui_MenuTimPanel);
    lv_obj_set_width(ui_MenuTimLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuTimLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_MenuTimLabel, 60);
    lv_obj_set_y(ui_MenuTimLabel, 0);
    lv_obj_set_align(ui_MenuTimLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_MenuTimLabel, "\xE7\xA7\x92 \xE8\xA1\xA8"); // 秒表
    lv_obj_set_style_text_font(ui_MenuTimLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 4. NFC卡包入口 ====== */
    ui_MenuCardPanel = lv_obj_create(ui_MenuPage);
    lv_obj_set_width(ui_MenuCardPanel, 240);
    lv_obj_set_height(ui_MenuCardPanel, 70);
    lv_obj_set_x(ui_MenuCardPanel, 0);
    lv_obj_set_y(ui_MenuCardPanel, 210);
    lv_obj_set_align(ui_MenuCardPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_MenuCardPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuCardPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuCardPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_MenuCardPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_MenuCardPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_MenuCardPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuCardPanel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_MenuCardPanel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_MenuCardButton = lv_btn_create(ui_MenuCardPanel);
    lv_obj_set_width(ui_MenuCardButton, 40);
    lv_obj_set_height(ui_MenuCardButton, 40);
    lv_obj_set_align(ui_MenuCardButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_MenuCardButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_MenuCardButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuCardButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuCardButton, lv_color_hex(0x80641E), LV_PART_MAIN | LV_STATE_DEFAULT); // 棕色
    lv_obj_set_style_bg_opa(ui_MenuCardButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuCardicon = lv_label_create(ui_MenuCardButton);
    lv_obj_set_width(ui_MenuCardicon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuCardicon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_MenuCardicon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_MenuCardicon, "\xEE\x9A\xBF");
    lv_obj_set_style_text_font(ui_MenuCardicon, &ui_font_iconfont32, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuCardLabel = lv_label_create(ui_MenuCardPanel);
    lv_obj_set_width(ui_MenuCardLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuCardLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_MenuCardLabel, 60);
    lv_obj_set_y(ui_MenuCardLabel, 0);
    lv_obj_set_align(ui_MenuCardLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_MenuCardLabel, "\xE5\x8D\xA1 \xE5\x8C\x85"); // 卡包
    lv_obj_set_style_text_font(ui_MenuCardLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 5. 运动入口 ====== */
    ui_MenuSprPanel = lv_obj_create(ui_MenuPage);
    lv_obj_set_width(ui_MenuSprPanel, 240);
    lv_obj_set_height(ui_MenuSprPanel, 70);
    lv_obj_set_x(ui_MenuSprPanel, 0);
    lv_obj_set_y(ui_MenuSprPanel, 280);
    lv_obj_set_align(ui_MenuSprPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_MenuSprPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuSprPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuSprPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_MenuSprPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_MenuSprPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_MenuSprPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuSprPanel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_MenuSprPanel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_MenuSprButton = lv_btn_create(ui_MenuSprPanel);
    lv_obj_set_width(ui_MenuSprButton, 40);
    lv_obj_set_height(ui_MenuSprButton, 40);
    lv_obj_set_align(ui_MenuSprButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_MenuSprButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_MenuSprButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuSprButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuSprButton, lv_color_hex(0x009680), LV_PART_MAIN | LV_STATE_DEFAULT); // 绿色
    lv_obj_set_style_bg_opa(ui_MenuSprButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuSpricon = lv_label_create(ui_MenuSprButton);
    lv_obj_set_width(ui_MenuSpricon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuSpricon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_MenuSpricon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_MenuSpricon, "\xEE\x98\x9C");
    lv_obj_set_style_text_font(ui_MenuSpricon, &ui_font_iconfont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuSprLabel = lv_label_create(ui_MenuSprPanel);
    lv_obj_set_width(ui_MenuSprLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuSprLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_MenuSprLabel, 60);
    lv_obj_set_y(ui_MenuSprLabel, 0);
    lv_obj_set_align(ui_MenuSprLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_MenuSprLabel, "\xE8\xBF\x90 \xE5\x8A\xA8"); // 运动
    lv_obj_set_style_text_font(ui_MenuSprLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 6. 心率入口 ====== */
    ui_MenuHRPanel = lv_obj_create(ui_MenuPage);
    lv_obj_set_width(ui_MenuHRPanel, 240);
    lv_obj_set_height(ui_MenuHRPanel, 70);
    lv_obj_set_x(ui_MenuHRPanel, 0);
    lv_obj_set_y(ui_MenuHRPanel, 350);
    lv_obj_set_align(ui_MenuHRPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_MenuHRPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuHRPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuHRPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_MenuHRPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_MenuHRPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_MenuHRPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuHRPanel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_MenuHRPanel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_MenuHRButton = lv_btn_create(ui_MenuHRPanel);
    lv_obj_set_width(ui_MenuHRButton, 40);
    lv_obj_set_height(ui_MenuHRButton, 40);
    lv_obj_set_align(ui_MenuHRButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_MenuHRButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_MenuHRButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuHRButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuHRButton, lv_color_hex(0xC80000), LV_PART_MAIN | LV_STATE_DEFAULT); // 红色
    lv_obj_set_style_bg_opa(ui_MenuHRButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuHRicon = lv_label_create(ui_MenuHRButton);
    lv_obj_set_width(ui_MenuHRicon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuHRicon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_MenuHRicon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_MenuHRicon, "\xEE\x9D\xA2");
    lv_obj_set_style_text_font(ui_MenuHRicon, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuHRLabel = lv_label_create(ui_MenuHRPanel);
    lv_obj_set_width(ui_MenuHRLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuHRLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_MenuHRLabel, 60);
    lv_obj_set_y(ui_MenuHRLabel, 0);
    lv_obj_set_align(ui_MenuHRLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_MenuHRLabel, "\xE5\xBF\x83 \xE7\x8E\x87"); // 心率
    lv_obj_set_style_text_font(ui_MenuHRLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 7. 血氧入口 ====== */
    ui_MenuO2Panel = lv_obj_create(ui_MenuPage);
    lv_obj_set_width(ui_MenuO2Panel, 240);
    lv_obj_set_height(ui_MenuO2Panel, 70);
    lv_obj_set_x(ui_MenuO2Panel, 0);
    lv_obj_set_y(ui_MenuO2Panel, 420);
    lv_obj_set_align(ui_MenuO2Panel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_MenuO2Panel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuO2Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuO2Panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_MenuO2Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_MenuO2Panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_MenuO2Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuO2Panel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_MenuO2Panel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_MenuO2Button = lv_btn_create(ui_MenuO2Panel);
    lv_obj_set_width(ui_MenuO2Button, 40);
    lv_obj_set_height(ui_MenuO2Button, 40);
    lv_obj_set_align(ui_MenuO2Button, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_MenuO2Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_MenuO2Button, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuO2Button, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuO2Button, lv_color_hex(0x0080FF), LV_PART_MAIN | LV_STATE_DEFAULT); // 蓝色
    lv_obj_set_style_bg_opa(ui_MenuO2Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuO2icon = lv_label_create(ui_MenuO2Button);
    lv_obj_set_width(ui_MenuO2icon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuO2icon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_MenuO2icon, 2);
    lv_obj_set_y(ui_MenuO2icon, 0);
    lv_obj_set_align(ui_MenuO2icon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_MenuO2icon, "\xEE\x99\x92");
    lv_obj_set_style_text_font(ui_MenuO2icon, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuO2Label = lv_label_create(ui_MenuO2Panel);
    lv_obj_set_width(ui_MenuO2Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuO2Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_MenuO2Label, 60);
    lv_obj_set_y(ui_MenuO2Label, 0);
    lv_obj_set_align(ui_MenuO2Label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_MenuO2Label, "\xE8\xA1\x80 \xE6\xB0\xA7"); // 血氧
    lv_obj_set_style_text_font(ui_MenuO2Label, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 8. 环境入口 ====== */
    ui_MenuEnvPanel = lv_obj_create(ui_MenuPage);
    lv_obj_set_width(ui_MenuEnvPanel, 240);
    lv_obj_set_height(ui_MenuEnvPanel, 70);
    lv_obj_set_x(ui_MenuEnvPanel, 0);
    lv_obj_set_y(ui_MenuEnvPanel, 490);
    lv_obj_set_align(ui_MenuEnvPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_MenuEnvPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuEnvPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuEnvPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_MenuEnvPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_MenuEnvPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_MenuEnvPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuEnvPanel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_MenuEnvPanel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_MenuEnvButton = lv_btn_create(ui_MenuEnvPanel);
    lv_obj_set_width(ui_MenuEnvButton, 40);
    lv_obj_set_height(ui_MenuEnvButton, 40);
    lv_obj_set_align(ui_MenuEnvButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_MenuEnvButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_MenuEnvButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuEnvButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuEnvButton, lv_color_hex(0x009632), LV_PART_MAIN | LV_STATE_DEFAULT); // 绿色
    lv_obj_set_style_bg_opa(ui_MenuEnvButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuEnvicon = lv_label_create(ui_MenuEnvButton);
    lv_obj_set_width(ui_MenuEnvicon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuEnvicon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_MenuEnvicon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_MenuEnvicon, "\xEE\x9C\x86");
    lv_obj_set_style_text_font(ui_MenuEnvicon, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuEnvLabel = lv_label_create(ui_MenuEnvPanel);
    lv_obj_set_width(ui_MenuEnvLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuEnvLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_MenuEnvLabel, 60);
    lv_obj_set_y(ui_MenuEnvLabel, 0);
    lv_obj_set_align(ui_MenuEnvLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_MenuEnvLabel, "\xE7\x8E\xAF \xE5\xA2\x83"); // 环境
    lv_obj_set_style_text_font(ui_MenuEnvLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 9. 指南针入口 ====== */
    ui_MenuCPPanel = lv_obj_create(ui_MenuPage);
    lv_obj_set_width(ui_MenuCPPanel, 240);
    lv_obj_set_height(ui_MenuCPPanel, 70);
    lv_obj_set_x(ui_MenuCPPanel, 0);
    lv_obj_set_y(ui_MenuCPPanel, 560);
    lv_obj_set_align(ui_MenuCPPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_MenuCPPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuCPPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuCPPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_MenuCPPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_MenuCPPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_MenuCPPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuCPPanel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_MenuCPPanel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_MenuCPButton = lv_btn_create(ui_MenuCPPanel);
    lv_obj_set_width(ui_MenuCPButton, 40);
    lv_obj_set_height(ui_MenuCPButton, 40);
    lv_obj_set_align(ui_MenuCPButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_MenuCPButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_MenuCPButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuCPButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuCPButton, lv_color_hex(0x800080), LV_PART_MAIN | LV_STATE_DEFAULT); // 紫色
    lv_obj_set_style_bg_opa(ui_MenuCPButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuCPicon = lv_label_create(ui_MenuCPButton);
    lv_obj_set_width(ui_MenuCPicon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuCPicon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_MenuCPicon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_MenuCPicon, "\xEE\x9E\x88");
    lv_obj_set_style_text_font(ui_MenuCPicon, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuCPLabel = lv_label_create(ui_MenuCPPanel);
    lv_obj_set_width(ui_MenuCPLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuCPLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_MenuCPLabel, 60);
    lv_obj_set_y(ui_MenuCPLabel, 0);
    lv_obj_set_align(ui_MenuCPLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_MenuCPLabel, "\xE6\x8C\x87 \xE5\x8D\x97 \xE9\x92\x88"); // 指南针
    lv_obj_set_style_text_font(ui_MenuCPLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 10. 游戏入口 ====== */
    ui_MenuGamePanel = lv_obj_create(ui_MenuPage);
    lv_obj_set_width(ui_MenuGamePanel, 240);
    lv_obj_set_height(ui_MenuGamePanel, 70);
    lv_obj_set_x(ui_MenuGamePanel, 0);
    lv_obj_set_y(ui_MenuGamePanel, 630);
    lv_obj_set_align(ui_MenuGamePanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_MenuGamePanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuGamePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuGamePanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_MenuGamePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_MenuGamePanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_MenuGamePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuGamePanel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_MenuGamePanel, 100, LV_PART_MAIN | LV_STATE_PRESSED);


    ui_MenuGameButton = lv_btn_create(ui_MenuGamePanel);
    lv_obj_set_width(ui_MenuGameButton, 40);
    lv_obj_set_height(ui_MenuGameButton, 40);
    lv_obj_set_align(ui_MenuGameButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_MenuGameButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_MenuGameButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuGameButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuGameButton, lv_color_hex(0xC07010), LV_PART_MAIN | LV_STATE_DEFAULT); // 金色
    lv_obj_set_style_bg_opa(ui_MenuGameButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuGameicon = lv_label_create(ui_MenuGameButton);
    lv_obj_set_width(ui_MenuGameicon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuGameicon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_MenuGameicon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_MenuGameicon, "\xEE\x98\xB5");
    lv_obj_set_style_text_font(ui_MenuGameicon, &ui_font_iconfont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuGameLabel = lv_label_create(ui_MenuGamePanel);
    lv_obj_set_width(ui_MenuGameLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuGameLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_MenuGameLabel, 60);
    lv_obj_set_y(ui_MenuGameLabel, 0);
    lv_obj_set_align(ui_MenuGameLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_MenuGameLabel, "\xE6\xB8\xB8 \xE6\x88\x8F"); // 游戏
    lv_obj_set_style_text_font(ui_MenuGameLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 11. 设置入口 ====== */
    ui_MenuSetPanel = lv_obj_create(ui_MenuPage);
    lv_obj_set_width(ui_MenuSetPanel, 240);
    lv_obj_set_height(ui_MenuSetPanel, 70);
    lv_obj_set_x(ui_MenuSetPanel, 0);
    lv_obj_set_y(ui_MenuSetPanel, 700);
    lv_obj_set_align(ui_MenuSetPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_MenuSetPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuSetPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuSetPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_MenuSetPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_MenuSetPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_MenuSetPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuSetPanel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_MenuSetPanel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_MenuSetButton = lv_btn_create(ui_MenuSetPanel);
    lv_obj_set_width(ui_MenuSetButton, 40);
    lv_obj_set_height(ui_MenuSetButton, 40);
    lv_obj_set_align(ui_MenuSetButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_MenuSetButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_MenuSetButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuSetButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuSetButton, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT); // 灰色
    lv_obj_set_style_bg_opa(ui_MenuSetButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuSeticon = lv_label_create(ui_MenuSetButton);
    lv_obj_set_width(ui_MenuSeticon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuSeticon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_MenuSeticon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_MenuSeticon, "\xEE\x98\x80");
    lv_obj_set_style_text_font(ui_MenuSeticon, &ui_font_iconfont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuSetLabel = lv_label_create(ui_MenuSetPanel);
    lv_obj_set_width(ui_MenuSetLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuSetLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_MenuSetLabel, 60);
    lv_obj_set_y(ui_MenuSetLabel, 0);
    lv_obj_set_align(ui_MenuSetLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_MenuSetLabel, "\xE8\xAE\xBE \xE7\xBD\xAE"); // 设置
    lv_obj_set_style_text_font(ui_MenuSetLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 12. 关于入口 ====== */
    ui_MenuAbPanel = lv_obj_create(ui_MenuPage);
    lv_obj_set_width(ui_MenuAbPanel, 240);
    lv_obj_set_height(ui_MenuAbPanel, 70);
    lv_obj_set_x(ui_MenuAbPanel, 0);
    lv_obj_set_y(ui_MenuAbPanel, 770);
    lv_obj_set_align(ui_MenuAbPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_MenuAbPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuAbPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuAbPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_MenuAbPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_MenuAbPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_MenuAbPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuAbPanel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_MenuAbPanel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_MenuAbButton = lv_btn_create(ui_MenuAbPanel);
    lv_obj_set_width(ui_MenuAbButton, 40);
    lv_obj_set_height(ui_MenuAbButton, 40);
    lv_obj_set_align(ui_MenuAbButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_MenuAbButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_MenuAbButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_MenuAbButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_MenuAbButton, lv_color_hex(0x646464), LV_PART_MAIN | LV_STATE_DEFAULT); // 深灰色
    lv_obj_set_style_bg_opa(ui_MenuAbButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuAbicon = lv_label_create(ui_MenuAbButton);
    lv_obj_set_width(ui_MenuAbicon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuAbicon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_MenuAbicon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_MenuAbicon, "\xEE\x98\x8B");
    lv_obj_set_style_text_font(ui_MenuAbicon, &ui_font_iconfont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_MenuAbLabel = lv_label_create(ui_MenuAbPanel);
    lv_obj_set_width(ui_MenuAbLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_MenuAbLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_MenuAbLabel, 60);
    lv_obj_set_y(ui_MenuAbLabel, 0);
    lv_obj_set_align(ui_MenuAbLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_MenuAbLabel, "\xE5\x85\xB3 \xE4\xBA\x8E"); // 关于
    lv_obj_set_style_text_font(ui_MenuAbLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 恢复到之前记录的滚动位置
	lv_obj_scroll_to(ui_MenuPage,0,ui_MenuScrollY,LV_ANIM_OFF);

	// 注册所有入口的点击事件
	lv_obj_add_event_cb(ui_MenuPage, ui_event_MenuPage, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui_MenuCalPanel, ui_event_MenuCalPanel, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui_MenuComPanel, ui_event_MenuComPanel, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui_MenuTimPanel, ui_event_MenuTimPanel, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui_MenuCardPanel, ui_event_MenuCardPanel, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui_MenuSprPanel, ui_event_MenuSprPanel, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui_MenuHRPanel, ui_event_MenuHRPanel, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui_MenuO2Panel, ui_event_MenuO2Panel, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui_MenuEnvPanel, ui_event_MenuEnvPanel, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui_MenuCPPanel, ui_event_MenuCPPanel, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui_MenuGamePanel, ui_event_MenuGamePanel, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui_MenuSetPanel, ui_event_MenuSetPanel, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(ui_MenuAbPanel, ui_event_MenuAbPanel, LV_EVENT_ALL, NULL);


}

/* ====== 页面注销 ====== */

/**
 * @brief  菜单页面注销 - 清理资源
 */
void ui_MenuPage_screen_deinit(void)
{}
