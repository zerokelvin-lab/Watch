/** @file ui_HRPage.h
 *  @brief 心率监测页面 - 实时显示心率数值（次/分），每秒更新传感器数据
 */
#ifndef _UI_HRPAGE_H
#define _UI_HRPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

extern lv_obj_t * ui_HRPage; // 心率页面对象
extern lv_obj_t * ui_HRPageNumLabel; // 心率数字标签
extern lv_obj_t * ui_HRPageUnitLabel; // 心率单位标签（次/分）
extern lv_obj_t * ui_HRPaggiconLabel; // 心率图标
extern lv_obj_t * ui_HRPageNoticeLabel; // 心率测量提示标签

extern Page_t Page_HR; // 心率页面管理器

void ui_HRPage_screen_init(void); // 心率页面初始化
void ui_HRPage_screen_deinit(void); // 心率页面注销

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
