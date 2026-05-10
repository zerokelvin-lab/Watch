/** @file mpu6050.h @brief MPU6050六轴陀螺仪驱动头文件 - 寄存器定义、I2C地址、API声明 */

#ifndef __MPU6050_H
#define __MPU6050_H

#include "iic_hal.h"   												  	  
#include "sys.h"
#include "math.h"
#include "delay.h"

/* ====== MPU6050 寄存器定义 ====== */

//#define MPU_ACCEL_OFFS_REG		0X06	//加速度偏移寄存器，可读取版本信息，寄存器手册未提到
//#define MPU_PROD_ID_REG				0X0C	//产品ID寄存器, 在寄存器手册未提到
#define MPU_SELF_TESTX_REG		0X0D	//自检寄存器X（陀螺仪自检）
#define MPU_SELF_TESTY_REG		0X0E	//自检寄存器Y
#define MPU_SELF_TESTZ_REG		0X0F	//自检寄存器Z
#define MPU_SELF_TESTA_REG		0X10	//自检寄存器A（加速度计自检）
#define MPU_SAMPLE_RATE_REG		0X19	//采样频率分频器 SMPLRT_DIV
#define MPU_CFG_REG						0X1A	//配置寄存器 CONFIG（DLPF设置，EXT_SYNC）
#define MPU_GYRO_CFG_REG			0X1B	//陀螺仪配置寄存器 GYRO_CONFIG（量程FS_SEL）
#define MPU_ACCEL_CFG_REG			0X1C	//加速度计配置寄存器 ACCEL_CONFIG（量程AFS_SEL）
#define MPU_MOTION_DET_REG		0X1F	//运动检测阈值配置寄存器 MOT_THR
#define MPU_MOTION_DUR_REG		0X20	//运动检测持续时间 MOT_DUR
#define MPU_FIFO_EN_REG				0X23	//FIFO使能寄存器 FIFO_EN
#define MPU_I2CMST_CTRL_REG		0X24	//I2C主机控制寄存器 I2C_MST_CTRL
#define MPU_I2CSLV0_ADDR_REG	0X25	//I2C从机0设备地址寄存器 I2C_SLV0_ADDR
#define MPU_I2CSLV0_REG				0X26	//I2C从机0数据地址寄存器 I2C_SLV0_REG
#define MPU_I2CSLV0_CTRL_REG	0X27	//I2C从机0控制寄存器 I2C_SLV0_CTRL
#define MPU_I2CSLV1_ADDR_REG	0X28	//I2C从机1设备地址寄存器
#define MPU_I2CSLV1_REG				0X29	//I2C从机1数据地址寄存器
#define MPU_I2CSLV1_CTRL_REG	0X2A	//I2C从机1控制寄存器
#define MPU_I2CSLV2_ADDR_REG	0X2B	//I2C从机2设备地址寄存器
#define MPU_I2CSLV2_REG				0X2C	//I2C从机2数据地址寄存器
#define MPU_I2CSLV2_CTRL_REG	0X2D	//I2C从机2控制寄存器
#define MPU_I2CSLV3_ADDR_REG	0X2E	//I2C从机3设备地址寄存器
#define MPU_I2CSLV3_REG				0X2F	//I2C从机3数据地址寄存器
#define MPU_I2CSLV3_CTRL_REG	0X30	//I2C从机3控制寄存器
#define MPU_I2CSLV4_ADDR_REG	0X31	//I2C从机4设备地址寄存器
#define MPU_I2CSLV4_REG				0X32	//I2C从机4数据地址寄存器
#define MPU_I2CSLV4_DO_REG		0X33	//I2C从机4写数据寄存器
#define MPU_I2CSLV4_CTRL_REG	0X34	//I2C从机4控制寄存器
#define MPU_I2CSLV4_DI_REG		0X35	//I2C从机4读数据寄存器

#define MPU_I2CMST_STA_REG		0X36	//I2C主机状态寄存器 I2C_MST_STATUS
#define MPU_INTBP_CFG_REG			0X37	//中断/旁路配置寄存器 INT_PIN_CFG
#define MPU_INT_EN_REG				0X38	//中断使能寄存器 INT_ENABLE
#define MPU_INT_STA_REG				0X3A	//中断状态寄存器 INT_STATUS

/* ----- 加速度计输出寄存器（X/Y/Z各占2字节，高字节在前） ----- */
#define MPU_ACCEL_XOUTH_REG		0X3B	//加速度值 X轴高8位寄存器
#define MPU_ACCEL_XOUTL_REG		0X3C	//加速度值 X轴低8位寄存器
#define MPU_ACCEL_YOUTH_REG		0X3D	//加速度值 Y轴高8位寄存器
#define MPU_ACCEL_YOUTL_REG		0X3E	//加速度值 Y轴低8位寄存器
#define MPU_ACCEL_ZOUTH_REG		0X3F	//加速度值 Z轴高8位寄存器
#define MPU_ACCEL_ZOUTL_REG		0X40	//加速度值 Z轴低8位寄存器

