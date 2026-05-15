/** @file user_MKS142AutoTask.c
 *  @brief MKS142后台自动测量任务 - 非健康页面时每10分钟整点自动测量，稳定后更新保存数据
 */
#include "user_TasksInit.h"
#include "user_MKS142AutoTask.h"
#include "mks_142.h"
#include "rtc.h"
#include "PageManager.h"
#include "ui_MKS142Page.h"
#include "ui_MKS142DetailPage.h"
#include "user_SosTask.h"
#include "comm_protocol.h"
#include <stdlib.h>

/* CommTask健康数据就绪标志 */
extern volatile uint8_t comm_health_data_ready;

#define AUTO_CHECK_TICKS     1000    /* 测量中每1秒检查一次 */
#define AUTO_STABLE_SECONDS  5       /* 连续稳定秒数 */
#define AUTO_HR_THRESHOLD    3       /* 心率波动阈值 */
#define AUTO_SPO2_THRESHOLD  2       /* 血氧波动阈值 */
#define AUTO_PARAM_THRESHOLD 5       /* 其他参数波动阈值 */

/**
 * @brief  判断当前是否在健康页面（心率或疲劳详情）
 * @retval 1=在健康页面, 0=不在
 */
static int is_on_health_page(void)
{
    lv_obj_t *cur = *Page_Get_NowPage()->page_obj;
    return (cur == ui_MKS142Page || cur == ui_MKS142DetailPage);
}

/**
 * @brief  计算距离下一个10分钟整点的毫秒数
 *         例如：当前12:07:30 → 等到12:10:00 → 返回150000ms
 * @retval 等待毫秒数
 */
static uint32_t calc_delay_to_next_10min(void)
{
    RTC_TimeTypeDef now;
    HAL_RTC_GetTime(&hrtc, &now, RTC_FORMAT_BIN);

    uint8_t min = now.Minutes;
    uint8_t sec = now.Seconds;

    /* 下一个10分钟整点 */
    uint8_t next_min = (min / 10 + 1) * 10;
    if(next_min >= 60)
        next_min = 0;  /* 整点溢出到下一个小时的:00 */

    /* 计算需要等待的秒数 */
    uint32_t wait_sec;
    if(next_min > min)
        wait_sec = (uint32_t)(next_min - min - 1) * 60 + (60 - sec);
    else
        /* next_min < min，说明跨小时，如 59分→0分 */
        wait_sec = (uint32_t)(60 - min - 1) * 60 + (60 - sec) + (uint32_t)next_min * 60;

    /* 至少等1秒，避免0值 */
    if(wait_sec < 1) wait_sec = 1;

    return wait_sec * 1000;
}

/**
 * @brief  MKS142后台自动测量任务
 *
 *  状态机:
 *    IDLE   - 等待到下一个10分钟整点（:00,:10,:20,:30,:40,:50），期间在健康页面则重新计时
 *    MEASURE - 启动测量，每秒检查数据稳定性
 *             如果用户进入健康页面则中止（页面接管）
 *             连续5秒稳定数据后保存并停止
 */
