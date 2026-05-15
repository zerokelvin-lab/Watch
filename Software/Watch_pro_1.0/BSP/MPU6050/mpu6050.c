/** @file mpu6050.c @brief MPU6050六轴陀螺仪驱动 - I2C通信、初始化、传感器数据读取、姿态计算、运动检测 */

#include "mpu6050.h"

/* ====== 引脚定义 ====== */
#define MPU_INT_PORT GPIOB                         // MPU6050中断信号端口
#define MPU_INT_PIN  GPIO_PIN_12                   // MPU6050中断信号引脚：PB12

/** @brief GPIOB时钟使能宏 */
#define CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE();

/* ====== MPU6050 I2C总线配置 ====== */
iic_bus_t MPU_bus =
{
	.IIC_SDA_PORT = GPIOB,                         // SDA数据线端口
	.IIC_SCL_PORT = GPIOB,                         // SCL时钟线端口
	.IIC_SDA_PIN  = GPIO_PIN_13,                   // SDA引脚：PB13
	.IIC_SCL_PIN  = GPIO_PIN_14,                   // SCL引脚：PB14
};

/* ====== GPIO与中断初始化 ====== */

/**
 * @brief  初始化MPU6050中断检测引脚（PB12）
 * @retval 无
 * @note   配置为下降沿触发中断，内部上拉，优先级5
 */
void MPU_INT_Pin_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : PB12（下降沿触发中断，内部上拉） */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;      // 下降沿触发中断模式
  GPIO_InitStruct.Pull = GPIO_PULLUP;               // 内部上拉
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);      // 设置中断优先级：抢占5，子优先级0
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);               // 使能外部中断线10~15
}

/* ====== 运动检测初始化 ====== */

/**
 * @brief  初始化MPU6050运动检测功能
 * @retval 无
 * @note   配置运动检测阈值、持续时间、中断输出方式
 *         MOTION_DET: 加速度阈值 = (写入值*2) mg
 *         MOTION_DUR: 检测持续时间
 *         INTBP_CFG: INT引脚低电平有效，50us脉冲后复位
 *         INT_EN: 使能运动检测中断
 */
void MPU_Motion_Init(void)
{
    MPU_Write_Byte(MPU_MOTION_DET_REG,0x01);    // 设置加速度阈值 = (LSB*2)mg，1对应2mg
    MPU_Write_Byte(MPU_MOTION_DUR_REG,0x01);    // 加速度检测持续时间
    MPU_Write_Byte(MPU_INTBP_CFG_REG,0X90);     // INT引脚低电平有效, 50us脉冲后复位
    // 0x90 = bit7: INT电平(0=低有效) | bit4: LATCH_INT_EN(1=锁存, 保持直到INT_STATUS被读取)
    MPU_Write_Byte(MPU_INT_EN_REG,0x40);       	// 使能运动检测中断 (bit6: WOM_INT_EN)
}

/* ====== I2C总线初始化 ====== */

/**
 * @brief  初始化与MPU6050通信的I2C总线
 * @retval 无
 */
void MPU_Bus_Init(void)
{
	CLK_ENABLE;                                    // 使能GPIOB时钟
	IICInit(&MPU_bus);                             // 初始化I2C总线

	/* I2C总线恢复：发送9个时钟脉冲解除SDA卡死（从机异常时SDA可能被拉低） */
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.Pin = MPU_bus.IIC_SCL_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(MPU_bus.IIC_SCL_PORT, &GPIO_InitStructure);
	for(int i = 0; i < 9; i++)
	{
		HAL_GPIO_WritePin(MPU_bus.IIC_SCL_PORT, MPU_bus.IIC_SCL_PIN, GPIO_PIN_RESET);
		delay_us(5);
		HAL_GPIO_WritePin(MPU_bus.IIC_SCL_PORT, MPU_bus.IIC_SCL_PIN, GPIO_PIN_SET);
		delay_us(5);
	}
	IICStop(&MPU_bus);                             // 发送STOP释放总线
}

/* ====== MPU6050 主初始化 ====== */

/**
 * @brief  初始化MPU6050传感器
 * @retval 0=成功, 1=失败（设备ID不匹配）
 * @note   初始化流程：复位 → 唤醒 → 设置陀螺仪量程(2000dps) → 设置加速度计量程(8g) →
 *         设置采样率(50Hz) → 关闭所有中断 → 关闭I2C主模式 → 关闭FIFO →
 *         验证设备ID → 进入低功耗模式（仅加速度计工作，20Hz唤醒频率）
 *         PWR_MGMT1[0x6B]: bit6=SLEEP, bit5=CYCLE, bit3=TEMP_DIS
 *         PWR_MGMT2[0x6C]: 控制各轴的待机状态
 */
