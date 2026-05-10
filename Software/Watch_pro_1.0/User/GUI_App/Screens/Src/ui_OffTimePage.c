/** @file ui_OffTimePage.c
 *  @brief 熄屏/常亮时间设置页面实现 - 通过滚轮选择预设时间值（秒），点击确认后保存
 */
#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_HomePage.h"
#include "../Inc/ui_SetPage.h"
#include "../Inc/ui_OffTimePage.h"

/* ====== 页面管理器 ====== */
Page_t Page_LOffTimeSet = {ui_LOffTimeSetPage_screen_init, ui_LOffTimeSetPage_screen_deinit, &ui_LOffTimeSetPage};
Page_t Page_TOffTimeSet = {ui_TOffTimeSetPage_screen_init, ui_TOffTimeSetPage_screen_deinit, &ui_TOffTimeSetPage};

/* ====== 变量定义 ====== */
// 常亮时间设置
lv_obj_t * ui_LOffTimeSetPage; // 常亮时间设置页面
lv_obj_t * ui_LTimeSetRoller; // 常亮时间滚轮
lv_obj_t * ui_LTimeSetUnitLabel; // 常亮时间单位标签（秒）
lv_obj_t * ui_LTimeSetOKButton; // 常亮时间确认按钮
lv_obj_t * ui_LTimeSetOKLabel; // 常亮时间确认标签
lv_obj_t * ui_LTimeSeticon; // 常亮时间图标

// 熄屏时间设置
lv_obj_t * ui_TOffTimeSetPage; // 熄屏时间设置页面
lv_obj_t * ui_TTimeSetRoller; // 熄屏时间滚轮
lv_obj_t * ui_TTimeSetUnitLabel; // 熄屏时间单位标签（秒）
lv_obj_t * ui_TTimeSetOKButton; // 熄屏时间确认按钮
lv_obj_t * ui_TTimeSetOKLabel; // 熄屏时间确认标签
lv_obj_t * ui_TTimeSeticon; // 熄屏时间图标

const uint8_t ui_LTimeOptions[6] = {10,15,20,30,45,60}; // 常亮时间选项（秒）
const uint8_t ui_TTimeOptions[6] = {15,20,25,35,50,65}; // 熄屏时间选项（秒）
uint8_t ui_LTimeValue = 10; // 当前常亮时间
uint8_t ui_TTimeValue = 15; // 当前熄屏时间

uint8_t ui_LTimeSelected = 0; // 常亮时间滚轮选中索引
uint8_t ui_TTimeSelected = 0; // 熄屏时间滚轮选中索引

/**
 * @brief  常亮时间确认按钮事件 - 保存选中值并返回
 * @param  e LVGL事件对象
 */
void ui_event_LTimeSetOKButton(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_CLICKED)
    {
      ui_LTimeSelected = lv_roller_get_selected(ui_LTimeSetRoller); // 获取选中的索引
			ui_LTimeValue = ui_LTimeOptions[ui_LTimeSelected]; // 根据索引查表得到实际值
      Page_Back(); // 返回上一页
    }
}

/**
 * @brief  熄屏时间确认按钮事件 - 保存选中值并返回
 * @param  e LVGL事件对象
 */
void ui_event_TTimeSetOKButton(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_CLICKED)
    {
			ui_TTimeSelected = lv_roller_get_selected(ui_TTimeSetRoller);
      ui_TTimeValue = ui_LTimeOptions[ui_TTimeSelected];
      Page_Back();
    }
}

/**
 * @brief  常亮时间设置页面初始化 - 创建滚轮选择10~60秒，底部确认按钮
 */
