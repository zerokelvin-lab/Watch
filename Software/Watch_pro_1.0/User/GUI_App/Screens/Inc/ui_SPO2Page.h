/** @file ui_SPO2Page.h
 *  @brief 血氧监测页面 - 实时显示血氧饱和度百分比，每秒更新传感器数据
 */
#ifndef _UI_SPO2PAGE_H
#define _UI_SPO2PAGE_H

#ifdef __cplusplus
extern "C" {
#endif

extern lv_obj_t * ui_SPO2Page; // 血氧页面对象
extern lv_obj_t * ui_SPO2NumLabel; // 血氧数字标签
extern lv_obj_t * ui_SPO2UnitLabel; // 血氧单位标签（%）
extern lv_obj_t * ui_SPO2NoticeLabel; // 血氧测量提示标签
extern lv_obj_t * ui_SPO2Icon; // 血氧图标

extern Page_t Page_SPO2; // 血氧页面管理器

void ui_SPO2Page_screen_init(void); // 血氧页面初始化
void ui_SPO2Page_screen_deinit(void); // 血氧页面注销

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
