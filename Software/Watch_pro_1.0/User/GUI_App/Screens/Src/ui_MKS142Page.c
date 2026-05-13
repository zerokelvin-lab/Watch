/** @file ui_MKS142Page.c
 *  @brief MKS_142健康监测页面 - 上半部分心率，下半部分血氧，图标在数据左侧
 *         稳定数据自动保存，连续有效5秒自动停止，点击屏幕重新测量
 */
#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_MKS142Page.h"
#include "../../../Func/Inc/HWDataAccess.h"
#include "mks_142.h"

extern uint32_t IdleTimerCount;

#define MKS142_STABLE_SECONDS  5  /** 连续稳定数据秒数，达到后自动停止 */
#define MKS142_HR_THRESHOLD    3  /** 心率波动阈值 bpm */
#define MKS142_SPO2_THRESHOLD  2  /** 血氧波动阈值 % */

/* ====== 页面管理器 ====== */
Page_t Page_MKS142 = {ui_MKS142Page_screen_init, ui_MKS142Page_screen_deinit, &ui_MKS142Page};

/* ====== 变量定义 ====== */
lv_obj_t * ui_MKS142Page;
lv_obj_t * ui_MKS142HRNum;
lv_obj_t * ui_MKS142HRUnit;
lv_obj_t * ui_MKS142HRIcon;
lv_obj_t * ui_MKS142SpO2Num;
lv_obj_t * ui_MKS142SpO2Unit;
lv_obj_t * ui_MKS142SpO2Icon;
lv_obj_t * ui_MKS142NoticeLabel;

lv_timer_t * ui_MKS142PageTimer;

/* ====== 历史数据保存（静态，跨页面生命周期保留） ====== */
uint8_t MKS142_saved_hr = 0;
uint8_t MKS142_saved_spo2 = 0;
static uint32_t saved_seconds_ago = 0;
static uint8_t has_saved_data = 0;
static uint8_t is_measuring = 0;       // 当前是否在测量
static uint8_t stable_count = 0;       // 连续稳定数据计数
static uint8_t prev_hr = 0;            // 上一次心率（用于稳定性判断）
static uint8_t prev_spo2 = 0;          // 上一次血氧（用于稳定性判断）

/**
 * @brief  页面点击事件 - 点击屏幕重新开始测量
 */
static void ui_MKS142Page_click_cb(lv_event_t * e)
{
    if(!is_measuring)
    {
        is_measuring = 1;
        stable_count = 0;
        prev_hr = 0;
        prev_spo2 = 0;
        saved_seconds_ago = 0;
        MKS142_StartMeasure();
        lv_label_set_text(ui_MKS142NoticeLabel, "\xE6\xAD\xA3\xE5\x9C\xA8\xE6\xB5\x8B\xE9\x87\x8F..."); // 正在测量...
    }
}

/**
 * @brief  定时器回调 - 每1000ms刷新心率和血氧
 */
