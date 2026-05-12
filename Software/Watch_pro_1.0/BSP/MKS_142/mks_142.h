/** @file mks_142.h @brief MKS_142健康监测模块驱动头文件 - 帧解析、数据结构、API声明 */
#ifndef _MKS_142_H_
#define _MKS_142_H_

#include "stm32f4xx_hal.h"

/* ====== MKS_142协议常量 ====== */
#define MKS142_FRAME_LEN  88    /** @brief 完整帧长度（字节） */
#define MKS142_PPG_LEN    64    /** @brief PPG波形数据长度 */

/* ====== 帧数据结构 ====== */
/**
 * @brief MKS_142健康数据结构体
 *        存储从88字节帧中解析出的所有健康指标
 *        数据帧格式: [0]=0xFF帧头, [1..64]=PPG, [65]=心率, ...
 */
typedef struct {
    /* 主要健康指标 */
    uint8_t heart_rate;         /** @brief [65] 心率 (bpm, 40-200) */
    uint8_t spo2;               /** @brief [66] 血氧饱和度 (%, 70-99) */
    uint8_t fatigue;            /** @brief [68] 疲劳指数 (Dc) */
    uint8_t microcirculation;   /** @brief [67] 微循环指数 (Bk) */

    /* 血压数据 */
    uint8_t sbp;                /** @brief [71] 收缩压 (mmHg) */
    uint8_t dbp;                /** @brief [72] 舒张压 (mmHg) */
    uint8_t mean_bp;            /** @brief 平均动脉压 (计算值) */

    /* 呼吸数据 */
    uint8_t resp_rate;          /** @brief [73] 呼吸频率 (次/分) */
    uint8_t resp_depth;         /** @brief [74] 呼吸幅度 */

    /* HRV心率变异性数据 */
    uint8_t rr_interval;        /** @brief [75] RR间期 */
    uint8_t rmssd;              /** @brief [77] RMSSD (ms) */
    uint8_t nn50;               /** @brief [78] NN50 */
    uint8_t pnn50;              /** @brief [79] PNN50 */

    /* PPG波形 */
    int8_t  ppg_waveform[MKS142_PPG_LEN]; /** @brief [1..64] 有符号PPG波形数据 */

    /* 状态 */
    uint8_t data_valid;         /** @brief 数据有效标志 (1=已收到有效帧, 0=无数据) */
    uint8_t body_temp;          /** @brief [69] 体温 (预留) */
} MKS142_DataTypeDef;

/* ====== 全局变量声明 ====== */
extern MKS142_DataTypeDef mks142_data;          /** @brief MKS_142健康数据实例 */
extern uint8_t mks142_frame_buf[MKS142_FRAME_LEN]; /** @brief 帧接收缓冲区 */

/* ====== API函数声明 ====== */
void MKS142_Init(void);                         /** @brief 初始化MKS_142模块 */
void MKS142_StartMeasure(void);                 /** @brief 发送0x8A启动连续测量 */
void MKS142_StopMeasure(void);                  /** @brief 发送0x88停止测量 */
void MKS142_CheckAlive(void);                   /** @brief 数据存活检测（帧计数器超时判断） */
void MKS142_ParseFrame(uint8_t *buf);           /** @brief 解析88字节帧数据 */

/* 数据获取接口 */
uint8_t MKS142_GetHeartRate(void);              /** @brief 获取心率 */
uint8_t MKS142_GetSpO2(void);                   /** @brief 获取血氧 */
uint8_t MKS142_GetFatigue(void);                /** @brief 获取疲劳指数 */
uint8_t MKS142_GetMicrocirculation(void);       /** @brief 获取微循环指数 */
uint8_t MKS142_GetRespRate(void);               /** @brief 获取呼吸频率 */
uint8_t MKS142_GetRespDepth(void);              /** @brief 获取呼吸幅度 */
uint8_t MKS142_GetRMSSD(void);                  /** @brief 获取RMSSD */
uint8_t MKS142_GetNN50(void);                   /** @brief 获取NN50 */
uint8_t MKS142_GetPNN50(void);                  /** @brief 获取PNN50 */
uint8_t MKS142_GetSBP(void);                    /** @brief 获取收缩压 */
uint8_t MKS142_GetDBP(void);                    /** @brief 获取舒张压 */
uint8_t MKS142_GetMeanBP(void);                 /** @brief 获取平均动脉压 */
uint8_t MKS142_GetRRInterval(void);             /** @brief 获取RR间期 */
uint8_t MKS142_GetBodyTemp(void);               /** @brief 获取体温 */
uint8_t MKS142_IsDataValid(void);               /** @brief 查询数据是否有效 */

#endif
