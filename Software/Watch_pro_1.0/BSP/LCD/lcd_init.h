/** @file lcd_init.h @brief LCD初始化头文件 - 屏幕尺寸定义、引脚宏定义、初始化函数声明 */

#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "sys.h"

/* ====== 屏幕方向配置 ====== */
// 设置屏幕横竖屏及显示方向：0=竖屏正向，1=竖屏翻转，2=横屏正向，3=横屏翻转
#define USE_HORIZONTAL 0

/* ====== 屏幕分辨率定义 ====== */
#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 240    // 竖屏模式：宽度240
#define LCD_H 280    // 竖屏模式：高度280
#else
#define LCD_W 280    // 横屏模式：宽度280
#define LCD_H 240    // 横屏模式：高度240
#endif

/* ====== LCD端口定义（GPIO引脚映射） ====== */
// SPI时钟引脚
#define SCLK_PORT			GPIOB
#define SCLK_PIN			GPIO_PIN_3

// SPI数据引脚（MOSI）
#define SDA_PORT			GPIOB
#define SDA_PIN				GPIO_PIN_5

// LCD复位引脚
#define RES_PORT			GPIOB
#define RES_PIN				GPIO_PIN_7

// LCD数据/命令选择引脚（DC: Data/Command）
#define DC_PORT				GPIOB
#define DC_PIN				GPIO_PIN_9

// LCD片选引脚
#define CS_PORT				GPIOB
#define CS_PIN				GPIO_PIN_8

// LCD背光控制引脚
#define BLK_PORT			GPIOB
#define BLK_PIN				GPIO_PIN_0

/* ====== GPIO引脚操作宏 ====== */
#define LCD_SCLK_Clr() HAL_GPIO_WritePin(SCLK_PORT,SCLK_PIN,GPIO_PIN_RESET)//SCL=SCLK，时钟拉低
#define LCD_SCLK_Set() HAL_GPIO_WritePin(SCLK_PORT,SCLK_PIN,GPIO_PIN_SET)  //时钟拉高

#define LCD_MOSI_Clr() HAL_GPIO_WritePin(SDA_PORT,SDA_PIN,GPIO_PIN_RESET)//SDA=MOSI，数据拉低
#define LCD_MOSI_Set() HAL_GPIO_WritePin(SDA_PORT,SDA_PIN,GPIO_PIN_SET)  //数据拉高

#define LCD_RES_Clr()  HAL_GPIO_WritePin(RES_PORT,RES_PIN,GPIO_PIN_RESET)//RES复位拉低
#define LCD_RES_Set()  HAL_GPIO_WritePin(RES_PORT,RES_PIN,GPIO_PIN_SET)  //复位拉高

#define LCD_DC_Clr()   HAL_GPIO_WritePin(DC_PORT,DC_PIN,GPIO_PIN_RESET)//DC拉低=命令模式
#define LCD_DC_Set()   HAL_GPIO_WritePin(DC_PORT,DC_PIN,GPIO_PIN_SET)  //DC拉高=数据模式
 		     
#define LCD_CS_Clr()   HAL_GPIO_WritePin(CS_PORT,CS_PIN,GPIO_PIN_RESET)//CS拉低=选中LCD
#define LCD_CS_Set()   HAL_GPIO_WritePin(CS_PORT,CS_PIN,GPIO_PIN_SET)  //CS拉高=取消选中

#define LCD_BLK_Clr()  HAL_GPIO_WritePin(BLK_PORT,BLK_PIN,GPIO_PIN_RESET)//背光关闭
#define LCD_BLK_Set()  HAL_GPIO_WritePin(BLK_PORT,BLK_PIN,GPIO_PIN_SET)  //背光开启

/* ====== LCD驱动函数声明 ====== */
void LCD_GPIO_Init(void);//初始化GPIO引脚
void LCD_Writ_Bus(u8 dat);//SPI底层写入（硬件SPI方式）
void LCD_WR_DATA8(u8 dat);//写入8位数据
void LCD_WR_DATA(u16 dat);//写入16位数据（RGB565像素）
void LCD_WR_REG(u8 dat);//写入8位命令/寄存器地址
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置显示窗口坐标
void LCD_Init(void);//LCD初始化
void LCD_Set_Light(uint8_t dc);//设置背光亮度（PWM占空比）
void LCD_Close_Light(void);//关闭背光
void LCD_ST7789_SleepIn(void);//ST7789进入休眠
void LCD_ST7789_SleepOut(void);//ST7789退出休眠
void LCD_Open_Light(void);//开启背光
#endif
