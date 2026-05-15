/** @file fall_detect.c
 *  @brief MPU6050跌倒检测实现 - 一阶IIR滤波 + 三阶段状态机(自由落体→撞击→姿态确认)
 *         UART1输出debug数据（整数运算，避免printf浮点栈溢出），阈值可运行时调整
 */
#include "fall_detect.h"
#include "mpu6050.h"
#include <stdio.h>

/*
 *  阈值全部用整数表示，内部单位:
 *    加速度: centi-g (1 = 0.01g)，即 raw/4096*100
 *    角度:   deci-degree (1 = 0.1度)
 *  这样避免浮点运算和printf %f 的巨大栈开销
 */

/* 加速度量程: DMP默认±2g → 16384 LSB/g, 乘100得centi-g */
#define ACCEL_TO_CG_NUM   100
#define ACCEL_TO_CG_DEN   16384

/* 默认阈值 (整数单位) */
#define DEFAULT_FREEFALL_CG       60      /* 0.70g = 70 centi-g */
#define DEFAULT_IMPACT_CG         120     /* 1.50g = 150 centi-g */
#define DEFAULT_TILT_DECIDEG      450     /* 45.0度 = 450 deci-degree */
#define DEFAULT_FALL_WINDOW_MS    2000
#define DEFAULT_CONFIRM_DELAY_MS  500
#define DEFAULT_FILTER_ALPHA_Q8   179     /* 0.7 * 256 ≈ 179, Q8定点 */

/* 私有状态 (全部整数) */
static FallDetect_ConfigTypeDef config;
static FallState_t state = FALL_STATE_NORMAL;
static int32_t  filtered_acc_cg = 100;    /* 滤波后加速度模 (centi-g), 初始1.00g */
static uint32_t state_timestamp = 0;
static uint8_t  debug_enabled = 0;

/* 内部整数配置缓存 */
static int32_t  thr_freefall_cg;
static int32_t  thr_impact_cg;
static int32_t  thr_tilt_decideg;

/**
 * @brief  初始化跌倒检测
 */
void FallDetect_Init(FallDetect_ConfigTypeDef *cfg)
{
    if(cfg != NULL)
    {
        config = *cfg;
    }
    else
    {
        config.freefall_threshold = 0.7f;
        config.impact_threshold   = 1.5f;
        config.tilt_threshold     = 45.0f;
        config.fall_time_window   = DEFAULT_FALL_WINDOW_MS;
        config.confirm_delay      = DEFAULT_CONFIRM_DELAY_MS;
        config.filter_alpha       = 0.7f;
    }

    /* 预计算整数阈值 */
    thr_freefall_cg   = (int32_t)(config.freefall_threshold * 100.0f);
    thr_impact_cg     = (int32_t)(config.impact_threshold * 100.0f);
    thr_tilt_decideg  = (int32_t)(config.tilt_threshold * 10.0f);

    state = FALL_STATE_NORMAL;
    filtered_acc_cg = 100;
    state_timestamp = 0;
    debug_enabled = 0;
}

/**
 * @brief  更新配置
 */
void FallDetect_SetConfig(FallDetect_ConfigTypeDef *cfg)
{
    if(cfg != NULL)
    {
        config = *cfg;
        thr_freefall_cg  = (int32_t)(config.freefall_threshold * 100.0f);
        thr_impact_cg    = (int32_t)(config.impact_threshold * 100.0f);
        thr_tilt_decideg = (int32_t)(config.tilt_threshold * 10.0f);
    }
}

FallState_t FallDetect_GetState(void)
{
    return state;
}

void FallDetect_Reset(void)
{
    state = FALL_STATE_NORMAL;
    filtered_acc_cg = 100;
}

void FallDetect_EnableDebug(uint8_t en)
{
    debug_enabled = en;
}

/**
 * @brief  快速整数平方根 (牛顿迭代，仅需几轮)
 */
static int32_t isqrt(int32_t x)
{
    if(x <= 0) return 0;
    int32_t r = x;
    int32_t nr = (r + x / r) >> 1;
    while(nr < r)
    {
        r = nr;
        nr = (r + x / r) >> 1;
    }
    return r;
}

