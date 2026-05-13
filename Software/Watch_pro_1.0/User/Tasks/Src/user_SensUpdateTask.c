/** @file user_SensUpdateTask.c
 *  @brief 传感器数据更新任务 - 定时刷新心率、温湿度、电子罗盘、气压高度等传感器数据到界面
 */

/* ====== 头文件包含 ====== */
/* Private includes -----------------------------------------------------------*/
//includes
#include "user_TasksInit.h"
#include "user_ScrRenewTask.h"
#include "user_SensUpdateTask.h"
#include "ui_HomePage.h"
#include "ui_MenuPage.h"
#include "ui_SetPage.h"
#include "ui_HRPage.h"
#include "ui_SPO2Page.h"
#include "ui_ENVPage.h"
#include "ui_CompassPage.h"
#include "main.h"

#include "AHT21.h"
#include "LSM303.h"
#include "SPL06_001.h"
#include "em70x8.h"
#include "HrAlgorythm.h"

#include "HWDataAccess.h"
#include "fall_detect.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/** @brief 心率检测时间计数 - 配合HR算法库，用于心率计算的时间累积变量 */
uint32_t user_HR_timecount=0;

/* Private function prototypes -----------------------------------------------*/
// 这是EM7028官方lib的库函数, 没有lib用不了
/** @brief 获取血压最大值（EM7028算法库函数） */
extern uint8_t GET_BP_MAX (void);
/** @brief 获取血压最小值（EM7028算法库函数） */
extern uint8_t GET_BP_MIN (void);
/** @brief 血氧算法处理函数（EM7028算法库） */
extern void Blood_Process (void);
/** @brief 血氧50ms周期处理（EM7028算法库） */
extern void Blood_50ms_process (void);
/** @brief 血氧500ms周期处理（EM7028算法库） */
extern void Blood_500ms_process(void);
/** @brief 心率动态算法（EM7028算法库，结合加速度数据动态补偿） */
extern int em70xx_bpm_dynamic(int RECEIVED_BYTE, int g_sensor_x, int g_sensor_y, int g_sensor_z);
/** @brief EM7028复位函数 */
extern int em70xx_reset(int ref);


/**
  * @brief  MPU6050 Check the state  // MPU6050姿态检测任务
  * @param  argument: Not used
  * @retval None
  */
void MPUCheckTask(void *argument)
{
	while(1)
	{
		if(HWInterface.IMU.wrist_is_enabled)  // 抬手亮屏功能已启用
		{
			if(MPU_isHorizontal())  // 检测到水平姿态（抬手）
			{
				HWInterface.IMU.wrist_state = WRIST_UP;  // 更新为抬起状态
			}
			else  // 非水平姿态（放下）
			{
				if(WRIST_UP == HWInterface.IMU.wrist_state)  // 从抬起变为放下
				{
					HWInterface.IMU.wrist_state = WRIST_DOWN;
					if(FallDetect_GetState() == FALL_STATE_NORMAL &&  // 跌倒检测空闲时才休眠
						( Page_Get_NowPage()->page_obj == &ui_HomePage ||  // 在首页/菜单/设置页
						Page_Get_NowPage()->page_obj == &ui_MenuPage ||
						Page_Get_NowPage()->page_obj == &ui_SetPage ))
					{
						uint8_t Stopstr;
						osMessageQueuePut(Stop_MessageQueue, &Stopstr, 0, 1);//sleep  // 进入休眠
					}
				}
				HWInterface.IMU.wrist_state = WRIST_DOWN;
			}
		}
		FallDetect_Process();  // 跌倒检测（含滤波+状态机+debug输出）
		osDelay(50);  // 50ms检测周期（需足够快捕捉撞击脉冲）
	}
}

/**
  * @brief  HR data renew task  // 心率数据刷新任务
  * @param  argument: Not used
  * @retval None
  */
void HRDataUpdateTask(void *argument)
{
	uint8_t IdleBreakstr=0;
	uint16_t dat=0;
	uint8_t hr_temp=0;
	while(1)
	{
		if(Page_Get_NowPage()->page_obj == &ui_HRPage)  // 用户位于心率页面
		{
			osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);  // 打断空闲
			//sensor wake up  // 唤醒传感器
			EM7028_hrs_Enable();
			//receive the sensor wakeup message, sensor wakeup
			if(!HWInterface.HR_meter.ConnectionError)  // 传感器连接正常
			{
				//Hr messure  // 心率测量
				vTaskSuspendAll();  // 临界区保护
				hr_temp = HR_Calculate(EM7028_Get_HRS1(),user_HR_timecount);  // 调用HR算法计算心率
				xTaskResumeAll();
				if(HWInterface.HR_meter.HrRate != hr_temp && hr_temp>50 && hr_temp<120)  // 过滤异常值
				{
					HWInterface.HR_meter.HrRate = hr_temp;  // 更新心率值
				}
			}
		}
		osDelay(50);  // 50ms刷新周期
	}
}


