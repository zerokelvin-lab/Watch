/** @file iic_hal.c @brief 软件I2C硬件抽象层实现，通过GPIO模拟I2C总线时序 */
#include "iic_hal.h"
#include "delay.h"

/**
  * @brief SDA引脚切换到输入模式（开漏上拉，用于读取数据）
  * @param bus I2C总线指针
  */
void SDA_Input_Mode(iic_bus_t *bus)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.Pin = bus->IIC_SDA_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;      // 输入模式
    GPIO_InitStructure.Pull = GPIO_PULLUP;           // 上拉
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(bus->IIC_SDA_PORT, &GPIO_InitStructure);
}

/**
  * @brief SDA引脚切换到输出模式（开漏输出，用于发送数据）
  * @param bus I2C总线指针
  */
void SDA_Output_Mode(iic_bus_t *bus)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.Pin = bus->IIC_SDA_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;  // 开漏输出模式(兼容I2C多设备)
    GPIO_InitStructure.Pull = GPIO_NOPULL;           // 无上下拉
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(bus->IIC_SDA_PORT, &GPIO_InitStructure);
}

/**
  * @brief SDA引脚输出高/低电平
  * @param bus I2C总线指针
  * @param val 输出值(0=低电平, 非0=高电平)
  * @note 使用BSRR寄存器直接操作，高16位写1清0(置低)，低16位写1置1(置高)
  */
void SDA_Output(iic_bus_t *bus, uint16_t val)
{
    if ( val )
    {
        bus->IIC_SDA_PORT->BSRR |= bus->IIC_SDA_PIN;   // BSRR低16位写1: 引脚输出高电平
    }
    else
    {
        bus->IIC_SDA_PORT->BSRR = (uint32_t)bus->IIC_SDA_PIN << 16U; // BSRR高16位写1: 引脚输出低电平
    }
}

/**
  * @brief SCL引脚输出高/低电平
  * @param bus I2C总线指针
  * @param val 输出值(0=低电平, 非0=高电平)
  */
void SCL_Output(iic_bus_t *bus, uint16_t val)
{
    if ( val )
    {
        bus->IIC_SCL_PORT->BSRR |= bus->IIC_SCL_PIN;   // 置高
    }
    else
    {
         bus->IIC_SCL_PORT->BSRR = (uint32_t)bus->IIC_SCL_PIN << 16U; // 置低
    }
}

/**
  * @brief SDA引脚读入1位数据
  * @param bus I2C总线指针
  * @return GPIO引脚当前电平(0或1)
  */
uint8_t SDA_Input(iic_bus_t *bus)
{
	if(HAL_GPIO_ReadPin(bus->IIC_SDA_PORT, bus->IIC_SDA_PIN) == GPIO_PIN_SET){
		return 1;
	}else{
		return 0;
	}
}

/**
  * @brief 产生I2C起始信号：SCL高电平时SDA从高到低跳变
  * @param bus I2C总线指针
  */
void IICStart(iic_bus_t *bus)
{
    SDA_Output(bus,1);       // SDA初始高电平
    //delay1(DELAY_TIME);
		delay_us(2);
    SCL_Output(bus,1);       // SCL拉高
		delay_us(1);
    SDA_Output(bus,0);       // SCL高电平时SDA下降沿 -> 起始信号
		delay_us(1);
    SCL_Output(bus,0);       // SCL拉低，准备发送数据
		delay_us(1);
}

/**
  * @brief 产生I2C停止信号：SCL高电平时SDA从低到高跳变
  * @param bus I2C总线指针
  */
void IICStop(iic_bus_t *bus)
{
    SCL_Output(bus,0);       // SCL先拉低
		delay_us(2);
    SDA_Output(bus,0);       // SDA拉低
		delay_us(1);
    SCL_Output(bus,1);       // SCL拉高
		delay_us(1);
    SDA_Output(bus,1);       // SCL高电平时SDA上升沿 -> 停止信号
		delay_us(1);

}

/**
  * @brief I2C等待从机应答信号(ACK)
  * @param bus I2C总线指针
  * @return SUCCESS=收到应答, ERROR=超时未应答
  * @note 应答时间超时检测，SCL第9个时钟脉冲时SDA被从机拉低表示ACK
  */
