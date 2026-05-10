/** @file LSM303.c @brief LSM303DLH加速度计/磁力计驱动 - I2C读写、初始化、三轴加速度/磁场/温度读取、角度计算 */

#include "LSM303.h"
#include <math.h>

/* ====== 常量定义 ====== */
#define PI 3.1415926                             // 圆周率

/** @brief 毫秒延时宏（HAL库延时） */
#define Delayms(X) HAL_Delay(X)

/** @brief LSM303 I2C总线时钟使能（GPIOB） */
#define LSM303_CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE()

/* ====== LSM303 I2C总线配置 ====== */
iic_bus_t LSM303_bus = 
{
	.IIC_SDA_PORT = GPIOB,                      // SDA数据线端口
	.IIC_SCL_PORT = GPIOB,                      // SCL时钟线端口
	.IIC_SDA_PIN  = GPIO_PIN_13,                // SDA引脚：PB13
	.IIC_SCL_PIN  = GPIO_PIN_14,                // SCL引脚：PB14
};

/* ====== 注意：LSM303不支持连续多字节读取，需逐字节读取 ====== */

/* ====== I2C寄存器读写函数 ====== */

/**
 * @brief  读取LSM303单个寄存器
 * @param  RegAddr 寄存器地址
 * @retval 读取到的寄存器值
 * @note   根据寄存器地址自动判断使用加速度计(0x19)还是磁力计(0x1E)从机地址
 */
unsigned char LSM303_ReadOneReg(unsigned char RegAddr)
{
	unsigned char dat;
	unsigned char SlaveAddr = (RegAddr>0x19)?LSM303_SlaveAddr_A:LSM303_SlaveAddr_M; // 判断从机地址
	dat = IIC_Read_One_Byte(&LSM303_bus, SlaveAddr,RegAddr);
	return dat;
}

/**
 * @brief  读取LSM303多个寄存器（伪连续读取）
 * @param  RegAddr 起始寄存器地址
 * @param  RegNum 要读取的寄存器数量
 * @param  DataBuff 存储读取数据的缓冲区
 * @retval 无
 * @note   LSM303硬件不支持真正的连续读取，因此循环调用单字节读取函数
 */
void LSM303_ReadMultiReg(unsigned char RegAddr, unsigned char RegNum, unsigned char DataBuff[])
{
	unsigned char i;
	unsigned char SlaveAddr = (RegAddr>0x19)?LSM303_SlaveAddr_A:LSM303_SlaveAddr_M;
	//IIC_Read_Multi_Byte(SlaveAddr,RegAddr,RegNum,DataBuff);  // 真正的连续读取（不可用）
	for(i=0;i<RegNum;i++)
	{
		DataBuff[i] = IIC_Read_One_Byte(&LSM303_bus, SlaveAddr,RegAddr+i); // 逐字节读取
	}
}

/**
 * @brief  读取LSM303温度传感器单个寄存器
 * @param  RegAddr 寄存器地址
 * @retval 读取到的寄存器值
 * @note   温度传感器使用磁力计的从机地址(0x1E)
 */
unsigned char LSM303_Temp_ReadOneReg(unsigned char RegAddr)
{
	unsigned char dat;
	dat = IIC_Read_One_Byte(&LSM303_bus, LSM303_SlaveAddr_M,RegAddr);
	return dat;
}

/**
 * @brief  向LSM303单个寄存器写入数据
 * @param  RegAddr 寄存器地址
 * @param  dat 要写入的8位数据
 * @retval 无
 */
void LSM303_WriteOneReg(unsigned char RegAddr, unsigned char dat)
{
	unsigned char SlaveAddr = (RegAddr>0x19)?LSM303_SlaveAddr_A:LSM303_SlaveAddr_M;
	IIC_Write_One_Byte(&LSM303_bus, SlaveAddr,RegAddr,dat);
}

/* ====== LSM303DLH 初始化 ====== */

/**
 * @brief  初始化LSM303DLH传感器（加速度计+磁力计）
 * @retval 0=成功, 1=失败
 * @note   加速度计配置：±4g量程，低功耗模式，10Hz数据速率
 *         磁力计配置：15Hz数据速率，±4.0Gauss增益，连续转换模式
 *         CTRL_REG4 FS[5:4]位：00=±2g, 01=±4g, 10=±8g, 11=±16g
 */
