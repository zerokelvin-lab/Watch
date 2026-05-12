#include "iic2.h"
#include "delay.h"





/****************************************************************************
* Function Name  : IIC_Config
* Description    : 初始化GPIO.
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void I2C_INIT_2()
{
	
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//使能GPIOB时钟
	GPIO_InitStructure.GPIO_Pin=I2C_SDA2;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);				//PA3
	
	RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//PB8
	GPIO_InitStructure.GPIO_Pin=I2C_SCL2;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType=GPIO_OType_OD;//开漏输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);				//PA2

	I2C_SCL_H_2;		//拉高总线
	I2C_SDA_H_2;


}

/*******************************************************************************
* 函 数 名         : I2C_SDA_OUT
* 函数功能		   : SDA输出配置	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void I2C_SDA_OUT_2()
{
  GPIO_InitTypeDef GPIO_InitStructure;	
	
	GPIO_InitStructure.GPIO_Pin=I2C_SDA2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType=GPIO_OType_OD;//开漏输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_25MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);//初始化GPIO
}

/*******************************************************************************
* 函 数 名         : I2C_SDA_IN
* 函数功能		   : SDA输入配置	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void I2C_SDA_IN_2(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	GPIO_InitStructure.GPIO_Pin=I2C_SDA2;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_OD; //复用开漏
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}


void I2C_SDA_in_2(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	GPIO_InitStructure.GPIO_Pin=I2C_SDA2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

//产生起始信号
void I2C_StarT_2(void)
{
    I2C_SDA_OUT_2();
	
	I2C_SDA_H_2;
	I2C_SCL_H_2;
	delay_us(5);
	I2C_SDA_L_2;
	delay_us(6);
	I2C_SCL_L_2;
}

//产生停止信号
void I2C_StoP_2(void)
{
   I2C_SDA_OUT_2();

   I2C_SCL_L_2;
   I2C_SDA_L_2;
   I2C_SCL_H_2;
   delay_us(6);
   I2C_SDA_H_2;
   delay_us(6);
}

//主机产生应答信号ACK
void I2C_Ack_2(void)
{
   I2C_SCL_L_2;
   I2C_SDA_OUT_2();
   I2C_SDA_L_2;
   delay_us(2);
   I2C_SCL_H_2;
   delay_us(5);
   I2C_SCL_L_2;
}

//主机不产生应答信号NACK
void I2C_NAck_2(void)
{
   I2C_SCL_L_2;
   I2C_SDA_OUT_2();
   I2C_SDA_H_2;
   delay_us(2);
   I2C_SCL_H_2;
   delay_us(5);
   I2C_SCL_L_2;
}
//等待从机应答信号
//返回值：1 接收应答失败
//		  0 接收应答成功
u8 I2C_Wait_Ack_2(void)
{
	u8 tempTime=0;

	I2C_SDA_IN_2();
	I2C_SDA_H_2;
	delay_us(1);
	I2C_SCL_H_2;
	delay_us(1);

	while(GPIO_ReadInputDataBit(GPIO_I2C2,I2C_SDA2))
	{
		tempTime++;
		delay_us(1);
		if(tempTime>250)
		{
			I2C_StoP_2();
			return 1;
		}	 
	}
	I2C_SCL_L_2;
	delay_us(1);
	return 0;
}
//I2C 发送一个字节
void I2C_Send_Byte_2(u8 txd)
{
	u8 i=0;

	I2C_SDA_OUT_2();

	for(i=0;i<8;i++)
	{
		I2C_SCL_L_2;//拉低时钟开始数据传输
		if((txd&0x80)>0) //0x80  1000 0000
			I2C_SDA_H_2;
		else
			I2C_SDA_L_2;

		txd<<=1;
		delay_us(1);
		I2C_SCL_H_2;
		delay_us(2); //发送数据
		I2C_SCL_L_2;
		delay_us(2);
	}
		I2C_SCL_L_2;
		delay_us(2);
		I2C_NAck_2();
}

//I2C 读取一个字节

u8 I2C_Read_Byte_2(u8 ack)
{
   u8 i=0,receive=0;

   I2C_SDA_in_2();
   for(i=0;i<8;i++)
   {
   		I2C_SCL_L_2;
		delay_us(2);
		I2C_SCL_H_2;
		while(!GPIO_ReadInputDataBit(GPIO_I2C2,I2C_SCL2));
		receive<<=1;
		if(GPIO_ReadInputDataBit(GPIO_I2C2,I2C_SDA2))
		   receive++;
		delay_us(1);	
   }

   	if(ack==0)
	   	I2C_NAck_2();
	else
		I2C_Ack_2();

	return receive;
}