unsigned char IICWaitAck(iic_bus_t *bus)
{
    unsigned short cErrTime = 5;   // 超时计数器
    SDA_Input_Mode(bus);            // SDA切换为输入模式
    SCL_Output(bus,1);             // SCL拉高，释放SDA让从机控制
    while(SDA_Input(bus))          // 等待SDA被从机拉低(ACK)
    {
        cErrTime--;
				delay_us(1);
        if (0 == cErrTime)          // 超时未应答
        {
            SDA_Output_Mode(bus);   // SDA切回输出模式
            IICStop(bus);           // 发送停止信号
            return ERROR;
        }
    }
    SDA_Output_Mode(bus);           // SDA切回输出模式
    SCL_Output(bus,0);             // SCL拉低
		delay_us(2);
    return SUCCESS;
}

/**
  * @brief 发送I2C应答信号(ACK)：在第9个时钟脉冲将SDA拉低
  * @param bus I2C总线指针
  */
void IICSendAck(iic_bus_t *bus)
{
    SDA_Output(bus,0);       // SDA拉低=应答
		delay_us(1);
    SCL_Output(bus,1);       // SCL脉冲
		delay_us(1);
    SCL_Output(bus,0);
		delay_us(1);
	
}

/**
  * @brief 发送I2C非应答信号(NACK)：在第9个时钟脉冲保持SDA高电平
  * @param bus I2C总线指针
  */
void IICSendNotAck(iic_bus_t *bus)
{
    SDA_Output(bus,1);       // SDA拉高=非应答
		delay_us(1);
    SCL_Output(bus,1);       // SCL脉冲
		delay_us(1);
    SCL_Output(bus,0);
		delay_us(2);

}

/**
  * @brief 通过I2C发送1个字节(MSB先发)
  * @param bus I2C总线指针
  * @param cSendByte 待发送的字节数据
  * @note 每发送1位后左移1位(cSendByte += cSendByte)，最高位先发
  */
void IICSendByte(iic_bus_t *bus,unsigned char cSendByte)
{
    unsigned char  i = 8;
    while (i--)
    {
        SCL_Output(bus,0);                       // SCL拉低，准备数据
        delay_us(2);
        SDA_Output(bus, cSendByte & 0x80);       // 输出最高位(bit7)
				delay_us(1);
        cSendByte += cSendByte;                    // 左移1位(等同于cSendByte<<=1)
				delay_us(1);
        SCL_Output(bus,1);                       // SCL拉高，从机读取数据
				delay_us(1);
    }
    SCL_Output(bus,0);                           // SCL拉低
		delay_us(2);
}

/**
  * @brief 通过I2C接收1个字节(MSB先收)
  * @param bus I2C总线指针
  * @return 接收到的字节数据
  * @note 每接收1位左移1位后与SDA电平做或运算
  */
unsigned char IICReceiveByte(iic_bus_t *bus)
{
    unsigned char i = 8;
    unsigned char cR_Byte = 0;
    SDA_Input_Mode(bus);             // SDA切换为输入模式
    while (i--)
    {
        cR_Byte += cR_Byte;          // 左移1位，为新数据腾出最低位
        SCL_Output(bus,0);           // SCL拉低，从机准备数据
				delay_us(2);
        SCL_Output(bus,1);           // SCL拉高，读取SDA数据
				delay_us(1);
        cR_Byte |=  SDA_Input(bus);  // 读取1位数据并入最低位
    }
    SCL_Output(bus,0);               // SCL拉低
    SDA_Output_Mode(bus);            // SDA切回输出模式
    return cR_Byte;
}

/**
  * @brief I2C组合写：向从设备指定寄存器写入1字节数据
  * @param bus I2C总线指针
  * @param daddr 7位设备地址
  * @param reg 目标寄存器地址
  * @param data 写入的数据
  * @return 0=成功, 1=设备无应答
  */
uint8_t IIC_Write_One_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg,uint8_t data)
{				   	  	    																 
  IICStart(bus);  
	
	IICSendByte(bus,daddr<<1);	       // 发送设备地址+写标志(bit0=0)
	if(IICWaitAck(bus))	               // 等待应答
	{
		IICStop(bus);		               // 无应答,发送停止
		return 1;		
	}
	IICSendByte(bus,reg);                // 发送寄存器地址
	IICWaitAck(bus);	   	 										  		   
	IICSendByte(bus,data);               // 发送数据   
	IICWaitAck(bus);  		    	   
  IICStop(bus);
	delay_us(1);
	return 0;
}

