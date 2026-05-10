/** @file ui.c @brief LVGL GUI主程序入口，初始化界面主题和定时器 */

// LVGL VERSION: 8.2.0

#include "ui.h"
#include "./Screens/Inc/ui_HomePage.h"
#include "./Screens/Inc/ui_MenuPage.h"
#include "ui_helpers.h"

///////////////////// TEST LVGL SETTINGS ////////////////////
// 编译期检查：确保色彩深度为16位，匹配SquareLine Studio设置
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
// 编译期检查：确保16位色序交换关闭，匹配SquareLine Studio设置
#if LV_COLOR_16_SWAP !=1
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

/////////////////////// Timer //////////////////////
/**
 * Main timer for Refreshing the screens
 *
 * @brief 主屏幕刷新定时器回调函数
 * @param timer 定时器对象指针
 */
static void main_timer(lv_timer_t * timer)
{
    // printf("timer\r\n");

}


/////////////////////// ui_initialize //////////////////////
/**
 * @brief GUI图形界面初始化函数
 *
 * 功能：
 * 1. 获取默认显示器并初始化默认主题（蓝色主色，红色辅色）
 * 2. 初始化所有页面（首页、菜单页等）
 * 3. 创建主定时器，周期刷新屏幕
 */
void ui_init(void)
{
    // 获取默认显示器对象
    lv_disp_t * dispp = lv_disp_get_default();
    // 初始化默认主题：主色蓝色，辅色红色，启用暗色模式
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               true, LV_FONT_DEFAULT);
    // 将主题应用到显示器
    lv_disp_set_theme(dispp, theme);
    // 初始化所有页面（首页、菜单页等）
    Pages_init();
    //timer
    // 创建主屏幕刷新定时器，周期1秒
    lv_timer_t * ui_MainTimer = lv_timer_create(main_timer, 1000,  NULL);
}
