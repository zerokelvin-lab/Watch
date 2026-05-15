/** @file comm_protocol.c
 *  @brief STM32 <-> ESP32-S3 二进制帧通信协议实现
 */
#include "comm_protocol.h"
#include "stm32f4xx_it.h"
#include "usart.h"
#include "rtc.h"
#include "HWDataAccess.h"
#include "ui_MKS142Page.h"
#include "ui_MKS142DetailPage.h"
#include "mks_142.h"
#include <string.h>
#include <stdio.h>

/* ====== 发送缓冲区 ====== */
static uint8_t comm_tx_buf[COMM_FRAME_OVERHEAD + COMM_PAYLOAD_MAX];

/* ====== PPG流控制 ====== */
uint8_t comm_ppg_streaming = 0;

/**
 * @brief  发送一帧数据 (阻塞模式, 避免与printf的DMA冲突)
 */
void Comm_SendFrame(uint8_t type, const uint8_t *payload, uint16_t len)
{
    if(len > COMM_PAYLOAD_MAX) return;

    comm_tx_buf[0] = COMM_FRAME_HEAD;
    comm_tx_buf[1] = type;
    comm_tx_buf[2] = (uint8_t)(len & 0xFF);
    comm_tx_buf[3] = (uint8_t)((len >> 8) & 0xFF);

    uint8_t chk = type ^ comm_tx_buf[2] ^ comm_tx_buf[3];
    for(uint16_t i = 0; i < len; i++)
    {
        comm_tx_buf[4 + i] = payload[i];
        chk ^= payload[i];
    }
    comm_tx_buf[4 + len] = chk;

    HAL_UART_Transmit(&huart1, comm_tx_buf, COMM_FRAME_OVERHEAD + len, 200);
}

/**
 * @brief  发送健康数据帧 (8字节)
 */
void Comm_SendHealth(void)
{
    uint8_t p[8];
    p[0] = MKS142_saved_hr;
    p[1] = MKS142_saved_spo2;
    p[2] = MKS142_saved_sbp;
    p[3] = MKS142_saved_dbp;
    p[4] = MKS142_saved_fatigue;
    p[5] = MKS142_saved_micro;
    p[6] = MKS142_saved_resp;
    p[7] = MKS142_saved_rmssd;
    Comm_SendFrame(COMM_TYPE_HEALTH, p, 8);
}

/**
 * @brief  发送PPG波形帧 (128字节: 64个int16_t小端)
 */
void Comm_SendPPG(void)
{
    uint8_t p[128];
    for(int i = 0; i < 64; i++)
    {
        int16_t val = (int16_t)mks142_data.ppg_waveform[i];
        p[i * 2]     = (uint8_t)(val & 0xFF);
        p[i * 2 + 1] = (uint8_t)((val >> 8) & 0xFF);
    }
    Comm_SendFrame(COMM_TYPE_PPG, p, 128);
}

/**
 * @brief  发送环境数据帧 (6字节)
 */
void Comm_SendEnv(void)
{
    uint8_t p[6];
    p[0] = HWInterface.AHT21.temperature;
    p[1] = HWInterface.AHT21.humidity;
    p[2] = (uint8_t)(HWInterface.Barometer.altitude >> 8);
    p[3] = (uint8_t)(HWInterface.Barometer.altitude & 0xFF);
    p[4] = (uint8_t)(HWInterface.Ecompass.direction >> 8);
    p[5] = (uint8_t)(HWInterface.Ecompass.direction & 0xFF);
    Comm_SendFrame(COMM_TYPE_ENV, p, 6);
}

/**
 * @brief  发送运动数据帧 (3字节)
 */
void Comm_SendMotion(void)
{
    uint8_t p[3];
    p[0] = (uint8_t)(HWInterface.IMU.Steps >> 8);
    p[1] = (uint8_t)(HWInterface.IMU.Steps & 0xFF);
    p[2] = HWInterface.Power.power_remain;
    Comm_SendFrame(COMM_TYPE_MOTION, p, 3);
}

/**
 * @brief  发送SOS报警帧 (1字节: abnormal_type bitmask)
 */
void Comm_SendSOS(uint8_t type)
{
    Comm_SendFrame(COMM_TYPE_SOS, &type, 1);
}

/**
 * @brief  发送时间同步帧 (6字节)
 */
void Comm_SendTime(void)
{
    RTC_TimeTypeDef t;
    RTC_DateTypeDef d;
    HAL_RTC_GetTime(&hrtc, &t, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &d, RTC_FORMAT_BIN);

    uint8_t p[6];
    p[0] = d.Year;
    p[1] = d.Month;
    p[2] = d.Date;
    p[3] = t.Hours;
    p[4] = t.Minutes;
    p[5] = t.Seconds;
    Comm_SendFrame(COMM_TYPE_TIME, p, 6);
}

/**
 * @brief  发送语音唤醒帧 (无payload)
 */
void Comm_SendVoiceWake(void)
{
    Comm_SendFrame(COMM_TYPE_WAKE, NULL, 0);
}

/**
 * @brief  解析ESP32发来的帧并分发处理
 */
void Comm_ParseFrame(const uint8_t *buf, uint16_t len)
{
    if(len < COMM_FRAME_OVERHEAD) return;
    if(buf[0] != COMM_FRAME_HEAD) return;

    uint8_t type = buf[1];
    uint16_t plen = (uint16_t)buf[2] | ((uint16_t)buf[3] << 8);
    if(len < COMM_FRAME_OVERHEAD + plen) return;

    const uint8_t *payload = &buf[4];

    /* 校验 */
    uint8_t chk = type ^ buf[2] ^ buf[3];
    for(uint16_t i = 0; i < plen; i++) chk ^= payload[i];
    if(chk != buf[4 + plen]) return;

    switch(type)
    {
    case COMM_TYPE_HEARTBEAT:
        /* 心跳: 回复运动+环境数据 */
        Comm_SendMotion();
        Comm_SendEnv();
        break;

    case COMM_TYPE_REQ_HEALTH:
        /* 请求健康数据 */
        Comm_SendHealth();
        break;

    case COMM_TYPE_PPG_CTRL:
        /* PPG开关 */
        if(plen >= 1)
            comm_ppg_streaming = payload[0] ? 1 : 0;
        break;

    case COMM_TYPE_SET_TIME:
        /* 校时: payload[0]=年,[1]=月,[2]=日,[3]=时,[4]=分,[5]=秒 */
        if(plen >= 6)
        {
            if(payload[0] < 100 && payload[1] >= 1 && payload[1] <= 12
               && payload[2] >= 1 && payload[2] <= 31
               && payload[3] <= 23 && payload[4] <= 59 && payload[5] <= 59)
            {
                RTC_SetDate(payload[0], payload[1], payload[2]);
                RTC_SetTime(payload[3], payload[4], payload[5]);
            }
        }
        break;

    case COMM_TYPE_QUERY:
        /* 查询状态: 返回运动+环境+时间 */
        Comm_SendMotion();
        Comm_SendEnv();
        Comm_SendTime();
        break;

    case COMM_TYPE_VOICE:
        /* 语音状态通知 (预留) */
        break;

    default:
        /* 未知类型: 按LEN跳过, 不影响解析 */
        break;
    }
}
