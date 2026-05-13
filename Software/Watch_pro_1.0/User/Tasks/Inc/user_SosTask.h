/** @file user_SosTask.h
 *  @brief SOS紧急告警任务 - 异常类型宏定义、任务函数和外部接口声明
 */
#ifndef __USER_SOSTASK_H
#define __USER_SOSTASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ====== 异常类型编码 (bitmask) ====== */
#define SOS_TYPE_FALL    (1 << 0)  /* 跌倒 */
#define SOS_TYPE_HR      (1 << 1)  /* 心率异常 (<60 或 >100) */
#define SOS_TYPE_SPO2    (1 << 2)  /* 血氧异常 (<95) */
#define SOS_TYPE_MICRO   (1 << 3)  /* 微循环异常 (<70) */
#define SOS_TYPE_BP      (1 << 4)  /* 血压异常 */
#define SOS_TYPE_HRV     (1 << 5)  /* HRV异常 (<22 或 >120) */
#define SOS_TYPE_FATIGUE (1 << 6)  /* 疲劳异常 (<15) */

/**
 * @brief  SOS任务主函数
 */
void SOSTask(void *argument);

/**
 * @brief  触发SOS - 发送异常类型到SOS消息队列
 * @param  type 异常类型编码 (SOS_TYPE_* bitmask)
 */
void SOS_Trigger(uint8_t type);

/**
 * @brief  SOS上报接口 (预留，当前仅printf)
 * @param  type 异常类型编码
 */
void SOS_Report(uint8_t type);

/**
 * @brief  取消SOS - 返回上一页
 */
void SOS_Cancel(void);

#ifdef __cplusplus
}
#endif

#endif