/**
  * @brief I2C组合写：向从设备指定寄存器连续写入多字节数据
  * @param bus I2C总线指针
  * @param daddr 7位设备地址
  * @param reg 起始寄存器地址
  * @param length 写入字节数
  * @param buff[] 数据缓冲区
  * @return 0=成功, 1=设备无应答
  */
uint8_t IIC_Write_Multi_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg,uint8_t length,uint8_t buff[])
{			
	unsigned char i;	
  IICStart(bus);  
	
	IICSendByte(bus,daddr<<1);	         // 发送设备地址+写标志
	if(IICWaitAck(bus))
	{
		IICStop(bus);
		return 1;
	}
	IICSendByte(bus,reg);                // 发送起始寄存器地址
	IICWaitAck(bus);	
	for(i=0;i<length;i++)               // 连续写入多字节
	{
		IICSendByte(bus,buff[i]);     						   
		IICWaitAck(bus); 
	}		    	   
  IICStop(bus);
	delay_us(1);
	return 0;
} 

/**
  * @brief I2C组合读：从从设备指定寄存器读取1字节数据
  * @param bus I2C总线指针
  * @param daddr 7位设备地址
  * @param reg 目标寄存器地址
  * @return 读取的寄存器值
  * @note 先发送写地址+寄存器地址, 再发送读地址读取数据
  */
unsigned char IIC_Read_One_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg)
{
	unsigned char dat;
	IICStart(bus);
	IICSendByte(bus,daddr<<1);           // 发送设备地址+写标志(先写入寄存器地址)
	IICWaitAck(bus);
	IICSendByte(bus,reg);                // 发送目标寄存器地址
	IICWaitAck(bus);
	
	IICStart(bus);                       // 重复起始信号
	IICSendByte(bus,(daddr<<1)+1);       // 发送设备地址+读标志(bit0=1)
	IICWaitAck(bus);
	dat = IICReceiveByte(bus);           // 读取1字节数据
	IICSendNotAck(bus);                  // 发送非应答(结束读取)
	IICStop(bus);
	return dat;
}


/**
  * @brief I2C组合读：从从设备指定寄存器连续读取多字节数据
  * @param bus I2C总线指针
  * @param daddr 7位设备地址
  * @param reg 起始寄存器地址
  * @param length 读取字节数
  * @param buff[] 数据缓冲区(输出)
  * @return 0=成功, 1=设备无应答
  * @note 前length-1字节读后发ACK，最后1字节读后发NACK结束
  */
uint8_t IIC_Read_Multi_Byte(iic_bus_t *bus, uint8_t daddr, uint8_t reg, uint8_t length, uint8_t buff[])
{
	unsigned char i;
	IICStart(bus);
	IICSendByte(bus,daddr<<1);           // 发送设备地址+写标志
	if(IICWaitAck(bus))
	{
		IICStop(bus);		 
		return 1;		
	}
	IICSendByte(bus,reg);                // 发送起始寄存器地址
	IICWaitAck(bus);
	
	IICStart(bus);                       // 重复起始信号
	IICSendByte(bus,(daddr<<1)+1);       // 发送设备地址+读标志
	IICWaitAck(bus);
	for(i=0;i<length;i++)
	{
		buff[i] = IICReceiveByte(bus);   // 读取1字节
		if(i<length-1)
		{IICSendAck(bus);}               // 非最后一字节,发送ACK继续读取
	}
	IICSendNotAck(bus);                  // 最后一字节后发送NACK结束
	IICStop(bus);
	return 0;
}


/**
  * @brief 初始化软件I2C总线GPIO引脚(推挽输出+上拉)
  * @param bus I2C总线指针
  */
void IICInit(iic_bus_t *bus)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

		//bus->CLK_ENABLE();
		
    // 配置SDA引脚
    GPIO_InitStructure.Pin = bus->IIC_SDA_PIN ;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;    // 推挽输出(初始状态)
    GPIO_InitStructure.Pull = GPIO_PULLUP;             // 内部上拉
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(bus->IIC_SDA_PORT, &GPIO_InitStructure);
		
    // 配置SCL引脚
		GPIO_InitStructure.Pin = bus->IIC_SCL_PIN ;
    HAL_GPIO_Init(bus->IIC_SCL_PORT, &GPIO_InitStructure);
}
