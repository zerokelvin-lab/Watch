/** @file ui_SOSPage.c
 *  @brief SOS紧急告警页面 - 红黑主题全屏，显示异常原因+5秒倒计时自动上报，按任意键取消
 */
#include "../../ui.h"
#include "../Inc/ui_SOSPage.h"
#include "../../../Func/Inc/PageManager.h"
#include "user_SosTask.h"
#include "user_TasksInit.h"
#include <stdio.h>
#include <string.h>

/* ====== 页面管理器 ====== */
Page_t Page_SOS = {ui_SOSPage_screen_init, ui_SOSPage_screen_deinit, &ui_SOSPage};

/* ====== 变量定义 ====== */
lv_obj_t * ui_SOSPage;           /* SOS页面对象 */
lv_obj_t * ui_SOS_TitleLabel;    /* "SOS" 大字 */
lv_obj_t * ui_SOS_ReasonLabel;   /* 异常原因文字 */
lv_obj_t * ui_SOS_CountdownLabel;/* 倒计时文字 */
lv_obj_t * ui_SOS_HintLabel;     /* 按键提示 */

uint8_t sos_abnormal_type = 0;   /* 当前异常类型编码 */

static lv_timer_t * ui_SOSPageTimer = NULL;
static uint8_t sos_countdown = 5;  /* 倒计时秒数 */

/**
 * @brief  根据异常类型编码生成原因文字
 */
static void sos_get_reason_text(char *buf, uint8_t type)
{
    buf[0] = '\0';
    if(type & SOS_TYPE_FALL)    strcat(buf, "跌倒 ");
    if(type & SOS_TYPE_HR)      strcat(buf, "心率异常 ");
    if(type & SOS_TYPE_SPO2)    strcat(buf, "血氧异常 ");
    if(type & SOS_TYPE_MICRO)   strcat(buf, "微循环异常 ");
    if(type & SOS_TYPE_BP)      strcat(buf, "血压异常 ");
    if(type & SOS_TYPE_HRV)     strcat(buf, "HRV异常 ");
    if(type & SOS_TYPE_FATIGUE) strcat(buf, "疲劳异常 ");
    /* 去掉末尾空格 */
    int len = strlen(buf);
    if(len > 0 && buf[len-1] == ' ') buf[len-1] = '\0';
}

/**
 * @brief  SOS页面定时器回调 - 每秒执行：倒计时、按键检测、自动上报
 */
static void SOSPage_timer_cb(lv_timer_t * timer)
{
    if(Page_Get_NowPage()->page_obj != &ui_SOSPage)
        return;

    /* 防止息屏：持续发送idle-break */
    uint8_t idle_break = 0;
    osMessageQueuePut(IdleBreak_MessageQueue, &idle_break, 0, 0);

    /* 检测按键（非阻塞） */
    uint8_t key_val;
    if(osMessageQueueGet(Key_MessageQueue, &key_val, NULL, 0) == osOK)
    {
        /* 任意按键 → 取消SOS */
        SOS_Cancel();
        return;
    }

    /* 倒计时 */
    if(sos_countdown > 0)
    {
        sos_countdown--;
        char buf[24];
        sprintf(buf, "%d秒后自动上报", sos_countdown);
        lv_label_set_text(ui_SOS_CountdownLabel, buf);
    }

    /* 倒计时结束 → 上报 */
    if(sos_countdown == 0)
    {
        SOS_Report(sos_abnormal_type);
        Page_Back();
    }
}

/**
 * @brief  SOS页面初始化
 */
void ui_SOSPage_screen_init(void)
{
    char reason_buf[64];

    sos_countdown = 5;  /* 重置倒计时 */

    /* 创建全屏页面 */
    ui_SOSPage = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_SOSPage, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_SOSPage, lv_color_hex(0xCC0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_SOSPage, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* "SOS" 大字 */
    ui_SOS_TitleLabel = lv_label_create(ui_SOSPage);
    lv_obj_set_width(ui_SOS_TitleLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_SOS_TitleLabel, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_SOS_TitleLabel, 0);
    lv_obj_set_y(ui_SOS_TitleLabel, -70);
    lv_obj_set_align(ui_SOS_TitleLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_SOS_TitleLabel, "SOS");
    lv_obj_set_style_text_color(ui_SOS_TitleLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_SOS_TitleLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_SOS_TitleLabel, &ui_font_Cuyuan80, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* 异常原因文字 */
    ui_SOS_ReasonLabel = lv_label_create(ui_SOSPage);
    lv_obj_set_width(ui_SOS_ReasonLabel, 200);
    lv_obj_set_height(ui_SOS_ReasonLabel, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_SOS_ReasonLabel, 0);
    lv_obj_set_y(ui_SOS_ReasonLabel, -10);
    lv_obj_set_align(ui_SOS_ReasonLabel, LV_ALIGN_CENTER);
    lv_label_set_long_mode(ui_SOS_ReasonLabel, LV_LABEL_LONG_WRAP);
    lv_label_set_text(ui_SOS_ReasonLabel, "异常");
    lv_obj_set_style_text_color(ui_SOS_ReasonLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_SOS_ReasonLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_SOS_ReasonLabel, &ui_font_Cuyuan24, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_SOS_ReasonLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    /* 根据异常类型设置原因文字 */
    sos_get_reason_text(reason_buf, sos_abnormal_type);
    if(strlen(reason_buf) > 0)
        lv_label_set_text(ui_SOS_ReasonLabel, reason_buf);

    /* 倒计时文字 */
    ui_SOS_CountdownLabel = lv_label_create(ui_SOSPage);
    lv_obj_set_width(ui_SOS_CountdownLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_SOS_CountdownLabel, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_SOS_CountdownLabel, 0);
    lv_obj_set_y(ui_SOS_CountdownLabel, 35);
    lv_obj_set_align(ui_SOS_CountdownLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_SOS_CountdownLabel, "5秒后自动上报");
    lv_obj_set_style_text_color(ui_SOS_CountdownLabel, lv_color_hex(0xFFFF00), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_SOS_CountdownLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_SOS_CountdownLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* 按键提示 */
    ui_SOS_HintLabel = lv_label_create(ui_SOSPage);
    lv_obj_set_width(ui_SOS_HintLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_SOS_HintLabel, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_SOS_HintLabel, 0);
    lv_obj_set_y(ui_SOS_HintLabel, 70);
    lv_obj_set_align(ui_SOS_HintLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_SOS_HintLabel, "按任意键取消");
    lv_obj_set_style_text_color(ui_SOS_HintLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_SOS_HintLabel, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_SOS_HintLabel, &ui_font_Cuyuan18, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* 创建1秒定时器 */
    ui_SOSPageTimer = lv_timer_create(SOSPage_timer_cb, 1000, NULL);
}

/**
 * @brief  SOS页面注销
 */
void ui_SOSPage_screen_deinit(void)
{
    if(ui_SOSPageTimer != NULL)
    {
        lv_timer_del(ui_SOSPageTimer);
        ui_SOSPageTimer = NULL;
    }
}
