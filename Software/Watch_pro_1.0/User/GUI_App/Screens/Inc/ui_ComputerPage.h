/** @file ui_ComputerPage.h
 *  @brief 计算器页面 - 实现基础四则运算计算器，支持加减乘除和退格功能
 */
#ifndef _UI_COMPUTERPAGE_H
#define _UI_COMPUTERPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ui.h"

extern lv_obj_t * ui_Computerpage; // 计算器页面对象
extern lv_obj_t * ui_CompageBtnM; // 计算器按钮矩阵
extern lv_obj_t * ui_CompageTextarea; // 计算器显示区域
extern lv_obj_t * ui_CompageBackBtn; // 计算器退格按钮

extern Page_t Page_Computer; // 计算器页面管理器

void ui_ComputerPage_screen_init(void); // 计算器页面初始化
void ui_ComputerPage_screen_deinit(void); // 计算器页面注销

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
