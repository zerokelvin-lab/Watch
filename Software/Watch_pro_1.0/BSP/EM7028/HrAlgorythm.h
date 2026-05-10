/** @file HrAlgorythm.h @brief 心率计算算法头文件，基于PPG信号的峰值检测计算心率 */
#ifndef __HRALGORYTHM_H__
#define __HRALGORYTHM_H__

#include "main.h"
#include "user_Queue.h"

void HR_AlgoInit(void);                                                     /** @brief 心率算法初始化（初始化3个队列） */
uint16_t HR_Calculate(uint16_t present_dat,uint32_t present_time);          /** @brief 心率计算 @param present_dat 当前PPG原始数据 @param present_time 当前时间戳(ms) @return 计算得到的心率值(bpm) */

#endif
