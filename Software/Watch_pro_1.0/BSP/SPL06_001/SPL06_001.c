/** @file SPL06_001.c @brief SPL06-001气压传感器驱动 - I2C通信、校准系数读取、温度/气压/海拔计算 */

#include <math.h>
#include "SPL06_001.h"

/* ====== GPIO时钟使能与I2C总线配置 ====== */
#define SPL_CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE()
iic_bus_t SPL_bus = 
{
	.IIC_SDA_PORT = GPIOB,                         // SDA数据线端口
	.IIC_SCL_PORT = GPIOB,                         // SCL时钟线端口
	.IIC_SDA_PIN  = GPIO_PIN_13,                   // SDA引脚：PB13
	.IIC_SCL_PIN  = GPIO_PIN_14,                   // SCL引脚：PB14
};

/* ====== 校准系数全局变量 ====== */
int16_t c0,c1,c01,c11,c20,c21,c30;                 // 16位有符号校准系数
int32_t c00,c10;                                   // 32位有符号校准系数

/* ====== I2C底层读写函数 ====== */

/**
 * @brief  读取SPL06-001单个寄存器
 * @param  addr 寄存器地址
 * @retval 读取到的寄存器值
 */
uint8_t SPL_ReadOneReg(uint8_t addr)
{
	uint8_t dat;
	dat = IIC_Read_One_Byte(&SPL_bus, SPL_CHIP_ADDRESS,addr);
	return dat;
}

/**
 * @brief  向SPL06-001单个寄存器写入数据
 * @param  addr 寄存器地址
 * @param  dat 要写入的8位数据
 * @retval 无
 */
void SPL_WriteOneReg(uint8_t addr, uint8_t dat)
{
	IIC_Write_One_Byte(&SPL_bus, SPL_CHIP_ADDRESS,addr,dat);
}

/* ====== 原始数据读取 ====== */

/**
 * @brief  获取原始温度值（24位有符号数据）
 * @retval 原始温度值（24位二进制补码格式）
 * @note   读取3字节温度数据(TMP_B2/TMP_B1/TMP_B0)，合成24位有符号整数
 */
int32_t Get_Traw()
{
	uint8_t buff[3];
	int32_t Traw;
	buff[0] = SPL_ReadOneReg(SPL_TMP_B0);           // 温度低字节
	buff[1] = SPL_ReadOneReg(SPL_TMP_B1);           // 温度中间字节
	buff[2] = SPL_ReadOneReg(SPL_TMP_B2);           // 温度高字节
	Traw = buff[2];
	Traw = Traw << 8 | buff[1];                      // 合并高字节和中间字节
	Traw = Traw << 8 | buff[0];                      // 合并低字节
	if(Traw & (1<<23))                               // 检查符号位（bit23=1表示负数）
	{Traw |= 0xFF000000;}// 24 bit 2's complement numbers（高位填充1以扩展为32位有符号数）
	return Traw;
}

/**
 * @brief  获取原始气压值（24位有符号数据）
 * @retval 原始气压值（24位二进制补码格式）
 */
int32_t Get_Praw()
{
	uint8_t buff[3];
	int32_t Praw;
	buff[0] = SPL_ReadOneReg(SPL_PRS_B0);           // 气压低字节
	buff[1] = SPL_ReadOneReg(SPL_PRS_B1);           // 气压中间字节
	buff[2] = SPL_ReadOneReg(SPL_PRS_B2);           // 气压高字节
	Praw = buff[2];
	Praw = Praw << 8 | buff[1];
	Praw = Praw << 8 | buff[0];
	if(Praw & (1<<23))                               // 检查符号位
	{Praw |= 0xFF000000;}// 24 bit 2's complement numbers
	return Praw;
}

/* ====== 校准系数读取函数 ====== */

/**
 * @brief  读取校准系数c0（12位有符号，横跨COEF_C0和COEF_C0_C1两个寄存器）
 * @retval 校准系数c0（16位有符号）
 */
int16_t get_c0()
{
	uint8_t buff[2];
	int16_t c0;
	buff[0] = SPL_ReadOneReg(COEF_C0);              // c0高8位
	buff[1] = SPL_ReadOneReg(COEF_C0_C1);           // c0低4位（在该寄存器高4位）+ c1高4位（在该寄存器低4位）
	c0 = buff[0];
	c0 = (c0 << 4) | (buff[1] >> 4);                // 拼接：高8位<<4 | 低4位（取buff[1]高4位）
	if(c0 & (1<<11))                                 // 12位有符号数符号扩展
	{c0 |= 0xF000;}
	return c0;
}

/**
 * @brief  读取校准系数c1（12位有符号）
 * @retval 校准系数c1（16位有符号）
 */
