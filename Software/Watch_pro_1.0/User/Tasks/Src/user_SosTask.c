/** @file user_SosTask.c
 *  @brief SOS紧急告警接口 - 触发时加载SOS页面（通过lv_async_call延迟到LVGL任务执行）
 */
#include "user_SosTask.h"
#include "user_TasksInit.h"
#include "ui_SOSPage.h"
#include "PageManager.h"
#include "comm_protocol.h"
#include "lvgl.h"

/**
 * @brief  LVGL异步回调 - 在LVGL任务上下文中加载SOS页面
 */
static void sos_load_async(void *arg)
{
    (void)arg;
    /* 防止重复加载 */
    if(Page_Get_NowPage()->page_obj == &ui_SOSPage)
        return;
    Page_Load(&Page_SOS);
}

/**
 * @brief  触发SOS - 通过lv_async_call延迟到LVGL任务中加载页面（线程安全）
 */
void SOS_Trigger(uint8_t type)
{
    /* 设置异常类型 */
    sos_abnormal_type = type;

    /* 唤醒屏幕 */
    uint8_t idle_break = 0;
    osMessageQueuePut(IdleBreak_MessageQueue, &idle_break, 0, 0);

    /* 延迟加载SOS页面（必须在LVGL任务上下文中执行） */
    lv_async_call(sos_load_async, NULL);
}

/**
 * @brief  SOS上报接口 - 倒计时结束且用户未取消时调用，通过UART1发送SOS帧给ESP32
 */
void SOS_Report(uint8_t type)
{
    Comm_SendSOS(type);
    /* TODO: 蜂鸣器报警、振动马达等 */
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
 * @brief  SOS任务主函数 (已弃用，SOS_Trigger直接加载页面)
 */
void SOSTask(void *argument)
{
    (void)argument;
    /* 不再需要独立任务，挂起自身 */
    osThreadSuspend(osThreadGetId());
    while(1) { osDelay(0xFFFFFFFF); }
}
