/** @file ui_CompassPage.h
 *  @brief 指南针页面 - 显示电子罗盘方向、海拔高度，指针实时跟随磁场方向旋转
 */
#ifndef _UI_COMPASSPAGE_H
#define _UI_COMPASSPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

extern lv_obj_t * ui_CompassPage; // 指南针页面对象
extern lv_obj_t * ui_Compassneedle; // 指南针指针图片
extern lv_obj_t * ui_CompassDirLabel; // 方向角度标签
extern lv_obj_t * ui_EnvAltitudeLabel; // 海拔高度标签

extern Page_t Page_Compass; // 指南针页面管理器

void ui_CompassPage_screen_init(void); // 指南针页面初始化
void ui_CompassPage_screen_deinit(void); // 指南针页面注销

LV_IMG_DECLARE(ui_img_compass_needle_png); // 指南针指针PNG图片声明

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