int16_t get_c1()
{
	uint8_t buff[2];
	int16_t c1;
	buff[0] = SPL_ReadOneReg(COEF_C0_C1);           // c1高4位（在该寄存器低4位）
	buff[1] = SPL_ReadOneReg(COEF_C1);              // c1低8位
	c1 = buff[0] & 0x0F;                             // 取低4位作为c1高4位
	c1 = (c1 << 8) | buff[1] ;                       // 拼接低8位
	if(c1 & (1<<11))                                 // 符号扩展
	{c1 |= 0xF000;}
	return c1;
}

/**
 * @brief  读取校准系数c00（20位有符号）
 * @retval 校准系数c00（32位有符号）
 */
int32_t get_c00()
{
	uint8_t buff[3];
	int32_t c00;
	buff[0] = SPL_ReadOneReg(COEF_C00_H);           // c00高8位
	buff[1] = SPL_ReadOneReg(COEF_C00_L);           // c00中间8位
	buff[2] = SPL_ReadOneReg(COEF_C00_C10);         // c00低4位（在该寄存器高4位）+ c10高4位
	c00 = buff[0];
	c00 = c00<<8 | buff[1];                          // 合并高8位和中间8位
	c00 = (c00<<4) | (buff[2]>>4);                   // 合并低4位（取buff[2]高4位）
	if(c00 & (1<<19))                                // 20位有符号数符号扩展
	{c00 |= 0xFFF00000;}
	return c00;
}

/**
 * @brief  读取校准系数c10（20位有符号）
 * @retval 校准系数c10（32位有符号）
 */
int32_t get_c10()
{
	uint8_t buff[3];
	int32_t c10;
	buff[0] = SPL_ReadOneReg(COEF_C00_C10);         // c10高4位（在该寄存器低4位）
	buff[1] = SPL_ReadOneReg(COEF_C10_M);           // c10中间8位
	buff[2] = SPL_ReadOneReg(COEF_C10_L);           // c10低8位
	c10 = buff[0] & 0x0F;                            // 取低4位作为c10高4位
	c10 = c10<<8 | buff[1];
	c10 = c10<<8 | buff[2];
	if(c10 & (1<<19))                                // 符号扩展
	{c10 |= 0xFFF00000;}
	return c10;
}

/** @brief 读取校准系数c01（16位有符号） */
int16_t get_c01()
{
	uint8_t buff[2];
	int32_t c01;
	buff[0] = SPL_ReadOneReg(COEF_C01_H);
	buff[1] = SPL_ReadOneReg(COEF_C01_L);
	c01 = buff[0];
	c01 = c01<<8 | buff[1];
	return c01;
}

/** @brief 读取校准系数c11（16位有符号） */
int16_t get_c11()
{
	uint8_t buff[2];
	int32_t c11;
	buff[0] = SPL_ReadOneReg(COEF_C11_H);
	buff[1] = SPL_ReadOneReg(COEF_C11_L);
	c11 = buff[0];
	c11 = c11<<8 | buff[1];
	return c11;
}

/** @brief 读取校准系数c20（16位有符号） */
int16_t get_c20()
{
	uint8_t buff[2];
	int32_t c20;
	buff[0] = SPL_ReadOneReg(COEF_C20_H);
	buff[1] = SPL_ReadOneReg(COEF_C20_L);
	c20 = buff[0];
	c20 = c20<<8 | buff[1];
	return c20;
}

/** @brief 读取校准系数c21（16位有符号） */
int16_t get_c21()
{
	uint8_t buff[2];
	int32_t c21;
	buff[0] = SPL_ReadOneReg(COEF_C21_H);
	buff[1] = SPL_ReadOneReg(COEF_C21_L);
	c21 = buff[0];
	c21 = c21<<8 | buff[1];
	return c21;
}

/** @brief 读取校准系数c30（16位有符号） */
int16_t get_c30()
{
	uint8_t buff[2];
	int32_t c30;
	buff[0] = SPL_ReadOneReg(COEF_C30_H);
	buff[1] = SPL_ReadOneReg(COEF_C30_L);
	c30 = buff[0];
	c30 = c30<<8 | buff[1];
	return c30;
}

/* ====== SPL06-001 初始化 ====== */

/**
 * @brief  初始化SPL06-001气压传感器
 * @retval 0=成功, 1=失败
 * @note   配置流程：
 *         1. 使能时钟并初始化I2C总线
 *         2. 压力传感器: 2倍过采样
 *         3. 温度传感器: 外部温度1倍过采样
 *         4. 连续压力和温度测量模式
 *         5. 读取所有校准系数
 *         6. 验证配置寄存器写入是否正确
 */
