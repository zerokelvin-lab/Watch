/** @file ui_HRPage.c
 *  @brief 心率监测页面实现 - 大字号显示实时心率数值，50ms高频刷新传感器数据
 */
#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_HRPage.h"
#include "../../../Func/Inc/HWDataAccess.h"

/* ====== 页面管理器 ====== */
Page_t Page_HR = {ui_HRPage_screen_init, ui_HRPage_screen_deinit, &ui_HRPage};

/* ====== 变量定义 ====== */
lv_obj_t * ui_HRPage; // 心率页面对象
lv_obj_t * ui_HRPageNumLabel; // 心率数字（大字号）
lv_obj_t * ui_HRPageUnitLabel; // 单位标签（次/分）
lv_obj_t * ui_HRPaggiconLabel; // 心率图标
lv_obj_t * ui_HRPageNoticeLabel; // 测量提示文字

lv_timer_t * ui_HRPageTimer; // 心率页面定时器

/**
 * @brief  心率定时器回调 - 每50ms更新心率数值
 * @param  timer LVGL定时器对象
 * @note   页面销毁时必须删除此定时器
 */
static void HRPage_timer_cb(lv_timer_t * timer)
{
    uint8_t value_strbuf[6];
    sprintf(value_strbuf, "%d", HWInterface.HR_meter.HrRate); // 读取心率传感器数据
    lv_label_set_text(ui_HRPageNumLabel, value_strbuf);
}

/* ====== 功能函数 ====== */

/**
 * @brief  心率页面初始化 - 创建大字号心率数字、单位、图标和测量提示
 */
void ui_HRPage_screen_init(void)
{
    uint8_t value_strbuf[5];

    ui_HRPage = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_HRPage, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    // 心率数字（80号大字体，居中显示）
    ui_HRPageNumLabel = lv_label_create(ui_HRPage);
    lv_obj_set_width(ui_HRPageNumLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_HRPageNumLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_HRPageNumLabel, LV_ALIGN_CENTER);
    sprintf(value_strbuf, "%d", HWInterface.HR_meter.HrRate);
    lv_label_set_text(ui_HRPageNumLabel, value_strbuf);
    lv_obj_set_style_text_font(ui_HRPageNumLabel, &ui_font_Cuyuan80, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 单位标签（次/分）
    ui_HRPageUnitLabel = lv_label_create(ui_HRPage);
    lv_obj_set_width(ui_HRPageUnitLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_HRPageUnitLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_HRPageUnitLabel, 85);
    lv_obj_set_y(ui_HRPageUnitLabel, 18);
    lv_obj_set_align(ui_HRPageUnitLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_HRPageUnitLabel, "\xE6\xAC\xA1/\xE5\x88\x86"); // 次/分
    lv_obj_set_style_text_color(ui_HRPageUnitLabel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_HRPageUnitLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_HRPageUnitLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 心率图标（红色爱心）
    ui_HRPaggiconLabel = lv_label_create(ui_HRPage);
    lv_obj_set_width(ui_HRPaggiconLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_HRPaggiconLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_HRPaggiconLabel, 0);
    lv_obj_set_y(ui_HRPaggiconLabel, 30);
    lv_obj_set_align(ui_HRPaggiconLabel, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_HRPaggiconLabel, "\xEE\x98\x83");
    lv_obj_set_style_text_color(ui_HRPaggiconLabel, lv_color_hex(0xFF1919), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_HRPaggiconLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_HRPaggiconLabel, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 测量提示文字
    ui_HRPageNoticeLabel = lv_label_create(ui_HRPage);
    lv_obj_set_width(ui_HRPageNoticeLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_HRPageNoticeLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_HRPageNoticeLabel, 0);
    lv_obj_set_y(ui_HRPageNoticeLabel, 90);
    lv_obj_set_align(ui_HRPageNoticeLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_HRPageNoticeLabel, "\xE6\xAD\xA3\xE5\x9C\xA8\xE6\xB5\x8B\xE9\x87\x8F..."); // 正在测量...
    lv_obj_set_style_text_color(ui_HRPageNoticeLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_HRPageNoticeLabel, 220, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_HRPageNoticeLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 创建定时器，每50ms刷新心率
    ui_HRPageTimer = lv_timer_create(HRPage_timer_cb, 50,  NULL);

}

/**
 * @brief  心率页面注销 - 删除定时器
 */
void ui_HRPage_screen_deinit(void)
{
    lv_timer_del(ui_HRPageTimer);
}
