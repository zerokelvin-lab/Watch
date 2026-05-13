/** @file user_MessageSendTask.c
 *  @brief BLE消息收发任务 - 通过UART接收蓝牙AT指令并回复设备数据或设置时间
 */

/* ====== 头文件包含 ====== */
/* Private includes -----------------------------------------------------------*/
//includes
#include "string.h"
#include "stdio.h"

#include "main.h"
#include "stm32f4xx_it.h"
#include "rtc.h"

#include "user_TasksInit.h"
#include "user_MessageSendTask.h"

#include "ui.h"
#include "ui_EnvPage.h"
#include "ui_MKS142Page.h"
#include "ui_HomePage.h"
#include "ui_DateTimeSetPage.h"

#include "HWDataAccess.h"
#include "version.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/** @brief BLE消息结构体 - 存储通过蓝牙回复的传感器数据（时间、温湿度、心率、血氧、步数） */
struct
{
	RTC_DateTypeDef nowdate;
	RTC_TimeTypeDef nowtime;
	int8_t humi;
	int8_t temp;
	uint8_t HR;
	uint8_t SPO2;
	uint16_t stepNum;
}BLEMessage;

/** @brief 时间设置消息结构体 - 存储从蓝牙接收的校时命令解析后的日期和时间 */
struct
{
	RTC_DateTypeDef nowdate;
	RTC_TimeTypeDef nowtime;
}TimeSetMessage;

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  从AT指令字符串中提取命令名（"="之前的部分）
 * @param  str 输入的AT指令字符串
 * @param  cmd 输���的命令名缓冲区
 */
void StrCMD_Get(uint8_t * str,uint8_t * cmd)
{
	uint8_t i=0;
  while(str[i]!='=')  // 找到"="分隔符
  {
      cmd[i] = str[i];
      i++;
  }
}

//set time//OV+ST=20230629125555  // 设置时间命令格式：OV+ST=YYYYMMDDHHmmss
/**
 * @brief  解析蓝牙校时命令并设置RTC时间
 * @param  str 格式为"OV+ST=YYYYMMDDHHmmss"的命令字符串
 * @return 无（通过RTC_SetDate/RTC_SetTime设置硬件时钟）
 */
uint8_t TimeFormat_Get(uint8_t * str)
{
	TimeSetMessage.nowdate.Year = (str[8]-'0')*10+str[9]-'0';
	TimeSetMessage.nowdate.Month = (str[10]-'0')*10+str[11]-'0';
	TimeSetMessage.nowdate.Date = (str[12]-'0')*10+str[13]-'0';
	TimeSetMessage.nowtime.Hours = (str[14]-'0')*10+str[15]-'0';
	TimeSetMessage.nowtime.Minutes = (str[16]-'0')*10+str[17]-'0';
	TimeSetMessage.nowtime.Seconds = (str[18]-'0')*10+str[19]-'0';
	if(TimeSetMessage.nowdate.Year>0 && TimeSetMessage.nowdate.Year<99  // 校验年
		&& TimeSetMessage.nowdate.Month>0 && TimeSetMessage.nowdate.Month<=12  // 校验月
		&& TimeSetMessage.nowdate.Date>0 && TimeSetMessage.nowdate.Date<=31  // 校验日
		&& TimeSetMessage.nowtime.Hours>=0 && TimeSetMessage.nowtime.Hours<=23  // 校验时
		&& TimeSetMessage.nowtime.Minutes>=0 && TimeSetMessage.nowtime.Minutes<=59  // 校验分
		&& TimeSetMessage.nowtime.Seconds>=0 && TimeSetMessage.nowtime.Seconds<=59)  // 校验秒
	{
		RTC_SetDate(TimeSetMessage.nowdate.Year, TimeSetMessage.nowdate.Month,TimeSetMessage.nowdate.Date);
		RTC_SetTime(TimeSetMessage.nowtime.Hours,TimeSetMessage.nowtime.Minutes,TimeSetMessage.nowtime.Seconds);
		printf("TIMESETOK\r\n");  // 回复设置成功
	}

}

/**
  * @brief  send the message via BLE, use uart  // 通过蓝牙UART发送消息
  * @param  argument: Not used
  * @retval None
  */
void MessageSendTask(void *argument)
{
	while(1)
	{
		uint32_t hardint_flags = osEventFlagsWait(HardIntEventHandle, HARDINT_EVENT_UART, osFlagsWaitAny, osWaitForever);  // 等待UART接收事件
		if((hardint_flags & HARDINT_EVENT_UART) != 0U)
		{
			uint8_t IdleBreakstr = 0;
			osMessageQueuePut(IdleBreak_MessageQueue,&IdleBreakstr,NULL,1);  // 打断空闲状态
			printf("RecStr:%s\r\n",HardInt_receive_str);  // 打印接收到的指令
			if(!strcmp(HardInt_receive_str,"OV"))  // 握手指令
			{
				printf("OK\r\n");
			}
			else if(!strcmp(HardInt_receive_str,"OV+VERSION"))  // 查询版本号
			{
				printf("VERSION=V%d.%d.%d\r\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
			}
			else if(!strcmp(HardInt_receive_str,"OV+SEND"))  // 请求发送传感器数据
			{
				HAL_RTC_GetTime(&hrtc,&(BLEMessage.nowtime),RTC_FORMAT_BIN);  // 获取当前时间
				HAL_RTC_GetDate(&hrtc,&BLEMessage.nowdate,RTC_FORMAT_BIN);   // 获取当前日期
				BLEMessage.humi = HWInterface.AHT21.humidity;     // 湿度
				BLEMessage.temp = HWInterface.AHT21.temperature;  // 温度
				BLEMessage.HR = MKS142_saved_hr;      // 心率
				BLEMessage.SPO2 = MKS142_saved_spo2;      // 血氧
				BLEMessage.stepNum = HWInterface.IMU.Steps;       // 步数

				printf("data:%2d-%02d\r\n",BLEMessage.nowdate.Month,BLEMessage.nowdate.Date);
				printf("time:%02d:%02d:%02d\r\n",BLEMessage.nowtime.Hours,BLEMessage.nowtime.Minutes,BLEMessage.nowtime.Seconds);
				printf("humidity:%d%%\r\n",BLEMessage.humi);       // 湿度回复
				printf("temperature:%d\r\n",BLEMessage.temp);      // 温度回复
				printf("Heart Rate:%d%%\r\n",BLEMessage.HR);       // 心率回复
				printf("SPO2:%d%%\r\n",BLEMessage.SPO2);           // 血氧回复
				printf("Step today:%d\r\n",BLEMessage.stepNum);    // 步数回复
			}
			//set time//OV+ST=20230629125555  // 设置时间
			else if(strlen(HardInt_receive_str)==20)  // 长度20的可能是校时命令
			{
				uint8_t cmd[10];
				memset(cmd,0,sizeof(cmd));
				StrCMD_Get(HardInt_receive_str,cmd);  // 提取命令名
				if(ui_APPSy_EN && !strcmp(cmd,"OV+ST"))  // APP同步已启用且命令正确
				{
					TimeFormat_Get(HardInt_receive_str);  // 解析并设置时间
				}
			}
			memset(HardInt_receive_str,0,sizeof(HardInt_receive_str));  // 清空接收缓冲区
		}
	}
}


