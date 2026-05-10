/** @file DataSave.c @brief EEPROM数据存储管理实现，包含完整性校验和设置读写 */
#include "DataSave.h"
#include "string.h"

/******************************************
EEPROM Data description（EEPROM数据布局说明）:
[0x00]:0x55 for check    // 校验魔数1
[0x01]:0xAA for check    // 校验魔数2

[0x10]:user wrist setting, HWInterface.IMU.wrist_is_enabled  // 用户手腕佩戴设置
[0x11]:user ui_APPSy_EN setting  // 用户UI应用同步使能设置

[0x20]:Last Save Day(0-31)   // 上次保存日期(日)
[0x21]:Day Steps              // 当日步数

*******************************************/

/**
  * @brief EEPROM初始化入口
  */
void EEPROM_Init(void)
{
	BL24C02_Init();
}


/**
  * @brief 校验EEPROM数据是否有效且EEPROM工作正常
  * @return 0=校验通过, 1=校验失败
  * @note 读取地址0x00-0x01检查魔数0x55 0xAA，若不一致则尝试写入后再次校验
  */
//to check the Data is right and the EEPROM is working
uint8_t EEPROM_Check(void)
{
	uint8_t check_buff[2];
	delay_ms(10);
	BL24C02_Read(0,2,check_buff);          // 读取校验魔数
	if(check_buff[0] == 0x55 && check_buff[1] == 0xAA)  // 校验魔数正确
	{
		return 0;//check OK
	}
	else
	{
		// EEPROM可能未初始化，写入魔数并重新校验
		check_buff[0] = 0x55;
		check_buff[1] = 0xAA;
		delay_ms(10);
		BL24C02_Write(0,2,check_buff);    // 写入校验魔数
		memset(check_buff,0,2);            // 清空缓冲区
		delay_ms(10);
		BL24C02_Read(0,2,check_buff);     // 重新读取验证
		if(check_buff[0] == 0x55 && check_buff[1] == 0xAA)
			return 0;//check ok
	}
	return 1;//check erro  // 校验错误,EEPROM可能硬件故障
}


/**
  * @brief 保存用户设置数据到EEPROM
  * @param buf 设置数据缓冲区指针
  * @param addr EEPROM写入地址(必须>1, 地址0-1为校验保留)
  * @param lenth 数据长度
  * @return 0=保存成功, 1=保存失败
  */
//to Save the settings
uint8_t SettingSave(uint8_t *buf, uint8_t addr, uint8_t lenth)
{
	if(addr > 1 && !EEPROM_Check())  // 地址合法性检查 & EEPROM校验
	{
		delay_ms(10);
		BL24C02_Write(addr,lenth,buf);
		return 0;
	}
	return 1;
}


/**
  * @brief 从EEPROM读取用户设置数据
  * @param buf 读取数据缓冲区指针(输出)
  * @param addr EEPROM读取地址(必须>1, 地址0-1为校验保留)
  * @param lenth 数据长度
  * @return 0=读取成功, 1=读取失败
  */
//to Get the settings
uint8_t SettingGet(uint8_t *buf, uint8_t addr, uint8_t lenth)
{
	if(addr > 1 && !EEPROM_Check())  // 地址合法性检查 & EEPROM校验
	{
		delay_ms(10);
		BL24C02_Read(addr,lenth,buf);
		return 0;
	}
	return 1;
}