/**
  * @brief  Sensor data update task  // 传感器数据更新任务
  * @param  argument: Not used
  * @retval None
  */
void SensorDataUpdateTask(void *argument)
{
	uint8_t value_strbuf[6];
	uint8_t IdleBreakstr=0;
	while(1)
	{
		// Update the sens data showed in Home  // 更新首页传感器数据
		uint8_t HomeUpdataStr;
		if(osMessageQueueGet(HomeUpdata_MessageQueue, &HomeUpdataStr, NULL, 0)==osOK)  // 收到首页更新请求
		{
			//bat  // 电池电量检测
			uint8_t value_strbuf[5];

			HWInterface.Power.power_remain = HWInterface.Power.BatCalculate();  // 计算剩余电量
			if(HWInterface.Power.power_remain>0 && HWInterface.Power.power_remain<=100)  // 有效值
			{}
			else  // 无效值，置0
			{HWInterface.Power.power_remain = 0;}

			//steps  // 获取当日步数
			if(!(HWInterface.IMU.ConnectionError))
			{
				HWInterface.IMU.Steps = HWInterface.IMU.GetSteps();
			}

			//temp and humi  // 获取温湿度
			if(!(HWInterface.AHT21.ConnectionError))
			{
				//temp and humi messure  // 温湿度测量
				float humi,temp;
				HWInterface.AHT21.GetHumiTemp(&humi,&temp);
				//check  // 数据校验
				if(temp>-10 && temp<50 && humi>0 && humi<100)
				{
					// ui_EnvTempValue = (int8_t)temp;
					// ui_EnvHumiValue = (int8_t)humi;
					HWInterface.AHT21.humidity = humi;
					HWInterface.AHT21.temperature = temp;
				}
			}

			//send data save message queue  // 触发数据保存
			uint8_t Datastr = 3;
			osMessageQueuePut(DataSave_MessageQueue, &Datastr, 0, 1);

		}


		// SPO2 Page  // 血氧页面——当前未实现
		if(Page_Get_NowPage()->page_obj == &ui_SPO2Page)
		{
			osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);
			//sensor wake up

			//receive the sensor wakeup message, sensor wakeup
			if(0)  // 血氧功能暂未实现
			{
				//SPO2 messure
			}
		}
		// Env Page  // 环境数据页面（温湿度）
		else if(Page_Get_NowPage()->page_obj == &ui_EnvPage)
		{
			osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);
			//receive the sensor wakeup message, sensor wakeup
			if(!HWInterface.AHT21.ConnectionError)  // AHT21连接正常
			{
				//temp and humi messure
				float humi,temp;
				HWInterface.AHT21.GetHumiTemp(&humi,&temp);
				//check
				if(temp>-10 && temp<50 && humi>0 && humi<100)
				{
					HWInterface.AHT21.temperature = (int8_t)temp;   // 转为整数显示
					HWInterface.AHT21.humidity = (int8_t)humi;      // 转为整数显示
				}
			}

		}
		// Compass page  // 指南针页面
		else if(Page_Get_NowPage()->page_obj == &ui_CompassPage)
		{
			osMessageQueuePut(IdleBreak_MessageQueue, &IdleBreakstr, 0, 1);
			//receive the sensor wakeup message, sensor wakeup
			LSM303DLH_Wakeup();  // 唤醒电子罗盘
			//SPL_Wakeup();  // 唤醒气压计
			//if the sensor is no problem  // 电子罗盘正常
			if(!HWInterface.Ecompass.ConnectionError)
			{
				//messure  // 读取加速度和磁场数据
				int16_t Xa,Ya,Za,Xm,Ym,Zm;
				LSM303_ReadAcceleration(&Xa,&Ya,&Za);  // 读取加速度
				LSM303_ReadMagnetic(&Xm,&Ym,&Zm);      // 读取地磁
				float temp = Azimuth_Calculate(Xa,Ya,Za,Xm,Ym,Zm)+0;//0 offset  // 计算方位角
				if(temp<0)
				{temp+=360;}  // 负角度转正
				//check
				if(temp>=0 && temp<=360)  // 有效角度范围
				{
					HWInterface.Ecompass.direction = (uint16_t)temp;  // 更新方向角
				}
			}
			//if the sensor is no problem  // 气压计正常
			if(!HWInterface.Barometer.ConnectionError)
			{
				//messure  // 计算海拔高度
				float alti = Altitude_Calculate();
				//check
				if(1)
				{
					HWInterface.Barometer.altitude = (int16_t)alti;  // 更新海拔
				}
			}
		}

		osDelay(500);  // 500ms采集周期
	}
}