u8 MPU_Init(void)
{
	u8 res;

	MPU_Bus_Init();                                // 初始化I2C总线

	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);	//复位MPU6050（PWR_MGMT1 bit7: DEVICE_RESET=1）
  delay_ms(100);
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X00);	//唤醒MPU6050（清除SLEEP位和RESET位）
	MPU_Set_Gyro_Fsr(3);										//陀螺仪传感器量程: 2000dps (3<<3 = 0x18)
	MPU_Set_Accel_Fsr(2);										//加速度传感器量程: 8g (2<<3 = 0x10)
	MPU_Set_Rate(50);												//采样率50Hz
	MPU_Write_Byte(MPU_INT_EN_REG,0X00);		//关闭所有中断
	MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);	//IIC主模式关闭（不控制外部传感器）
	MPU_Write_Byte(MPU_FIFO_EN_REG,0X00);		//关闭FIFO
	MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);	//INT引脚低电平有效

	res=MPU_Read_Byte(MPU_DEVICE_ID_REG);          // 读取设备ID寄存器(0x75)
	if(res==MPU_ADDR)//验证ID是否为0x68
	{
		// 进入低功耗循环模式：
		// PWR_MGMT1 = 0x28: SLEEP=0(不睡眠), CYCLE=1(循环模式), TEMP_DIS=1(关闭温度传感器), 内部8MHz时钟
		MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X28);	// 进入低功耗模式
		// PWR_MGMT2 = 0x87: 使能加速度计X/Y/Z(清除待机位), 禁用陀螺仪X/Y/Z, 唤醒频率=20Hz
		// 0x87 = 0b10000111: 低2位=11(WakeUpFreq=20Hz), bit2~7=待机控制(0=active)
		MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X87);	// 使能加速度计, 禁用陀螺仪, 设置唤醒频率=20Hz
		MPU_Set_Rate(50);												//采样率50Hz
 	}else return 1;

	MPU_Motion_Init();                             // 初始化运动检测
	MPU_INT_Pin_Init();                            // 初始化中断引脚

	return 0;
}

/* ====== 休眠/唤醒 ====== */

/**
 * @brief  MPU6050进入休眠模式
 * @retval 无
 * @note   PWR_MGMT1=0x48: SLEEP=1, CYCLE=0, TEMP_DIS=1, 内部8MHz时钟
 */
void MPU_Sleep()
{
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0x48);       // sleep=1, cycle=0, temp_dis=1, internal 8MHz
}

/**
 * @brief  MPU6050从休眠唤醒（进入低功耗循环模式）
 * @retval 无
 * @note   PWR_MGMT1=0x28: SLEEP=0, CYCLE=1, TEMP_DIS=1, 内部8MHz时钟
 */
void MPU_Wakeup()
{
	//low power modes
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0x28);       // sleep=0, cycle=1, temp_dis=1, internal 8MHz
}

/** @brief 读取MPU6050中断状态寄存器 */
uint8_t MPU_Read_Status()
{
	return MPU_Read_Byte(MPU_INT_STA_REG);
}

/* ====== 传感器配置函数 ====== */

/**
 * @brief  设置MPU6050陀螺仪传感器满量程范围
 * @param  fsr 量程选择: 0=±250dps, 1=±500dps, 2=±1000dps, 3=±2000dps
 * @retval 0=成功
 * @note   寄存器 GYRO_CONFIG(0x1B) bit[4:3] = FS_SEL，写入时需左移3位
 */
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3); // 量程值左移3位写入 GYRO_CONFIG[4:3]
}

/**
 * @brief  设置MPU6050加速度计满量程范围
 * @param  fsr 量程选择: 0=±2g, 1=±4g, 2=±8g, 3=±16g
 * @retval 0=成功
 * @note   寄存器 ACCEL_CONFIG(0x1C) bit[4:3] = AFS_SEL，写入时需左移3位
 */
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3); // 量程值左移3位写入 ACCEL_CONFIG[4:3]
}

/**
 * @brief  设置MPU6050数字低通滤波器(DLPF)截止频率
 * @param  lpf 低通滤波器频率(Hz)，取最近似值
 * @retval 0=成功
 * @note   DLPF_CFG[2:0] 取值: 0=256Hz(实际8kHz), 1=188Hz, 2=98Hz, 3=42Hz, 4=20Hz, 5=10Hz, 6=5Hz
 */
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;                            // 188Hz
	else if(lpf>=98)data=2;                        // 98Hz
	else if(lpf>=42)data=3;                        // 42Hz
	else if(lpf>=20)data=4;                        // 20Hz
	else if(lpf>=10)data=5;                        // 10Hz
	else data=6;                                   // 5Hz
	return MPU_Write_Byte(MPU_CFG_REG,data);       // 写入 CONFIG 寄存器 (0x1A)
}

