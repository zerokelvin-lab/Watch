/** @file SPL06_001.h @brief SPL06-001气压传感器驱动头文件 - 寄存器定义、校准配置、API声明 */

#ifndef _SPL06_001_H_
#define _SPL06_001_H_

#include "iic_hal.h"

/* ====== 温度/压力换算因子（根据数据手册设置） ====== */
#define KT							524288.0		// 温度换算因子（Temperature k-factor）
#define KP							1572864.0		// 压力换算因子（Pressure k-factor）

/* ====== 芯片信息 ====== */
#define SPL_CHIP_ID					0X10          // 芯片ID
#define SPL_CHIP_ADDRESS		0x76           // I2C设备地址（7位地址）

/* ====== 寄存器地址映射 ====== */

/* ----- 压力数据寄存器（24位） ----- */
#define SPL_PRS_B2					0x00          // 压力值最高字节 B2[23:16]
#define SPL_PRS_B1					0x01          // 压力值中间字节 B1[15:8]
#define SPL_PRS_B0					0x02          // 压力值最低字节 B0[7:0]

/* ----- 温度数据寄存器（24位） ----- */
#define SPL_TMP_B2					0x03          // 温度值最高字节 B2[23:16]
#define SPL_TMP_B1					0x04          // 温度值中间字节 B1[15:8]
#define SPL_TMP_B0					0x05          // 温度值最低字节 B0[7:0]

/* ----- 配置寄存器 ----- */
#define SPL_PRS_CFG					0x06          // 压力配置寄存器（过采样率和测量速率）
#define SPL_TMP_CFG					0x07          // 温度配置寄存器（过采样率和测量速率）
#define SPL_MEAS_CFG				0x08          // 测量配置寄存器（工作模式选择）
#define SPL_CFG_REG					0x09          // 配置寄存器（中断/FIFO/移位设置）
#define SPL_INT_STS					0x0A          // 中断状态寄存器
#define SPL_FIFO_STS				0x0B          // FIFO状态寄存器
#define SPL_RESET_REG				0x0C          // 软件复位寄存器
/* ----- 芯片ID ----- */
#define SPL_ID_REG					0x0D          // 芯片ID和硬件版本

/* ----- 校准系数寄存器（出厂校准值，共18个寄存器） ----- */
#define COEF_C0						0x10          // 校准系数c0（高8位）
#define COEF_C0_C1				0x11          // c0低4位 + c1高4位
#define COEF_C1						0x12          // 校准系数c1（低8位）
#define COEF_C00_H				0x13          // 校准系数c00（高8位）
#define COEF_C00_L				0x14          // 校准系数c00（中间8位）
#define COEF_C00_C10			0x15          // c00低4位 + c10高4位
#define COEF_C10_M				0x16          // 校准系数c10（中间8位）
#define COEF_C10_L				0x17          // 校准系数c10（低8位）
#define COEF_C01_H				0x18          // 校准系数c01（高8位）
#define COEF_C01_L				0x19          // 校准系数c01（低8位）
#define COEF_C11_H				0x1A          // 校准系数c11（高8位）
#define COEF_C11_L				0x1B          // 校准系数c11（低8位）
#define COEF_C20_H				0x1C          // 校准系数c20（高8位）
#define COEF_C20_L				0x1D          // 校准系数c20（低8位）
#define COEF_C21_H				0x1E          // 校准系数c21（高8位）
#define COEF_C21_L				0x1F          // 校准系数c21（低8位）
#define COEF_C30_H				0x20          // 校准系数c30（高8位）
#define COEF_C30_L				0x21          // 校准系数c30（低8位）

/* ====== 压力测量速率配置（PRS_CFG[6:4]） ====== */
#define PM_RATE1				0x00		//1   次/秒
#define PM_RATE2				0x01		//2   次/秒
#define PM_RATE3				0x02		//4   次/秒
#define PM_RATE4				0x03		//8   次/秒
#define PM_RATE5				0x04		//16  次/秒
#define PM_RATE6				0x05		//32  次/秒
#define PM_RATE7				0x06		//64  次/秒
#define PM_RATE8				0x07		//128 次/秒

/* ====== 压力过采样率配置（PRS_CFG[3:0]） ====== */
#define	PM_PRC1					0x00		//单次（single）
#define	PM_PRC2					0x01		//2   倍过采样（低功耗）
#define	PM_PRC3					0x02		//4   倍过采样
#define	PM_PRC4					0x03		//8   倍过采样（标准）
#define	PM_PRC5					0x04		//16  倍过采样（需要CFG_CFG移位）
#define	PM_PRC6					0x05		//32  倍过采样（需要CFG_CFG移位）
#define	PM_PRC7					0x06		//64  倍过采样（高精度，需要CFG_CFG移位）
#define	PM_PRC8					0x07		//128 倍过采样（需要CFG_CFG移位）

/* ====== 温度测量配置（TMP_CFG[7]/[6:4]/[3:0]） ====== */
#define TMP_EXT 0x01            // 外部温度传感器模式
#define TMP_INT 0x00            // 内部温度传感器模式

/* ----- 温度测量速率（TMP_CFG[6:4]） ----- */
#define TMP_RATE1				0x00		//1   次/秒
#define TMP_RATE2				0x01		//2   次/秒
#define TMP_RATE3				0x02		//4   次/秒
#define TMP_RATE4				0x03		//8   次/秒
#define TMP_RATE5				0x04		//16  次/秒
#define TMP_RATE6				0x05		//32  次/秒
#define TMP_RATE7				0x06		//64  次/秒
#define TMP_RATE8				0x07		//128 次/秒

/* ----- 温度过采样率（TMP_CFG[3:0]） ----- */
#define	TMP_PRC1				0x00		//单次
#define	TMP_PRC2				0x01		//2   倍
#define	TMP_PRC3				0x02		//4   倍
#define	TMP_PRC4				0x03		//8   倍
#define	TMP_PRC5				0x04		//16  倍
#define	TMP_PRC6				0x05		//32  倍
#define	TMP_PRC7				0x06		//64  倍
#define	TMP_PRC8				0x07		//128 倍

/* ====== SPL06-001 API函数声明 ====== */

/* I2C底层读写 */
uint8_t SPL_ReadOneReg(uint8_t addr);                // 读取单个寄存器
void SPL_WriteOneReg(uint8_t addr, uint8_t dat);     // 写入单个寄存器

/* 初始化/休眠/唤醒 */
uint8_t SPL_init(void);                              // 初始化传感器
void SPL_Sleep(void);                                // 进入待机模式
void SPL_Wakeup(void);                               // 从待机唤醒
uint8_t SPL_GetID(void);                             // 读取芯片ID

/* 传感器数据计算 */
float Altitude_Calculate(void);                      // 计算海拔高度
float Pressure_Calculate(void);                      // 计算气压值
float Temperature_Calculate(void);                   // 计算温度值

#endif
