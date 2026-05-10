/** @file BL24C02.c @brief BL24C02 EEPROM驱动实现，封装I2C读写操作 */
#include "BL24C02.h"

/** @brief BL24C02时钟使能，使用GPIOA时钟 */
#define BL_CLK_ENABLE __HAL_RCC_GPIOA_CLK_ENABLE()

/** @brief BL24C02 I2C总线配置：SDA=PA11, SCL=PA12 */
iic_bus_t BL_bus = 
{
	.IIC_SDA_PORT = GPIOA,       // SDA端口
	.IIC_SCL_PORT = GPIOA,       // SCL端口
	.IIC_SDA_PIN  = GPIO_PIN_11, // SDA引脚 PA11
	.IIC_SCL_PIN  = GPIO_PIN_12, // SCL引脚 PA12
};


/**
  * @brief 向EEPROM指定地址写入多字节数据
  * @param addr EEPROM内部起始地址(0-255)
  * @param length 写入数据长度
  * @param buff[] 写入数据缓冲区
  */
void BL24C02_Write(uint8_t addr,uint8_t length,uint8_t buff[])
{
	IIC_Write_Multi_Byte(&BL_bus, BL_ADDRESS, addr, length, buff);
}


/**
  * @brief 从EEPROM指定地址读取多字节数据
  * @param addr EEPROM内部起始地址(0-255)
  * @param length 读取数据长度
  * @param buff[] 读取数据缓冲区(输出)
  */
void BL24C02_Read(uint8_t addr, uint8_t length, uint8_t buff[])
{
	IIC_Read_Multi_Byte(&BL_bus, BL_ADDRESS, addr, length, buff);
}


/**
  * @brief 初始化BL24C02：使能时钟、初始化I2C总线
  */
void BL24C02_Init(void)
{
	BL_CLK_ENABLE;      // 使能GPIOA时钟
	IICInit(&BL_bus);   // 初始化软件I2C引脚
}
