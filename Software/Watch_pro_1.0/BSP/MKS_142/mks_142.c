/** @file mks_142.c @brief MKS_142健康监测模块驱动 - UART帧解析、健康数据提取、血压平均计算 */
#include "mks_142.h"
#include <string.h>
#include <stdlib.h>

/* ====== 全局变量定义 ====== */
MKS142_DataTypeDef mks142_data;
uint8_t mks142_frame_buf[MKS142_FRAME_LEN];

/* ====== 血压平均值计算（中位值滤波） ====== */
#define BP_FILTER_SIZE 3
#define BP_AVG_COUNT   4

typedef struct {
    uint8_t sys_buf[BP_FILTER_SIZE];
    uint8_t dias_buf[BP_FILTER_SIZE];
    uint8_t sys_idx;
    uint8_t dias_idx;
    uint8_t sys_cnt;
    uint8_t dias_cnt;
    uint16_t sys_sum;
    uint16_t dias_sum;
    uint8_t sys_result;
    uint8_t dias_result;
    uint8_t mean_bp;
} BP_FilterTypeDef;

static BP_FilterTypeDef bp_filter;

static int bp_compare(const void *a, const void *b)
{
    return (*(int*)a - *(int*)b);
}

static void blood_pressure_update(uint8_t sbp, uint8_t dbp)
{
    /* 收缩压滤波 (有效范围100-140) */
    if(sbp >= 100 && sbp <= 140)
    {
        bp_filter.sys_buf[bp_filter.sys_idx] = sbp;
        bp_filter.sys_idx++;
        if(bp_filter.sys_idx >= BP_FILTER_SIZE)
        {
            qsort(bp_filter.sys_buf, BP_FILTER_SIZE, sizeof(int), bp_compare);
            bp_filter.sys_idx = 0;
            bp_filter.sys_cnt++;
            bp_filter.sys_sum += bp_filter.sys_buf[1]; /* 取中位值 */
        }
        if(bp_filter.sys_cnt >= BP_AVG_COUNT)
        {
            bp_filter.sys_result = bp_filter.sys_sum >> 2;
            bp_filter.sys_sum = 0;
            bp_filter.sys_cnt = 0;
        }
    }

    /* 舒张压滤波 (有效范围70-100) */
    if(dbp >= 70 && dbp <= 100)
    {
        bp_filter.dias_buf[bp_filter.dias_idx] = dbp;
        bp_filter.dias_idx++;
        if(bp_filter.dias_idx >= BP_FILTER_SIZE)
        {
            qsort(bp_filter.dias_buf, BP_FILTER_SIZE, sizeof(int), bp_compare);
            bp_filter.dias_idx = 0;
            bp_filter.dias_cnt++;
            bp_filter.dias_sum += bp_filter.dias_buf[1]; /* 取中位值 */
        }
        if(bp_filter.dias_cnt >= BP_AVG_COUNT)
        {
            bp_filter.dias_result = bp_filter.dias_sum >> 2;
            bp_filter.dias_sum = 0;
            bp_filter.dias_cnt = 0;
        }
    }

    /* 平均动脉压 = (舒张压*2 + 收缩压) / 3 */
    bp_filter.mean_bp = ((bp_filter.dias_result * 2) + bp_filter.sys_result) / 3;

    /* 心率和血氧为0时清除平均血压 */
    if(mks142_data.heart_rate == 0 && mks142_data.spo2 == 0)
    {
        bp_filter.mean_bp = 0;
    }

    mks142_data.mean_bp = bp_filter.mean_bp;
}

/* ====== 初始化 ====== */

void MKS142_Init(void)
{
    memset(&mks142_data, 0, sizeof(MKS142_DataTypeDef));
    memset(&bp_filter, 0, sizeof(BP_FilterTypeDef));
    memset(mks142_frame_buf, 0, MKS142_FRAME_LEN);
}

/* ====== 启动/停止测量 ====== */

void MKS142_StartMeasure(void)
{
    extern UART_HandleTypeDef huart2;
    uint8_t cmd = 0x8A;
    HAL_UART_Transmit(&huart2, &cmd, 1, 100);
}

