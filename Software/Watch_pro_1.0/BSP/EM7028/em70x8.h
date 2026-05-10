/** @file em70x8.h @brief EM7028心率血氧传感器驱动头文件，定义所有寄存器地址和操作接口 */
#ifndef __EM70X8_H__
#define __EM70X8_H__

#include "iic_hal.h"

/** @brief EM7028芯片ID */
#define EM7028_ID					0x36
/** @brief EM7028 I2C从机地址(7位) */
#define EM7028_ADDR				0x24

/* ====== EM7028寄存器地址定义 ====== */
#define ID_REG						0x00  /** @brief 芯片ID寄存器 */
#define HRS_CFG						0x01  /** @brief 心率传感器配置寄存器(HRS1使能/LED控制) */
#define HRS_INT_CTRL			0x02  /** @brief 心率传感器中断控制 */
#define HRS_LT_L					0X03  /** @brief 心率下限阈值低字节 */
#define HRS_LT_H					0x04  /** @brief 心率下限阈值高字节 */
#define HRS_HT_L					0x05  /** @brief 心率上限阈值低字节 */
#define HRS_HT_H					0x06  /** @brief 心率上限阈值高字节 */
#define LED_CRT						0x07  /** @brief LED电流控制 */
#define	HRS2_DATA_OFFSET	0x08  /** @brief HRS2数据偏移 */
#define HRS2_CTRL					0x09  /** @brief HRS2控制寄存器 */
#define HRS2_GAIN_CTRL		0x0A  /** @brief HRS2增益控制 */
#define HRS1_CTRL					0x0D  /** @brief HRS1控制寄存器(增益/量程/频率/分辨率) */
#define INT_CTRL					0x0E  /** @brief 中断控制寄存器(LED编程电流) */
#define SOFT_RESET				0x0F  /** @brief 软复位寄存器 */

/** @brief HRS2(辅助通道)数据寄存器 */
#define HRS2_DATA0_L		0x20  /** @brief HRS2通道0数据低字节 */
#define HRS2_DATA0_H		0x21  /** @brief HRS2通道0数据高字节 */
#define HRS2_DATA1_L		0x22  /** @brief HRS2通道1数据低字节 */
#define HRS2_DATA1_H		0x23  /** @brief HRS2通道1数据高字节 */
#define HRS2_DATA2_L		0x24  /** @brief HRS2通道2数据低字节 */
#define HRS2_DATA2_H		0x25  /** @brief HRS2通道2数据高字节 */
#define HRS2_DATA3_L		0x26  /** @brief HRS2通道3数据低字节 */
#define HRS2_DATA3_H		0x27  /** @brief HRS2通道3数据高字节 */

/** @brief HRS1(主通道-红光/红外)数据寄存器 */
#define HRS1_DATA0_L		0x28  /** @brief HRS1通道0数据低字节(红光传感器) */
#define HRS1_DATA0_H		0x29  /** @brief HRS1通道0数据高字节 */
#define HRS1_DATA1_L		0x2A  /** @brief HRS1通道1数据低字节 */
#define HRS1_DATA1_H		0x2B  /** @brief HRS1通道1数据高字节 */
#define HRS1_DATA2_L		0x2C  /** @brief HRS1通道2数据低字节 */
#define HRS1_DATA2_H		0x2D  /** @brief HRS1通道2数据高字节 */
#define HRS1_DATA3_L		0x2E  /** @brief HRS1通道3数据低字节 */
#define HRS1_DATA3_H		0x2F  /** @brief HRS1通道3数据高字节 */

uint8_t  EM7028_ReadOneReg(unsigned char RegAddr);        /** @brief 读取单个寄存器 @return 寄存器值 */
void  EM7028_WriteOneReg(unsigned char RegAddr, unsigned char dat); /** @brief 写入单个寄存器 */

uint8_t EM7028_Get_ID(void);            /** @brief 读取芯片ID(应为0x36) */
uint8_t EM7028_hrs_init(void);          /** @brief 初始化心率传感器 */
uint8_t EM7028_hrs_Enable(void);        /** @brief 使能心率传感器(HRS1开启) */
uint8_t EM7028_hrs_DisEnable(void);     /** @brief 关闭心率传感器 */
uint16_t EM7028_Get_HRS1(void);         /** @brief 读取HRS1通道0(红光)16位数据 */

#endif
