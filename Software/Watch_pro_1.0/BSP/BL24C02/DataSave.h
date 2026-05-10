/** @file DataSave.h @brief EEPROM数据存储管理头文件，负责用户配置和步数数据的持久化 */
#ifndef __DATASAVE_H__
#define __DATASAVE_H__

#include "BL24C02.h"

void EEPROM_Init(void);                                                  /** @brief EEPROM数据存储初始化 */
uint8_t EEPROM_Check(void);                                              /** @brief 校验EEPROM数据完整性(检查魔数0x55 0xAA) */
uint8_t SettingSave(uint8_t *buf, uint8_t addr, uint8_t lenth);         /** @brief 保存用户设置到EEPROM */
uint8_t SettingGet(uint8_t *buf, uint8_t addr, uint8_t lenth);          /** @brief 从EEPROM读取用户设置 */

#endif
