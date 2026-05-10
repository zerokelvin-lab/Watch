/** @file lcd.h @brief LCD显示驱动头文件 - 绘图函数声明与颜色宏定义 */

#ifndef __LCD_H
#define __LCD_H
#include "sys.h"

/* ====== LCD刷新完成回调函数类型 ====== */
typedef void (*LCD_CallbackFunc_t)(void);

/* ====== 绘图函数声明 ====== */
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);//在指定区域内填充纯色
void LCD_Color_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 *color);//使用DMA在指定区域内填充颜色
void LCD_Set_Flush_Complete_Callback(LCD_CallbackFunc_t cb);//设置LCD刷新完成回调函数
void LCD_DrawPoint(u16 x,u16 y,u16 color);//在指定位置画一个点
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);//在指定位置画一条线（Bresenham算法）
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);//在指定位置画一个矩形
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color);//在指定位置画一个圆（中点画圆法）

/* ====== 汉字显示函数声明 ====== */
void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示汉字字符串
void LCD_ShowChinese12x12(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个12x12汉字
void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个16x16汉字
void LCD_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个24x24汉字
void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个32x32汉字

/* ====== ASCII字符/字符串显示函数声明 ====== */
void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode);//显示一个字符
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode);//显示字符串
u32 mypow(u8 m,u8 n);//幂运算辅助函数
void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey);//显示整数变量
void LCD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey);//显示两位小数变量

/* ====== 图片显示函数声明 ====== */
void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[]);//显示图片

/* ====== RGB565 颜色预定义 ====== */
//画笔颜色常量（16位RGB565格式：R[15:11], G[10:5], B[4:0]）
#define WHITE         	 0xFFFF  // 白色
#define BLACK         	 0x0000  // 黑色
#define BLUE           	 0x001F  // 蓝色
#define BRED             0XF81F  // 棕红色
#define GRED 			       0XFFE0  // 灰红色
#define GBLUE			       0X07FF  // 灰蓝色
#define RED           	 0xF800  // 红色
#define MAGENTA       	 0xF81F  // 品红/洋红色
#define GREEN         	 0x07E0  // 绿色
#define CYAN          	 0x7FFF  // 青色
#define YELLOW        	 0xFFE0  // 黄色
#define BROWN 			     0XBC40  // 棕色
#define BRRED 			     0XFC07  // 棕红色
#define GRAY  			     0X8430  // 灰色
#define DARKBLUE      	 0X01CF  // 深蓝色
#define LIGHTBLUE      	 0X7D7C  // 浅蓝色
#define GRAYBLUE       	 0X5458  // 灰蓝色
#define LIGHTGREEN     	 0X841F  // 浅绿色
#define LGRAY 			     0XC618  // 浅灰色(PANNEL)，面板背景色
#define LGRAYBLUE        0XA651  // 浅灰蓝色（中间层颜色）
#define LBBLUE           0X2B12  // 浅棕蓝色（选择项的反色）

#endif