void MKS142_StopMeasure(void)
{
    extern UART_HandleTypeDef huart2;
    uint8_t cmd = 0x88;
    HAL_UART_Transmit(&huart2, &cmd, 1, 100);
}

/* ====== 数据存活检测（参考Usart_Handle） ====== */

static uint8_t last_frame_counter = 0;
static uint16_t timeout_cnt = 0;

void MKS142_CheckAlive(void)
{
    if(mks142_frame_buf[1] == last_frame_counter)
    {
        timeout_cnt++;
        if(timeout_cnt >= 200)
        {
            mks142_data.data_valid = 0;
            timeout_cnt = 200;
        }
    }
    else
    {
        timeout_cnt = 0;
        mks142_data.data_valid = 1;
    }
    last_frame_counter = mks142_frame_buf[1];
}

/* ====== 帧解析 ====== */

/**
 * @brief  解析MKS_142模块的88字节数据帧
 * @param  buf 88字节帧缓冲区指针（buf[0]=0xFF帧头）
 * @note   帧格式参考协议文档：
 *         [1..64]=PPG波形, [65]=心率, [66]=血氧, [67]=微循环,
 *         [68]=疲劳, [71]=收缩压, [72]=舒张压, [73]=呼吸率,
 *         [74]=呼吸幅度, [75]=RR间期, [77]=RMSSD, [78]=NN50, [79]=PNN50
 */
void MKS142_ParseFrame(uint8_t *buf)
{
    /* PPG波形数据 [1..64] */
    for(uint8_t i = 0; i < MKS142_PPG_LEN; i++)
    {
        mks142_data.ppg_waveform[i] = (int8_t)buf[i + 1];
    }

    /* 主要健康指标 */
    mks142_data.heart_rate       = buf[65];
    mks142_data.spo2             = buf[66];
    mks142_data.microcirculation = buf[67];
    mks142_data.fatigue          = buf[68];
    mks142_data.body_temp        = buf[69];

    /* 血压 */
    mks142_data.sbp              = buf[71];
    mks142_data.dbp              = buf[72];

    /* 呼吸 */
    mks142_data.resp_rate        = buf[73];
    mks142_data.resp_depth       = buf[74];

    /* HRV */
    mks142_data.rr_interval      = buf[75];
    mks142_data.rmssd            = buf[77];
    mks142_data.nn50             = buf[78];
    mks142_data.pnn50            = buf[79];

    /* 计算平均动脉压 */
    blood_pressure_update(mks142_data.sbp, mks142_data.dbp);

    /* 标记数据有效 */
    mks142_data.data_valid = 1;
}

/* ====== 数据获取接口 ====== */

uint8_t MKS142_GetHeartRate(void)        { return mks142_data.heart_rate; }
uint8_t MKS142_GetSpO2(void)             { return mks142_data.spo2; }
uint8_t MKS142_GetFatigue(void)          { return mks142_data.fatigue; }
uint8_t MKS142_GetMicrocirculation(void) { return mks142_data.microcirculation; }
uint8_t MKS142_GetRespRate(void)         { return mks142_data.resp_rate; }
uint8_t MKS142_GetRespDepth(void)        { return mks142_data.resp_depth; }
uint8_t MKS142_GetRMSSD(void)            { return mks142_data.rmssd; }
uint8_t MKS142_GetNN50(void)             { return mks142_data.nn50; }
uint8_t MKS142_GetPNN50(void)            { return mks142_data.pnn50; }
uint8_t MKS142_GetSBP(void)              { return mks142_data.sbp; }
uint8_t MKS142_GetDBP(void)              { return mks142_data.dbp; }
uint8_t MKS142_GetMeanBP(void)           { return mks142_data.mean_bp; }
uint8_t MKS142_GetRRInterval(void)       { return mks142_data.rr_interval; }
uint8_t MKS142_GetBodyTemp(void)         { return mks142_data.body_temp; }
uint8_t MKS142_IsDataValid(void)         { return mks142_data.data_valid; }
