/** @file lcd_init.c @brief LCD初始化驱动 - ST7789 GPIO初始化、SPI通信、背光控制、寄存器配置 */

#include "lcd_init.h"
#include "delay.h"
#include "spi.h"
#include "tim.h"

/* ====== LCD GPIO端口初始化 ====== */

/**
 * @brief  LCD控制端口（RES/CS/DC）初始化
 * @retval 无
 * @note   配置GPIOB的RES（复位）、CS（片选）、DC（数据/命令选择）为推挽输出
 */
void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure = {0};
	
 	__HAL_RCC_GPIOB_CLK_ENABLE();              // 使能GPIOB时钟
	
	GPIO_InitStructure.Pin = RES_PIN|CS_PIN|DC_PIN;  // 复位+片选+数据/命令选择引脚 
 	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;   // 推挽输出模式
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;//速度最高等级
 	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);  // 初始化GPIOB
 	HAL_GPIO_WritePin(GPIOB, RES_PIN|CS_PIN|DC_PIN, GPIO_PIN_SET); // 初始置高电平
}

/* ====== SPI通信函数 ====== */

/**
 * @brief  LCD SPI总线写入一个字节（硬件SPI方式）
 * @param  dat 要写入的8位数据
 * @retval 无
 */
void LCD_Writ_Bus(u8 dat) 
{	
	//hard SPI（使用硬件SPI1发送数据）
	HAL_SPI_Transmit(&hspi1,&dat,1,1);
	
	//soft SPI（软件模拟SPI方式，已注释）
	/*
	u8 i;
	for(i=0;i<8;i++)
	{			  
		LCD_SCLK_Clr();                       // SCLK拉低
		if(dat&0x80)
		{
		   LCD_MOSI_Set();                   // 数据位为1，MOSI拉高
		}
		else
		{
		   LCD_MOSI_Clr();                   // 数据位为0，MOSI拉低
		}
		LCD_SCLK_Set();                       // SCLK拉高，上升沿数据被采样
		dat<<=1;                              // 左移，准备发送下一位
	}	
	*/
}

/**
 * @brief  LCD写入8位数据
 * @param  dat 要写入的8位数据
 * @retval 无
 */
void LCD_WR_DATA8(u8 dat)
{
	LCD_Writ_Bus(dat);
}

/**
 * @brief  LCD写入16位数据（RGB565像素数据）
 * @param  dat 要写入的16位数据
 * @retval 无
 * @note   将16位数据拆分为2个字节通过SPI发送
 */
void LCD_WR_DATA(u16 dat)
{
//	LCD_Writ_Bus(dat>>8);                     // 先发高字节（已废弃方式）
//	LCD_Writ_Bus(dat);                        // 再发低字节
	uint8_t temp[2];
	temp[0]=(dat>>8)&0xff;                    // 取高8位
	temp[1]=dat&0xff;                         // 取低8位
	HAL_SPI_Transmit(&hspi1,temp,2,1);        // 通过SPI一次性发送2字节
}

/**
 * @brief  LCD写入命令寄存器
 * @param  dat 命令值（8位）
 * @retval 无
 * @note   先将DC引脚拉低（命令模式），写入命令字节后再拉高（恢复数据模式）
 */
void LCD_WR_REG(u8 dat)
{
	LCD_DC_Clr();//写命令（DC=0表示命令）
	LCD_Writ_Bus(dat);
	LCD_DC_Set();//写数据（DC=1表示数据）
}

/* ====== 显示区域与背光控制 ====== */

/**
 * @brief  设置LCD的显示窗口区域（列地址和页地址）
 * @param  x1 起始X坐标
 * @param  y1 起始Y坐标
 * @param  x2 结束X坐标
 * @param  y2 结束Y坐标
 * @retval 无
 * @note   通过写入列地址寄存器(0x2A)和行地址寄存器(0x2B)来设定显示窗口
 */
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	LCD_WR_REG(0x2a);//列地址设置寄存器 CASET (Column Address Set)
	LCD_WR_DATA(x1); // 起始列地址
	LCD_WR_DATA(x2); // 结束列地址
	LCD_WR_REG(0x2b);//行地址设置寄存器 PASET (Page Address Set)
	LCD_WR_DATA(y1); // 起始行地址
	LCD_WR_DATA(y2); // 结束行地址
	LCD_WR_REG(0x2c);//存储器写命令 RAMWR (Memory Write)
}

/**
 * @brief  设置LCD背光亮度（PWM占空比控制）
 * @param  dc 占空比百分比（5%~100%）
 * @retval 无
 */
void LCD_Set_Light(uint8_t dc)
{
	if(dc>=5 && dc<=100)
		__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_3,dc*PWM_PERIOD/100); // 设置TIM3通道3的比较值控制PWM占空比
}

