/** @file ui_SetPage.c
 *  @brief 设置页面实现 - 常亮时间、熄屏时间、日期时间设置、抬腕亮屏开关、密码设置等入口
 */
#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_HomePage.h"
#include "../Inc/ui_MenuPage.h"
#include "../Inc/ui_SetPage.h"
#include "../Inc/ui_OffTimePage.h"
#include "../Inc/ui_DateTimeSetPage.h"

#include "../../../Func/Inc/HWDataAccess.h"

/* ====== 页面管理器 ====== */
Page_t Page_Set = {ui_SetPage_screen_init, ui_SetPage_screen_deinit, &ui_SetPage};

/* ====== 变量定义 ====== */
lv_obj_t * ui_SetPage; // 设置页面对象

// 常亮时间
lv_obj_t * ui_LightTimePanel; // 常亮时间面板
lv_obj_t * ui_LightTimeButton; // 常亮时间图标按钮
lv_obj_t * ui_LightTimeicon; // 常亮时间图标
lv_obj_t * ui_LightTimeLabel; // 常亮时间标签

// 熄屏时间
lv_obj_t * ui_TurnOffTimePanel; // 熄屏时间面板
lv_obj_t * ui_TOffTimeButton; // 熄屏时间图标按钮
lv_obj_t * ui_TOffTimeicon; // 熄屏时间图标
lv_obj_t * ui_TOffTimeLabel; // 熄屏时间标签

// 日期时间设置
lv_obj_t * ui_DateTimeSetPanel; // 日期时间设置面板
lv_obj_t * ui_DateTimeSetButton; // 日期时间设置图标按钮
lv_obj_t * ui_LOffTimeicon; // 日期时间设置图标
lv_obj_t * ui_DateTimeSetLabel; // 日期时间设置标签

// 抬腕亮屏
lv_obj_t * ui_WristPanel; // 抬腕亮屏面板
lv_obj_t * ui_WristButton; // 抬腕亮屏图标按钮
lv_obj_t * ui_Wristicon; // 抬腕亮屏图标
lv_obj_t * ui_WristLabel; // 抬腕亮屏标签
lv_obj_t * ui_WristSwitch; // 抬腕亮屏开关

// 密码设置
lv_obj_t * ui_PasswordPanel; // 密码设置面板
lv_obj_t * ui_PasswordButton; // 密码设置图标按钮
lv_obj_t * ui_Passwordicon; // 密码设置图标
lv_obj_t * ui_PasswordLabel; // 密码设置标签

/* ====== 功能函数 ====== */

/**
 * @brief  设置页面手势事件 - 右滑返回
 * @param  e LVGL事件对象
 */
void ui_event_SetPage(lv_event_t * e)
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
}

/**
 * @brief  常亮时间入口点击 - 跳转常亮时间设置
 * @param  e LVGL事件对象
 */
void ui_event_LightTimePanel(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
        Page_Load(&Page_LOffTimeSet);
    }
}

/**
 * @brief  熄屏时间入口点击 - 跳转熄屏时间设置
 * @param  e LVGL事件对象
 */
void ui_event_TOffTimePanel(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
        Page_Load(&Page_TOffTimeSet);
    }
}

/**
 * @brief  抬腕亮屏开关事件 - 通过IMU传感器使能/禁用抬腕唤醒功能
 * @param  e LVGL事件对象
 */
void ui_event_WristSwitch(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED)
    {
       if(lv_obj_has_state(ui_WristSwitch,LV_STATE_CHECKED))
       {
            HWInterface.IMU.WristEnable(); // 开启抬腕亮屏
       }
       else
       {
			HWInterface.IMU.WristDisable(); // 关闭抬腕亮屏
       }
    }
}

/**
 * @brief  日期时间设置入口点击 - 跳转日期时间设置
 * @param  e LVGL事件对象
 */
void ui_event_DateTimeSetPanel(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
        Page_Load(&Page_DateTimeSet);
    }
}

/**
 * @brief  密码设置入口点击（预留功能）
 * @param  e LVGL事件对象
 */
void ui_event_PasswordPanel(lv_event_t * e)
{
	lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {

    }
}


/**
 * @brief  设置页面初始化 - 创建常亮时间、熄屏时间、日期时间、抬腕亮屏、密码设置等入口面板
 */
