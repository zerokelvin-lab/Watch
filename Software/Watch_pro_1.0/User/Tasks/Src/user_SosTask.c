/** @file user_SosTask.c
 *  @brief SOS紧急告警任务 - 消费SOS消息队列，加载SOS页面，提供触发和上报接口
 */
#include "user_SosTask.h"
#include "user_TasksInit.h"
#include "ui_SOSPage.h"
#include "PageManager.h"
#include "stdio.h"

/**
 * @brief  触发SOS - 外部调用，发送异常类型到SOS消息队列
 */
void SOS_Trigger(uint8_t type)
{
    osMessageQueuePut(SOS_MessageQueue, &type, 0, 0);
}

/**
 * @brief  SOS上报接口 (预留，当前仅printf输出)
 *         后续可替换为BLE上报、蜂鸣器、振动马达等
 */
void SOS_Report(uint8_t type)
{
    printf("SOS REPORT: type=0x%02X\r\n", type);
    /* TODO: BLE上报、蜂鸣器报警、振动马达等 */
}

/**
 * @brief  取消SOS - 返回上一页
 */
void SOS_Cancel(void)
{
    if(Page_Get_NowPage()->page_obj == &ui_SOSPage)
    {
        Page_Back();
    }
}

/**
 * @brief  SOS任务主函数 - 阻塞等待SOS消息队列，加载SOS页面
 */
void SOSTask(void *argument)
{
    uint8_t sos_type;
    (void)argument;

    while(1)
    {
        /* 阻塞等待SOS消息 */
        osMessageQueueGet(SOS_MessageQueue, &sos_type, NULL, osWaitForever);

        /* 防止重复加载 */
        if(Page_Get_NowPage()->page_obj == &ui_SOSPage)
            continue;

        /* 设置异常类型并加载SOS页面 */
        sos_abnormal_type = sos_type;

        /* 唤醒屏幕 */
        uint8_t idle_break = 0;
        osMessageQueuePut(IdleBreak_MessageQueue, &idle_break, 0, 0);

        Page_Load(&Page_SOS);
    }
}