unsigned char LSM303DLH_Init()
{
	unsigned char temp;
	unsigned char retry = 0;
	
	LSM303_CLK_ENABLE;                           // 使能GPIOB时钟
	IICInit(&LSM303_bus);                        // 初始化I2C总线
	
	// 加速度计初始化（最多重试3次）
	for(retry = 0;retry < 3;retry ++)
	{
		LSM303_WriteOneReg(LSM303_CTRL_REG4_A, 0x10);		// 加速度计满量程: ±4g (FS[5:4]=01)
		Delayms(1);
		LSM303_WriteOneReg(LSM303_CTRL_REG1_A, 0x2F);		// 低功耗模式, 10Hz数据速率, XYZ三轴使能
		Delayms(1);															
		temp = LSM303_ReadOneReg(LSM303_CTRL_REG1_A);		// 读取0x20寄存器进行验证，默认值为0x07
		if(temp != 0x2F)	
		{
			Delayms(10);	
		}
		else break;
	}
	if(temp != 0x2F)
	{
		return 1;                                  // 加速度计初始化失败
	}
	Delayms(1);

	// 磁力计初始化（最多重试3次）
	for(retry = 0;retry < 3;retry ++)
	{
		LSM303_WriteOneReg(LSM303_CRA_REG_M, 0x10);		// 磁场传感器15Hz数据速率, 正常测量模式; 关闭温度计
		Delayms(1);		
		LSM303_WriteOneReg(LSM303_CRB_REG_M, 0x80);		// 磁场传感器增益: ±4.0Gauss, XY轴450LSB/Gauss, Z轴400LSB/Gauss
		Delayms(1);	
		LSM303_WriteOneReg(LSM303_MR_REG_M, 0x00);		// 磁场传感器连续转换模式
		Delayms(1);										
		temp = LSM303_ReadOneReg(LSM303_MR_REG_M);		
		if(temp != 0)	
		{
			Delayms(10);	
		}
		else break;
	}
	if(temp != 0)	
	{
		return 1;	                                  // 磁力计初始化失败
	}
	return 0;
}

/* ====== 休眠/唤醒 ====== */

/**
 * @brief  LSM303DLH进入休眠模式
 * @retval 无
 * @note   磁力计进入休眠(MR_REG=0x03)，加速度计关闭XYZ使能(只保留最低功耗)
 */
void LSM303DLH_Sleep()
{
	LSM303_WriteOneReg(LSM303_MR_REG_M,0x03);       // 磁力计休眠模式
	LSM303_WriteOneReg(LSM303_CTRL_REG1_A,0x0f);		// 加速度计关闭所有轴使能
}

/**
 * @brief  LSM303DLH从休眠唤醒
 * @retval 无
 */
void LSM303DLH_Wakeup()
{
	LSM303_WriteOneReg(LSM303_MR_REG_M,0x00);       // 磁场传感器恢复连续转换模式
	LSM303_WriteOneReg(LSM303_CTRL_REG1_A,0x2f);    // 低功耗模式, 10Hz数据速率
}

/* ====== 传感器数据读取 ====== */

/**
 * @brief  读取三轴加速度原始值
 * @param  Xa 输出参数：X轴加速度原始值（16位有符号）
 * @param  Ya 输出参数：Y轴加速度原始值
 * @param  Za 输出参数：Z轴加速度原始值
 * @retval 无
 * @note   原始值需要除以LSB/g因子得到实际加速度值(g)
 */
void LSM303_ReadAcceleration(int16_t *Xa, int16_t *Ya, int16_t *Za)
{
	uint8_t buff[6];
	int16_t temp;
	LSM303_ReadMultiReg(LSM303_OUT_X_L_A,6,buff);   // 连续读取6字节（X,Y,Z各占2字节，低字节在前）
	
	temp = buff[1];
	temp <<= 8;
	temp |= buff[0];                                 // 合成X轴16位值（高字节<<8 | 低字节）
	*Xa = temp;
	
	temp = buff[3];
	temp <<= 8;
	temp |= buff[2];                                 // 合成Y轴16位值
	*Ya = temp;
	
	temp = buff[5];
	temp <<= 8;
	temp |= buff[4];                                 // 合成Z轴16位值
	*Za = temp;
}

/**
 * @brief  读取三轴磁场（磁力计）原始值
 * @param  Xm 输出参数：X轴磁场原始值（16位有符号）
 * @param  Ym 输出参数：Y轴磁场原始值
 * @param  Zm 输出参数：Z轴磁场原始值
 * @retval 无
 * @note   磁场数据读取顺序：X(高字节→低字节)，Z(高字节→低字节)，Y(高字节→低字节)
 */
void LSM303_ReadMagnetic(int16_t *Xm, int16_t *Ym, int16_t *Zm)
{
	uint8_t buff[6];
	int16_t temp;
	LSM303_ReadMultiReg(LSM303_OUT_X_H_M,6, buff);
	
	temp = buff[0];
	temp <<= 8;
	temp |= buff[1];                                 // 合成X轴磁场值（高字节在前）
	*Xm = temp;
	
	temp = buff[2];
	temp <<= 8;
	temp |= buff[3];                                 // 合成Z轴磁场值（注意Z在Y之前）
	*Zm = temp;
	
	temp = buff[4];
	temp <<= 8;
	temp |= buff[5];                                 // 合成Y轴磁场值
	*Ym = temp;
}

/**
 * @brief  读取LSM303温度值
 * @param  Temp 输出参数：温度值（已换算）
 * @retval 无
 * @note   温度数据为12位(高12位有效)，右移4位后除以8(8LSB/deg)，加0.5进行四舍五入
 */
