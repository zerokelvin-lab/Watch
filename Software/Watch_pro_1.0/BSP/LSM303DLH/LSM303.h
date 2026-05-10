/** @file LSM303.h @brief LSM303DLH加速度计/磁力计驱动头文件 - 寄存器定义、I2C地址、API声明 */

#ifndef _LSM303_H_
#define _LSM303_H_

#include "iic_hal.h"

/* ====== LSM303DLH 寄存器定义 ====== */

/* ----- 磁力计（罗盘）寄存器 ----- */
#define	LSM303_CRA_REG_M					0x00	//RW 磁力计配置寄存器A（数据速率配置）
#define	LSM303_CRB_REG_M					0x01	//RW 磁力计配置寄存器B（增益配置）
#define	LSM303_MR_REG_M						0x02	//RW 磁力计模式寄存器（工作模式选择）
#define	LSM303_OUT_X_H_M					0x03	//R  X轴磁场输出高字节
#define	LSM303_OUT_X_L_M					0x04	//R  X轴磁场输出低字节
#define	LSM303_OUT_Z_H_M					0x05	//R  Z轴磁场输出高字节
#define	LSM303_OUT_Z_L_M					0x06	//R  Z轴磁场输出低字节
#define	LSM303_OUT_Y_H_M					0x07	//R  Y轴磁场输出高字节
#define	LSM303_OUT_Y_L_M					0x08	//R  Y轴磁场输出低字节
#define	LSM303_SR_REG_M						0x09	//R  状态寄存器
#define	LSM303_IRA_REG_M					0x0A	//R  中断寄存器A
#define	LSM303_IRB_REG_M					0x0B	//R  中断寄存器B
#define	LSM303_IRC_REG_M					0x0C	//R  中断寄存器C

/* ----- 加速度计寄存器 ----- */
#define	LSM303_CTRL_REG1_A					0x20	//RW 加速度计控制寄存器1（数据速率/轴使能）
#define	LSM303_CTRL_REG2_A					0x21	//RW 加速度计控制寄存器2
#define	LSM303_CTRL_REG3_A					0x22	//RW 加速度计控制寄存器3（中断配置）
#define	LSM303_CTRL_REG4_A					0x23	//RW 加速度计控制寄存器4（满量程/模式）
#define	LSM303_CTRL_REG5_A					0x24	//RW 加速度计控制寄存器5
#define	LSM303_HP_FILTER_RESET_A		0x25	//R  高通滤波器复位
#define	LSM303_REFERENCE_A					0x26	//RW 加速度计参考值
#define	LSM303_STATUS_REG_A					0x27	//R  状态寄存器
#define	LSM303_OUT_X_L_A						0x28	//R  X轴加速度低字节输出
#define	LSM303_OUT_X_H_A						0x29	//R  X轴加速度高字节输出
#define	LSM303_OUT_Y_L_A						0x2A	//R  Y轴加速度低字节输出
#define	LSM303_OUT_Y_H_A						0x2B	//R  Y轴加速度高字节输出
#define	LSM303_OUT_Z_L_A						0x2C	//R  Z轴加速度低字节输出
#define	LSM303_OUT_Z_H_A						0x2D	//R  Z轴加速度高字节输出
#define	LSM303_INT1_CFG_A						0x30	//RW 中断1配置
#define	LSM303_INT1_SOURCE_A				0x31	//R  中断1源
#define	LSM303_INT1_THS_A						0x32	//RW 中断1阈值
#define	LSM303_INT1_DURATION_A			0x33	//RW 中断1持续时间
#define	LSM303_INT2_CFG_A						0x34	//RW 中断2配置
#define	LSM303_INT2_SOURCE_A				0x35	//R  中断2源
#define	LSM303_INT2_THS_A						0x36	//RW 中断2阈值
#define	LSM303_INT2_DURATION_A			0x37	//RW 中断2持续时间

/* ----- 温度传感器寄存器 ----- */
#define TEMP_OUT_H_M								0x31	//R  温度输出高字节
#define TEMP_OUT_L_M								0x32	//R  温度输出低字节

/* ====== I2C从机地址 ====== */
#define LSM303_SlaveAddr_A					0x19	//加速度计I2C地址（SA0引脚接GND时为0x19）
#define LSM303_SlaveAddr_M					0x1E	//磁力计和温度的I2C地址

/* ====== LSM303DLH API 函数声明 ====== */

/* I2C底层读写函数 */
unsigned char LSM303_ReadOneReg(unsigned char RegAddr);          // 读取单个寄存器
void LSM303_ReadMultiReg(unsigned char RegAddr, unsigned char RegNum, unsigned char DataBuff[]); // 读取多个寄存器
unsigned char LSM303_Temp_ReadOneReg(unsigned char RegAddr);     // 读取温度传感器寄存器
void LSM303_WriteOneReg(unsigned char RegAddr, unsigned char dat); // 写入单个寄存器

/* 初始化/休眠/唤醒 */
unsigned char LSM303DLH_Init(void);                              // 初始化传感器
void LSM303DLH_Sleep(void);                                      // 进入休眠
void LSM303DLH_Wakeup(void);                                     // 从休眠唤醒

/* 传感器数据读取 */
void LSM303_ReadAcceleration(int16_t *Xa, int16_t *Ya, int16_t *Za); // 读取三轴加速度
void LSM303_ReadMagnetic(int16_t *Xm, int16_t *Ym, int16_t *Zm);     // 读取三轴磁场
void LSM303_ReadTemperature(int16_t *Temp);                          // 读取温度

/* 姿态计算 */
int LSM303DLH_CalculationZAxisAngle(int16_t Xa, int16_t Ya, int16_t Za);  // 计算Z轴倾角
int LSM303DLH_CalculationXAxisAngle(int16_t Xa, int16_t Ya, int16_t Za);  // 计算X轴倾角
float Azimuth_Calculate(int16_t Xa, int16_t Ya, int16_t Za, int16_t Xm, int16_t Ym, int16_t Zm); // 计算电子罗盘方位角

#endif /*_LSM303DLH_H_*/