static void MKS142Page_timer_cb(lv_timer_t * timer)
{
    if(Page_Get_NowPage()->page_obj == &ui_MKS142Page)
    {
        uint8_t strbuf[24];

        if(!is_measuring)
        {
            /* 已停止测量，只更新时间显示 */
            if(has_saved_data)
            {
                saved_seconds_ago++;
                uint32_t min = saved_seconds_ago / 60;
                if(min == 0)
                    sprintf(strbuf, "\xE5\x88\x9A\xE5\x88\x9A\xE6\xB5\x8B\xE9\x87\x8F"); // 刚刚测量
                else
                    sprintf(strbuf, "%d\xE5\x88\x86\xE9\x92\x9F\xE5\x89\x8D\xE6\xB5\x8B\xE9\x87\x8F", min); // X分钟前测量
                lv_label_set_text(ui_MKS142NoticeLabel, strbuf);
            }
            return;
        }

        /* 正在测量中 */
        MKS142_CheckAlive();

        if(MKS142_IsDataValid())
        {
            IdleTimerCount = 0;

            uint8_t cur_hr = MKS142_GetHeartRate();
            uint8_t cur_spo2 = MKS142_GetSpO2();

            /* 判断是否稳定：不为0，且波动在阈值内 */
            uint8_t hr_stable = (cur_hr > 0) &&
                (abs((int)cur_hr - (int)prev_hr) <= MKS142_HR_THRESHOLD);
            uint8_t spo2_stable = (cur_spo2 > 0) &&
                (abs((int)cur_spo2 - (int)prev_spo2) <= MKS142_SPO2_THRESHOLD);

            prev_hr = cur_hr;
            prev_spo2 = cur_spo2;

            if(hr_stable && spo2_stable)
                stable_count++;
            else
                stable_count = 0;

            /* 保存并显示当前值 */
            MKS142_saved_hr = cur_hr;
            MKS142_saved_spo2 = cur_spo2;
            saved_seconds_ago = 0;
            has_saved_data = 1;

            sprintf(strbuf, "%d", MKS142_saved_hr);
            lv_label_set_text(ui_MKS142HRNum, strbuf);

            sprintf(strbuf, "%d%%", MKS142_saved_spo2);
            lv_label_set_text(ui_MKS142SpO2Num, strbuf);

            /* 连续稳定达到阈值，自动停止 */
            if(stable_count >= MKS142_STABLE_SECONDS)
            {
                MKS142_StopMeasure();
                is_measuring = 0;
                lv_label_set_text(ui_MKS142NoticeLabel, "\xE7\x82\xB9\xE5\x87\xBB\xE5\xB1\x8F\xE5\xB9\x95\xE9\x87\x8D\xE6\x96\xB0\xE6\xB5\x8B\xE9\x87\x8F"); // 点击屏幕重新测量
            }
            else
            {
                lv_label_set_text(ui_MKS142NoticeLabel, "\xE6\xAD\xA3\xE5\x9C\xA8\xE6\xB5\x8B\xE9\x87\x8F..."); // 正在测量...
            }
        }
        else
        {
            /* 数据无效，重置稳定计数 */
            stable_count = 0;
            saved_seconds_ago++;

            if(has_saved_data)
            {
                /* 显示历史数据 */
                sprintf(strbuf, "%d", MKS142_saved_hr);
                lv_label_set_text(ui_MKS142HRNum, strbuf);

                sprintf(strbuf, "%d%%", MKS142_saved_spo2);
                lv_label_set_text(ui_MKS142SpO2Num, strbuf);

                uint32_t min = saved_seconds_ago / 60;
                if(min == 0)
                    sprintf(strbuf, "\xE5\x88\x9A\xE5\x88\x9A\xE6\xB5\x8B\xE9\x87\x8F"); // 刚刚测量
                else
                    sprintf(strbuf, "%d\xE5\x88\x86\xE9\x92\x9F\xE5\x89\x8D\xE6\xB5\x8B\xE9\x87\x8F", min); // X分钟前测量
                lv_label_set_text(ui_MKS142NoticeLabel, strbuf);
            }
            else
            {
                lv_label_set_text(ui_MKS142HRNum, "--");
                lv_label_set_text(ui_MKS142SpO2Num, "--");
                lv_label_set_text(ui_MKS142NoticeLabel, "\xE6\x97\xA0\xE4\xBF\xA1\xE5\x8F\xB7"); // 无信号
            }
        }
    }
}

/**
 * @brief  健康页面初始化 - 上半心率，下半血氧，图标在左侧
 */
