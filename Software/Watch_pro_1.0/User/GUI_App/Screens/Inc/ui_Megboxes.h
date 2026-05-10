/** @file ui_Megboxes.h
 *  @brief 消息弹框组件 - 通用消息对话框，支持警告文字、确定和取消按钮
 */
#ifndef _UI_MEGBOXES_H
#define _UI_MEGBOXES_H

#ifdef __cplusplus
extern "C" {
#endif

void ui_mbox_create(uint8_t * str); // 创建消息框
void ui_mbox_close(void); // 关闭消息框

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
