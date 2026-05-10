/** @file ui_CalendarPage.c
 *  @brief 日历页面实现 - 显示LVGL日历控件，自动读取RTC当前日期作为默认选中日期
 */
#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_CalendarPage.h"
#include "../../../Func/Inc/HWDataAccess.h"

/* ====== 页面管理器 ====== */
Page_t Page_Calender = {ui_CalendarPage_screen_init, ui_CalendarPage_screen_deinit, &ui_CalendarPage};

/* ====== 变量定义 ====== */
lv_obj_t * ui_CalendarPage; // 日历页面对象
lv_obj_t * ui_CalendarPageCalendar; // LVGL日历控件

/* ====== 功能函数 ====== */


/**
 * @brief  日历页面初始化 - 创建日历控件并从RTC读取当前日期
 */
void ui_CalendarPage_screen_init(void)
{
    HW_DateTimeTypeDef DateTime; // 获取RTC当前时间
    HWInterface.RealTimeClock.GetTimeDate(&DateTime);

    ui_CalendarPage = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_CalendarPage, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    // 创建日历控件并设置今天日期
    ui_CalendarPageCalendar = lv_calendar_create(ui_CalendarPage);
    lv_calendar_set_today_date(ui_CalendarPageCalendar, 2000+DateTime.Year, DateTime.Month, DateTime.Date);
    lv_calendar_set_showed_date(ui_CalendarPageCalendar, 2000+DateTime.Year, DateTime.Month);
    lv_obj_t * ui_CalendarPageCalendar_header = lv_calendar_header_arrow_create(ui_CalendarPageCalendar); // 带箭头的日历头部
    lv_obj_set_width(ui_CalendarPageCalendar, 240);
    lv_obj_set_height(ui_CalendarPageCalendar, 280);
    lv_obj_set_align(ui_CalendarPageCalendar, LV_ALIGN_CENTER);
    lv_obj_set_style_radius(ui_CalendarPageCalendar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_CalendarPageCalendar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

}

/**
 * @brief  日历页面注销 - 清理资源
 */
void ui_CalendarPage_screen_deinit(void)
{}