void ui_SetPage_screen_init(void)
{
    ui_SetPage = lv_obj_create(NULL);

    /* ====== 1. 常亮时间入口 ====== */
    ui_LightTimePanel = lv_obj_create(ui_SetPage);
    lv_obj_set_width(ui_LightTimePanel, 240);
    lv_obj_set_height(ui_LightTimePanel, 70);
    lv_obj_set_align(ui_LightTimePanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_LightTimePanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_LightTimePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_LightTimePanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_LightTimePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_LightTimePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_LightTimePanel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_LightTimePanel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_LightTimeButton = lv_btn_create(ui_LightTimePanel);
    lv_obj_set_width(ui_LightTimeButton, 40);
    lv_obj_set_height(ui_LightTimeButton, 40);
    lv_obj_set_align(ui_LightTimeButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_LightTimeButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_LightTimeButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_LightTimeButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_LightTimeButton, lv_color_hex(0x0080FF), LV_PART_MAIN | LV_STATE_DEFAULT); // 蓝色
    lv_obj_set_style_bg_opa(ui_LightTimeButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LightTimeicon = lv_label_create(ui_LightTimeButton);
    lv_obj_set_width(ui_LightTimeicon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LightTimeicon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_LightTimeicon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LightTimeicon, "\xEE\x98\x8E");
    lv_obj_set_style_text_font(ui_LightTimeicon, &ui_font_iconfont32, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LightTimeLabel = lv_label_create(ui_LightTimePanel);
    lv_obj_set_width(ui_LightTimeLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LightTimeLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_LightTimeLabel, 60);
    lv_obj_set_y(ui_LightTimeLabel, 0);
    lv_obj_set_align(ui_LightTimeLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_LightTimeLabel, "\xE5\xB8\xB8\xE4\xBA\xAE\xE6\x97\xB6\xE9\x97\xB4"); // 常亮时间
    lv_obj_set_style_text_font(ui_LightTimeLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 2. 熄屏时间入口 ====== */
    ui_TurnOffTimePanel = lv_obj_create(ui_SetPage);
    lv_obj_set_width(ui_TurnOffTimePanel, 240);
    lv_obj_set_height(ui_TurnOffTimePanel, 70);
    lv_obj_set_x(ui_TurnOffTimePanel, 0);
    lv_obj_set_y(ui_TurnOffTimePanel, 70);
    lv_obj_set_align(ui_TurnOffTimePanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_TurnOffTimePanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_TurnOffTimePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_TurnOffTimePanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_TurnOffTimePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_TurnOffTimePanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_TurnOffTimePanel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_TurnOffTimePanel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_TOffTimeButton = lv_btn_create(ui_TurnOffTimePanel);
    lv_obj_set_width(ui_TOffTimeButton, 40);
    lv_obj_set_height(ui_TOffTimeButton, 40);
    lv_obj_set_align(ui_TOffTimeButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_TOffTimeButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_TOffTimeButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_TOffTimeButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_TOffTimeButton, lv_color_hex(0xE18019), LV_PART_MAIN | LV_STATE_DEFAULT); // 橙色
    lv_obj_set_style_bg_opa(ui_TOffTimeButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_TOffTimeicon = lv_label_create(ui_TOffTimeButton);
    lv_obj_set_width(ui_TOffTimeicon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_TOffTimeicon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_TOffTimeicon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_TOffTimeicon, "\xEE\x9A\x96");
    lv_obj_set_style_text_font(ui_TOffTimeicon, &ui_font_iconfont45, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_TOffTimeLabel = lv_label_create(ui_TurnOffTimePanel);
    lv_obj_set_width(ui_TOffTimeLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_TOffTimeLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_TOffTimeLabel, 60);
    lv_obj_set_y(ui_TOffTimeLabel, 0);
    lv_obj_set_align(ui_TOffTimeLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_TOffTimeLabel, "\xE7\x86\x84\xE5\xB1\x8F\xE6\x97\xB6\xE9\x97\xB4"); // 熄屏时间
    lv_obj_set_style_text_font(ui_TOffTimeLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 3. 日期时间设置入口 ====== */
    ui_DateTimeSetPanel = lv_obj_create(ui_SetPage);
    lv_obj_set_width(ui_DateTimeSetPanel, 240);
    lv_obj_set_height(ui_DateTimeSetPanel, 70);
    lv_obj_set_x(ui_DateTimeSetPanel, 0);
    lv_obj_set_y(ui_DateTimeSetPanel, 140);
    lv_obj_set_align(ui_DateTimeSetPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_DateTimeSetPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_DateTimeSetPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_DateTimeSetPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_DateTimeSetPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_DateTimeSetPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_DateTimeSetPanel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_DateTimeSetPanel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_DateTimeSetButton = lv_btn_create(ui_DateTimeSetPanel);
    lv_obj_set_width(ui_DateTimeSetButton, 40);
    lv_obj_set_height(ui_DateTimeSetButton, 40);
    lv_obj_set_align(ui_DateTimeSetButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_DateTimeSetButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_DateTimeSetButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_DateTimeSetButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_DateTimeSetButton, lv_color_hex(0x0080FF), LV_PART_MAIN | LV_STATE_DEFAULT); // 蓝色
    lv_obj_set_style_bg_opa(ui_DateTimeSetButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LOffTimeicon = lv_label_create(ui_DateTimeSetButton);
    lv_obj_set_width(ui_LOffTimeicon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LOffTimeicon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_LOffTimeicon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LOffTimeicon, "\xEE\x9B\xA4");
    lv_obj_set_style_text_font(ui_LOffTimeicon, &ui_font_iconfont28, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_DateTimeSetLabel = lv_label_create(ui_DateTimeSetPanel);
    lv_obj_set_width(ui_DateTimeSetLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_DateTimeSetLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_DateTimeSetLabel, 60);
    lv_obj_set_y(ui_DateTimeSetLabel, 0);
    lv_obj_set_align(ui_DateTimeSetLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_DateTimeSetLabel, "\xE6\x97\xB6\xE9\x97\xB4\xE6\x97\xA5\xE6\x9C\x9F"); // 时间日期
    lv_obj_set_style_text_font(ui_DateTimeSetLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 4. 抬腕亮屏入口 ====== */
    ui_WristPanel = lv_obj_create(ui_SetPage);
    lv_obj_set_width(ui_WristPanel, 240);
    lv_obj_set_height(ui_WristPanel, 70);
    lv_obj_set_x(ui_WristPanel, 0);
    lv_obj_set_y(ui_WristPanel, 210);
    lv_obj_set_align(ui_WristPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_WristPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_WristPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_WristPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_WristPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_WristPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_WristButton = lv_btn_create(ui_WristPanel);
    lv_obj_set_width(ui_WristButton, 40);
    lv_obj_set_height(ui_WristButton, 40);
    lv_obj_set_align(ui_WristButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_WristButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_WristButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_WristButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_WristButton, lv_color_hex(0x7D197D), LV_PART_MAIN | LV_STATE_DEFAULT); // 紫色
    lv_obj_set_style_bg_opa(ui_WristButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Wristicon = lv_label_create(ui_WristButton);
    lv_obj_set_width(ui_Wristicon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Wristicon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Wristicon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Wristicon, "\xEE\x98\xAA");
    lv_obj_set_style_text_font(ui_Wristicon, &ui_font_iconfont45, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_WristLabel = lv_label_create(ui_WristPanel);
    lv_obj_set_width(ui_WristLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_WristLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_WristLabel, 60);
    lv_obj_set_y(ui_WristLabel, 0);
    lv_obj_set_align(ui_WristLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_WristLabel, "\xE6\x8A\xAC\xE8\x85\x95\xE4\xBA\xAE\xE5\xB1\x8F"); // 抬腕亮屏
    lv_obj_set_style_text_font(ui_WristLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_WristSwitch = lv_switch_create(ui_WristPanel); // 开关控件
    lv_obj_set_width(ui_WristSwitch, 55);
    lv_obj_set_height(ui_WristSwitch, 25);
    lv_obj_set_align(ui_WristSwitch, LV_ALIGN_RIGHT_MID);

		if(HWInterface.IMU.wrist_is_enabled) // 根据当前传感器状态设置开关
		{lv_obj_add_state(ui_WristSwitch, LV_STATE_CHECKED);}

    /* ====== 5. 密码设置入口 ====== */
    ui_PasswordPanel = lv_obj_create(ui_SetPage);
    lv_obj_set_width(ui_PasswordPanel, 240);
    lv_obj_set_height(ui_PasswordPanel, 70);
    lv_obj_set_x(ui_PasswordPanel, 0);
    lv_obj_set_y(ui_PasswordPanel, 280);
    lv_obj_set_align(ui_PasswordPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_PasswordPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_PasswordPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_PasswordPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_PasswordPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_PasswordPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_PasswordPanel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_PasswordPanel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_PasswordButton = lv_btn_create(ui_PasswordPanel);
    lv_obj_set_width(ui_PasswordButton, 40);
    lv_obj_set_height(ui_PasswordButton, 40);
    lv_obj_set_align(ui_PasswordButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_PasswordButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_PasswordButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_PasswordButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_PasswordButton, lv_color_hex(0xE18019), LV_PART_MAIN | LV_STATE_DEFAULT); // 橙色
    lv_obj_set_style_bg_opa(ui_PasswordButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Passwordicon = lv_label_create(ui_PasswordButton);
    lv_obj_set_width(ui_Passwordicon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Passwordicon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Passwordicon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Passwordicon, "\xEE\x99\x9C");
    lv_obj_set_style_text_font(ui_Passwordicon, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_PasswordLabel = lv_label_create(ui_PasswordPanel);
    lv_obj_set_width(ui_PasswordLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_PasswordLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_PasswordLabel, 60);
    lv_obj_set_y(ui_PasswordLabel, 0);
    lv_obj_set_align(ui_PasswordLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_PasswordLabel, "\xE5\xAF\x86\xE7\xA0\x81\xE8\xAE\xBE\xE7\xBD\xAE"); // 密码设置
    lv_obj_set_style_text_font(ui_PasswordLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 注册事件回调
		lv_obj_add_event_cb(ui_SetPage, ui_event_SetPage, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_LightTimePanel, ui_event_LightTimePanel, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_TurnOffTimePanel, ui_event_TOffTimePanel, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_WristSwitch, ui_event_WristSwitch, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_DateTimeSetPanel, ui_event_DateTimeSetPanel, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_PasswordPanel, ui_event_PasswordPanel, LV_EVENT_ALL, NULL);

}

/**
 * @brief  设置页面注销 - 清理资源
 */
void ui_SetPage_screen_deinit(void)
{}
