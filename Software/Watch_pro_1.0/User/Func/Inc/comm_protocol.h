/** @file comm_protocol.h
 *  @brief STM32 <-> ESP32-S3 二进制帧通信协议定义
 */
#ifndef __COMM_PROTOCOL_H__
#define __COMM_PROTOCOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

/* ====== 帧格式常量 ====== */
#define COMM_FRAME_HEAD      0xAA
#define COMM_FRAME_OVERHEAD  5    /* HEAD(1) + TYPE(1) + LEN(2) + CHK(1) */
#define COMM_PAYLOAD_MAX     512

/* ====== STM32 -> ESP32 帧类型 ====== */
#define COMM_TYPE_HEALTH     0x01  /* 健康数据 (8B) */
#define COMM_TYPE_PPG        0x02  /* PPG波形 (128B) */
#define COMM_TYPE_ENV        0x03  /* 环境数据 (6B) */
#define COMM_TYPE_MOTION     0x04  /* 运动数据 (3B) */
#define COMM_TYPE_SOS        0x05  /* SOS报警 (1B: abnormal_type bitmask) */
#define COMM_TYPE_TIME       0x06  /* 时间同步 (6B) */
#define COMM_TYPE_RESPONSE   0x10  /* 命令响应 */
#define COMM_TYPE_WAKE       0x86  /* 语音助手唤醒 */

/* ====== ESP32 -> STM32 帧类型 ====== */
#define COMM_TYPE_HEARTBEAT  0x80  /* 心跳 */
#define COMM_TYPE_REQ_HEALTH 0x81  /* 请求健康数据 */
#define COMM_TYPE_PPG_CTRL   0x82  /* PPG控制 (1B: 0=停, 1=启) */
#define COMM_TYPE_SET_TIME   0x83  /* 设置时间 (6B: 年月日时分秒) */
#define COMM_TYPE_QUERY      0x84  /* 查询状态 */
#define COMM_TYPE_VOICE      0x85  /* 语音状态 (1B: 0=关, 1=开) */

/* ====== 发送API ====== */
void Comm_SendFrame(uint8_t type, const uint8_t *payload, uint16_t len);
void Comm_SendHealth(void);
void Comm_SendPPG(void);
void Comm_SendEnv(void);
void Comm_SendMotion(void);
void Comm_SendSOS(uint8_t type);
void Comm_SendTime(void);
void Comm_SendVoiceWake(void);

/* ====== 接收解析 ====== */
void Comm_ParseFrame(const uint8_t *buf, uint16_t len);

/* ====== PPG流控制 ====== */
extern uint8_t comm_ppg_streaming;

#ifdef __cplusplus
}
#endif

#endif