/* ----- 温度输出寄存器 ----- */
#define MPU_TEMP_OUTH_REG			0X41	//温度值高8位寄存器
#define MPU_TEMP_OUTL_REG			0X42	//温度值低8位寄存器

/* ----- 陀螺仪输出寄存器（X/Y/Z各占2字节，高字节在前） ----- */
#define MPU_GYRO_XOUTH_REG		0X43	//角速度值 X轴高8位寄存器
#define MPU_GYRO_XOUTL_REG		0X44	//角速度值 X轴低8位寄存器
#define MPU_GYRO_YOUTH_REG		0X45	//角速度值 Y轴高8位寄存器
#define MPU_GYRO_YOUTL_REG		0X46	//角速度值 Y轴低8位寄存器
#define MPU_GYRO_ZOUTH_REG		0X47	//角速度值 Z轴高8位寄存器
#define MPU_GYRO_ZOUTL_REG		0X48	//角速度值 Z轴低8位寄存器

#define MPU_MOT_DET_STA_REG 	0x61	//运动检测状态寄存器

/* ----- I2C从机输出寄存器 ----- */
#define MPU_I2CSLV0_DO_REG		0X63	//I2C从机0读数据寄存器
#define MPU_I2CSLV1_DO_REG		0X64	//I2C从机1读数据寄存器
#define MPU_I2CSLV2_DO_REG		0X65	//I2C从机2读数据寄存器
#define MPU_I2CSLV3_DO_REG		0X66	//I2C从机3读数据寄存器

/* ----- 电源管理与控制寄存器 ----- */
#define MPU_I2CMST_DELAY_REG	0X67	//I2C主机延迟控制寄存器
#define MPU_SIGPATH_RST_REG		0X68	//信号通道复位寄存器
#define MPU_MDETECT_CTRL_REG	0X69	//运动检测控制寄存器
#define MPU_USER_CTRL_REG			0X6A	//用户控制寄存器 USER_CTRL
#define MPU_PWR_MGMT1_REG			0X6B	//电源管理寄存器1 PWR_MGMT_1
#define MPU_PWR_MGMT2_REG			0X6C	//电源管理寄存器2 PWR_MGMT_2
#define MPU_FIFO_CNTH_REG			0X72	//FIFO计数寄存器高8位
#define MPU_FIFO_CNTL_REG			0X73	//FIFO计数寄存器低8位
#define MPU_FIFO_RW_REG				0X74	//FIFO读写寄存器 FIFO_R_W
#define MPU_DEVICE_ID_REG			0X75	//设备ID寄存器 WHO_AM_I（默认值0x68）

/* ====== 设备I2C地址 ====== */
// 当AD0引脚(9脚)接地时，I2C地址为0x68（不包含最低位）
// 当AD0接V3.3时，I2C地址为0x69（不包含最低位）
#define MPU_ADDR				0X68         // MPU6050的7位I2C地址
#define MPU_ID					0x68         // 期望的设备ID值

////由于AD0接地，读写地址分别为0xD1和0xD0（如接VCC则为0xD3和0xD2）
//#define MPU_READ    0XD1
//#define MPU_WRITE   0XD0

/* ====== MPU6050 API 函数声明 ====== */

/* 初始化和配置 */
void MPU_INT_Pin_Init(void);                        // 初始化中断引脚
void MPU_Motion_Init(void);                         // 初始化运动检测
void MPU_Bus_Init(void);                            // 初始化I2C总线
u8 MPU_Init(void); 															// 初始化MPU6050

/* I2C底层读写函数 */
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf);    // I2C连续写
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf);     // I2C连续读
u8 MPU_Write_Byte(u8 reg,u8 data);							// I2C写一个字节
u8 MPU_Read_Byte(u8 reg);												// I2C读一个字节

/* 传感器参数配置 */
u8 MPU_Set_Gyro_Fsr(u8 fsr);                        // 设置陀螺仪量程
u8 MPU_Set_Accel_Fsr(u8 fsr);                       // 设置加速度计量程
u8 MPU_Set_LPF(u16 lpf);                            // 设置低通滤波器
u8 MPU_Set_Rate(u16 rate);                          // 设置采样率
u8 MPU_Set_Fifo(u8 sens);                           // 设置FIFO

/* 批量读写 */
uint8_t MPU_Read_Multi_Byte(uint8_t addr, uint8_t length, uint8_t buff[]);  // I2C批量读
uint8_t MPU_Write_Multi_Byte(uint8_t addr,uint8_t length,uint8_t buff[]);   // I2C批量写

/* 休眠/唤醒 */
void MPU_Sleep(void);                               // 进入休眠
void MPU_Wakeup(void);                              // 从休眠唤醒
uint8_t MPU_Read_Status(void);                      // 读取中断状态

/* 传感器数据读取 */
short MPU_Get_Temperature(void);                    // 获取温度值
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz); // 获取陀螺仪三轴数据
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az); // 获取加速度计三轴数据

/* 姿态计算 */
void MPU_Get_Angles(float * roll,float * pitch);    // 获取横滚角和俯仰角
uint8_t MPU_isHorizontal(void);                     // 检测是否处于水平状态

#endif
