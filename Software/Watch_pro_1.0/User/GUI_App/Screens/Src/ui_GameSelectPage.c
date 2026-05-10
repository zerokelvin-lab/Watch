/** @file ui_GameSelectPage.c
 *  @brief 游戏选择页面实现 - 游戏菜单列表，提供2048、记忆方块、反弹球等游戏的入口按钮
 */
#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_MenuPage.h"
#include "../Inc/ui_GameSelectPage.h"
#include "../Inc/ui_Game2048Page.h"
#include "../Inc/ui_GameMemPage.h"

/* ====== 页面管理器 ====== */
Page_t Page_GameSelect = {ui_GameSelectPage_screen_init, ui_GameSelectPage_screen_deinit, &ui_GameSelectPage};

/* ====== 变量定义 ====== */
lv_obj_t * ui_GameSelectPage; // 游戏选择页面对象
lv_obj_t * ui_Game2048Panel; // 2048游戏入口面板
lv_obj_t * ui_GameMemPanel; // 记忆方块游戏入口面板
lv_obj_t * ui_GameBallPanel; // 反弹球游戏入口面板

/**
 * @brief  页面手势事件 - 右滑返回上一页
 * @param  e LVGL事件对象
 */
void ui_event_GameSelectPage(lv_event_t * e)
{
   lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);

    if(event_code == LV_EVENT_GESTURE)
    {
        if(lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) // 右滑返回
        {
			Page_Back();
        }
    }
}

/**
 * @brief  2048游戏入口点击事件 - 加载2048游戏
 * @param  e LVGL事件对象
 */
void ui_event_Game2048Panel(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
        Page_Load(&Page_Game_2048);
    }
}

/**
 * @brief  记忆方块入口点击事件 - 加载记忆方块游戏
 * @param  e LVGL事件对象
 */
void ui_event_GameMemPanel(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
        Page_Load(&Page_GameMem);
    }
}


/**
 * @brief  游戏选择页面初始化 - 创建三个游戏入口面板（2048、记忆方块、反弹球）
 */
