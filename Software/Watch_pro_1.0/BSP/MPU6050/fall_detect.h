/** @file fall_detect.h
 *  @brief MPU6050跌倒检测模块 - 基于加速度+陀螺仪的三阶段跌倒判定
 */
#ifndef __FALL_DETECT_H
#define __FALL_DETECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* 跌倒检测状态 */
typedef enum {
    FALL_STATE_NORMAL = 0,    /* 正常状态 */
    FALL_STATE_FREEFALL,      /* 自由落体中 */
    FALL_STATE_IMPACT,        /* 检测到撞击 */
    FALL_STATE_CONFIRMED      /* 跌倒确认 */
} FallState_t;

/* 阈值配置结构体（所有参数可运行时调整） */
typedef struct {
    float freefall_threshold;   /* 自由落体加速度阈值 (g)，低于此值判定为失重，默认0.5 */
    float impact_threshold;     /* 撞击加速度阈值 (g)，高于此值判定为撞击，默认2.5 */
    float tilt_threshold;       /* 倾斜角阈值 (度)，撞击后角度偏离超过此值确认跌倒，默认45.0 */
    uint16_t fall_time_window;  /* 自由落体→撞击时间窗口 (ms)，超时则复位，默认2000 */
    uint16_t confirm_delay;     /* 撞击→确认延迟 (ms)，用于姿态稳定，默认500 */
    float filter_alpha;         /* 一阶IIR滤波系数 (0.0~1.0)，越大响应越快，默认0.3 */
} FallDetect_ConfigTypeDef;

/**
 * @brief  初始化跌倒检测模块
 * @param  cfg 阈值配置指针，传NULL使用默认值
 */
void FallDetect_Init(FallDetect_ConfigTypeDef *cfg);

/**
 * @brief  跌倒检测主处理函数 - 在任务主循环中调用
 *         包含传感器读取、滤波、状态机、debug输出
 */
void FallDetect_Process(void);

/**
 * @brief  获取当前跌倒检测状态
 * @retval FallState_t 状态枚举
 */
FallState_t FallDetect_GetState(void);

/**
 * @brief  更新跌倒检测阈值配置
 * @param  cfg 新配置指针
 */
void FallDetect_SetConfig(FallDetect_ConfigTypeDef *cfg);

/**
 * @brief  手动复位跌倒检测状态到NORMAL
 */
void FallDetect_Reset(void);

/**
 * @brief  开关UART1 debug输出
 * @param  en 1=开启, 0=关闭
 */
void FallDetect_EnableDebug(uint8_t en);

#ifdef __cplusplus
}
#endif

#endif