void LSM303_ReadTemperature(int16_t *Temp)
{
	uint8_t buff[2];
	int16_t temp;
	IIC_Read_Multi_Byte(&LSM303_bus, LSM303_SlaveAddr_M, TEMP_OUT_H_M, 2, buff);
	temp = buff[0];
	temp <<= 8;
	temp |= buff[1];
	temp >>= 4;                                      // 只保留高12位（温度数据为12位）
	*Temp = temp / 3 + 0.5;                         // 换算公式：除以8 LSB/deg，加0.5四舍五入
}

/* ====== 姿态角度计算 ====== */

/**
 * @brief  计算Z轴相对竖直方向的倾角
 * @param  Xa X轴加速度原始值
 * @param  Ya Y轴加速度原始值
 * @param  Za Z轴加速度原始值
 * @retval Z轴角度（放大100倍的整数值，单位0.01度）
 */
int LSM303DLH_CalculationZAxisAngle(int16_t Xa, int16_t Ya, int16_t Za)
{
	double A;
	float fx,fy,fz;
	
	A = sqrt((int)Xa*Xa + (int)Ya*Ya + (int)Za*Za);	//计算加速度的矢量模长 |A|=sqrt(X²+Y²+Z²)
	fx = Xa/A;                                         // X轴归一化分量
	fy = Ya/A;                                         // Y轴归一化分量
	fz = Za/A;                                         // Z轴归一化分量
	
	// Z方向倾角计算：使用XoY平面的合分量与Z轴分量的比值求反正切
	A = fx*fx+fy*fy;                                   // XoY平面的合分量长度（未开方）
	A = sqrt(A);
	A = (double)A/fz;                                  // 比值（XoY平面分量/Z轴分量）
	A = atan(A);                                       // 反正切得到弧度值
	A = A*180/PI;                                      // 弧度转角度
	if(A < 0)
	{
		A += 90;                                        // 向上为正（角度修正，使0~180度范围）
		A = 0-A;
	}
	else
	{
		A = 90-A;
	}
	return A*100;                                      // 放大100倍返回整数
}

/**
 * @brief  计算X轴相对水平面的倾角
 * @param  Xa X轴加速度原始值
 * @param  Ya Y轴加速度原始值
 * @param  Za Z轴加速度原始值
 * @retval X轴角度（放大100倍的整数值，单位0.01度）
 */
int LSM303DLH_CalculationXAxisAngle(int16_t Xa, int16_t Ya, int16_t Za)
{
	double A;
	float fx,fy,fz;
	
	A = sqrt((int)Xa*Xa + (int)Ya*Ya + (int)Za*Za);	//计算加速度的矢量模长
	fx = Xa/A;                                         // X轴归一化分量
	fy = Ya/A;                                         // Y轴归一化分量
	fz = Za/A;                                         // Z轴归一化分量
	
	// X方向倾角计算：使用YoZ平面的合分量与X轴分量的比值求反正切
	A = fz*fz+fy*fy;                                   // YoZ平面的合分量长度
	A = sqrt(A);
	
	A = (double)A/fx;                                  // 比值（YoZ平面分量/X轴分量）
	A = atan(A); 
	A = A*180/PI;
	if(A < 0)
	{
		A += 90;                                        // 向上为正
	}
	else
	{
		A = 90-A;
		A = 0-A;                                        // 向下为负
	}
	return A*100;
}

/**
 * @brief  计算方位角（电子罗盘功能）
 * @param  Xa X轴加速度原始值
 * @param  Ya Y轴加速度原始值
 * @param  Za Z轴加速度原始值
 * @param  Xm X轴磁场原始值
 * @param  Ym Y轴磁场原始值
 * @param  Zm Z轴磁场原始值
 * @retval 方位角（角度值：0~360度）
 * @note   结合加速度计的姿态角度（俯仰角和横滚角）对磁力计数据进行倾斜补偿，
 *         然后通过反正切函数计算地磁方位角
 */
float Azimuth_Calculate(int16_t Xa, int16_t Ya, int16_t Za, int16_t Xm, int16_t Ym, int16_t Zm)
{
	float pitch, roll, Hy, Hx, Azimuth; 
	pitch   = atan2f(Xa, sqrtf(Ya * Ya + Za * Za));   // 计算俯仰角（绕Y轴旋转）
	roll    = atan2f(Ya, sqrtf(Xa * Xa + Za * Za));   // 计算横滚角（绕X轴旋转）
	Hy      = Ym * cosf(roll) + Xm * sinf(roll) * sinf(pitch) - Zm * cosf(pitch) * sinf(roll); // 倾斜补偿后的Y轴磁场分量
	Hx      = Xm * cosf(pitch) + Zm * sinf(pitch);    // 倾斜补偿后的X轴磁场分量
	Azimuth = atan2f(Hy,Hx)*180.0/PI;                 // 计算方位角（弧度转角度）
	return Azimuth;
}
