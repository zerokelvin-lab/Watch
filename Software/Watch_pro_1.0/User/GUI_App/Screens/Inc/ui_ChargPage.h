/** @file ui_ChargPage.h
 *  @brief 充电页面 - 显示电池电量弧线、充电图标、电量百分比及当前时间
 */
#ifndef _UI_CHARGPAGE_H
#define _UI_CHARGPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

extern lv_obj_t * ui_ChargPage; // 充电页面对象
extern lv_obj_t * ui_CharPageBatArc; // 电池电量弧线控件
extern lv_obj_t * ui_ChargPageBatIcon; // 电池图标
extern lv_obj_t * ui_ChargPageBatNum; // 电池百分比数字
extern lv_obj_t * ui_ChargPagebHourLabel; // 充电页面小时标签
extern lv_obj_t * ui_ChargPagebMinLabel; // 充电页面分钟标签
extern lv_obj_t * ui_ChargPageTimePoint; // 充电页面时间冒号

extern Page_t Page_Charg; // 充电页面管理器

void ui_ChargPage_screen_init(void); // 充电页面初始化
void ui_ChargPage_screen_deinit(void); // 充电页面注销

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