/**
 * @brief  整数绝对值
 */
static int32_t iabs(int32_t x)
{
    return x < 0 ? -x : x;
}

/**
 * @brief  跌倒检测主处理函数 (全整数运算)
 */
void FallDetect_Process(void)
{
    short raw_ax, raw_ay, raw_az;
    short raw_gx, raw_gy, raw_gz;
    float roll_f, pitch_f;
    int32_t acc_raw_cg;
    int32_t roll_decideg, pitch_decideg;
    uint32_t now;

    /* 读取原始数据 */
    if(MPU_Get_Accelerometer(&raw_ax, &raw_ay, &raw_az) != 0)
        return;
    MPU_Get_Gyroscope(&raw_gx, &raw_gy, &raw_gz);
    MPU_Get_Angles(&roll_f, &pitch_f);

    /* 转换为整数: 弧度→角度→deci-degree (rad * 180/PI * 10) */
    roll_decideg  = (int32_t)(roll_f * 572.96f);
    pitch_decideg = (int32_t)(pitch_f * 572.96f);

    /* 计算加速度模 (centi-g), 用整数平方根避免sqrtf */
    {
        int32_t ax = (int32_t)raw_ax;
        int32_t ay = (int32_t)raw_ay;
        int32_t az = (int32_t)raw_az;
        int32_t sum_sq = ax * ax + ay * ay + az * az;
        /* sqrt(sum_sq) 单位是 LSB, 转换为 centi-g:
         *   acc_cg = sqrt(sum_sq) / 4096 * 100
         * 为了避免溢出，先乘100再除4096 */
        acc_raw_cg = isqrt(sum_sq) * ACCEL_TO_CG_NUM / ACCEL_TO_CG_DEN;
    }

    /* 一阶IIR低通滤波 (Q8定点) */
    filtered_acc_cg = filtered_acc_cg +
        (int32_t)((int32_t)(acc_raw_cg - filtered_acc_cg) * DEFAULT_FILTER_ALPHA_Q8 / 256);

    now = HAL_GetTick();

    /* ====== 状态机 (全整数比较) ====== */
    switch(state)
    {
    case FALL_STATE_NORMAL:
        if(filtered_acc_cg < thr_freefall_cg)
        {
            state = FALL_STATE_FREEFALL;
            state_timestamp = now;
        }
        break;

    case FALL_STATE_FREEFALL:
        if((now - state_timestamp) > config.fall_time_window)
        {
            state = FALL_STATE_NORMAL;
            break;
        }
        if(filtered_acc_cg > thr_impact_cg)
        {
            state = FALL_STATE_IMPACT;
            state_timestamp = now;
        }
        break;

    case FALL_STATE_IMPACT:
        if((now - state_timestamp) > config.confirm_delay)
        {
            if(iabs(pitch_decideg) > thr_tilt_decideg ||
               iabs(roll_decideg)  > thr_tilt_decideg)
            {
                state = FALL_STATE_CONFIRMED;
            }
            else
            {
                state = FALL_STATE_NORMAL;
            }
        }
        break;

    case FALL_STATE_CONFIRMED:
        break;
    }

    /* ====== UART1 Debug (全整数printf，无%f) ====== */
    if(debug_enabled)
    {
        /* 输出格式: DT:acc=195 roll=-2 pitch=-15 state=NORMAL
         * acc单位centi-g (195 = 1.95g)
         * roll/pitch单位deci-degree (-2 = -0.2度) */
        const char *s0 = "NORMAL";
        const char *s1 = "FREEFALL";
        const char *s2 = "IMPACT";
        const char *s3 = "CONFIRMED";
        const char *st = s0;
        if(state == FALL_STATE_FREEFALL) st = s1;
        else if(state == FALL_STATE_IMPACT) st = s2;
        else if(state == FALL_STATE_CONFIRMED) st = s3;

        printf("DT:acc=%ld roll=%ld pitch=%ld state=%s\r\n",
               (long)filtered_acc_cg, (long)roll_decideg, (long)pitch_decideg, st);
    }
}
