/** @file ui_NFCCardPage.h
 *  @brief NFC卡包页面 - 管理校园卡、门禁卡、交通卡等多张NFC卡，支持改写和选择操作
 */
#ifndef _UI_NFCCARDPAGE_H
#define _UI_NFCCARDPAGE_H

#ifdef __cplusplus
extern "C" {
#endif

extern lv_obj_t * ui_NFCCardPage; // NFC卡包页面对象
// 卡片1 - 校园卡
extern lv_obj_t * ui_Card1Panel;
extern lv_obj_t * ui_Card1Btn;
extern lv_obj_t * ui_Card1icon;
extern lv_obj_t * ui_Card1Label;
// 卡片2 - 门禁卡
extern lv_obj_t * ui_Card2Panel;
extern lv_obj_t * ui_Card2Btn;
extern lv_obj_t * ui_Card2icon;
extern lv_obj_t * ui_Card2Label;
// 卡片3 - 交通卡
extern lv_obj_t * ui_Card3Panel;
extern lv_obj_t * ui_Card3Btn;
extern lv_obj_t * ui_Card3icon;
extern lv_obj_t * ui_Card3Label;

extern Page_t Page_NFCCard; // NFC卡包页面管理器

void ui_NFCCardPage_screen_init(void); // NFC卡包页面初始化
void ui_NFCCardPage_screen_deinit(void); // NFC卡包页面注销

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
