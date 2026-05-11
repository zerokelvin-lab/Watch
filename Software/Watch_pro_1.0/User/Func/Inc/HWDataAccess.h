/** @file HWDataAccess.h
 *  @brief 硬件中间层访问接口，为UI和应用层提供统一的硬件数据读取方法
 */

/**
 *  @addtogroup  Hareware Middle Layer
 *  @brief       Hardware Middle Layer, to access data from BSP and STM32 HAL Library
 *
 *  @{
 *      @file       HWDataAccess.h
 *      @brief      middleware, for UI and APP Layer to get the hardware data
 *      @details    you can enable or disable in .h file.
 * 					        加这个文件就是为了方便UI移植, 比如你要把工程移植到PC仿真,
 *   				        直接把MidFunc中的文件和UI文件都复制过去,
 * 					        然后直接把.h文件中的HW_USE_HARDWARE变成0就行了.
 */


#ifndef HWDATAACCESS_H
#define HWDATAACCESS_H

#ifdef __cplusplus
extern "C" {
#endif


/* ====== 硬件模块开关定义 ====== */
/**
 *  if not use, just set 0
 *
 *
 *  if just test ui, no hardware, just set HW_USE_HARDWARE 0
 *
 */

#define HW_USE_HARDWARE 1 // 硬件总开关，置0可切换到纯软件仿真模式

#if HW_USE_HARDWARE
  #define HW_USE_RTC    1 // 使用RTC实时时钟
  #define HW_USE_BLE    1 // 使用BLE蓝牙
  #define HW_USE_BAT    1 // 使用电池管理
  #define HW_USE_LCD    1 // 使用LCD屏幕
  #define HW_USE_IMU    1 // 使用IMU惯性测量单元（MPU6050）
  #define HW_USE_AHT21  1 // 使用AHT21温湿度传感器
  #define HW_USE_SPL06  1 // 使用SPL06气压计
  #define HW_USE_LSM303 1 // 使用LSM303电子罗盘
  #define HW_USE_EM7028 1 // 使用EM7028心率传感器
#endif

/* ====== 头文件包含 ====== */

#include "stdint.h"
#include <stdbool.h>

#if HW_USE_RTC
  #include "rtc.h"
#endif

#if HW_USE_BLE
  #include "kt6328.h"
#endif

#if HW_USE_BAT
  #include "power.h"
#endif

#if HW_USE_LCD
  #include "lcd_init.h"
#endif

#if HW_USE_IMU
  #include "MPU6050.h"
  #include "inv_mpu.h"
  #include "inv_mpu_dmp_motion_driver.h"
#endif

#if HW_USE_AHT21
  #include "AHT21.h"
#endif

#if HW_USE_SPL06
  #include "SPL06_001.h"
#endif

#if HW_USE_LSM303
  #include "LSM303.h"
#endif

#if HW_USE_EM7028
  #include "em70x8.h"
#endif

/* ====== 数据类型定义 ====== */
/**
  * @brief  HW RTC DateTime structure definition
  */
typedef struct
{
    uint8_t WeekDay;    // 星期（1=周一，7=周日）

    uint8_t Month;      // 月份（1-12 BCD格式）

    uint8_t Date;       // 日期（1-31）

    uint8_t Year;       // 年份（0-99，如24表示2024年）

    uint8_t Hours;      // 小时（0-23，24小时制）

    uint8_t Minutes;    // 分钟（0-59）

    uint8_t Seconds;    // 秒钟（0-59）

} HW_DateTimeTypeDef;


/**
  * @brief  HW RTC Interface definition
  */
typedef struct
{
    void (*GetTimeDate)(HW_DateTimeTypeDef *nowdatetime);                                          // 获取当前日期时间
    void (*SetDate)(uint8_t year, uint8_t month, uint8_t date);                                   // 设置日期
    void (*SetTime)(uint8_t hours, uint8_t minutes, uint8_t seconds);                             // 设置时间
    uint8_t (*CalculateWeekday)(uint8_t setyear, uint8_t setmonth, uint8_t setday, uint8_t century); // 计算星期几
} HW_RTC_InterfaceTypeDef;


/**
  * @brief  HW BLE Interface definition
  */
typedef struct
{
    void (*Init)(void);    // 蓝牙初始化
    void (*Enable)(void);  // 蓝牙使能
    void (*Disable)(void); // 蓝牙禁用
} HW_BLE_InterfaceTypeDef;


/**
  * @brief  HW Power Interface definition
  */
typedef struct
{
    uint8_t power_remain;          // 电池剩余电量百分比

    void (*Init)(void);            // 电源初始化
    void (*Shutdown)(void);        // 关机（电源由硬件管理，软件无操作）
    uint8_t (*BatCalculate)(void); // 计算电池电量
} HW_Power_InterfaceTypeDef;


/**
  * @brief  HW LCD Interface definition
  */
typedef struct
{
    void (*SetLight)(uint8_t dc); // 设置LCD背光亮度
} HW_LCD_InterfaceTypeDef;


/**
  * @brief  HW IMU 抬手状态定义
  */
#define WRIST_UP 1   // 抬腕
#define WRIST_DOWN 0 // 垂腕

/**
  * @brief  HW IMU Interface definition
  */
typedef struct
{
    uint8_t ConnectionError; // IMU连接状态，1表示连接异常
    uint16_t Steps;          // 当前步数
    uint8_t wrist_state;     // 手腕状态（抬腕/垂腕）
    uint8_t wrist_is_enabled; // 抬腕唤醒功能是否启用

    uint8_t (*Init)(void);                          // IMU初始化
    void (*WristEnable)(void);                       // 启用抬腕唤醒
    void (*WristDisable)(void);                      // 禁用抬腕唤醒
    uint16_t (*GetSteps)(void);                      // 获取当前步数
    int (*SetSteps)(unsigned long count);            // 设置步数
} HW_IMU_InterfaceTypeDef;


/**
  * @brief  HW AHT21温湿度传感器接口定义
  */
typedef struct
{
  uint8_t ConnectionError; // 传感器连接状态
  uint8_t temperature;     // 当前温度值
  uint8_t humidity;        // 当前湿度值
  uint8_t (*Init)(void);                              // 传感器初始化
  void (*GetHumiTemp)(float *humi, float *temp);      // 读取湿度温度
} HW_AHT21_InterfaceTypeDef;


/**
  * @brief  HW SPL06-001气压计接口定义
  */
typedef struct
{
  uint8_t ConnectionError; // 传感器连接状态
  uint16_t altitude;       // 当前海拔高度
  uint8_t (*Init)(void);   // 传感器初始化

} HW_Barometer_InterfaceTypeDef;

/**
  * @brief  HW LSM303电子罗盘接口定义
  */
typedef struct
{
  uint8_t ConnectionError; // 传感器连接状态
  uint16_t direction;      // 当前方位角度
  uint8_t (*Init)(void);   // 传感器初始化
  void (*Sleep)(void);     // 传感器休眠

} HW_Ecompass_InterfaceTypeDef;

/**
  * @brief  HW EM7028心率传感器接口定义
  */
typedef struct
{
  uint8_t ConnectionError; // 传感器连接状态
  uint8_t HrRate;          // 当前心率值
  uint8_t SPO2;            // 当前血氧值
  uint8_t (*Init)(void);   // 传感器初始化
  void (*Sleep)(void);     // 传感器休眠

} HW_HRmeter_InterfaceTypeDef;

/**
  * @brief  硬件接口总结构体，统一管理所有硬件模块的函数指针和数据
  */
typedef struct
{
    HW_RTC_InterfaceTypeDef RealTimeClock; // RTC实时时钟接口
    HW_BLE_InterfaceTypeDef BLE;           // 蓝牙接口
    HW_Power_InterfaceTypeDef Power;       // 电源管理接口
    HW_LCD_InterfaceTypeDef LCD;           // LCD屏幕接口
    HW_IMU_InterfaceTypeDef IMU;           // IMU姿态传感器接口
    HW_AHT21_InterfaceTypeDef AHT21;       // AHT21温湿度传感器接口
    HW_Barometer_InterfaceTypeDef Barometer; // 气压计接口
    HW_Ecompass_InterfaceTypeDef Ecompass; // 电子罗盘接口
    HW_HRmeter_InterfaceTypeDef HR_meter;  // 心率血氧传感器接口
} HW_InterfaceTypeDef;



/* ====== 函数原型声明 ====== */


/* ====== 外部变量声明 ====== */
extern HW_InterfaceTypeDef HWInterface; // 全局硬件接口实例，在.c文件中定义



#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
