/** @file AHT21.c @brief AHT21温湿度传感器驱动实现，通过软件I2C读写温湿度数据 */
#include "AHT21.h"

/** @brief AHT21时钟使能，使用GPIOB时钟 */
#define AHT_CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE()

/** @brief AHT21 I2C总线配置：SDA=PB13, SCL=PB14 */
iic_bus_t AHT_bus = 
{
	.IIC_SDA_PORT = GPIOB,  // SDA端口
	.IIC_SCL_PORT = GPIOB,  // SCL端口
	.IIC_SDA_PIN  = GPIO_PIN_13,  // SDA引脚 PB13
	.IIC_SCL_PIN  = GPIO_PIN_14,  // SCL引脚 PB14
};

/**
  * @brief 读取AHT21状态寄存器值
  * @return 状态字节
  */
uint8_t AHT_Read_Status(void)
{
	uint8_t Byte_first;	
	IICStart(&AHT_bus);                  // 发送起始信号
	IICSendByte(&AHT_bus,0x71);          // 发送读地址(0x70|1)
	IICWaitAck(&AHT_bus);                // 等待应答
	Byte_first = IICReceiveByte(&AHT_bus);// 接收状态字节
	IICSendNotAck(&AHT_bus);             // 发送非应答
	IICStop(&AHT_bus);                   // 发送停止信号
	return Byte_first;
}

/**
  * @brief 检查AHT21校准使能位和NOR模式
  * @return 1=校准已使能且为NOR模式, 0=未就绪
  */
uint8_t AHT_Read_Cal_Enable(void)  //check cal enable bit 
{
	uint8_t val = 0;//ret = 0,
 
  val = AHT_Read_Status();
  if((val & 0x68)==0x08)  //check NOR mode: bit[6:3]检查是否为NOR工作模式(0b00001xxx)
		return 1;
  else  
		return 0;
}

/**
  * @brief 发送软复位命令(0xBA)复位AHT21传感器
  */
void AHT_Reset(void)//AHT21 send 0xBA reset call
{
	IICStart(&AHT_bus);
	IICSendByte(&AHT_bus,0x70);  // 写地址
	IICWaitAck(&AHT_bus);
	IICSendByte(&AHT_bus,0xBA);  // 软复位命令
	IICWaitAck(&AHT_bus);
	IICStop(&AHT_bus);
}

/**
  * @brief AHT21初始化：使能时钟、初始化I2C、配置传感器
  * @return 0成功
  */
uint8_t AHT_Init(void)
{
	AHT_CLK_ENABLE;           // 使能GPIOB时钟
	IICInit(&AHT_bus);        // 初始化软件I2C引脚
	
	delay_ms(40);             // 上电等待40ms
	
	if(AHT_Read_Status()&&0x08!=0x08) // 检查状态，若未校准则初始化配置
	{
		IICStart(&AHT_bus);
		IICSendByte(&AHT_bus,0x70);  // 写地址
		IICWaitAck(&AHT_bus);
		IICSendByte(&AHT_bus,0xBE);  // 初始化命令高字节
		IICWaitAck(&AHT_bus);
		IICSendByte(&AHT_bus,0x08);  // 初始化命令(校准使能)
		IICWaitAck(&AHT_bus);
		IICSendByte(&AHT_bus,0x00);  // 初始化命令低字节
		IICWaitAck(&AHT_bus);
		IICStop(&AHT_bus);
		delay_ms(10);
	}
	//AHT_Reset();
	
	return 0;
}	
 
/**
  * @brief 触发AHT21测量并读取温湿度原始数据，计算实际值
  * @param humi[out] 湿度值输出(%RH)
  * @param temp[out] 温度值输出(℃)
  * @return 0=成功, 1=超时失败
  * @note AHT21数据帧格式: [状态][湿度高][湿度中][湿度低][湿度/温度][温度高][温度低]
  */
