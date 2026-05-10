/** @file ui_helpers.c @brief SquareLine Studio生成的LVGL控件属性设置与动画辅助函数实现 */

// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.2.0
// PROJECT: MenuPage

#include "ui_helpers.h"

/**
 * @brief 设置滑动条属性
 * @param target 目标滑动条对象
 * @param id 属性ID（_UI_BAR_PROPERTY_VALUE 或 _UI_BAR_PROPERTY_VALUE_WITH_ANIM）
 * @param val 要设置的属性值
 */
void _ui_bar_set_property(lv_obj_t * target, int id, int val)
{
    // 带动画设置滑动条值
    if(id == _UI_BAR_PROPERTY_VALUE_WITH_ANIM) lv_bar_set_value(target, val, LV_ANIM_ON);
    // 直接设置滑动条值（无动画）
    if(id == _UI_BAR_PROPERTY_VALUE) lv_bar_set_value(target, val, LV_ANIM_OFF);
}

/**
 * @brief 设置控件基本属性（位置、尺寸）
 * @param target 目标对象
 * @param id 属性ID（_UI_BASIC_PROPERTY_POSITION_X/Y/WIDTH/HEIGHT）
 * @param val 属性值
 */
void _ui_basic_set_property(lv_obj_t * target, int id, int val)
{
    // 设置X坐标
    if(id == _UI_BASIC_PROPERTY_POSITION_X) lv_obj_set_x(target, val);
    // 设置Y坐标
    if(id == _UI_BASIC_PROPERTY_POSITION_Y) lv_obj_set_y(target, val);
    // 设置宽度
    if(id == _UI_BASIC_PROPERTY_WIDTH) lv_obj_set_width(target, val);
    // 设置高度
    if(id == _UI_BASIC_PROPERTY_HEIGHT) lv_obj_set_height(target, val);
}


/**
 * @brief 设置下拉框选中项
 * @param target 目标下拉框对象
 * @param id 属性ID
 * @param val 选中的选项索引
 */
void _ui_dropdown_set_property(lv_obj_t * target, int id, int val)
{
    // 设置下拉框选中项
    if(id == _UI_DROPDOWN_PROPERTY_SELECTED) lv_dropdown_set_selected(target, val);
}

/**
 * @brief 设置图像源
 * @param target 目标图像对象
 * @param id 属性ID
 * @param val 图像源数据指针
 */
void _ui_image_set_property(lv_obj_t * target, int id, uint8_t * val)
{
    // 设置图像源
    if(id == _UI_IMAGE_PROPERTY_IMAGE) lv_img_set_src(target, val);
}

/**
 * @brief 设置标签文本内容
 * @param target 目标标签对象
 * @param id 属性ID
 * @param val 文本字符串
 */
void _ui_label_set_property(lv_obj_t * target, int id, char * val)
{
    // 设置标签文本
    if(id == _UI_LABEL_PROPERTY_TEXT) lv_label_set_text(target, val);
}


/**
 * @brief 设置滚轮选中项
 * @param target 目标滚轮对象
 * @param id 属性ID
 * @param val 选中的选项索引
 */
void _ui_roller_set_property(lv_obj_t * target, int id, int val)
{
    // 带动画设置滚轮选中项
    if(id == _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM) lv_roller_set_selected(target, val, LV_ANIM_ON);
    // 直接设置滚轮选中项（无动画）
    if(id == _UI_ROLLER_PROPERTY_SELECTED) lv_roller_set_selected(target, val, LV_ANIM_OFF);
}

/**
 * @brief 设置滑块值
 * @param target 目标滑块对象
 * @param id 属性ID
 * @param val 滑块值
 */
void _ui_slider_set_property(lv_obj_t * target, int id, int val)
{
    // 带动画设置滑块值
    if(id == _UI_SLIDER_PROPERTY_VALUE_WITH_ANIM) lv_slider_set_value(target, val, LV_ANIM_ON);
    // 直接设置滑块值（无动画）
    if(id == _UI_SLIDER_PROPERTY_VALUE) lv_slider_set_value(target, val, LV_ANIM_OFF);
}


/**
 * @brief 切换屏幕显示，支持淡入淡出等动画效果
 * @param target 目标屏幕对象
 * @param fademode 动画过渡模式（如淡入、淡出、滑动等）
 * @param spd 动画速度（ms）
 * @param delay 延迟时间（ms）
 */
void _ui_screen_change(lv_obj_t * target, lv_scr_load_anim_t fademode, int spd, int delay)
{
    // 带动画加载目标屏幕
    lv_scr_load_anim(target, fademode, spd, delay, false);
}

/**
 * @brief 弧形控件值增加指定增量
 * @param target 目标弧形控件
 * @param val 增量值（可正可负）
 */
