/** @file ui_MKS142DetailPage.c
 *  @brief MKS_142健康详情页面 - 疲劳度、微循环、呼吸率、血压、RMSSD
 *         稳定数据自动保存，连续有效5秒自动停止，点击屏幕重新测量
 */
#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_MKS142DetailPage.h"
#include "../../../Func/Inc/HWDataAccess.h"
#include "mks_142.h"

extern uint32_t IdleTimerCount;

#define MKS142_STABLE_SECONDS  5  /** 连续稳定数据秒数，达到后自动停止 */
#define MKS142_PARAM_THRESHOLD 5  /** 通用参数波动阈值 */

/* ====== 页面管理器 ====== */
Page_t Page_MKS142Detail = {ui_MKS142DetailPage_screen_init, ui_MKS142DetailPage_screen_deinit, &ui_MKS142DetailPage};

/* ====== 变量定义 ====== */
lv_obj_t * ui_MKS142DetailPage;
lv_obj_t * ui_MKS142DetailFatigueNum;
lv_obj_t * ui_MKS142DetailMicroNum;
lv_obj_t * ui_MKS142DetailRespNum;
lv_obj_t * ui_MKS142DetailBPNum;
lv_obj_t * ui_MKS142DetailRMSSDNum;
lv_obj_t * ui_MKS142DetailNoticeLabel;

lv_timer_t * ui_MKS142DetailPageTimer;

/* ====== 历史数据保存（静态，跨页面生命周期保留） ====== */
uint8_t MKS142_saved_fatigue = 0;
uint8_t MKS142_saved_micro = 0;
uint8_t MKS142_saved_resp = 0;
uint8_t MKS142_saved_sbp = 0;
uint8_t MKS142_saved_dbp = 0;
uint8_t MKS142_saved_rmssd = 0;
static uint32_t saved_seconds_ago = 0;
static uint8_t has_saved_data = 0;
static uint8_t is_measuring = 0;
static uint8_t stable_count = 0;

/* 上一次读数（用于稳定性判断） */
static uint8_t prev_fatigue = 0;
static uint8_t prev_micro = 0;
static uint8_t prev_resp = 0;
static uint8_t prev_sbp = 0;
static uint8_t prev_dbp = 0;
static uint8_t prev_rmssd = 0;

/**
 * @brief  页面点击事件 - 点击屏幕重新开始测量
 */
static void ui_MKS142DetailPage_click_cb(lv_event_t * e)
{
    if(!is_measuring)
    {
        is_measuring = 1;
        stable_count = 0;
        prev_fatigue = 0; prev_micro = 0; prev_resp = 0;
        prev_sbp = 0; prev_dbp = 0; prev_rmssd = 0;
        saved_seconds_ago = 0;
        MKS142_StartMeasure();
        lv_label_set_text(ui_MKS142DetailNoticeLabel, "\xE6\xAD\xA3\xE5\x9C\xA8\xE6\xB5\x8B\xE9\x87\x8F..."); // 正在测量...
    }
}

/**
 * @brief  更新所有数值显示
 */
static void update_display(void)
{
    uint8_t strbuf[16];

    sprintf(strbuf, "%d", MKS142_saved_fatigue);
    lv_label_set_text(ui_MKS142DetailFatigueNum, strbuf);

    sprintf(strbuf, "%d", MKS142_saved_micro);
    lv_label_set_text(ui_MKS142DetailMicroNum, strbuf);

    sprintf(strbuf, "%d", MKS142_saved_resp);
    lv_label_set_text(ui_MKS142DetailRespNum, strbuf);

    sprintf(strbuf, "%d/%d", MKS142_saved_sbp, MKS142_saved_dbp);
    lv_label_set_text(ui_MKS142DetailBPNum, strbuf);

    sprintf(strbuf, "%dms", MKS142_saved_rmssd);
    lv_label_set_text(ui_MKS142DetailRMSSDNum, strbuf);
}

/**
 * @brief  定时器回调 - 每1000ms刷新
 */
