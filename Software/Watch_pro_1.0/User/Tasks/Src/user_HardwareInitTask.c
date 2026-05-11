/** @file user_HardwareInitTask.c
 *  @brief 硬件初始化任务 - 系统上电后依次初始化所有外设（传感器、显示屏、触摸、BLE、LVGL等）
 */

/* ====== 头文件包含 ====== */
/* Private includes -----------------------------------------------------------*/

// includes
// sys
#include "usart.h"
#include "tim.h"
#include "stm32f4xx_it.h"
#include "delay.h"

// user
#include "user_TasksInit.h"
#include "HWDataAccess.h"
#include "version.h"

// bsp
#include "key.h"
#include "lcd.h"
#include "lcd_init.h"
#include "CST816.h"
#include "DataSave.h"

// ui
//gui
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "ui.h"

// APP SYS setting
#include "ui_DateTimeSetPage.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/


/**
  * @brief  hardwares init task  // 硬件初始化任务
  * @param  argument: Not used
  * @retval None
  */
void HardwareInitTask(void *argument)
{
	while(1)
	{
    vTaskSuspendAll();  // 挂起所有任务，确保初始化过程不被其他任务打断

    // RTC Wake  // RTC唤醒定时器
    if(HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 2000, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
    {
      Error_Handler();
    }
    // usart start  // 启动串口DMA接收
    HAL_UART_Receive_DMA(&huart1,(uint8_t*)HardInt_receive_str,25);
    __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);  // 使能串口空闲中断

    // usart2 start  // 启动UART2 DMA接收
    HAL_UART_Receive_DMA(&huart2,(uint8_t*)HardInt_receive2_str,25);
    __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);

    // PWM Start  // 启动PWM（用于背光控制）
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);

    // sys delay  // 系统延时初始化
    delay_init();
    // wait
    // delay_ms(1000);

    // power  // 电源管理初始化
    HWInterface.Power.Init();

    // key  // 按键初始化
    Key_Port_Init();

    // sensors  // 传感器初始化（带重试机制）
    uint8_t num = 3;  // 最多重试3次
    while(num && HWInterface.AHT21.ConnectionError)  // 温湿度传感器AHT21
    {
      num--;
      HWInterface.AHT21.ConnectionError = HWInterface.AHT21.Init();
    }

    num = 3;
    while(num && HWInterface.Ecompass.ConnectionError)  // 电子罗盘LSM303
    {
      num--;
      HWInterface.Ecompass.ConnectionError = HWInterface.Ecompass.Init();
    }
    if(!HWInterface.Ecompass.ConnectionError)
      HWInterface.Ecompass.Sleep();  // 初始化后立即休眠省电

    num = 3;
    while(num && HWInterface.Barometer.ConnectionError)  // 气压计SPL06
    {
      num--;
      HWInterface.Barometer.ConnectionError = HWInterface.Barometer.Init();
    }

    num = 3;
    while(num && HWInterface.IMU.ConnectionError)  // 惯性测量单元MPU6050
    {
      num--;
      HWInterface.IMU.ConnectionError = HWInterface.IMU.Init();
      // Sensor_MPU_Erro = MPU_Init();
    }

    num = 3;
    while(num && HWInterface.HR_meter.ConnectionError)  // 心率传感器EM7028
    {
      num--;
      HWInterface.HR_meter.ConnectionError = HWInterface.HR_meter.Init();
    }
    if(!HWInterface.HR_meter.ConnectionError)
      HWInterface.HR_meter.Sleep();  // 初始化后立即休眠省电


    // EEPROM  // EEPROM初始化与数据校验
    EEPROM_Init();
    if(!EEPROM_Check())  // EEPROM校验通过
    {
      uint8_t recbuf[3];
      SettingGet(recbuf,0x10,2);  // 读取用户设置
      if((recbuf[0]!=0 && recbuf[0]!=1) || (recbuf[1]!=0 && recbuf[1]!=1))  // 数据无效
      {
        HWInterface.IMU.wrist_is_enabled = 0;  // 使用默认值
        ui_APPSy_EN = 0;
      }
      else  // 数据有效，恢复上次设置
      {
        HWInterface.IMU.wrist_is_enabled = recbuf[0];
        ui_APPSy_EN = recbuf[1];
      }

      RTC_DateTypeDef nowdate;
      HAL_RTC_GetDate(&hrtc,&nowdate,RTC_FORMAT_BIN);  // 获取当前日期

      SettingGet(recbuf,0x20,3);  // 读取上次保存的步数
      if(recbuf[0] == nowdate.Date)  // 同一天，恢复步数
      {
        uint16_t steps=0;
        steps = recbuf[1]&0x00ff;
        steps = steps<<8 | recbuf[2];
        if(!HWInterface.IMU.ConnectionError)
          dmp_set_pedometer_step_count((unsigned long)steps);
      }
    }


    // BLE  // 蓝牙模块初始化
    HWInterface.BLE.Init();
    HWInterface.BLE.Disable();  // 初始化后先关闭

    //set the KT6328 BautRate 9600  // 设置KT6328波特率为9600
    //default is 115200
    //printf("AT+CT01\r\n");

    // touch  // 触摸屏初始化
    CST816_GPIO_Init();
    CST816_RESET();

    // lcd  // 显示屏初始化与开机画面
    LCD_Init();
    LCD_Fill(0,0, LCD_W, LCD_H, BLACK);  // 清屏黑色
    delay_ms(10);
    LCD_Set_Light(50);  // 背光亮度50%
    LCD_ShowString(72,LCD_H/2,(uint8_t*)"Welcome!", WHITE, BLACK, 24, 0);//12*6,16*8,24*12,32*16
    uint8_t lcd_buf_str[17];
    sprintf(lcd_buf_str, "OV-Watch V%d.%d.%d", watch_version_major(), watch_version_minor(), watch_version_patch());  // 显示版本号
    LCD_ShowString(34, LCD_H/2+48, (uint8_t*)lcd_buf_str, WHITE, BLACK, 24, 0);
    delay_ms(1000);  // 停留1秒
    LCD_Fill(0, LCD_H/2-24, LCD_W, LCD_H/2+49, BLACK);  // 清除欢迎信息


    // ui  // 用户界面初始化
    // LVGL init
    lv_init();
    lv_port_disp_init();  // LVGL显示驱动
    lv_port_indev_init(); // LVGL输入设备驱动
    ui_init();            // UI界面初始化

    xTaskResumeAll();  // 恢复所有任务调度
		vTaskDelete(NULL);  // 删除自身任务（初始化只执行一次）
		osDelay(500);
	}
}


