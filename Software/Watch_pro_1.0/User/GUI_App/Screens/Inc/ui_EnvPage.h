/** @file ui_EnvPage.h
 *  @brief 环境监测页面 - 显示温度和湿度，使用柱状条和数字实时展示传感器数据
 */
#ifndef _UI_ENVPAGE_H
#define _UI_ENVPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

extern lv_obj_t * ui_EnvPage; // 环境页面对象
extern lv_obj_t * ui_EnvTempBar; // 温度柱状条
extern lv_obj_t * ui_EnvHumiBar; // 湿度柱状条
extern lv_obj_t * ui_EnvTempNumLabel; // 温度数字标签
extern lv_obj_t * ui_EnvHumiNumLabel; // 湿度数字标签
extern lv_obj_t * ui_EnvTempUnitLabel; // 温度单位标签（℃）
extern lv_obj_t * ui_EnvHumiUnitLabel; // 湿度单位标签（%）
extern lv_obj_t * ui_EnvHumiIcon; // 湿度图标
extern lv_obj_t * ui_EnvTempIcon; // 温度图标

extern Page_t Page_Env; // 环境页面管理器

void ui_EnvPage_screen_init(void); // 环境页面初始化
void ui_EnvPage_screen_deinit(void); // 环境页面注销

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
