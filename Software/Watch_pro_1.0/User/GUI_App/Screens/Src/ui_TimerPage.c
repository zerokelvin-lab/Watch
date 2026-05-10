/** @file ui_TimerPage.c
 *  @brief 秒表页面实现 - 高精度秒表，支持开始/暂停/复位，显示分:秒.毫秒格式，仪表盘刻度显示秒进度
 */
#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_TimerPage.h"

/* ====== 页面管理器 ====== */
Page_t Page_Timer = {ui_TimerPage_screen_init, ui_TimerPage_screen_deinit, &ui_TimerPage};

/* ====== 变量定义 ====== */
lv_obj_t * ui_TimerPage; // 秒表页面对象
lv_obj_t * ui_TimerMeter; // LVGL仪表控件（显示秒进度）
lv_obj_t * ui_TimerStartBtn; // 开始/暂停按钮
lv_obj_t * ui_TimerReBtn; // 复位按钮
lv_obj_t * ui_TimerStartLabel; // 开始按钮标签（播放/暂停图标）
lv_obj_t * ui_TimerReLabel; // 复位按钮标签
lv_obj_t * ui_TimerMinLabel; // 分钟标签
lv_obj_t * ui_TimerSecColonLabel; // 分:秒冒号
lv_obj_t * ui_TimerSecLabel; // 秒标签
lv_obj_t * ui_TimerMilSecColonLabel; // 秒.毫秒冒号
lv_obj_t * ui_TimerMilSecLabel; // 毫秒标签

lv_timer_t * ui_TimerPageTimer; // 秒表定时器（10ms周期）

// 秒表状态
uint8_t ui_TimerPageFlag=0; // 运行标志（1=运行，0=暂停）
uint8_t ui_TimerPage_min=0; // 分钟（0-59）
uint8_t ui_TimerPage_sec=0; // 秒（0-59）
uint8_t ui_TimerPage_10ms=0; // 百毫秒（0-99）
uint8_t ui_TimerPage_ms=0; // 毫秒（0-9）
uint32_t ui_TimerPage_elapsed_ms=0; // 累计毫秒数

/**
 * @brief  秒表数据复位 - 将所有计时变量清零
 */
static void ui_TimerPage_Data_init(void)
{
    ui_TimerPageFlag=0; // 停止状态
    ui_TimerPage_min=0;
    ui_TimerPage_sec=0;
	ui_TimerPage_10ms=0;
    ui_TimerPage_ms=0;
    ui_TimerPage_elapsed_ms=0; // 累计时间清零
}

/**
 * @brief  秒表显示刷新定时器 - 每10ms将累计毫秒数转换为分:秒.毫秒显示
 * @param  timer LVGL定时器对象
 */
static void LabelRefresh_timer(lv_timer_t * timer)
{
    uint32_t elapsed = ui_TimerPage_elapsed_ms;
    uint8_t strbuf[2];

    ui_TimerPage_min = (elapsed / 60000U) % 60U; // 计算分钟
    ui_TimerPage_sec = (elapsed / 1000U) % 60U; // 计算秒
    ui_TimerPage_10ms = (elapsed / 10U) % 100U; // 计算百毫秒（2位）
    ui_TimerPage_ms = elapsed % 10U; // 计算毫秒（1位）

    // 更新显示
    sprintf(strbuf,"%02d",ui_TimerPage_min);
    lv_label_set_text(ui_TimerMinLabel, strbuf);
    sprintf(strbuf,"%02d",ui_TimerPage_sec);
    lv_label_set_text(ui_TimerSecLabel, strbuf);
    sprintf(strbuf,"%02d",ui_TimerPage_10ms);
    lv_label_set_text(ui_TimerMilSecLabel, strbuf);


}

/**
 * @brief  开始/暂停按钮事件 - 切换秒表运行状态
 * @param  e LVGL事件对象
 */
static void ui_event_TimerStartBtn(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED &&  lv_obj_has_state(target, LV_STATE_CHECKED))
    {
        ui_TimerPageFlag = 1; // 开始计时
        lv_label_set_text(ui_TimerStartLabel, LV_SYMBOL_PAUSE); // 显示暂停图标
        lv_timer_resume(ui_TimerPageTimer); // 恢复定时器

    }
    if(event_code == LV_EVENT_VALUE_CHANGED &&  !lv_obj_has_state(target, LV_STATE_CHECKED))
    {
        ui_TimerPageFlag = 0; // 暂停计时
        lv_label_set_text(ui_TimerStartLabel, LV_SYMBOL_PLAY); // 显示播放图标
        lv_timer_pause(ui_TimerPageTimer); // 暂停定时器

    }
}

/**
 * @brief  复位按钮事件 - 仅在暂停状态时清零所有计时
 * @param  e LVGL事件对象
 */
static void ui_event_TimerReBtn(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED && !lv_obj_has_state(ui_TimerStartBtn, LV_STATE_CHECKED)) // 仅在暂停时有效
    {
        lv_timer_pause(ui_TimerPageTimer);
        lv_label_set_text(ui_TimerMinLabel, "00");
        lv_label_set_text(ui_TimerSecLabel, "00");
        lv_label_set_text(ui_TimerMilSecLabel, "00");
        ui_TimerPage_elapsed_ms = 0; // 清零累计时间
        ui_TimerPage_min = 0;
        ui_TimerPage_sec = 0;
		ui_TimerPage_10ms = 0;
        ui_TimerPage_ms = 0;

    }

}

/**
 * @brief  秒表页面初始化 - 创建仪表盘刻度、时间显示标签、开始/暂停和复位按钮
 */
