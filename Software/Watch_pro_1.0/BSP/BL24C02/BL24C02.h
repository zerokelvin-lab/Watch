/** @file BL24C02.h @brief BL24C02 EEPROM存储芯片驱动头文件(256字节I2C EEPROM) */
#ifndef __BL24C02_H
#define __BL24C02_H

#include "stm32f4xx_hal.h"
#include "iic_hal.h"
#include "delay.h"

/** @brief BL24C02 I2C从机地址(7位地址0x50) */
#define BL_ADDRESS	0x50

void BL24C02_Write(uint8_t addr,uint8_t length,uint8_t buff[]);    /** @brief 向EEPROM指定地址写入多字节 */
void BL24C02_Read(uint8_t addr, uint8_t length, uint8_t buff[]);   /** @brief 从EEPROM指定地址读取多字节 */
void BL24C02_Init(void);                                            /** @brief 初始化BL24C02 EEPROM */

#endif