void ui_LOffTimeSetPage_screen_init(void)
{
    ui_LOffTimeSetPage = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_LOffTimeSetPage, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    // 时间滚轮（10/15/20/30/45/60秒）
    ui_LTimeSetRoller = lv_roller_create(ui_LOffTimeSetPage);

    lv_roller_set_options(ui_LTimeSetRoller, "10\n15\n20\n30\n45\n60", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(ui_LTimeSetRoller,ui_LTimeSelected,LV_ANIM_OFF);
    lv_obj_set_height(ui_LTimeSetRoller, 150);
    lv_obj_set_width(ui_LTimeSetRoller, LV_SIZE_CONTENT);   /// 0
    lv_obj_set_align(ui_LTimeSetRoller, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(ui_LTimeSetRoller, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LTimeSetRoller, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LTimeSetRoller, &ui_font_Cuyuan38, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_LTimeSetRoller, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_LTimeSetRoller, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_LTimeSetRoller, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_color(ui_LTimeSetRoller, lv_color_hex(0xFFFFFF), LV_PART_SELECTED | LV_STATE_DEFAULT); // 选中项白色
    lv_obj_set_style_text_opa(ui_LTimeSetRoller, 255, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_LTimeSetRoller, lv_color_hex(0xFFFFFF), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_LTimeSetRoller, 0, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(ui_LTimeSetRoller, lv_color_hex(0x0064FF), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(ui_LTimeSetRoller, 255, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui_LTimeSetRoller, 2, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(ui_LTimeSetRoller, 1, LV_PART_SELECTED | LV_STATE_DEFAULT);

    // 单位标签（秒）
    ui_LTimeSetUnitLabel = lv_label_create(ui_LOffTimeSetPage);
    lv_obj_set_width(ui_LTimeSetUnitLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LTimeSetUnitLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_LTimeSetUnitLabel, 60);
    lv_obj_set_y(ui_LTimeSetUnitLabel, 0);
    lv_obj_set_align(ui_LTimeSetUnitLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LTimeSetUnitLabel, "\xE7\xA7\x92"); // 秒
    lv_obj_set_style_text_font(ui_LTimeSetUnitLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 确认按钮
    ui_LTimeSetOKButton = lv_btn_create(ui_LOffTimeSetPage);
    lv_obj_set_width(ui_LTimeSetOKButton, 240);
    lv_obj_set_height(ui_LTimeSetOKButton, 50);
    lv_obj_set_align(ui_LTimeSetOKButton, LV_ALIGN_BOTTOM_MID);
    lv_obj_add_flag(ui_LTimeSetOKButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_LTimeSetOKButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_LTimeSetOKButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_LTimeSetOKButton, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_LTimeSetOKButton, 128, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_LTimeSetOKButton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_LTimeSetOKButton, 128, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_LTimeSetOKLabel = lv_label_create(ui_LTimeSetOKButton);
    lv_obj_set_width(ui_LTimeSetOKLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LTimeSetOKLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_LTimeSetOKLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LTimeSetOKLabel, "\xEE\xB8\x8D"); // 确认图标
    lv_obj_set_style_text_font(ui_LTimeSetOKLabel, &ui_font_iconfont45, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 常亮图标
    ui_LTimeSeticon = lv_label_create(ui_LOffTimeSetPage);
    lv_obj_set_width(ui_LTimeSeticon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LTimeSeticon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_LTimeSeticon, 0);
    lv_obj_set_y(ui_LTimeSeticon, -105);
    lv_obj_set_align(ui_LTimeSeticon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LTimeSeticon, "\xEE\x98\x8E");
    lv_obj_set_style_text_font(ui_LTimeSeticon, &ui_font_iconfont45, LV_PART_MAIN | LV_STATE_DEFAULT);


    lv_obj_add_event_cb(ui_LTimeSetOKButton, ui_event_LTimeSetOKButton, LV_EVENT_ALL, NULL);

}


/**
 * @brief  熄屏时间设置页面初始化 - 创建滚轮选择15~65秒，底部确认按钮
 */
void ui_TOffTimeSetPage_screen_init(void)
{
    ui_TOffTimeSetPage = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_TOffTimeSetPage, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    // 时间滚轮（15/20/25/35/50/65秒）
    ui_TTimeSetRoller = lv_roller_create(ui_TOffTimeSetPage);
    lv_roller_set_options(ui_TTimeSetRoller, "15\n20\n25\n35\n50\n65", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(ui_TTimeSetRoller,ui_TTimeSelected,LV_ANIM_OFF);
    lv_obj_set_height(ui_TTimeSetRoller, 150);
    lv_obj_set_width(ui_TTimeSetRoller, LV_SIZE_CONTENT);   /// 0
    lv_obj_set_align(ui_TTimeSetRoller, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(ui_TTimeSetRoller, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_TTimeSetRoller, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_TTimeSetRoller, &ui_font_Cuyuan38, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_TTimeSetRoller, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_TTimeSetRoller, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_TTimeSetRoller, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_color(ui_TTimeSetRoller, lv_color_hex(0xFFFFFF), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_TTimeSetRoller, 255, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_TTimeSetRoller, lv_color_hex(0xFFFFFF), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_TTimeSetRoller, 0, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(ui_TTimeSetRoller, lv_color_hex(0x0064FF), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(ui_TTimeSetRoller, 255, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui_TTimeSetRoller, 2, LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(ui_TTimeSetRoller, 1, LV_PART_SELECTED | LV_STATE_DEFAULT);

    // 单位标签（秒）
    ui_TTimeSetUnitLabel = lv_label_create(ui_TOffTimeSetPage);
    lv_obj_set_width(ui_TTimeSetUnitLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_TTimeSetUnitLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_TTimeSetUnitLabel, 60);
    lv_obj_set_y(ui_TTimeSetUnitLabel, 0);
    lv_obj_set_align(ui_TTimeSetUnitLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_TTimeSetUnitLabel, "\xE7\xA7\x92"); // 秒
    lv_obj_set_style_text_font(ui_TTimeSetUnitLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 确认按钮
    ui_TTimeSetOKButton = lv_btn_create(ui_TOffTimeSetPage);
    lv_obj_set_width(ui_TTimeSetOKButton, 240);
    lv_obj_set_height(ui_TTimeSetOKButton, 50);
    lv_obj_set_align(ui_TTimeSetOKButton, LV_ALIGN_BOTTOM_MID);
    lv_obj_add_flag(ui_TTimeSetOKButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_TTimeSetOKButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_TTimeSetOKButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_TTimeSetOKButton, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_TTimeSetOKButton, 128, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_TTimeSetOKButton, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_TTimeSetOKButton, 128, LV_PART_MAIN | LV_STATE_PRESSED);

    ui_TTimeSetOKLabel = lv_label_create(ui_TTimeSetOKButton);
    lv_obj_set_width(ui_TTimeSetOKLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_TTimeSetOKLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_TTimeSetOKLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_TTimeSetOKLabel, "\xEE\xB8\x8D"); // 确认图标
    lv_obj_set_style_text_font(ui_TTimeSetOKLabel, &ui_font_iconfont45, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 熄屏图标
    ui_TTimeSeticon = lv_label_create(ui_TOffTimeSetPage);
    lv_obj_set_width(ui_TTimeSeticon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_TTimeSeticon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_TTimeSeticon, 0);
    lv_obj_set_y(ui_TTimeSeticon, -105);
    lv_obj_set_align(ui_TTimeSeticon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_TTimeSeticon, "\xEE\x9A\x96");
    lv_obj_set_style_text_font(ui_TTimeSeticon, &ui_font_iconfont45, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_TTimeSetOKButton, ui_event_TTimeSetOKButton, LV_EVENT_ALL, NULL);

}

/* ====== 页面注销 ====== */

/**
 * @brief  常亮时间页面注销
 */
void ui_LOffTimeSetPage_screen_deinit(void)
{}

/**
 * @brief  熄屏时间页面注销
 */
void ui_TOffTimeSetPage_screen_deinit(void)
{}
