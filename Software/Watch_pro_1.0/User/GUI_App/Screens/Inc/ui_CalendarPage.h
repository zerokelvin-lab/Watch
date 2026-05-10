/** @file ui_CalendarPage.h
 *  @brief 日历页面 - 显示日历控件，支持日期浏览和今日日期标记
 */
#ifndef _UI_CALENDARPAGE_H
#define _UI_CALENDARPAGE_H

#ifdef __cplusplus
extern "C" {
#endif


extern lv_obj_t * ui_CalendarPage; // 日历页面对象
extern lv_obj_t * ui_CalendarPageCalendar; // 日历控件对象

extern Page_t Page_Calender; // 日历页面管理器

void ui_CalendarPage_screen_init(void); // 日历页面初始化
void ui_CalendarPage_screen_deinit(void); // 日历页面注销

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
