/** @file ui_MKS142Page.h
 *  @brief MKS_142健康监测页面 - 显示心率和血氧
 */
#ifndef _UI_MKS142PAGE_H
#define _UI_MKS142PAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

extern lv_obj_t * ui_MKS142Page;          // 健康页面对象
extern lv_obj_t * ui_MKS142HRNum;         // 心率数字
extern lv_obj_t * ui_MKS142HRUnit;        // 心率单位
extern lv_obj_t * ui_MKS142HRIcon;        // 心率图标
extern lv_obj_t * ui_MKS142SpO2Num;       // 血氧数字
extern lv_obj_t * ui_MKS142SpO2Unit;      // 血氧单位
extern lv_obj_t * ui_MKS142SpO2Icon;      // 血氧图标
extern lv_obj_t * ui_MKS142NoticeLabel;   // 状态提示

extern uint8_t MKS142_saved_hr;   // 保存的心率数据
extern uint8_t MKS142_saved_spo2; // 保存的血氧数据

extern Page_t Page_MKS142; // 健康页面管理器

void ui_MKS142Page_screen_init(void);   // 健康页面初始化
void ui_MKS142Page_screen_deinit(void); // 健康页面注销

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