uint8_t AHT_Read(float *humi, float *temp)
{
	uint8_t cnt=5;            // 重试计数器
	uint8_t  Byte_1th=0;     // 状态字节
	uint8_t  Byte_2th=0;     // 湿度高字节
	uint8_t  Byte_3th=0;     // 湿度中字节
	uint8_t  Byte_4th=0;     // 湿度低字节(低4位) + 温度(高4位)
	uint8_t  Byte_5th=0;     // 温度中字节
	uint8_t  Byte_6th=0;     // 温度低字节
	uint32_t RetuData = 0;   // 中间计算结果
	
	/* ====== 发送触发测量命令(0xAC 0x33 0x00) ====== */
	IICStart(&AHT_bus);
	IICSendByte(&AHT_bus,0x70);    // 写地址
	IICWaitAck(&AHT_bus);
	IICSendByte(&AHT_bus,0xAC);    // 触发测量命令
	IICWaitAck(&AHT_bus);
	IICSendByte(&AHT_bus,0x33);    // 测量参数
	IICWaitAck(&AHT_bus);
	IICSendByte(&AHT_bus,0x00);    // 测量参数
	IICWaitAck(&AHT_bus);
	IICStop(&AHT_bus);	
	
	/* ====== 等待测量完成(忙状态bit7=1时等待) ====== */
	delay_ms(80);
	while(AHT_Read_Status()&0x80==0x80 && cnt) // 检查bit7忙标志
	{
		delay_ms(5);
		cnt--;
		AHT_Read_Status();
	}
	if(!cnt)
	{return 1;}  // 超时,传感器未就绪
	
	/* ====== 读取6字节测量数据 ====== */
	IICStart(&AHT_bus);
	IICSendByte(&AHT_bus,0x71);       // 读地址
	IICWaitAck(&AHT_bus);
	Byte_1th = IICReceiveByte(&AHT_bus);  // 状态字节
	IICSendAck(&AHT_bus);
	Byte_2th = IICReceiveByte(&AHT_bus);  // 湿度[19:12]
	IICSendAck(&AHT_bus);
	Byte_3th = IICReceiveByte(&AHT_bus);  // 湿度[11:4]
	IICSendAck(&AHT_bus);
	Byte_4th = IICReceiveByte(&AHT_bus);  // 湿度[3:0] | 温度[19:16]
	IICSendAck(&AHT_bus);
	Byte_5th = IICReceiveByte(&AHT_bus);  // 温度[15:8]
	IICSendAck(&AHT_bus);
	Byte_6th = IICReceiveByte(&AHT_bus);  // 温度[7:0]
	IICSendNotAck(&AHT_bus);
	IICStop(&AHT_bus);

	/* ====== 计算湿度: RH(%) = (raw >> 4) * 100 / 2^20 = raw * 1000 >> 20 / 10 ====== */
	RetuData = (RetuData|Byte_2th)<<8;  // 湿度高字节
	RetuData = (RetuData|Byte_3th)<<8;  // 湿度中字节
	RetuData = (RetuData|Byte_4th);     // 湿度低字节(含4位)
	RetuData =RetuData >>4;             // 去掉低4位对齐,得到20位湿度原始值
	*humi = (RetuData * 1000 >> 20);    // 公式: RH% = S_RH / 2^20 * 100%
	*humi /= 10;                        // 保留1位小数
	
	/* ====== 计算温度: T(℃) = (raw & 0xFFFFF) * 200 / 2^20 - 50 ====== */
	RetuData = 0;
	RetuData = (RetuData|(Byte_4th&0x0f))<<8; // 取湿度低4位,组装温度高4位
	RetuData = (RetuData|Byte_5th)<<8;         // 温度中字节
	RetuData = (RetuData|Byte_6th);            // 温度低字节
	RetuData = RetuData&0xfffff;               // 取低20位温度原始值
	*temp = ((RetuData * 2000 >> 20)- 500);    // 公式: T℃ = S_T / 2^20 * 200 - 50
	*temp /= 10;                               // 保留1位小数
	
	return 0;
}