void _ui_arc_increment(lv_obj_t * target, int val)
{
    // 获取当前值，累加增量后设置
    int old = lv_arc_get_value(target);
    lv_arc_set_value(target, old + val);
}

/**
 * @brief 滑动条值增加指定增量
 * @param target 目标滑动条
 * @param val 增量值
 * @param anm 是否启用动画
 */
void _ui_bar_increment(lv_obj_t * target, int val, int anm)
{
    // 获取当前值，累加增量后设置
    int old = lv_bar_get_value(target);
    lv_bar_set_value(target, old + val, anm);
}

/**
 * @brief 滑块值增加指定增量
 * @param target 目标滑块
 * @param val 增量值
 * @param anm 是否启用动画
 */
void _ui_slider_increment(lv_obj_t * target, int val, int anm)
{
    // 获取当前值，累加增量后设置
    int old = lv_slider_get_value(target);
    lv_slider_set_value(target, old + val, anm);
}

/**
 * @brief 修改对象标志位（添加、移除或切换）
 * @param target 目标对象
 * @param flag 要修改的标志位
 * @param value 操作类型：_UI_MODIFY_FLAG_ADD 添加, _UI_MODIFY_FLAG_REMOVE 移除, _UI_MODIFY_FLAG_TOGGLE 切换
 */
void _ui_flag_modify(lv_obj_t * target, int32_t flag, int value)
{
    if(value == _UI_MODIFY_FLAG_TOGGLE) {
        // 切换标志位：存在则清除，不存在则添加
        if(lv_obj_has_flag(target, flag)) lv_obj_clear_flag(target, flag);
        else lv_obj_add_flag(target, flag);
    }
    // 添加标志位
    else if(value == _UI_MODIFY_FLAG_ADD) lv_obj_add_flag(target, flag);
    // 移除标志位
    else lv_obj_clear_flag(target, flag);
}

/**
 * @brief 修改对象状态（添加、移除或切换）
 * @param target 目标对象
 * @param state 要修改的状态
 * @param value 操作类型：_UI_MODIFY_STATE_ADD 添加, _UI_MODIFY_STATE_REMOVE 移除, _UI_MODIFY_STATE_TOGGLE 切换
 */
void _ui_state_modify(lv_obj_t * target, int32_t state, int value)
{
    if(value == _UI_MODIFY_STATE_TOGGLE) {
        // 切换状态：存在则清除，不存在则添加
        if(lv_obj_has_state(target, state)) lv_obj_clear_state(target, state);
        else lv_obj_add_state(target, state);
    }
    // 添加状态
    else if(value == _UI_MODIFY_STATE_ADD) lv_obj_add_state(target, state);
    // 移除状态
    else lv_obj_clear_state(target, state);
}

/**
 * @brief 设置对象不透明度
 * @param target 目标对象
 * @param val 不透明度值（0=完全透明, 255=完全不透明）
 */
void _ui_opacity_set(lv_obj_t * target, int val)
{
    // 设置对象样式的不透明度
    lv_obj_set_style_opa(target, val, 0);
}

/**
 * @brief 动画回调：设置对象X坐标
 * @param a 动画控制结构体
 * @param v 目标X坐标值
 */
void _ui_anim_callback_set_x(lv_anim_t * a, int32_t v)
{
    // 通过user_data获取对象，设置X坐标
    lv_obj_set_x((lv_obj_t *)a->user_data, v);
}

/**
 * @brief 动画回调：设置对象Y坐标
 * @param a 动画控制结构体
 * @param v 目标Y坐标值
 */
void _ui_anim_callback_set_y(lv_anim_t * a, int32_t v)
{
    // 通过user_data获取对象，设置Y坐标
    lv_obj_set_y((lv_obj_t *)a->user_data, v);
}

/**
 * @brief 动画回调：设置对象宽度
 * @param a 动画控制结构体
 * @param v 目标宽度值
 */
void _ui_anim_callback_set_width(lv_anim_t * a, int32_t v)
{
    // 通过user_data获取对象，设置宽度
    lv_obj_set_width((lv_obj_t *)a->user_data, v);
}

/**
 * @brief 动画回调：设置对象高度
 * @param a 动画控制结构体
 * @param v 目标高度值
 */
void _ui_anim_callback_set_height(lv_anim_t * a, int32_t v)
{
    // 通过user_data获取对象，设置高度
    lv_obj_set_height((lv_obj_t *)a->user_data, v);
}

/**
 * @brief 动画回调：设置对象不透明度
 * @param a 动画控制结构体
 * @param v 目标不透明度值
 */
void _ui_anim_callback_set_opacity(lv_anim_t * a, int32_t v)
{
    // 通过user_data获取对象，设置不透明度
    lv_obj_set_style_opa((lv_obj_t *)a->user_data, v, 0);
}

