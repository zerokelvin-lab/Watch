/** @file ui_MKS142DetailPage.h
 *  @brief MKS_142健康详情页面 - 显示疲劳度、微循环、呼吸率、血压、RMSSD
 */
#ifndef _UI_MKS142DETAILPAGE_H
#define _UI_MKS142DETAILPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"
#include "PageManager.h"

extern lv_obj_t * ui_MKS142DetailPage;       // 详情页面对象
extern lv_obj_t * ui_MKS142DetailFatigueNum; // 疲劳数字
extern lv_obj_t * ui_MKS142DetailMicroNum;   // 微循环数字
extern lv_obj_t * ui_MKS142DetailRespNum;    // 呼吸率数字
extern lv_obj_t * ui_MKS142DetailBPNum;      // 血压数字
extern lv_obj_t * ui_MKS142DetailRMSSDNum;   // RMSSD数字
extern lv_obj_t * ui_MKS142DetailNoticeLabel;// 状态提示

extern uint8_t MKS142_saved_fatigue; // 保存的疲劳度
extern uint8_t MKS142_saved_micro;   // 保存的微循环
extern uint8_t MKS142_saved_resp;    // 保存的呼吸率
extern uint8_t MKS142_saved_sbp;     // 保存的收缩压
extern uint8_t MKS142_saved_dbp;     // 保存的舒张压
extern uint8_t MKS142_saved_rmssd;   // 保存的RMSSD

extern Page_t Page_MKS142Detail; // 详情页面管理器

void ui_MKS142DetailPage_screen_init(void);   // 详情页面初始化
void ui_MKS142DetailPage_screen_deinit(void); // 详情页面注销

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