void MKS142AutoMeasureTask(void *argument)
{
    uint8_t prev_hr = 0, prev_spo2 = 0;
    uint8_t prev_fatigue = 0, prev_micro = 0, prev_resp = 0;
    uint8_t prev_sbp = 0, prev_dbp = 0, prev_rmssd = 0;
    uint8_t stable_count = 0;

    (void)argument;

    while(1)
    {
        /* ====== IDLE: 等待到下一个10分钟整点 ====== */
        while(1)
        {
            uint32_t delay_ms = calc_delay_to_next_10min();

            /* 分段等待，每5秒检查一次是否在健康页面 */
            uint32_t waited = 0;
            while(waited < delay_ms)
            {
                uint32_t chunk = delay_ms - waited;
                if(chunk > 5000) chunk = 5000;
                osDelay(chunk);
                waited += chunk;

                /* 如果在健康页面，重新计算等待时间 */
                if(is_on_health_page())
                {
                    delay_ms = calc_delay_to_next_10min();
                    waited = 0;
                }
            }

            /* 等待结束，确认不在健康页面 */
            if(!is_on_health_page())
                break;
        }

        /* ====== MEASURE: 开始测量 ====== */
        MKS142_StartMeasure();
        stable_count = 0;
        prev_hr = 0; prev_spo2 = 0;
        prev_fatigue = 0; prev_micro = 0; prev_resp = 0;
        prev_sbp = 0; prev_dbp = 0; prev_rmssd = 0;

        while(1)
        {
            osDelay(AUTO_CHECK_TICKS);

            /* 用户进入健康页面 → 中止后台测量，页面接管 */
            if(is_on_health_page())
            {
                MKS142_StopMeasure();
                break;
            }

            MKS142_CheckAlive();

            if(MKS142_IsDataValid())
            {
                uint8_t cur_hr = MKS142_GetHeartRate();
                uint8_t cur_spo2 = MKS142_GetSpO2();
                uint8_t cur_fatigue = MKS142_GetFatigue();
                uint8_t cur_micro   = MKS142_GetMicrocirculation();
                uint8_t cur_resp    = MKS142_GetRespRate();
                uint8_t cur_sbp     = MKS142_GetSBP();
                uint8_t cur_dbp     = MKS142_GetDBP();
                uint8_t cur_rmssd   = MKS142_GetRMSSD();

                /* 稳定性判断：所有参数非0且波动在阈值内 */
                uint8_t stable =
                    (cur_hr > 0) && (cur_spo2 > 0) &&
                    (cur_fatigue > 0) && (cur_micro > 0) &&
                    (cur_resp > 0) && (cur_sbp > 0) &&
                    (cur_dbp > 0) && (cur_rmssd > 0) &&
                    (abs((int)cur_hr - (int)prev_hr) <= AUTO_HR_THRESHOLD) &&
                    (abs((int)cur_spo2 - (int)prev_spo2) <= AUTO_SPO2_THRESHOLD) &&
                    (abs((int)cur_fatigue - (int)prev_fatigue) <= AUTO_PARAM_THRESHOLD) &&
                    (abs((int)cur_micro - (int)prev_micro) <= AUTO_PARAM_THRESHOLD) &&
                    (abs((int)cur_resp - (int)prev_resp) <= AUTO_PARAM_THRESHOLD) &&
                    (abs((int)cur_sbp - (int)prev_sbp) <= AUTO_PARAM_THRESHOLD) &&
                    (abs((int)cur_dbp - (int)prev_dbp) <= AUTO_PARAM_THRESHOLD) &&
                    (abs((int)cur_rmssd - (int)prev_rmssd) <= AUTO_PARAM_THRESHOLD);

                prev_hr = cur_hr; prev_spo2 = cur_spo2;
                prev_fatigue = cur_fatigue; prev_micro = cur_micro;
                prev_resp = cur_resp; prev_sbp = cur_sbp;
                prev_dbp = cur_dbp; prev_rmssd = cur_rmssd;

                if(stable)
                    stable_count++;
                else
                    stable_count = 0;

                /* 保存当前值（页面显示和BLE会读取） */
                MKS142_saved_hr = cur_hr;
                MKS142_saved_spo2 = cur_spo2;
                MKS142_saved_fatigue = cur_fatigue;
                MKS142_saved_micro = cur_micro;
                MKS142_saved_resp = cur_resp;
                MKS142_saved_sbp = cur_sbp;
                MKS142_saved_dbp = cur_dbp;
                MKS142_saved_rmssd = cur_rmssd;

                /* 连续稳定达到阈值 → 停止测量 */
                if(stable_count >= AUTO_STABLE_SECONDS)
                {
                    /* 检查健康指标异常 → 触发SOS */
                    uint8_t sos_type = 0;
                    if(cur_hr < 60 || cur_hr > 100)       sos_type |= SOS_TYPE_HR;
                    if(cur_spo2 < 95)                      sos_type |= SOS_TYPE_SPO2;
                    if(cur_micro < 70)                     sos_type |= SOS_TYPE_MICRO;
                    if(cur_sbp < 90 || cur_sbp > 140 ||
                       cur_dbp < 60 || cur_dbp > 90)       sos_type |= SOS_TYPE_BP;
                    if(cur_rmssd < 22 || cur_rmssd > 120)  sos_type |= SOS_TYPE_HRV;
                    if(cur_fatigue < 15)                   sos_type |= SOS_TYPE_FATIGUE;
                    if(sos_type) SOS_Trigger(sos_type);

                    /* 通知CommTask发送健康数据给ESP32 */
                    comm_health_data_ready = 1;

                    MKS142_StopMeasure();
                    break;
                }
            }
            else
            {
                /* 数据无效，重置稳定计数 */
                stable_count = 0;
            }
        }
    }
}
