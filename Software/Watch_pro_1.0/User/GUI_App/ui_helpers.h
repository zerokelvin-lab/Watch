/** @file ui_helpers.h @brief SquareLine Studio生成的LVGL控件属性定义与辅助函数声明 */

// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.2.0
// PROJECT: MenuPage

// 头文件保护宏
#ifndef _MENUPAGE_UI_HELPERS_H
#define _MENUPAGE_UI_HELPERS_H

#include "ui.h"

#define _UI_TEMPORARY_STRING_BUFFER_SIZE 32  // 临时字符串缓冲区大小

// 滑动条属性ID定义
#define _UI_BAR_PROPERTY_VALUE 0             // 直接设置值
#define _UI_BAR_PROPERTY_VALUE_WITH_ANIM 1   // 带动画设置值
/** @brief 设置滑动条属性 @param target 目标对象 @param id 属性ID @param val 属性值 */
void _ui_bar_set_property(lv_obj_t * target, int id, int val);

// 基本属性ID定义
#define _UI_BASIC_PROPERTY_POSITION_X 0      // X坐标
#define _UI_BASIC_PROPERTY_POSITION_Y 1      // Y坐标
#define _UI_BASIC_PROPERTY_WIDTH 2           // 宽度
#define _UI_BASIC_PROPERTY_HEIGHT 3          // 高度
/** @brief 设置控件基本属性 @param target 目标对象 @param id 属性ID @param val 属性值 */
void _ui_basic_set_property(lv_obj_t * target, int id, int val);

// 下拉框属性ID定义
#define _UI_DROPDOWN_PROPERTY_SELECTED 0     // 选中项索引
/** @brief 设置下拉框选中项 @param target 目标对象 @param id 属性ID @param val 选中索引 */
void _ui_dropdown_set_property(lv_obj_t * target, int id, int val);

// 图像属性ID定义
#define _UI_IMAGE_PROPERTY_IMAGE 0           // 图像源
/** @brief 设置图像源 @param target 目标对象 @param id 属性ID @param val 图像源指针 */
void _ui_image_set_property(lv_obj_t * target, int id, uint8_t * val);

// 标签属性ID定义
#define _UI_LABEL_PROPERTY_TEXT 0            // 文本内容
/** @brief 设置标签文本 @param target 目标对象 @param id 属性ID @param val 文本字符串 */
void _ui_label_set_property(lv_obj_t * target, int id, char * val);

// 滚轮属性ID定义
#define _UI_ROLLER_PROPERTY_SELECTED 0       // 直接设置选中项
#define _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM 1  // 带动画设置选中项
/** @brief 设置滚轮选中项 @param target 目标对象 @param id 属性ID @param val 选中索引 */
void _ui_roller_set_property(lv_obj_t * target, int id, int val);

// 滑块属性ID定义
#define _UI_SLIDER_PROPERTY_VALUE 0          // 直接设置值
#define _UI_SLIDER_PROPERTY_VALUE_WITH_ANIM 1  // 带动画设置值
/** @brief 设置滑块值 @param target 目标对象 @param id 属性ID @param val 滑块值 */
void _ui_slider_set_property(lv_obj_t * target, int id, int val);

/** @brief 切换屏幕，支持动画效果 @param target 目标屏幕 @param fademode 动画模式 @param spd 动画速度 @param delay 延迟时间 */
void _ui_screen_change(lv_obj_t * target, lv_scr_load_anim_t fademode, int spd, int delay);

/** @brief 弧形控件值增量 @param target 目标对象 @param val 增量值 */
void _ui_arc_increment(lv_obj_t * target, int val);

/** @brief 滑动条值增量 @param target 目标对象 @param val 增量值 @param anm 是否动画 */
void _ui_bar_increment(lv_obj_t * target, int val, int anm);

/** @brief 滑块值增量 @param target 目标对象 @param val 增量值 @param anm 是否动画 */
void _ui_slider_increment(lv_obj_t * target, int val, int anm);

// 标志位修改操作类型
#define _UI_MODIFY_FLAG_ADD 0               // 添加标志位
#define _UI_MODIFY_FLAG_REMOVE 1            // 移除标志位
#define _UI_MODIFY_FLAG_TOGGLE 2            // 切换标志位
/** @brief 修改对象标志位 @param target 目标对象 @param flag 标志位 @param value 操作类型 */
void _ui_flag_modify(lv_obj_t * target, int32_t flag, int value);