void ui_GameSelectPage_screen_init(void)
{
    ui_GameSelectPage = lv_obj_create(NULL);

    /* ====== 2048游戏入口 ====== */
    ui_Game2048Panel = lv_obj_create(ui_GameSelectPage);
    lv_obj_set_width(ui_Game2048Panel, 240);
    lv_obj_set_height(ui_Game2048Panel, 70);
    lv_obj_set_align(ui_Game2048Panel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_Game2048Panel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Game2048Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Game2048Panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Game2048Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Game2048Panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Game2048Panel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_Game2048Panel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    // 2048图标按钮（青色圆形）
    lv_obj_t * ui_Game2048Button = lv_btn_create(ui_Game2048Panel);
    lv_obj_set_width(ui_Game2048Button, 40);
    lv_obj_set_height(ui_Game2048Button, 40);
    lv_obj_set_align(ui_Game2048Button, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_Game2048Button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Game2048Button, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Game2048Button, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Game2048Button, lv_color_hex(0x009A8D), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Game2048Button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * ui_Game2048icon = lv_label_create(ui_Game2048Button);
    lv_obj_set_width(ui_Game2048icon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Game2048icon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Game2048icon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Game2048icon, "\xEE\x9E\xBC");
    lv_obj_set_style_text_font(ui_Game2048icon, &ui_font_iconfont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * ui_Game2048Label = lv_label_create(ui_Game2048Panel);
    lv_obj_set_width(ui_Game2048Label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Game2048Label, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Game2048Label, 60);
    lv_obj_set_y(ui_Game2048Label, 0);
    lv_obj_set_align(ui_Game2048Label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_Game2048Label, "2048");
    lv_obj_set_style_text_font(ui_Game2048Label, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 记忆方块游戏入口 ====== */
    ui_GameMemPanel = lv_obj_create(ui_GameSelectPage);
    lv_obj_set_width(ui_GameMemPanel, 240);
    lv_obj_set_height(ui_GameMemPanel, 70);
    lv_obj_set_x(ui_GameMemPanel, 0);
    lv_obj_set_y(ui_GameMemPanel, 70);
    lv_obj_set_align(ui_GameMemPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_GameMemPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_GameMemPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_GameMemPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_GameMemPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_GameMemPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_GameMemPanel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_GameMemPanel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    // 记忆方块图标按钮（金色圆形）
    lv_obj_t * ui_GameMemButton = lv_btn_create(ui_GameMemPanel);
    lv_obj_set_width(ui_GameMemButton, 40);
    lv_obj_set_height(ui_GameMemButton, 40);
    lv_obj_set_align(ui_GameMemButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_GameMemButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_GameMemButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_GameMemButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_GameMemButton, lv_color_hex(0xC07010), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_GameMemButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * ui_GameMemicon = lv_label_create(ui_GameMemButton);
    lv_obj_set_width(ui_GameMemicon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_GameMemicon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_GameMemicon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_GameMemicon, "\xEE\x9E\xA0");
    lv_obj_set_style_text_font(ui_GameMemicon, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * ui_GameMemLabel = lv_label_create(ui_GameMemPanel);
    lv_obj_set_width(ui_GameMemLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_GameMemLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_GameMemLabel, 60);
    lv_obj_set_y(ui_GameMemLabel, 0);
    lv_obj_set_align(ui_GameMemLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_GameMemLabel, "\xE8\xAE\xB0\xE5\xBF\x86\xE6\x96\xB9\xE5\x9D\x97"); // 记忆方块
    lv_obj_set_style_text_font(ui_GameMemLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 反弹球游戏入口 ====== */
    ui_GameBallPanel = lv_obj_create(ui_GameSelectPage);
    lv_obj_set_width(ui_GameBallPanel, 240);
    lv_obj_set_height(ui_GameBallPanel, 70);
    lv_obj_set_x(ui_GameBallPanel, 0);
    lv_obj_set_y(ui_GameBallPanel, 140);
    lv_obj_set_align(ui_GameBallPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_GameBallPanel, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_GameBallPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_GameBallPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_GameBallPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_GameBallPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_GameBallPanel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_GameBallPanel, 100, LV_PART_MAIN | LV_STATE_PRESSED);

    // 反弹球图标按钮（粉红色圆形）
    lv_obj_t * ui_GameBallButton = lv_btn_create(ui_GameBallPanel);
    lv_obj_set_width(ui_GameBallButton, 40);
    lv_obj_set_height(ui_GameBallButton, 40);
    lv_obj_set_align(ui_GameBallButton, LV_ALIGN_LEFT_MID);
    lv_obj_add_flag(ui_GameBallButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_GameBallButton, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_GameBallButton, 40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_GameBallButton, lv_color_hex(0xFF8080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_GameBallButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * ui_GameBallicon = lv_label_create(ui_GameBallButton);
    lv_obj_set_width(ui_GameBallicon, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_GameBallicon, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_GameBallicon, LV_ALIGN_CENTER);
    lv_label_set_text(ui_GameBallicon, "\xEE\x99\x93");
    lv_obj_set_style_text_font(ui_GameBallicon, &ui_font_iconfont28, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * ui_GameBallLabel = lv_label_create(ui_GameBallPanel);
    lv_obj_set_width(ui_GameBallLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_GameBallLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_GameBallLabel, 60);
    lv_obj_set_y(ui_GameBallLabel, 0);
    lv_obj_set_align(ui_GameBallLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_GameBallLabel, "\xE5\x8F\x8D \xE5\xBC\xB9 \xE7\x90\x83"); // 反弹球
    lv_obj_set_style_text_font(ui_GameBallLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 注册事件回调
		lv_obj_add_event_cb(ui_GameSelectPage, ui_event_GameSelectPage, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Game2048Panel, ui_event_Game2048Panel, LV_EVENT_ALL, NULL);
		lv_obj_add_event_cb(ui_GameMemPanel, ui_event_GameMemPanel, LV_EVENT_ALL, NULL);

}

/**
 * @brief  游戏选择页面注销 - 清理资源
 */
void ui_GameSelectPage_screen_deinit(void)
{}
