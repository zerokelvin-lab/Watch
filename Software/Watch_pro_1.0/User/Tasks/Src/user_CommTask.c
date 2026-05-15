/** @file user_CommTask.c
 *  @brief ESP32通信任务 - UART1二进制帧协议收发、定时数据上报、双击语音唤醒
 */
#include "user_CommTask.h"
#include "user_TasksInit.h"
#include "comm_protocol.h"
#include "stm32f4xx_it.h"
#include "usart.h"
#include "mks_142.h"
#include <stdio.h>

/* 健康数据就绪标志 (MKS142AutoTask设置, CommTask清除) */
volatile uint8_t comm_health_data_ready = 0;

/* 定时上报间隔 */
#define COMM_MOTION_ENV_INTERVAL_MS  1000  /* 运动+环境: 1秒 */
#define COMM_PPG_INTERVAL_MS         200   /* PPG波形: 200ms (~5Hz) */

/**
 * @brief  ESP32通信主任务
 */
void CommTask(void *argument)
{
    (void)argument;
    uint32_t last_report_tick = 0;
    uint32_t last_ppg_tick = 0;

    while(1)
    {
        /* 等待UART接收事件 (阻塞, 最多等100ms以驱动定时上报) */
        uint32_t flags = osEventFlagsWait(HardIntEventHandle, HARDINT_EVENT_UART,
                                          osFlagsWaitAny | osFlagsNoClear, 100);

        /* ====== 处理UART接收数据 ====== */
        if((flags & HARDINT_EVENT_UART) != 0U)
        {
            osEventFlagsClear(HardIntEventHandle, HARDINT_EVENT_UART);

            /* HardInt_rx_len由ISR在停止/重启DMA前设置 */
            uint16_t rx_len = HardInt_rx_len;
            if(rx_len > 0 && rx_len <= 520)
            {
                printf("[COMM] rx %d: %02X %02X %02X %02X %02X\r\n", rx_len,
                       HardInt_receive_str[0], HardInt_receive_str[1],
                       HardInt_receive_str[2], HardInt_receive_str[3],
                       HardInt_receive_str[4]);
                Comm_ParseFrame(HardInt_receive_str, rx_len);
            }
        }

        /* ====== 检测双击按键 → 语音唤醒 ====== */
        if(VoiceKey_MessageQueue != NULL)
        {
            uint8_t key_val;
            if(osMessageQueueGet(VoiceKey_MessageQueue, &key_val, NULL, 0) == osOK)
            {
                Comm_SendVoiceWake();
            }
        }

        /* ====== 定时上报运动+环境数据 (每1秒) ====== */
        uint32_t now = osKernelGetTickCount();
        if((now - last_report_tick) >= COMM_MOTION_ENV_INTERVAL_MS)
        {
            last_report_tick = now;
            Comm_SendMotion();
            Comm_SendEnv();
        }

        /* ====== 健康数据就绪时上报 ====== */
        if(comm_health_data_ready)
        {
            comm_health_data_ready = 0;
            Comm_SendHealth();
        }

        /* ====== PPG流模式: 每200ms发送波形 ====== */
        if(comm_ppg_streaming)
        {
            if((now - last_ppg_tick) >= COMM_PPG_INTERVAL_MS)
            {
                last_ppg_tick = now;
                if(mks142_data.data_valid)
                {
                    Comm_SendPPG();
                }
            }
        }
    }
}