/**
 * @brief  设置MPU6050的采样率
 * @param  rate 采样率(Hz)，范围 4~1000
 * @retval 0=成功
 * @note   采样率 = 陀螺仪输出频率 / (1 + SMPLRT_DIV)
 *         陀螺仪输出频率 = 8kHz (当DLPF禁用时) 或 1kHz (当DLPF启用时)
 *         设置采样率后自动将LPF设为采样率的一半
 */
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;                              // 计算分频值
	data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data); // 写入 SMPLRT_DIV 寄存器 (0x19)
 	return MPU_Set_LPF(rate/2);                    // 自动设置LPF为采样率的一半
}

/* ====== 传感器数据读取 ====== */

/**
 * @brief  获取MPU6050温度值
 * @retval 温度值（放大100倍的short整数，单位0.01°C）
 * @note   温度转换公式: Temperature = 36.53 + raw/340 (°C)
 */
short MPU_Get_Temperature(void)
{
    u8 buf[2];
    short raw;
		float temp;
		MPU_Read_Len(MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf); // 读取温度高字节和低字节
    raw=((u16)buf[0]<<8)|buf[1];                       // 合成16位原始温度值
    temp=36.53+((double)raw)/340;                      // 换算为摄氏温度
    return temp*100;;                                  // 放大100倍返回
}

/**
 * @brief  获取MPU6050陀螺仪三轴原始值
 * @param  gx 输出参数：X轴角速度原始值
 * @param  gy 输出参数：Y轴角速度原始值
 * @param  gz 输出参数：Z轴角速度原始值
 * @retval 0=成功
 */
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    u8 buf[6],res;
		res=MPU_Read_Len(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,buf); // 连续读取6字节（X/Y/Z各2字节）
		if(res==0)
		{
			*gx=((u16)buf[0]<<8)|buf[1];             // X轴：高字节<<8 | 低字节
			*gy=((u16)buf[2]<<8)|buf[3];             // Y轴：高字节<<8 | 低字节
			*gz=((u16)buf[4]<<8)|buf[5];             // Z轴：高字节<<8 | 低字节
		}
    return res;;
}

/**
 * @brief  获取MPU6050加速度计三轴原始值
 * @param  ax 输出参数：X轴加速度原始值
 * @param  ay 输出参数：Y轴加速度原始值
 * @param  az 输出参数：Z轴加速度原始值
 * @retval 0=成功
 */
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    u8 buf[6],res;
		res=MPU_Read_Len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,buf); // 连续读取6字节加速度数据
		if(res==0)
		{
			*ax=((u16)buf[0]<<8)|buf[1];             // X轴加速度
			*ay=((u16)buf[2]<<8)|buf[3];             // Y轴加速度
			*az=((u16)buf[4]<<8)|buf[5];             // Z轴加速度
		}
    return res;;
}

/* ====== I2C底层通信函数 ====== */

/**
 * @brief  I2C连续写多个字节
 * @param  addr 设备I2C地址
 * @param  reg  寄存器地址
 * @param  len  写入长度
 * @param  buf  数据缓冲区
 * @retval 0=成功, 1=失败
 * @note   I2C写时序: START → 设备地址(W) → 寄存器地址 → 数据字节... → STOP
 */
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
	u8 i;
  IICStart(&MPU_bus);                             // 发送起始信号
	IICSendByte(&MPU_bus,(addr<<1)|0);              // 发送设备地址 + 写标志(bit0=0)
	if(IICWaitAck(&MPU_bus))                        // 等待应答
	{
		IICStop(&MPU_bus);
		return 1;
	}
    IICSendByte(&MPU_bus,reg);                     // 发送寄存器地址
    IICWaitAck(&MPU_bus);                          // 等待应答
	for(i=0;i<len;i++)
	{
		IICSendByte(&MPU_bus,buf[i]);               // 逐字节发送数据
		if(IICWaitAck(&MPU_bus))                    // 等待应答
		{
			IICStop(&MPU_bus);
			return 1;
		}
	}
    IICStop(&MPU_bus);                             // 发送停止信号
	return 0;
}

/**
 * @brief  I2C写单个字节
 * @param  reg  寄存器地址
 * @param  data 数据（8位）
 * @retval 0=成功, 1=失败
 */
u8 MPU_Write_Byte(u8 reg,u8 data)
{
  IICStart(&MPU_bus);
	IICSendByte(&MPU_bus, (MPU_ADDR<<1)|0);         // 设备地址 + 写标志
	if(IICWaitAck(&MPU_bus))
	{
		IICStop(&MPU_bus);
		return 1;
	}
	IICSendByte(&MPU_bus,reg);                      // 发送寄存器地址
	IICWaitAck(&MPU_bus);
	IICSendByte(&MPU_bus,data);                     // 发送单字节数据
	if(IICWaitAck(&MPU_bus))
	{
		IICStop(&MPU_bus);
		return 1;
	}
  IICStop(&MPU_bus);
	return 0;
}