static void MKS142DetailPage_timer_cb(lv_timer_t * timer)
{
    if(Page_Get_NowPage()->page_obj != &ui_MKS142DetailPage)
        return;

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
            lv_label_set_text(ui_MKS142DetailNoticeLabel, strbuf);
        }
        return;
    }

    /* 正在测量中 */
    MKS142_CheckAlive();

    if(MKS142_IsDataValid())
    {
        IdleTimerCount = 0;

        uint8_t cur_fatigue = MKS142_GetFatigue();
        uint8_t cur_micro   = MKS142_GetMicrocirculation();
        uint8_t cur_resp    = MKS142_GetRespRate();
        uint8_t cur_sbp     = MKS142_GetSBP();
        uint8_t cur_dbp     = MKS142_GetDBP();
        uint8_t cur_rmssd   = MKS142_GetRMSSD();

        /* 判断是否稳定：不为0，且波动在阈值内 */
        uint8_t stable = (cur_fatigue > 0) && (cur_micro > 0) && (cur_resp > 0) &&
                         (cur_sbp > 0) && (cur_dbp > 0) && (cur_rmssd > 0) &&
                         (abs((int)cur_fatigue - (int)prev_fatigue) <= MKS142_PARAM_THRESHOLD) &&
                         (abs((int)cur_micro - (int)prev_micro) <= MKS142_PARAM_THRESHOLD) &&
                         (abs((int)cur_resp - (int)prev_resp) <= MKS142_PARAM_THRESHOLD) &&
                         (abs((int)cur_sbp - (int)prev_sbp) <= MKS142_PARAM_THRESHOLD) &&
                         (abs((int)cur_dbp - (int)prev_dbp) <= MKS142_PARAM_THRESHOLD) &&
                         (abs((int)cur_rmssd - (int)prev_rmssd) <= MKS142_PARAM_THRESHOLD);

        prev_fatigue = cur_fatigue; prev_micro = cur_micro;
        prev_resp = cur_resp; prev_sbp = cur_sbp;
        prev_dbp = cur_dbp; prev_rmssd = cur_rmssd;

        if(stable) stable_count++;
        else       stable_count = 0;

        /* 保存并显示当前值 */
        MKS142_saved_fatigue = cur_fatigue; MKS142_saved_micro = cur_micro;
        MKS142_saved_resp = cur_resp; MKS142_saved_sbp = cur_sbp;
        MKS142_saved_dbp = cur_dbp; MKS142_saved_rmssd = cur_rmssd;
        saved_seconds_ago = 0;
        has_saved_data = 1;

        update_display();

        /* 连续稳定达到阈值，自动停止 */
        if(stable_count >= MKS142_STABLE_SECONDS)
        {
            MKS142_StopMeasure();
            is_measuring = 0;
            lv_label_set_text(ui_MKS142DetailNoticeLabel, "\xE7\x82\xB9\xE5\x87\xBB\xE5\xB1\x8F\xE5\xB9\x95\xE9\x87\x8D\xE6\x96\xB0\xE6\xB5\x8B\xE9\x87\x8F"); // 点击屏幕重新测量
        }
        else
        {
            lv_label_set_text(ui_MKS142DetailNoticeLabel, "\xE6\xAD\xA3\xE5\x9C\xA8\xE6\xB5\x8B\xE9\x87\x8F..."); // 正在测量...
        }
    }
    else
    {
        /* 数据无效，重置稳定计数 */
        stable_count = 0;
        saved_seconds_ago++;

        if(has_saved_data)
        {
            update_display();
            uint32_t min = saved_seconds_ago / 60;
            if(min == 0)
                sprintf(strbuf, "\xE5\x88\x9A\xE5\x88\x9A\xE6\xB5\x8B\xE9\x87\x8F"); // 刚刚测量
            else
                sprintf(strbuf, "%d\xE5\x88\x86\xE9\x92\x9F\xE5\x89\x8D\xE6\xB5\x8B\xE9\x87\x8F", min); // X分钟前测量
            lv_label_set_text(ui_MKS142DetailNoticeLabel, strbuf);
        }
        else
        {
            lv_label_set_text(ui_MKS142DetailNoticeLabel, "\xE6\x97\xA0\xE4\xBF\xA1\xE5\x8F\xB7"); // 无信号
        }
    }
}

/**
 * @brief  创建一行参数：标签(左,彩色) + 数值(右,白色)
 */
