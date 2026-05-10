/** @file user_DataSaveTask.c
 *  @brief 数据保存任务 - 将用户设置和计步数据保存到板载EEPROM
 */

/* ====== 头文件包含 ====== */
/* Private includes -----------------------------------------------------------*/
//includes
#include "user_DataSaveTask.h"
//APP SYS setting
#include "ui_DateTimeSetPage.h"
#include "ui_HomePage.h"

#include "main.h"
#include "rtc.h"
#include "DataSave.h"
#include "inv_mpu_dmp_motion_driver.h"

#include "HWDataAccess.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/******************************************
EEPROM Data description:
[0x00]:0x55 for check        // 校验字节1
[0x01]:0xAA for check        // 校验字节2

[0x10]:user wrist setting, HWInterface.IMU.wrist_is_enabled  // 抬手亮屏设置
[0x11]:user ui_APPSy_EN setting  // APP同步使能设置

[0x20]:Last Save Day(0-31)  // 上次保存的日期（日）
[0x21]:Day Steps             // 当日步数高字节
[0x22]:Day Steps             // 当日步数低字节

*******************************************/


/* Private function prototypes -----------------------------------------------*/

/* Tasks ---------------------------------------------------------------------*/

/**
  * @brief  Data Save in the EEPROM  // 将数据保存至EEPROM
  * @param  argument: Not used
  * @retval None
  */
void DataSaveTask(void *argument)
{

	while(1)
	{
		uint8_t Datastr=0;
		if(osMessageQueueGet(DataSave_MessageQueue,&Datastr,NULL,1)==osOK)  // 收到保存请求
		{
			/****************
			Setting change  // 设置变更
			date change     // 日期变更
			Step change     // 步数变更
			****************/
			uint8_t dat[3];
			dat[0] = HWInterface.IMU.wrist_is_enabled;  // 抬手亮屏设置
			dat[1] = ui_APPSy_EN;                        // APP同步设置
			SettingSave(dat,0x10,2);                     // 保存到EEPROM 0x10地址

			RTC_DateTypeDef nowdate;
			HAL_RTC_GetDate(&hrtc,&nowdate,RTC_FORMAT_BIN);  // 获取当前日期

			SettingGet(dat,0x20,3);  // 读取上次保存的日期和步数
			if(dat[0] != nowdate.Date)  // 日期已变更，新的一天
			{
				if(!HWInterface.IMU.ConnectionError)
					HWInterface.IMU.SetSteps(0);  // 重置当日步数

				dat[0] = nowdate.Date;
				dat[2] = 0;
				dat[1] = 0;
				SettingSave(dat,0x20,3);
			}
			else  // 同一天，保存当前步数
			{
				uint16_t temp = HWInterface.IMU.GetSteps();
				dat[0] = nowdate.Date;
				dat[2] = temp & 0xff;       // 步数低字节
				dat[1] = temp>>8 & 0xff;    // 步数高字节
				SettingSave(dat,0x20,3);
			}

		}
		osDelay(100);  // 任务延时
	}
}