/**
 * @brief  I2C读单个字节
 * @param  reg 寄存器地址
 * @retval 读取到的寄存器值
 * @note   I2C读时序: START → 设备地址(W) → 寄存器地址 → RESTART → 设备地址(R) → 读数据(NACK) → STOP
 */
u8 MPU_Read_Byte(u8 reg)
{
	u8 res;
  IICStart(&MPU_bus);
	IICSendByte(&MPU_bus,(MPU_ADDR<<1)|0);           // 设备地址 + 写标志
	IICWaitAck(&MPU_bus);
  IICSendByte(&MPU_bus,reg);                        // 发送要读取的寄存器地址
  IICWaitAck(&MPU_bus);
  IICStart(&MPU_bus);                               // 重复起始信号
	IICSendByte(&MPU_bus,(MPU_ADDR<<1)|1);            // 设备地址 + 读标志(bit0=1)
  IICWaitAck(&MPU_bus);
	res=IICReceiveByte(&MPU_bus);                    // 接收数据
	IICSendNotAck(&MPU_bus);                         // 发送NACK，结束读取
  IICStop(&MPU_bus);
	return res;
}

/**
 * @brief  I2C连续读多个字节
 * @param  addr 设备I2C地址
 * @param  reg  起始寄存器地址
 * @param  len  读取长度
 * @param  buf  数据缓冲区
 * @retval 0=成功
 * @note   连续读取时，除最后一个字节回复NACK外，其余字节均回复ACK
 */
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
 	IICStart(&MPU_bus);
	IICSendByte(&MPU_bus,(addr<<1)|0);               // 设备地址 + 写标志
	if(IICWaitAck(&MPU_bus))
	{
		IICStop(&MPU_bus);
		return 1;
	}
    IICSendByte(&MPU_bus,reg);                       // 发送起始寄存器地址
    IICWaitAck(&MPU_bus);
    IICStart(&MPU_bus);                              // 重复起始信号
		IICSendByte(&MPU_bus,(addr<<1)|1);               // 设备地址 + 读标志
    IICWaitAck(&MPU_bus);
		while(len)
		{
			if(len==1)                                 // 最后一个字节
			{
				*buf=IICReceiveByte(&MPU_bus);
				IICSendNotAck(&MPU_bus);               // 回复NACK，结束传输
			}
			else
			{
				*buf=IICReceiveByte(&MPU_bus);
				IICSendAck(&MPU_bus);                  // 回复ACK，继续接收下一字节
			}
			len--;
			buf++;
		}
    IICStop(&MPU_bus);
		return 0;
}

/** @brief  I2C连续写多个字节（封装版本） */
uint8_t MPU_Write_Multi_Byte(uint8_t addr,uint8_t length,uint8_t buff[])
{
	if(IIC_Write_Multi_Byte(&MPU_bus,MPU_ADDR<<1,addr,length,buff))
	{
		return 1;
	}
	return 0;
}

/** @brief  I2C连续读多个字节（封装版本） */
uint8_t MPU_Read_Multi_Byte(uint8_t addr, uint8_t length, uint8_t buff[])
{
	if(IIC_Read_Multi_Byte(&MPU_bus, MPU_ADDR<<1, addr, length, buff))
	{
		return 1;
	}
	return 0;
}

/* ====== 姿态计算函数 ====== */

/**
 * @brief  通过加速度计数据计算横滚角和俯仰角
 * @param  roll 输出参数：横滚角（弧度，绕X轴旋转）
 * @param  pitch 输出参数：俯仰角（弧度，绕Y轴旋转）
 * @retval 无
 * @note   pitch = -atan(ax / sqrt(ay² + az²))
 *         roll  = atan(ay / az)
 */
void MPU_Get_Angles(float * roll,float * pitch)
{
	short ax,ay,az;
	MPU_Get_Accelerometer(&ax,&ay,&az);
	*pitch = -atanf(ax/sqrtf(ay*ay+az*az));         // 计算俯仰角（绕Y轴）
	*roll = atanf((float)ay/(float)az);              // 计算横滚角（绕X轴）
}

/**
 * @brief  检测MPU6050是否处于水平状态
 * @retval 1=水平（横滚角和俯仰角均在±0.5弧度内）
 * @note   用于检测手表是否处于平放状态（如放在桌面充电时）
 */
uint8_t MPU_isHorizontal(void)
{
	float roll,pitch;
	MPU_Get_Angles(&roll,&pitch);
	if(roll<=0.50 && roll>=-0.50 && pitch<=0.50 && pitch>=-0.50)  // 角度阈值为±0.5弧度(约±28.6度)
	{return 1;}
	return 0;
}