void ui_MKS142Page_screen_init(void)
{
    ui_MKS142Page = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_MKS142Page, LV_OBJ_FLAG_SCROLLABLE);

    /* 注册点击事件，点击屏幕重新测量 */
    lv_obj_add_event_cb(ui_MKS142Page, ui_MKS142Page_click_cb, LV_EVENT_CLICKED, NULL);

    /* ====== 上半部分：心率 ====== */

    /* 心率图标（红色爱心，位于数字左侧） */
    ui_MKS142HRIcon = lv_label_create(ui_MKS142Page);
    lv_obj_set_width(ui_MKS142HRIcon, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_MKS142HRIcon, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_MKS142HRIcon, LV_ALIGN_CENTER);
    lv_obj_set_x(ui_MKS142HRIcon, -65);
    lv_obj_set_y(ui_MKS142HRIcon, -50);
    lv_label_set_text(ui_MKS142HRIcon, "\xEE\x98\x83");
    lv_obj_set_style_text_color(ui_MKS142HRIcon, lv_color_hex(0xFF1919), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_MKS142HRIcon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_MKS142HRIcon, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* 心率数字（80号大字，红色） */
    ui_MKS142HRNum = lv_label_create(ui_MKS142Page);
    lv_obj_set_width(ui_MKS142HRNum, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_MKS142HRNum, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_MKS142HRNum, LV_ALIGN_CENTER);
    lv_obj_set_x(ui_MKS142HRNum, 5);
    lv_obj_set_y(ui_MKS142HRNum, -55);
    lv_label_set_text(ui_MKS142HRNum, "--");
    lv_obj_set_style_text_color(ui_MKS142HRNum, lv_color_hex(0xFF1919), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_MKS142HRNum, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_MKS142HRNum, &ui_font_Cuyuan80, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* bpm单位 */
    ui_MKS142HRUnit = lv_label_create(ui_MKS142Page);
    lv_obj_set_width(ui_MKS142HRUnit, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_MKS142HRUnit, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_MKS142HRUnit, LV_ALIGN_CENTER);
    lv_obj_set_x(ui_MKS142HRUnit, 25);
    lv_obj_set_y(ui_MKS142HRUnit, -15);
    lv_label_set_text(ui_MKS142HRUnit, "bpm");
    lv_obj_set_style_text_color(ui_MKS142HRUnit, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_MKS142HRUnit, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_MKS142HRUnit, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 下半部分：血氧 ====== */

    /* 血氧图标（蓝色，位于数字左侧） */
    ui_MKS142SpO2Icon = lv_label_create(ui_MKS142Page);
    lv_obj_set_width(ui_MKS142SpO2Icon, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_MKS142SpO2Icon, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_MKS142SpO2Icon, LV_ALIGN_CENTER);
    lv_obj_set_x(ui_MKS142SpO2Icon, -55);
    lv_obj_set_y(ui_MKS142SpO2Icon, 40);
    lv_label_set_text(ui_MKS142SpO2Icon, "\xEE\x99\x92");
    lv_obj_set_style_text_color(ui_MKS142SpO2Icon, lv_color_hex(0x0080FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_MKS142SpO2Icon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_MKS142SpO2Icon, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* 血氧数字（48号字，蓝色） */
    ui_MKS142SpO2Num = lv_label_create(ui_MKS142Page);
    lv_obj_set_width(ui_MKS142SpO2Num, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_MKS142SpO2Num, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_MKS142SpO2Num, LV_ALIGN_CENTER);
    lv_obj_set_x(ui_MKS142SpO2Num, 10);
    lv_obj_set_y(ui_MKS142SpO2Num, 35);
    lv_label_set_text(ui_MKS142SpO2Num, "--");
    lv_obj_set_style_text_color(ui_MKS142SpO2Num, lv_color_hex(0x0080FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_MKS142SpO2Num, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_MKS142SpO2Num, &ui_font_Cuyuan48, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* 血氧单位 % */
    ui_MKS142SpO2Unit = lv_label_create(ui_MKS142Page);
    lv_obj_set_width(ui_MKS142SpO2Unit, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_MKS142SpO2Unit, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_MKS142SpO2Unit, LV_ALIGN_CENTER);
    lv_obj_set_x(ui_MKS142SpO2Unit, 55);
    lv_obj_set_y(ui_MKS142SpO2Unit, 48);
    lv_label_set_text(ui_MKS142SpO2Unit, "%");
    lv_obj_set_style_text_color(ui_MKS142SpO2Unit, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_MKS142SpO2Unit, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_MKS142SpO2Unit, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 状态提示 ====== */
    ui_MKS142NoticeLabel = lv_label_create(ui_MKS142Page);
    lv_obj_set_width(ui_MKS142NoticeLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_MKS142NoticeLabel, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_MKS142NoticeLabel, 0);
    lv_obj_set_y(ui_MKS142NoticeLabel, 100);
    lv_obj_set_align(ui_MKS142NoticeLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_MKS142NoticeLabel, "\xE6\xAD\xA3\xE5\x9C\xA8\xE6\xB5\x8B\xE9\x87\x8F..."); // 正在测量...
    lv_obj_set_style_text_color(ui_MKS142NoticeLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_MKS142NoticeLabel, 220, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_MKS142NoticeLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* 初始化显示：有历史数据则显示旧值 */
    if(has_saved_data)
    {
        uint8_t strbuf[16];
        sprintf(strbuf, "%d", MKS142_saved_hr);
        lv_label_set_text(ui_MKS142HRNum, strbuf);
        sprintf(strbuf, "%d%%", MKS142_saved_spo2);
        lv_label_set_text(ui_MKS142SpO2Num, strbuf);
    }

    /* 创建定时器，每1000ms刷新 */
    ui_MKS142PageTimer = lv_timer_create(MKS142Page_timer_cb, 1000, NULL);

    /* 启动测量 */
    is_measuring = 1;
    stable_count = 0;
    prev_hr = 0;
    prev_spo2 = 0;
    saved_seconds_ago = 0;
    MKS142_StartMeasure();
    IdleTimerCount = 0;
}

/**
 * @brief  健康页面注销
 */
void ui_MKS142Page_screen_deinit(void)
{
    MKS142_StopMeasure();
    is_measuring = 0;
    lv_timer_del(ui_MKS142PageTimer);
}