void ui_TimerPage_screen_init(void)
{

    ui_TimerPage_Data_init(); // 初始化秒表数据
    ui_TimerPage = lv_obj_create(NULL);

    // 创建仪表盘控件（240x240，居中）
    ui_TimerMeter = lv_meter_create(ui_TimerPage);
    lv_obj_set_size(ui_TimerMeter, 240, 240);
    lv_obj_center(ui_TimerMeter);

    lv_obj_remove_style(ui_TimerMeter, NULL, LV_PART_INDICATOR); // 移除默认指示器
    lv_meter_scale_t * scale_min = lv_meter_add_scale(ui_TimerMeter);
    lv_meter_set_scale_ticks(ui_TimerMeter, scale_min, 61, 1, 10, lv_palette_main(LV_PALETTE_BLUE)); // 61条刻度（0~60秒）
    lv_meter_set_scale_range(ui_TimerMeter, scale_min, 0, 60, 360, 270); // 270度弧形，120度空缺口

    // 开始/暂停按钮（右侧，可选中状态）
    ui_TimerStartBtn = lv_btn_create(ui_TimerMeter);
    lv_obj_set_width(ui_TimerStartBtn, 60);
    lv_obj_set_height(ui_TimerStartBtn, 60);
    lv_obj_set_style_radius(ui_TimerStartBtn, 50, LV_PART_MAIN | LV_STATE_DEFAULT); // 圆形
    lv_obj_set_style_bg_opa(ui_TimerStartBtn, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(ui_TimerStartBtn, LV_ALIGN_CENTER);
    lv_obj_set_pos(ui_TimerStartBtn,40,50);
    lv_obj_add_flag(ui_TimerStartBtn, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_TimerStartBtn, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    // 复位按钮（左侧）
    ui_TimerReBtn = lv_btn_create(ui_TimerMeter);
    lv_obj_set_width(ui_TimerReBtn, 60);
    lv_obj_set_height(ui_TimerReBtn, 60);
    lv_obj_set_style_radius(ui_TimerReBtn, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_TimerReBtn, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(ui_TimerReBtn, LV_ALIGN_CENTER);
    lv_obj_set_pos(ui_TimerReBtn,-40,50);

    // 开始按钮标签（初始显示播放图标）
    ui_TimerStartLabel = lv_label_create(ui_TimerStartBtn);
    lv_label_set_text(ui_TimerStartLabel, LV_SYMBOL_PLAY);
    lv_obj_center(ui_TimerStartLabel);
    lv_obj_set_style_text_font(ui_TimerStartLabel, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 复位按钮标签（刷新图标）
    ui_TimerReLabel = lv_label_create(ui_TimerReBtn);
    lv_label_set_text(ui_TimerReLabel, LV_SYMBOL_REFRESH);
    lv_obj_center(ui_TimerReLabel);
    lv_obj_set_style_text_font(ui_TimerReLabel, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 时间数字显示（分:秒.毫秒，38号大字体）
    ui_TimerMinLabel = lv_label_create(ui_TimerMeter);
    lv_obj_set_align(ui_TimerMinLabel, LV_ALIGN_CENTER);
    lv_obj_set_pos(ui_TimerMinLabel,-60,-20);
    lv_label_set_text(ui_TimerMinLabel, "00");
    lv_obj_set_style_text_font(ui_TimerMinLabel, &ui_font_Cuyuan38, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_TimerSecColonLabel = lv_label_create(ui_TimerMeter);
    lv_obj_set_align(ui_TimerSecColonLabel, LV_ALIGN_CENTER);
    lv_obj_set_pos(ui_TimerSecColonLabel,-30,-20);
    lv_label_set_text(ui_TimerSecColonLabel, ":"); // 分秒冒号
    lv_obj_set_style_text_font(ui_TimerSecColonLabel, &ui_font_Cuyuan38, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_TimerSecLabel = lv_label_create(ui_TimerMeter);
    lv_obj_set_align(ui_TimerSecLabel, LV_ALIGN_CENTER);
    lv_obj_set_pos(ui_TimerSecLabel,0,-20);
    lv_label_set_text(ui_TimerSecLabel, "00");
    lv_obj_set_style_text_font(ui_TimerSecLabel, &ui_font_Cuyuan38, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_TimerMilSecColonLabel = lv_label_create(ui_TimerMeter);
    lv_obj_set_align(ui_TimerMilSecColonLabel, LV_ALIGN_CENTER);
    lv_obj_set_pos(ui_TimerMilSecColonLabel,30,-20);
    lv_label_set_text(ui_TimerMilSecColonLabel, "."); // 秒毫秒分隔符
    lv_obj_set_style_text_font(ui_TimerMilSecColonLabel, &ui_font_Cuyuan38, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_TimerMilSecLabel = lv_label_create(ui_TimerMeter);
    lv_obj_set_align(ui_TimerMilSecLabel, LV_ALIGN_CENTER);
    lv_obj_set_pos(ui_TimerMilSecLabel,60,-20);
    lv_label_set_text(ui_TimerMilSecLabel, "00");
    lv_obj_set_style_text_font(ui_TimerMilSecLabel, &ui_font_Cuyuan38, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 注册事件
    lv_obj_add_event_cb(ui_TimerStartBtn, ui_event_TimerStartBtn, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_TimerReBtn, ui_event_TimerReBtn, LV_EVENT_ALL, NULL);

    // 创建10ms定时器，初始暂停
    ui_TimerPageTimer = lv_timer_create(LabelRefresh_timer, 10,  NULL);
    lv_timer_pause(ui_TimerPageTimer);

}

/**
 * @brief  秒表页面注销 - 清理资源
 */
void ui_TimerPage_screen_deinit(void)
{}
