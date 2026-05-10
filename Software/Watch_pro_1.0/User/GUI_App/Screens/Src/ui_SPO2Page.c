/** @file ui_SPO2Page.c
 *  @brief 血氧监测页面实现 - 大字号显示实时血氧饱和度百分比，500ms定时刷新传感器数据
 */
#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_SPO2Page.h"
#include "../../../Func/Inc/HWDataAccess.h"

/* ====== 页面管理器 ====== */
Page_t Page_SPO2 = {ui_SPO2Page_screen_init, ui_SPO2Page_screen_deinit, &ui_SPO2Page};

/* ====== 变量定义 ====== */
lv_obj_t * ui_SPO2Page; // 血氧页面对象
lv_obj_t * ui_SPO2NumLabel; // 血氧数字（大字号）
lv_obj_t * ui_SPO2UnitLabel; // 单位标签（%）
lv_obj_t * ui_SPO2NoticeLabel; // 测量提示文字
lv_obj_t * ui_SPO2Icon; // 血氧图标

lv_timer_t * ui_SPO2PageTimer; // 血氧页面定时器

/**
 * @brief  血氧定时器回调 - 每500ms更新血氧数值
 * @param  timer LVGL定时器对象
 * @note   页面销毁时必须删除此定时器
 */
static void SPO2Page_timer_cb(lv_timer_t * timer)
{
    uint8_t value_strbuf[6];
    sprintf(value_strbuf, "%d", HWInterface.HR_meter.SPO2); // 读取血氧传感器数据
    lv_label_set_text(ui_SPO2NumLabel, value_strbuf);
}

/**
 * @brief  血氧页面初始化 - 创建大字号血氧数字、单位标签、图标和测量提示
 */
void ui_SPO2Page_screen_init(void)
{
    uint8_t value_strbuf[5];

    ui_SPO2Page = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_SPO2Page, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    // 血氧数字（80号大字体，居中显示）
    ui_SPO2NumLabel = lv_label_create(ui_SPO2Page);
    lv_obj_set_width(ui_SPO2NumLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_SPO2NumLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_SPO2NumLabel, LV_ALIGN_CENTER);
    sprintf(value_strbuf, "%d", HWInterface.HR_meter.SPO2);
    lv_label_set_text(ui_SPO2NumLabel, value_strbuf);
    lv_obj_set_style_text_font(ui_SPO2NumLabel, &ui_font_Cuyuan80, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 单位标签（%）
    ui_SPO2UnitLabel = lv_label_create(ui_SPO2Page);
    lv_obj_set_width(ui_SPO2UnitLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_SPO2UnitLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_SPO2UnitLabel, 70);
    lv_obj_set_y(ui_SPO2UnitLabel, 18);
    lv_obj_set_align(ui_SPO2UnitLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_SPO2UnitLabel, "%");
    lv_obj_set_style_text_font(ui_SPO2UnitLabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 测量提示
    ui_SPO2NoticeLabel = lv_label_create(ui_SPO2Page);
    lv_obj_set_width(ui_SPO2NoticeLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_SPO2NoticeLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_SPO2NoticeLabel, 0);
    lv_obj_set_y(ui_SPO2NoticeLabel, 90);
    lv_obj_set_align(ui_SPO2NoticeLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_SPO2NoticeLabel, "\xE6\xAD\xA3\xE5\x9C\xA8\xE6\xB5\x8B\xE9\x87\x8F..."); // 正在测量...
    lv_obj_set_style_text_color(ui_SPO2NoticeLabel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_SPO2NoticeLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_SPO2NoticeLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 血氧图标（蓝色）
    ui_SPO2Icon = lv_label_create(ui_SPO2Page);
    lv_obj_set_width(ui_SPO2Icon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_SPO2Icon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_SPO2Icon, 0);
    lv_obj_set_y(ui_SPO2Icon, 30);
    lv_obj_set_align(ui_SPO2Icon, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_SPO2Icon, "\xEE\x99\x92");
    lv_obj_set_style_text_color(ui_SPO2Icon, lv_color_hex(0x0080FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_SPO2Icon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_SPO2Icon, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 创建定时器，每500ms刷新
    ui_SPO2PageTimer = lv_timer_create(SPO2Page_timer_cb, 500,  NULL);

}

/**
 * @brief  血氧页面注销 - 删除定时器
 */
void ui_SPO2Page_screen_deinit(void)
{
    lv_timer_del(ui_SPO2PageTimer);
}