/**
 * @brief 动画回调：设置图像缩放比例
 * @param a 动画控制结构体
 * @param v 目标缩放值（256=100%）
 */
void _ui_anim_callback_set_image_zoom(lv_anim_t * a, int32_t v)
{
    // 通过user_data获取图像对象，设置缩放
    lv_img_set_zoom((lv_obj_t *)a->user_data, v);
}

/**
 * @brief 动画回调：设置图像旋转角度
 * @param a 动画控制结构体
 * @param v 目标角度值（单位：0.1度）
 */
void _ui_anim_callback_set_image_angle(lv_anim_t * a, int32_t v)
{
    // 通过user_data获取图像对象，设置旋转角度
    lv_img_set_angle((lv_obj_t *)a->user_data, v);
}


/**
 * @brief 动画回调：获取对象X坐标
 * @param a 动画控制结构体
 * @return 当前X坐标值
 */
int32_t _ui_anim_callback_get_x(lv_anim_t * a)
{
    return lv_obj_get_x_aligned((lv_obj_t *)a->user_data);
}

/**
 * @brief 动画回调：获取对象Y坐标
 * @param a 动画控制结构体
 * @return 当前Y坐标值
 */
int32_t _ui_anim_callback_get_y(lv_anim_t * a)
{
    return lv_obj_get_y_aligned((lv_obj_t *)a->user_data);
}

/**
 * @brief 动画回调：获取对象宽度
 * @param a 动画控制结构体
 * @return 当前宽度值
 */
int32_t _ui_anim_callback_get_width(lv_anim_t * a)
{
    return lv_obj_get_width((lv_obj_t *)a->user_data);
}

/**
 * @brief 动画回调：获取对象高度
 * @param a 动画控制结构体
 * @return 当前高度值
 */
int32_t _ui_anim_callback_get_height(lv_anim_t * a)
{
    return lv_obj_get_height((lv_obj_t *)a->user_data);
}

/**
 * @brief 动画回调：获取对象不透明度
 * @param a 动画控制结构体
 * @return 当前不透明度值
 */
int32_t _ui_anim_callback_get_opacity(lv_anim_t * a)
{
    return lv_obj_get_style_opa((lv_obj_t *)a->user_data, 0);
}

/**
 * @brief 动画回调：获取图像缩放比例
 * @param a 动画控制结构体
 * @return 当前缩放值
 */
int32_t _ui_anim_callback_get_image_zoom(lv_anim_t * a)
{
    return lv_img_get_zoom((lv_obj_t *)a->user_data);
}

/**
 * @brief 动画回调：获取图像旋转角度
 * @param a 动画控制结构体
 * @return 当前角度值
 */
int32_t _ui_anim_callback_get_image_angle(lv_anim_t * a)
{
    return lv_img_get_angle((lv_obj_t *)a->user_data);
}

/**
 * @brief 将弧形控件的当前值格式化显示到文本标签
 * @param trg 目标标签对象
 * @param src 源弧形控件
 * @param prefix 显示前缀字符串（可为空）
 * @param postfix 显示后缀字符串（可为空）
 */
void _ui_arc_set_text_value(lv_obj_t * trg, lv_obj_t * src, char * prefix, char * postfix)
{
    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE];
    // 格式化字符串：前缀 + 弧形值 + 后缀
    lv_snprintf(buf, sizeof(buf), "%s%d%s", prefix, (int)lv_arc_get_value(src), postfix);
    lv_label_set_text(trg, buf);
}

/**
 * @brief 将滑块的当前值格式化显示到文本标签
 * @param trg 目标标签对象
 * @param src 源滑块控件
 * @param prefix 显示前缀字符串
 * @param postfix 显示后缀字符串
 */
void _ui_slider_set_text_value(lv_obj_t * trg, lv_obj_t * src, char * prefix, char * postfix)
{
    char buf[_UI_TEMPORARY_STRING_BUFFER_SIZE];
    // 格式化字符串：前缀 + 滑块值 + 后缀
    lv_snprintf(buf, sizeof(buf), "%s%d%s", prefix, (int)lv_slider_get_value(src), postfix);
    lv_label_set_text(trg, buf);
}

/**
 * @brief 根据对象的勾选状态设置文本标签内容
 * @param trg 目标标签对象
 * @param src 源对象（需支持LV_STATE_CHECKED状态）
 * @param txt_on 勾选状态时显示的文本
 * @param txt_off 未勾选状态时显示的文本
 */
void _ui_checked_set_text_value(lv_obj_t * trg, lv_obj_t * src, char * txt_on, char * txt_off)
{
    // 根据勾选状态设置不同文本
    if(lv_obj_has_state(src, LV_STATE_CHECKED)) lv_label_set_text(trg, txt_on);
    else lv_label_set_text(trg, txt_off);
}