// 状态修改操作类型
#define _UI_MODIFY_STATE_ADD 0              // 添加状态
#define _UI_MODIFY_STATE_REMOVE 1           // 移除状态
#define _UI_MODIFY_STATE_TOGGLE 2           // 切换状态
/** @brief 修改对象状态 @param target 目标对象 @param state 状态值 @param value 操作类型 */
void _ui_state_modify(lv_obj_t * target, int32_t state, int value);

/** @brief 设置对象透明度 @param target 目标对象 @param val 透明度值 */
void _ui_opacity_set(lv_obj_t * target, int val);

// 动画回调函数：设置属性
/** @brief 动画回调：设置X坐标 @param a 动画对象 @param v 坐标值 */
void _ui_anim_callback_set_x(lv_anim_t * a, int32_t v);

/** @brief 动画回调：设置Y坐标 @param a 动画对象 @param v 坐标值 */
void _ui_anim_callback_set_y(lv_anim_t * a, int32_t v);

/** @brief 动画回调：设置宽度 @param a 动画对象 @param v 宽度值 */
void _ui_anim_callback_set_width(lv_anim_t * a, int32_t v);

/** @brief 动画回调：设置高度 @param a 动画对象 @param v 高度值 */
void _ui_anim_callback_set_height(lv_anim_t * a, int32_t v);

/** @brief 动画回调：设置透明度 @param a 动画对象 @param v 透明度值 */
void _ui_anim_callback_set_opacity(lv_anim_t * a, int32_t v);

/** @brief 动画回调：设置图像缩放 @param a 动画对象 @param v 缩放值 */
void _ui_anim_callback_set_image_zoom(lv_anim_t * a, int32_t v);

/** @brief 动画回调：设置图像旋转角度 @param a 动画对象 @param v 角度值 */
void _ui_anim_callback_set_image_angle(lv_anim_t * a, int32_t v);

// 动画回调函数：获取属性
/** @brief 动画回调：获取X坐标 @param a 动画对象 @return X坐标值 */
int32_t _ui_anim_callback_get_x(lv_anim_t * a);

/** @brief 动画回调：获取Y坐标 @param a 动画对象 @return Y坐标值 */
int32_t _ui_anim_callback_get_y(lv_anim_t * a);

/** @brief 动画回调：获取宽度 @param a 动画对象 @return 宽度值 */
int32_t _ui_anim_callback_get_width(lv_anim_t * a);

/** @brief 动画回调：获取高度 @param a 动画对象 @return 高度值 */
int32_t _ui_anim_callback_get_height(lv_anim_t * a);

/** @brief 动画回调：获取透明度 @param a 动画对象 @return 透明度值 */
int32_t _ui_anim_callback_get_opacity(lv_anim_t * a);

/** @brief 动画回调：获取图像缩放 @param a 动画对象 @return 缩放值 */
int32_t _ui_anim_callback_get_image_zoom(lv_anim_t * a);

/** @brief 动画回调：获取图像旋转角度 @param a 动画对象 @return 角度值 */
int32_t _ui_anim_callback_get_image_angle(lv_anim_t * a);

/** @brief 将弧形控件的值格式化显示到文本标签 @param trg 目标标签 @param src 源弧形控件 @param prefix 前缀字符串 @param postfix 后缀字符串 */
void _ui_arc_set_text_value(lv_obj_t * trg, lv_obj_t * src, char * prefix, char * postfix);

/** @brief 将滑块的值格式化显示到文本标签 @param trg 目标标签 @param src 源滑块 @param prefix 前缀字符串 @param postfix 后缀字符串 */
void _ui_slider_set_text_value(lv_obj_t * trg, lv_obj_t * src, char * prefix, char * postfix);

/** @brief 根据勾选状态设置文本标签 @param trg 目标标签 @param src 源对象 @param txt_on 选中时显示的文本 @param txt_off 未选中时显示的文本 */
void _ui_checked_set_text_value(lv_obj_t * trg, lv_obj_t * src, char * txt_on, char * txt_off);


#endif
