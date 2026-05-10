/** @file iic_hal.h @brief 软件I2C硬件抽象层头文件，定义I2C总线结构和操作接口 */
#ifndef __IIC_HAL_H
#define __IIC_HAL_H

#include "stm32f4xx_hal.h"

/** @brief 软件I2C总线结构体，支持多路I2C复用 */
typedef struct
{
	GPIO_TypeDef * IIC_SDA_PORT;  // SDA数据线GPIO端口
	GPIO_TypeDef * IIC_SCL_PORT;  // SCL时钟线GPIO端口
	uint16_t IIC_SDA_PIN;         // SDA数据线引脚号
	uint16_t IIC_SCL_PIN;         // SCL时钟线引脚号
	//void (*CLK_ENABLE)(void);   // GPIO时钟使能回调(预留)
}iic_bus_t;

/* ====== I2C底层时序函数 ====== */
void IICStart(iic_bus_t *bus);                              /** @brief 产生I2C起始信号 */
void IICStop(iic_bus_t *bus);                               /** @brief 产生I2C停止信号 */
unsigned char IICWaitAck(iic_bus_t *bus);                    /** @brief 等待从机应答信号 @return SUCCESS/ERROR */
void IICSendAck(iic_bus_t *bus);                            /** @brief 发送应答信号(ACK) */
void IICSendNotAck(iic_bus_t *bus);                         /** @brief 发送非应答信号(NACK) */
void IICSendByte(iic_bus_t *bus, unsigned char cSendByte);  /** @brief 发送一个字节 @param cSendByte 待发送的字节 */
unsigned char IICReceiveByte(iic_bus_t *bus);               /** @brief 接收一个字节 @return 接收到的字节 */
void IICInit(iic_bus_t *bus);                               /** @brief 初始化I2C总线GPIO引脚 */

/* ====== I2C组合读写函数 ====== */
uint8_t IIC_Write_One_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg,uint8_t data);               /** @brief 向指定设备寄存器写入1个字节 */
uint8_t IIC_Write_Multi_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg,uint8_t length,uint8_t buff[]); /** @brief 向指定设备寄存器写入多个字节 */
unsigned char IIC_Read_One_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg);                       /** @brief 从指定设备寄存器读取1个字节 */
uint8_t IIC_Read_Multi_Byte(iic_bus_t *bus, uint8_t daddr, uint8_t reg, uint8_t length, uint8_t buff[]); /** @brief 从指定设备寄存器读取多个字节 */
#endif
