/** @file ui_EnvPage.c
 *  @brief 环境监测页面实现 - 通过柱状条和数字显示温度（℃）和湿度（%），500ms定时刷新传感器数据
 */
#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_EnvPage.h"
#include "../../../Func/Inc/HWDataAccess.h"

/* ====== 页面管理器 ====== */
Page_t Page_Env = {ui_EnvPage_screen_init, ui_EnvPage_screen_deinit, &ui_EnvPage};

/* ====== 变量定义 ====== */
lv_obj_t * ui_EnvPage; // 环境页面对象
lv_obj_t * ui_EnvTempBar; // 温度柱状条
lv_obj_t * ui_EnvHumiBar; // 湿度柱状条
lv_obj_t * ui_EnvTempNumLabel; // 温度数字
lv_obj_t * ui_EnvHumiNumLabel; // 湿度数字
lv_obj_t * ui_EnvTempUnitLabel; // 温度单位（℃）
lv_obj_t * ui_EnvHumiUnitLabel; // 湿度单位（%）
lv_obj_t * ui_EnvHumiIcon; // 湿度图标
lv_obj_t * ui_EnvTempIcon; // 温度图标


lv_timer_t * ui_EnvPageTimer; // 环境页面定时器

/**
 * @brief  环境页面定时器回调 - 每500ms刷新温度和湿度柱状条及数值
 * @param  timer LVGL定时器对象
 * @note   页面销毁时必须删除此定时器
 */
static void EnvPage_timer_cb(lv_timer_t * timer)
{
    uint8_t value_strbuf[6];
    // 更新温度柱状条和数字
    lv_bar_set_value(ui_EnvTempBar, HWInterface.AHT21.temperature, LV_ANIM_OFF);
    lv_bar_set_value(ui_EnvHumiBar, HWInterface.AHT21.humidity, LV_ANIM_OFF);
    sprintf(value_strbuf,"%d", HWInterface.AHT21.temperature);
    lv_label_set_text(ui_EnvTempNumLabel, value_strbuf);
    sprintf(value_strbuf,"%d", HWInterface.AHT21.humidity);
    lv_label_set_text(ui_EnvHumiNumLabel, value_strbuf);
}


/**
 * @brief  环境页面初始化 - 创建温湿度柱状条、数值标签、单位标签和图标
 */
