/** @file em70x8.c @brief EM7028心率血氧传感器驱动实现，包含寄存器读写和初始化配置 */

#include "em70x8.h"

/** @brief EM7028时钟使能，使用GPIOB时钟 */
#define CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE();

/** @brief EM7028 I2C总线配置：SDA=PB13, SCL=PB14（与AHT21共用I2C总线） */
iic_bus_t EM7028_bus = 
{
	.IIC_SDA_PORT = GPIOB,       // SDA端口
	.IIC_SCL_PORT = GPIOB,       // SCL端口
	.IIC_SDA_PIN  = GPIO_PIN_13, // SDA引脚 PB13
	.IIC_SCL_PIN  = GPIO_PIN_14, // SCL引脚 PB14
};

/**
  * @brief 读取EM7028单个寄存器值
  * @param RegAddr 寄存器地址
  * @return 寄存器当前值
  */
uint8_t  EM7028_ReadOneReg(unsigned char RegAddr)
{
	unsigned char dat;
	dat = IIC_Read_One_Byte(&EM7028_bus, EM7028_ADDR, RegAddr); // I2C读单字节
	return dat;
}

/**
  * @brief 写入EM7028单个寄存器值
  * @param RegAddr 寄存器地址
  * @param dat 写入的数据
  */
void  EM7028_WriteOneReg(unsigned char RegAddr, unsigned char dat)
{
	IIC_Write_One_Byte(&EM7028_bus, EM7028_ADDR, RegAddr, dat); // I2C写单字节
}

/**
  * @brief 读取EM7028芯片ID(用于检测芯片是否存在)
  * @return 芯片ID(正确值应为0x36)
  */
uint8_t EM7028_Get_ID()
{
	return EM7028_ReadOneReg(ID_REG);
}

/**
  * @brief 初始化EM7028心率传感器
  * @return 0=成功, 1=芯片识别失败
  * @note 配置HRS1为主通道(红光+红外)，16位分辨率，2.62144MHz采样频率
  */
uint8_t EM7028_hrs_init()
{
	uint8_t i = 5;  // 重试次数
	
	CLK_ENABLE;          // 使能GPIOB时钟
	IICInit(&EM7028_bus);// 初始化软件I2C引脚
	
	// 检测芯片ID，最多重试5次
	while(EM7028_Get_ID() != 0x36 && i)
	{
		HAL_Delay(100);
		i--;
	}
	if(!i)
	{return 1;}  // 芯片未识别
	
	// 配置HRS_CFG寄存器(0x01)=0x00: 关闭HRS2，准备配置HRS1
	EM7028_WriteOneReg(HRS_CFG,0x00);				
	// HRS1_EN=1, HRS2_dis: 使能HRS1通道,关闭HRS2辅助通道
	// Heart Beat Measurement is enabled with LED1 turned on and only Red Light Sensor and IR sensor enabled. 
	// 心率测量使能，仅开启红光传感器和红外传感器
	// When LED1 turned on, the result stores to HRS_DATA0;
	// LED1开启时，结果存入HRS1_DATA0寄存器
	
	// HRS2数据偏移=0x00，无偏移
	EM7028_WriteOneReg(HRS2_DATA_OFFSET, 0x00);
	//0 offset
	
	// HRS2增益控制=0x7F, 增益=1
	EM7028_WriteOneReg(HRS2_GAIN_CTRL, 0x7f);		
	//HRS2 GAIN = 1
	
	// HRS1控制寄存器(0x0D)=0x47: 增益=1, 量程=8, 频率=2.62144MHz, 16位分辨率
	EM7028_WriteOneReg(HRS1_CTRL, 0x47);
	// HRS1 GAIN=1(bits[7:6]=01), RANGE=8(bits[5:4]=00)
	// FREQ=2.62144MHz(bits[3:2]=01, 1.5625ms采样周期)
	// RES=16bits(bits[1:0]=11), HRS1 mode
	
	// 中断控制寄存器(0x0E)=0x00: LED编程电流=2.5mA
	EM7028_WriteOneReg(INT_CTRL, 0x00);
	//LED programmed current = 2.5mA
	
	return 0;
}

/**
  * @brief 使能心率传感器(HRS1开启，LED点亮)
  * @return 0=成功, 1=芯片识别失败
  */
uint8_t EM7028_hrs_Enable()
{
	uint8_t i = 5;
	// 检测芯片ID
	while(EM7028_Get_ID() != 0x36 && i)
	{
		HAL_Delay(100);
		i--;
	}
	if(!i)
	{return 1;}
	// 配置HRS_CFG=0x08: bit3=1, 使能HRS1
	EM7028_WriteOneReg(HRS_CFG,0x08);
	return 0;
}

/**
  * @brief 关闭心率传感器(HRS1关闭)
  * @return 0=成功, 1=芯片识别失败
  */
uint8_t EM7028_hrs_DisEnable()
{
	uint8_t i = 5;
	// 检测芯片ID
	while(EM7028_Get_ID() != 0x36 && i)
	{
		HAL_Delay(100);
		i--;
	}
	if(!i)
	{return 1;}
	// 配置HRS_CFG=0x00: 关闭HRS1
	EM7028_WriteOneReg(HRS_CFG,0x00);
	return 0;
}

/**
  * @brief 读取HRS1通道0红光传感器16位原始数据
  * @return 16位PPG原始数据(高字节在前)
  */
uint16_t EM7028_Get_HRS1(void)
{
	uint16_t dat;
	dat = EM7028_ReadOneReg(HRS1_DATA0_H);  // 读取高字节
	dat <<= 8;                               // 左移8位
	dat |= EM7028_ReadOneReg(HRS1_DATA0_L); // 拼接低字节
	return dat;
}