/**
 * @brief  关闭LCD背光
 * @retval 无
 */
void LCD_Close_Light(void)
{
	__HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_3,0);                     // 将PWM比较值设为0，关闭背光
	//HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_3);                         // 停止PWM输出（备选方案）
}

/**
 * @brief  开启LCD背光
 * @retval 无
 */
void LCD_Open_Light(void)
{
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);                          // 启动TIM3通道3的PWM输出
}

/* ====== ST7789 休眠/唤醒 ====== */

/**
 * @brief  ST7789进入休眠模式（SLP IN 命令 0x10）
 * @retval 无
 */
void LCD_ST7789_SleepIn(void)
{
	LCD_WR_REG(0x10);                                                 // SLPIN 休眠命令
	delay_ms(100);
}

/**
 * @brief  ST7789退出休眠模式（SLP OUT 命令 0x11）
 * @retval 无
 */
void LCD_ST7789_SleepOut(void)
{
	LCD_WR_REG(0x11);                                                 // SLPOUT 退出休眠命令
	delay_ms(100);
}

/* ====== LCD初始化 ====== */

/**
 * @brief  LCD初始化（ST7789V驱动芯片）
 * @retval 无
 * @note   初始化流程：GPIO初始化 -> 硬件复位 -> 软件复位退出休眠 ->
 *         设置扫描方向 -> 设置像素格式(RGB565) -> 配置时序参数 ->
 *         设置Gamma校正 -> 反色显示 -> 开启显示
 */
void LCD_Init(void)
{
	LCD_GPIO_Init();//初始化GPIO引脚
	LCD_CS_Clr();		//使能片选（CS拉低选中LCD）
	
	// 硬件复位：RES引脚拉低100ms后拉高100ms
	LCD_RES_Clr();	//复位
	delay_ms(100);
	LCD_RES_Set();
	delay_ms(100);
	
	LCD_WR_REG(0x11);      // SLPOUT 退出休眠模式
	delay_ms(120); 
	LCD_WR_REG(0x36);      // MADCTL 内存数据访问控制寄存器
	// 根据 USE_HORIZONTAL 宏设置扫描方向
	// 0x00: 正常方向, 0xC0: 垂直翻转+水平翻转, 0x70: YX交换, 0xA0: YX交换+翻转
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
	else LCD_WR_DATA8(0xA0);

	LCD_WR_REG(0x3A);      // COLMOD 像素格式设置
	LCD_WR_DATA8(0x05);    // 0x05 = 65K色，16bit/pixel (RGB565)

	LCD_WR_REG(0xB2);      // PORCTRL 门电路控制
	LCD_WR_DATA8(0x0C);    // 后沿设置
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x00);    // 前沿设置
	LCD_WR_DATA8(0x33);    // VGH/VGL设置
	LCD_WR_DATA8(0x33); 

	LCD_WR_REG(0xB7);      // GCTRL 门控制
	LCD_WR_DATA8(0x35);    // VGH=14.92V, VGL=-7.67V

	LCD_WR_REG(0xBB);      // VCOMS VCOM设置
	LCD_WR_DATA8(0x19);    // VCOM = -1.35V

	LCD_WR_REG(0xC0);      // LCMCTRL LCM控制
	LCD_WR_DATA8(0x2C);

	LCD_WR_REG(0xC2);      // VDVVRHEN 电源控制
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0xC3);      // VRHS VRH设置
	LCD_WR_DATA8(0x12);   

	LCD_WR_REG(0xC4);      // VDVS VDV设置
	LCD_WR_DATA8(0x20);  

	LCD_WR_REG(0xC6);      // FRCTRL2 帧率控制
	LCD_WR_DATA8(0x0F);    // 60Hz帧率  

	LCD_WR_REG(0xD0);      // PWCTRL1 电源控制1
	LCD_WR_DATA8(0xA4);    // AVDD=6.8V
	LCD_WR_DATA8(0xA1);    // GVDD=4.6V

	// 正极性Gamma校正表（PVGAMCTRL 寄存器 0xE0）
	LCD_WR_REG(0xE0);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x2B);
	LCD_WR_DATA8(0x3F);
	LCD_WR_DATA8(0x54);
	LCD_WR_DATA8(0x4C);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x23);

	// 负极性Gamma校正表（NVGAMCTRL 寄存器 0xE1）
	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x11);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x2C);
	LCD_WR_DATA8(0x3F);
	LCD_WR_DATA8(0x44);
	LCD_WR_DATA8(0x51);
	LCD_WR_DATA8(0x2F);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x20);
	LCD_WR_DATA8(0x23);

	LCD_WR_REG(0x21);      // INVON 反色显示

	LCD_WR_REG(0x29);      // DISPON 开启显示
}