void ui_EnvPage_screen_init(void)
{
		uint8_t strbuf[5];
    ui_EnvPage = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_EnvPage, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    // 温度柱状条（-10~50℃范围，红色）
    ui_EnvTempBar = lv_bar_create(ui_EnvPage);
    lv_bar_set_range(ui_EnvTempBar, -10, 50);
    lv_bar_set_value(ui_EnvTempBar, HWInterface.AHT21.temperature, LV_ANIM_OFF);
    lv_obj_set_width(ui_EnvTempBar, 20);
    lv_obj_set_height(ui_EnvTempBar, 150);
    lv_obj_set_x(ui_EnvTempBar, -50);
    lv_obj_set_y(ui_EnvTempBar, 0);
    lv_obj_set_align(ui_EnvTempBar, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(ui_EnvTempBar, lv_color_hex(0xC80000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_EnvTempBar, 50, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_EnvTempBar, lv_color_hex(0xC83232), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_EnvTempBar, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // 湿度柱状条（0~100%，蓝色）
    ui_EnvHumiBar = lv_bar_create(ui_EnvPage);
    lv_bar_set_value(ui_EnvHumiBar, HWInterface.AHT21.humidity, LV_ANIM_OFF);
    lv_obj_set_width(ui_EnvHumiBar, 20);
    lv_obj_set_height(ui_EnvHumiBar, 150);
    lv_obj_set_x(ui_EnvHumiBar, 50);
    lv_obj_set_y(ui_EnvHumiBar, 0);
    lv_obj_set_align(ui_EnvHumiBar, LV_ALIGN_CENTER);

    // 温度数字标签
    ui_EnvTempNumLabel = lv_label_create(ui_EnvPage);
    lv_obj_set_width(ui_EnvTempNumLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_EnvTempNumLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_EnvTempNumLabel, -50);
    lv_obj_set_y(ui_EnvTempNumLabel, 100);
    lv_obj_set_align(ui_EnvTempNumLabel, LV_ALIGN_CENTER);
		sprintf(strbuf,"%d",HWInterface.AHT21.temperature);
    lv_label_set_text(ui_EnvTempNumLabel, strbuf);
    lv_obj_set_style_text_font(ui_EnvTempNumLabel, &ui_font_Cuyuan30, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 湿度数字标签
    ui_EnvHumiNumLabel = lv_label_create(ui_EnvPage);
    lv_obj_set_width(ui_EnvHumiNumLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_EnvHumiNumLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_EnvHumiNumLabel, 50);
    lv_obj_set_y(ui_EnvHumiNumLabel, 100);
    lv_obj_set_align(ui_EnvHumiNumLabel, LV_ALIGN_CENTER);
		sprintf(strbuf,"%d", HWInterface.AHT21.humidity);
    lv_label_set_text(ui_EnvHumiNumLabel, strbuf);
    lv_obj_set_style_text_font(ui_EnvHumiNumLabel, &ui_font_Cuyuan30, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 温度单位标签（℃）
    ui_EnvTempUnitLabel = lv_label_create(ui_EnvPage);
    lv_obj_set_width(ui_EnvTempUnitLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_EnvTempUnitLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_EnvTempUnitLabel, -20);
    lv_obj_set_y(ui_EnvTempUnitLabel, 100);
    lv_obj_set_align(ui_EnvTempUnitLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_EnvTempUnitLabel, "C");
    lv_obj_set_style_text_color(ui_EnvTempUnitLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_EnvTempUnitLabel, 128, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_EnvTempUnitLabel, &ui_font_Cuyuan30, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 湿度单位标签（%）
    ui_EnvHumiUnitLabel = lv_label_create(ui_EnvPage);
    lv_obj_set_width(ui_EnvHumiUnitLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_EnvHumiUnitLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_EnvHumiUnitLabel, 80);
    lv_obj_set_y(ui_EnvHumiUnitLabel, 100);
    lv_obj_set_align(ui_EnvHumiUnitLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_EnvHumiUnitLabel, "%");
    lv_obj_set_style_text_color(ui_EnvHumiUnitLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_EnvHumiUnitLabel, 128, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_EnvHumiUnitLabel, &ui_font_Cuyuan30, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 湿度图标
    ui_EnvHumiIcon = lv_label_create(ui_EnvPage);
    lv_obj_set_width(ui_EnvHumiIcon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_EnvHumiIcon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_EnvHumiIcon, 50);
    lv_obj_set_y(ui_EnvHumiIcon, -95);
    lv_obj_set_align(ui_EnvHumiIcon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_EnvHumiIcon, "\xEE\x99\xB3");
    lv_obj_set_style_text_color(ui_EnvHumiIcon, lv_color_hex(0x1496FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_EnvHumiIcon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_EnvHumiIcon, &ui_font_iconfont28, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 温度图标
    ui_EnvTempIcon = lv_label_create(ui_EnvPage);
    lv_obj_set_width(ui_EnvTempIcon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_EnvTempIcon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_EnvTempIcon, -50);
    lv_obj_set_y(ui_EnvTempIcon, -95);
    lv_obj_set_align(ui_EnvTempIcon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_EnvTempIcon, "\xEE\x99\xBA");
    lv_obj_set_style_text_color(ui_EnvTempIcon, lv_color_hex(0xA03232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_EnvTempIcon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_EnvTempIcon, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 创建定时器，每500ms刷新
    ui_EnvPageTimer = lv_timer_create(EnvPage_timer_cb, 500,  NULL);

}

/**
 * @brief  环境页面注销 - 删除定时器
 */
void ui_EnvPage_screen_deinit(void)
{
  lv_timer_del(ui_EnvPageTimer);
}