uint8_t SPL_init()
{
	
	SPL_CLK_ENABLE;                                  // 使能GPIOB时钟
	IICInit(&SPL_bus);                               // 初始化I2C总线
	
	SPL_WriteOneReg(SPL_PRS_CFG, 0x01);		// 气压传感器: 2倍过采样（提高精度）
	// 0x01 = PM_RATE=1(1Hz) | PM_PRC=1(2倍过采样)

	SPL_WriteOneReg(SPL_TMP_CFG, 0x80);		// 温度传感器: 外部传感器模式, 1倍过采样
	// 0x80 = bit7=TMP_EXT(1=外部温度传感器) | TMP_RATE=0(1Hz) | TMP_PRC=0(单次)

	SPL_WriteOneReg(SPL_MEAS_CFG, 0x07);	// 连续压力和温度测量模式
	// 0x07 = bit2=压力测量使能 | bit1=温度测量使能 | bit0=后台模式

	SPL_WriteOneReg(SPL_CFG_REG, 0x00);		// 配置寄存器默认值（中断关闭，FIFO关闭）
	
	// 读取所有出厂校准系数（每个芯片独立校准，用于温度补偿和压力换算）
	c0 = get_c0();
	c1 = get_c1();
	c01 = get_c01();
	c11 = get_c11(); 
	c20 = get_c20();
	c21 = get_c21();
	c30 = get_c30();
	c00 = get_c00();
	c10 = get_c10();
	
	// 验证配置是否正确写入（回读检查）
	if(SPL_ReadOneReg(SPL_PRS_CFG)!=0x01 || SPL_ReadOneReg(SPL_CFG_REG)!=0x00)
	{return 1;}//ERRO（配置验证失败）
	else 
	{return 0;}//SUCCESS
}

/* ====== 休眠/唤醒 ====== */

/**
 * @brief  SPL06-001进入待机模式
 * @retval 无
 */
void SPL_Sleep()
{
	SPL_WriteOneReg(SPL_MEAS_CFG, 0x00);	// 待机模式：停止所有测量
	//SPL_WriteOneReg(SPL_RESET_REG,0x09);// 软件复位（备选方案）
}

/**
 * @brief  SPL06-001从待机唤醒
 * @retval 无
 */
void SPL_Wakeup()
{
	SPL_WriteOneReg(SPL_MEAS_CFG, 0x07);	// 恢复连续压力和温度测量
	//SPL_init();                                   // 重新初始化（备选方案）
}

/** @brief 读取SPL06-001芯片ID */
uint8_t SPL_GetID()
{
	return SPL_ReadOneReg(SPL_ID_REG);
}

/* ====== 温度/气压/海拔计算 ====== */

/**
 * @brief  计算温度值
 * @retval 温度值（摄氏度）
 * @note   温度换算公式: Tcomp = c0 * 0.5 + c1 * (Traw / kT)
 *         kT = 524288.0 (温度换算因子)
 */
float Temperature_Calculate()
{
	float Traw_sc, Tcomp;
	Traw_sc = Get_Traw();                            // 获取原始温度值
	Traw_sc /= KT;                                   // 除以温度换算因子
	Tcomp = c0 * 0.5 + c1 * Traw_sc;                 // 应用校准系数计算温度
	return Tcomp;
}

/**
 * @brief  计算气压值
 * @retval 气压值（单位：帕斯卡 Pa）
 * @note   气压换算公式（三阶多项式拟合）:
 *         Pcomp = c00 + Praw_sc*(c10 + Praw_sc*(c20 + Praw_sc*c30))
 *                + Traw_sc*c01 + Traw_sc*Praw_sc*(c11 + Praw_sc*c21)
 *         kP = 1572864.0 (压力换算因子)
 */
float Pressure_Calculate()
{
	float Traw_sc, Praw_sc, Pcomp;
	Traw_sc = Get_Traw();                            // 获取原始温度值
	Traw_sc /= KT;                                   // 温度归一化
	Praw_sc = Get_Praw();                            // 获取原始压力值
	Praw_sc /= KP;                                   // 压力归一化
	// 三阶温度补偿压力计算公式
	Pcomp = (c00) + Praw_sc * ((c10) + Praw_sc * ((c20) + Praw_sc * (c30))) + Traw_sc * (c01) + Traw_sc * Praw_sc * ((c11) + Praw_sc * (c21));
	 
	return Pcomp;
}

/**
 * @brief  根据气压计算海拔高度
 * @retval 海拔高度（单位：米）
 * @note   国际标准大气压公式: Altitude = 44330 * (1 - (P/101325)^0.1903)
 *         101325 Pa = 标准海平面气压
 */
float Altitude_Calculate()
{
	float Altitude;
	Altitude = 44330 * (1 - powf(Pressure_Calculate()/101325, 0.1903)); // 气压-海拔转换公式
	return Altitude;
}
