/** @file ui_SOSPage.h
 *  @brief SOS紧急告警页面 - 红黑主题全屏覆盖，显示异常原因和倒计时自动上报
 */
#ifndef _UI_SOSPAGE_H
#define _UI_SOSPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"
#include "PageManager.h"

extern lv_obj_t * ui_SOSPage;        /* SOS页面对象 */
extern uint8_t sos_abnormal_type;    /* 当前异常类型编码 (SOS_TYPE_* bitmask) */

extern Page_t Page_SOS;              /* SOS页面管理器 */

void ui_SOSPage_screen_init(void);   /* SOS页面初始化 */
void ui_SOSPage_screen_deinit(void); /* SOS页面注销 */

#ifdef __cplusplus
}
#endif

#endif
