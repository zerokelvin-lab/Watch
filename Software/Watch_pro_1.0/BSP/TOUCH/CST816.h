/** @file CST816.h @brief CST816触摸屏驱动头文件 - 寄存器定义、引脚宏定义、数据结构、API声明 */

#ifndef __CST816_H
#define __CST816_H

#include "stdint.h"
#include "iic_hal.h"
#include "delay.h"

/* ====== CST816 设备引脚定义 ====== */
#define TOUCH_RST_PIN GPIO_PIN_15                    // 触摸屏复位引脚：PA15
#define TOUCH_INT_PIN GPIO_PIN_1                     // 触摸屏中断引脚：PB1
#define TOUCH_RST_PORT GPIOA                         // 复位引脚端口
#define TOUCH_INT_PORT GPIOB                         // 中断引脚端口

/* ====== 引脚操作宏 ====== */
#define TOUCH_RST_0 HAL_GPIO_WritePin(TOUCH_RST_PORT, TOUCH_RST_PIN, GPIO_PIN_RESET) // 复位拉低
#define TOUCH_RST_1 HAL_GPIO_WritePin(TOUCH_RST_PORT, TOUCH_RST_PIN, GPIO_PIN_SET)   // 复位拉高

/* ====== 设备I2C地址 ====== */
#define Device_Addr 	0x15                         // CST816的7位I2C设备地址

/* ====== 触摸屏寄存器地址映射 ====== */
#define GestureID 			0x01                     // 手势ID寄存器
#define FingerNum 			0x02                     // 手指触摸数量寄存器
#define XposH 					0x03                     // X坐标高字节（高4位有效）
#define XposL 					0x04                     // X坐标低字节
#define YposH 					0x05                     // Y坐标高字节（高4位有效）
#define YposL 					0x06                     // Y坐标低字节
#define ChipID 					0xA7                     // 芯片ID寄存器
#define SleepMode				0xE5                     // 睡眠模式控制寄存器
#define MotionMask 			0xEC                     // 连续动作使能配置寄存器
#define IrqPluseWidth 	0xED                     // 中断脉冲宽度配置寄存器
#define NorScanPer 			0xEE                     // 正常扫描周期配置寄存器
#define MotionSlAngle 	0xEF                     // 手势滑动角度配置寄存器
#define LpAutoWakeTime 	0xF4                     // 低功耗自动唤醒时间寄存器
#define LpScanTH 				0xF5                     // 低功耗扫描唤醒门限寄存器
#define LpScanWin 			0xF6                     // 低功耗扫描窗口寄存器
#define LpScanFreq 			0xF7                     // 低功耗扫描频率寄存器
#define LpScanIdac 			0xF8                     // 低功耗扫描电流寄存器
#define AutoSleepTime 	0xF9                     // 自动休眠时间寄存器
#define IrqCtl 					0xFA                     // 中断控制寄存器
#define AutoReset 			0xFB                     // 自动复位寄存器
#define LongPressTime 	0xFC                     // 长按时间寄存器
#define IOCtl 					0xFD                     // IO控制寄存器
#define DisAutoSleep 		0xFE                     // 禁用自动休眠寄存器

/* ====== 触摸屏坐标结构体 ====== */
/** @brief 存储触摸点坐标信息 */
typedef struct
{
	unsigned int X_Pos;            // 触摸点X坐标
	unsigned int Y_Pos;            // 触摸点Y坐标
} CST816_Info;

/* ====== 手势ID识别选项 ====== */
/** @brief CST816支持的手势类型枚举 */
typedef enum
{
	NOGESTURE = 	0x00,         // 无手势
	DOWNGLIDE = 	0x01,         // 向下滑动
	UPGLIDE = 		0x02,         // 向上滑动
	LEFTGLIDE = 	0x03,         // 向左滑动
	RIGHTGLIDE = 	0x04,         // 向右滑动
	CLICK = 			0x05,         // 单击
	DOUBLECLICK = 0x0B,         // 双击
	LONGPRESS = 	0x0C,         // 长按
} GestureID_TypeDef;

/* ====== 连续动作配置选项 ====== */
/** @brief 连续动作使能配置枚举 */
typedef enum
{
	M_DISABLE = 	0x00,         // 禁用所有连续动作
	EnConLR = 		0x01,         // 使能连续左右滑动
	EnConUD = 		0x02,         // 使能连续上下滑动
	EnDClick = 		0x03,         // 使能双击
	M_ALLENABLE = 0x07,         // 使能所有连续动作
} MotionMask_TypeDef;

/* ====== 中断低脉冲发射方式选项 ====== */
/** @brief 中断低脉冲发射模式枚举 */
typedef enum
{
	OnceWLP = 		0x00,         // 单次低脉冲
	EnMotion = 		0x10,         // 使能动作检测
	EnChange = 		0x20,         // 使能变化检测
	EnTouch = 		0x40,         // 使能触摸检测
	EnTest = 			0x80,         // 使能测试模式
} IrqCtl_TypeDef;

/* ====== CST816 API函数声明 ====== */

/* 触摸屏初始化相关函数 */
void CST816_GPIO_Init(void);                         // 初始化GPIO引脚
void CST816_RESET(void);                             // 硬件复位触摸屏
void CST816_Init(void);                              // 初始化触摸屏

/* 触摸屏操作函数 */
void CST816_Get_XY_AXIS(void);                       // 读取触摸坐标
uint8_t CST816_Get_ChipID(void);                     // 读取芯片ID
uint8_t CST816_Get_FingerNum(void);                  // 读取触摸手指数量

/* 触摸屏读写函数 */
void CST816_IIC_WriteREG(uint8_t addr, uint8_t dat); // I2C写寄存器
uint8_t CST816_IIC_ReadREG(unsigned char addr);      // I2C读寄存器

/* 触摸屏有关参数配置函数 */
void CST816_Config_MotionMask(uint8_t mode);         // 配置连续动作检测
void CST816_Config_AutoSleepTime(uint8_t time);      // 配置自动休眠时间
void CST816_Config_MotionSlAngle(uint8_t x_right_y_up_angle); // 配置手势滑动角度
void CST816_Config_NorScanPer(uint8_t Period);       // 配置正常扫描周期
void CST816_Config_IrqPluseWidth(uint8_t Width);     // 配置中断脉冲宽度
void CST816_Config_LpScanTH(uint8_t TH);             // 配置低功耗扫描门限
void CST816_Wakeup(void);                            // 触摸屏唤醒
void CST816_Sleep(void);                             // 触摸屏进入休眠

#endif