static lv_obj_t * create_param_row(lv_obj_t *page, lv_coord_t y, const char *label_text, uint32_t label_color)
{
    lv_obj_t *lbl = lv_label_create(page);
    lv_obj_set_align(lbl, LV_ALIGN_CENTER);
    lv_obj_set_x(lbl, -60); lv_obj_set_y(lbl, y);
    lv_label_set_text(lbl, label_text);
    lv_obj_set_style_text_color(lbl, lv_color_hex(label_color), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(lbl, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl, &ui_font_Cuyuan18, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *val = lv_label_create(page);
    lv_obj_set_align(val, LV_ALIGN_CENTER);
    lv_obj_set_x(val, 35); lv_obj_set_y(val, y);
    lv_label_set_text(val, "--");
    lv_obj_set_style_text_color(val, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(val, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(val, &ui_font_Cuyuan24, LV_PART_MAIN | LV_STATE_DEFAULT);

    return val;
}

/**
 * @brief  详情页面初始化
 */
void ui_MKS142DetailPage_screen_init(void)
{
    ui_MKS142DetailPage = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_MKS142DetailPage, LV_OBJ_FLAG_SCROLLABLE);

    /* 注册点击事件 */
    lv_obj_add_event_cb(ui_MKS142DetailPage, ui_MKS142DetailPage_click_cb, LV_EVENT_CLICKED, NULL);

    /* ====== 标题 ====== */
    lv_obj_t *title = lv_label_create(ui_MKS142DetailPage);
    lv_obj_set_align(title, LV_ALIGN_TOP_MID);
    lv_obj_set_x(title, 0); lv_obj_set_y(title, 10);
    lv_label_set_text(title, "\xE5\x81\xA5\xE5\xBA\xB7\xE8\xAF\xA6\xE6\x83\x85"); // 健康详情
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(title, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(title, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 行1: 疲劳 y=-50 ====== */
    ui_MKS142DetailFatigueNum = create_param_row(ui_MKS142DetailPage, -50,
        "\xE7\x96\xB2\xE5\x8A\xB3", 0xFF8000); // 疲劳, 橙色

    /* ====== 行2: 微循环 y=-20 ====== */
    ui_MKS142DetailMicroNum = create_param_row(ui_MKS142DetailPage, -20,
        "\xE5\xBE\xAA\xE7\x8E\xAF", 0x00C8C8); // 循环, 青色

    /* ====== 行3: 呼吸率 y=10 ====== */
    ui_MKS142DetailRespNum = create_param_row(ui_MKS142DetailPage, 10,
        "\xE5\x91\xBC\xE5\x90\xB8", 0x00C800); // 呼吸, 绿色

    /* ====== 行4: 血压 y=40 ====== */
    ui_MKS142DetailBPNum = create_param_row(ui_MKS142DetailPage, 40,
        "\xE8\xA1\x80\xE5\x8E\x8B", 0xC80000); // 血压, 深红色
    lv_label_set_text(ui_MKS142DetailBPNum, "--/--");

    /* ====== 行5: RMSSD y=70 ====== */
    ui_MKS142DetailRMSSDNum = create_param_row(ui_MKS142DetailPage, 70,
        "RMSSD", 0xDC80E6); // RMSSD, 紫色

    /* ====== 状态提示 ====== */
    ui_MKS142DetailNoticeLabel = lv_label_create(ui_MKS142DetailPage);
    lv_obj_set_width(ui_MKS142DetailNoticeLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_MKS142DetailNoticeLabel, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_MKS142DetailNoticeLabel, LV_ALIGN_CENTER);
    lv_obj_set_x(ui_MKS142DetailNoticeLabel, 0); lv_obj_set_y(ui_MKS142DetailNoticeLabel, 105);

    /* 初始化显示：有历史数据则显示旧值 */
    if(has_saved_data)
        update_display();

    lv_label_set_text(ui_MKS142DetailNoticeLabel, "\xE6\xAD\xA3\xE5\x9C\xA8\xE6\xB5\x8B\xE9\x87\x8F..."); // 正在测量...
    lv_obj_set_style_text_color(ui_MKS142DetailNoticeLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_MKS142DetailNoticeLabel, 220, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_MKS142DetailNoticeLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* 创建定时器，每1000ms刷新 */
    ui_MKS142DetailPageTimer = lv_timer_create(MKS142DetailPage_timer_cb, 1000, NULL);

    /* 启动测量 */
    is_measuring = 1;
    stable_count = 0;
    prev_fatigue = 0; prev_micro = 0; prev_resp = 0;
    prev_sbp = 0; prev_dbp = 0; prev_rmssd = 0;
    saved_seconds_ago = 0;
    MKS142_StartMeasure();
    IdleTimerCount = 0;
}

/**
 * @brief  详情页面注销
 */
void ui_MKS142DetailPage_screen_deinit(void)
{
    MKS142_StopMeasure();
    is_measuring = 0;
    lv_timer_del(ui_MKS142DetailPageTimer);
}
