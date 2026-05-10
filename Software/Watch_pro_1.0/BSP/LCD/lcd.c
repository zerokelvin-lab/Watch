/** @file lcd.c @brief LCD显示驱动 - 画点、线、矩形、圆、汉字、字符、图片等显示功能 */

#include "lcd.h"
#include "lcd_init.h"
#include "lcdfont.h"
#include "delay.h"
#include "spi.h"
#include "cmsis_os.h"

/* ====== 显示区域Y轴偏移 ====== */
#define OFFSET_Y 20

/* ====== DMA传输信号量与回调函数 ====== */
extern osSemaphoreId_t DMA_SemaphoreHandle;
static LCD_CallbackFunc_t lcd_ready_cb = NULL;

/* ====== 区域填充函数 ====== */

/**
 * @brief  在指定区域填充纯色（逐点写入方式，速度较慢）
 * @param  xsta 起始X坐标
 * @param  ysta 起始Y坐标
 * @param  xend 结束X坐标
 * @param  yend 结束Y坐标
 * @param  color 要填充的16位RGB565颜色值
 * @retval 无
 */
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{
	u16 i,j;
	LCD_Address_Set(xsta,ysta+OFFSET_Y,xend-1,yend-1+OFFSET_Y);//设置显示范围（带Y轴偏移）
	for(i=ysta;i<yend;i++)
	{
		for(j=xsta;j<xend;j++)
		{
			LCD_WR_DATA(color);
		}
	}
}

/**
 * @brief  在指定区域使用DMA快速填充颜色数据
 * @param  xsta 起始X坐标
 * @param  ysta 起始Y坐标
 * @param  xend 结束X坐标
 * @param  yend 结束Y坐标
 * @param  color_p 指向颜色数据的指针
 * @retval 无
 * @note   使用SPI DMA方式传输，需要等待DMA_Semaphore完成信号
 */
void LCD_Color_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 *color_p)
{
	u16 i,j,width,height;
	width = xend-xsta+1;
	height = yend-ysta+1;
	uint32_t pixel_count = width * height;          // 计算总像素数
	uint32_t byte_count = pixel_count * 2U;          // 计算总字节数（每个像素2字节）

	LCD_Address_Set(xsta,ysta+OFFSET_Y,xend,yend+OFFSET_Y);

	HAL_SPI_Transmit_DMA(&hspi1,(uint8_t*)color_p,byte_count);  // 使用DMA传输颜色数据到LCD
}

/* ====== LCD刷新完成回调 ====== */

/** @brief 设置DMA传输完成后的回调函数 */
// 设置通知的回调函数
void LCD_Set_Flush_Complete_Callback(LCD_CallbackFunc_t cb)
{
	lcd_ready_cb = cb;
}

/** @brief SPI DMA传输完成中断回调，检查BSY标志后调用用户回调 */
// DMA 传输完成中断回调
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi->Instance == SPI1)
	{
		// 确保最后一位数据离开 STM32 的移位寄存器（等待SPI忙标志清除）
		while (hspi->Instance->SR & SPI_FLAG_BSY);
		if (lcd_ready_cb)
		{
			lcd_ready_cb();
		}
	}
}

/* ====== 基础绘图函数 ====== */

/**
 * @brief  在指定位置画一个点
 * @param  x 点的X坐标
 * @param  y 点的Y坐标
 * @param  color 点的16位RGB565颜色
 * @retval 无
 */
void LCD_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_Address_Set(x,y,x,y);//设置光标位置
	LCD_WR_DATA(color);
}

/**
 * @brief  使用Bresenham算法画直线
 * @param  x1 起始X坐标
 * @param  y1 起始Y坐标
 * @param  x2 结束X坐标
 * @param  y2 结束Y坐标
 * @param  color 线的16位RGB565颜色
 * @retval 无
 */
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
	u16 t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向（X轴正向）
	else if (delta_x==0)incx=0;//垂直线，X不移动
	else {incx=-1;delta_x=-delta_x;} //X轴负向
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线，Y不移动
	else {incy=-1;delta_y=-delta_y;} //Y轴负向
	if(delta_x>delta_y)distance=delta_x; //选取最大增量轴作为基本步进方向
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		LCD_DrawPoint(uRow,uCol,color);//画点
		xerr+=delta_x;                   // Bresenham误差累加（X方向）
		yerr+=delta_y;                   // Bresenham误差累加（Y方向）
		if(xerr>distance)                // X方向误差超过阈值，X坐标步进
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)                // Y方向误差超过阈值，Y坐标步进
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

/**
 * @brief  画矩形边框
 * @param  x1 左上角X坐标
 * @param  y1 左上角Y坐标
 * @param  x2 右下角X坐标
 * @param  y2 右下角Y坐标
 * @param  color 矩形边框的16位RGB565颜色
 * @retval 无
 */
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);    // 上边
	LCD_DrawLine(x1,y1,x1,y2,color);    // 左边
	LCD_DrawLine(x1,y2,x2,y2,color);    // 下边
	LCD_DrawLine(x2,y1,x2,y2,color);    // 右边
}

/**
 * @brief  使用中点画圆法绘制圆
 * @param  x0 圆心X坐标
 * @param  y0 圆心Y坐标
 * @param  r  圆的半径
 * @param  color 圆的16位RGB565颜色
 * @retval 无
 */
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	a=0;b=r;                              // 初始化：a从0开始，b从半径开始
	while(a<=b)
	{
		// 利用圆的八对称性，一次画8个点
		LCD_DrawPoint(x0-b,y0-a,color);   // 左下方弧段
		LCD_DrawPoint(x0+b,y0-a,color);   // 右下方弧段
		LCD_DrawPoint(x0-a,y0+b,color);   // 右下弧段
		LCD_DrawPoint(x0-a,y0-b,color);   // 右上弧段
		LCD_DrawPoint(x0+b,y0+a,color);   // 左下弧段
		LCD_DrawPoint(x0+a,y0-b,color);   // 左上弧段
		LCD_DrawPoint(x0+a,y0+b,color);   // 右上方弧段
		LCD_DrawPoint(x0-b,y0+a,color);   // 左上方弧段
		a++;
		if((a*a+b*b)>(r*r))//判断要画的点是否过远（决策参数：a^2+b^2 > r^2则减b）
		{
			b--;
		}
	}
}

/* ====== 汉字显示函数 ====== */

/**
 * @brief  显示汉字字符串
 * @param  x 起始X坐标
 * @param  y 起始Y坐标
 * @param  s 要显示的汉字字符串指针（GB2312编码，每汉字占2字节）
 * @param  fc 字的前景色（16位RGB565）
 * @param  bc 字的背景色（16位RGB565）
 * @param  sizey 字号大小（12/16/24/32）
 * @param  mode 显示模式：0=非叠加（带背景色），1=叠加（只画点，不画背景）
 * @retval 无
 */
void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	while(*s!=0)
	{
		if(sizey==12) LCD_ShowChinese12x12(x,y,s,fc,bc,sizey,mode);
		else if(sizey==16) LCD_ShowChinese16x16(x,y,s,fc,bc,sizey,mode);
		else if(sizey==24) LCD_ShowChinese24x24(x,y,s,fc,bc,sizey,mode);
		else if(sizey==32) LCD_ShowChinese32x32(x,y,s,fc,bc,sizey,mode);
		else return;
		s+=2;    // 每个GB2312汉字占2字节，指针向下移动2字节
		x+=sizey;// X坐标偏移一个字的宽度
	}
}

/** @brief 显示单个12x12点阵汉字（内部函数） */
void LCD_ShowChinese12x12(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j,m=0;
	u16 k;
	u16 HZnum;//汉字数目
	u16 TypefaceNum;//一个字符所占字节大小
	u16 x0=x;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey; // 计算单个汉字点阵所占字节数

	HZnum=sizeof(tfont12)/sizeof(typFNT_GB12);	//统计12x12字库中的汉字数目
	for(k=0;k<HZnum;k++)
	{
		if((tfont12[k].Index[0]==*(s))&&(tfont12[k].Index[1]==*(s+1)))  // 匹配汉字内码
		{
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)       // 遍历点阵数据的每个字节
			{
				for(j=0;j<8;j++)             // 逐位检查每个字节的8个位
				{
					if(!mode)//非叠加方式
					{
						if(tfont12[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);  // 点阵位为1：画前景色
						else LCD_WR_DATA(bc);                              // 点阵位为0：画背景色
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//叠加方式
					{
						if(tfont12[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//画一个点（不画背景）
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}

/** @brief 显示单个16x16点阵汉字（内部函数） */
void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j,m=0;
	u16 k;
	u16 HZnum;//汉字数目
	u16 TypefaceNum;//一个字符所占字节大小
	u16 x0=x;
  TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//统计16x16字库中的汉字数目
	for(k=0;k<HZnum;k++)
	{
		if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
		{
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{
					if(!mode)//非叠加方式
					{
						if(tfont16[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
						else LCD_WR_DATA(bc);
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//叠加方式
					{
						if(tfont16[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//画一个点
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}

/** @brief 显示单个24x24点阵汉字（内部函数） */
void LCD_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j,m=0;
	u16 k;
	u16 HZnum;//汉字数目
	u16 TypefaceNum;//一个字符所占字节大小
	u16 x0=x;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//统计24x24字库中的汉字数目
	for(k=0;k<HZnum;k++)
	{
		if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
		{
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{
					if(!mode)//非叠加方式
					{
						if(tfont24[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
						else LCD_WR_DATA(bc);
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//叠加方式
					{
						if(tfont24[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//画一个点
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}

/** @brief 显示单个32x32点阵汉字（内部函数） */
void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j,m=0;
	u16 k;
	u16 HZnum;//汉字数目
	u16 TypefaceNum;//一个字符所占字节大小
	u16 x0=x;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//统计32x32字库中的汉字数目
	for(k=0;k<HZnum;k++)
	{
		if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
		{
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{
					if(!mode)//非叠加方式
					{
						if(tfont32[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
						else LCD_WR_DATA(bc);
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//叠加方式
					{
						if(tfont32[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//画一个点
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}

/* ====== ASCII字符显示函数 ====== */

/**
 * @brief  显示单个ASCII字符
 * @param  x 起始X坐标
 * @param  y 起始Y坐标
 * @param  num 要显示的ASCII字符
 * @param  fc 字的前景色（16位RGB565）
 * @param  bc 字的背景色（16位RGB565）
 * @param  sizey 字号大小（12/16/24/32）
 * @param  mode 显示模式：0=非叠加（带背景色），1=叠加（只画点，不画背景）
 * @retval 无
 */
void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 temp,sizex,t,m=0;
	u16 i,TypefaceNum;//一个字符所占字节大小
	u16 x0=x;
	sizex=sizey/2;                             // ASCII字符宽度为高度的一半
	TypefaceNum=(sizex/8+((sizex%8)?1:0))*sizey;
	num=num-' ';    //得到偏移后的值（以空格为起点索引）
	LCD_Address_Set(x,y,x+sizex-1,y+sizey-1);  //设置光标位置
	for(i=0;i<TypefaceNum;i++)
	{
		if(sizey==12)temp=ascii_1206[num][i];		       //调用6x12字体
		else if(sizey==16)temp=ascii_1608[num][i];		 //调用8x16字体
		else if(sizey==24)temp=ascii_2412[num][i];		 //调用12x24字体
		else if(sizey==32)temp=ascii_3216[num][i];		 //调用16x32字体
		else return;
		for(t=0;t<8;t++)
		{
			if(!mode)//非叠加模式
			{
				if(temp&(0x01<<t))LCD_WR_DATA(fc);
				else LCD_WR_DATA(bc);
				m++;
				if(m%sizex==0)
				{
					m=0;
					break;
				}
			}
			else//叠加模式
			{
				if(temp&(0x01<<t))LCD_DrawPoint(x,y,fc);//画一个点
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y++;
					break;
				}
			}
		}
	}
}

/**
 * @brief  显示ASCII字符串
 * @param  x 起始X坐标
 * @param  y 起始Y坐标
 * @param  p 要显示的ASCII字符串指针
 * @param  fc 字的前景色（16位RGB565）
 * @param  bc 字的背景色（16位RGB565）
 * @param  sizey 字号大小（12/16/24/32）
 * @param  mode 显示模式：0=非叠加，1=叠加
 * @retval 无
 */
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	while(*p!='\0')
	{
		LCD_ShowChar(x,y,*p,fc,bc,sizey,mode);
		x+=sizey/2;  // 下一个字符的X偏移
		p++;
	}
}

/* ====== 数字显示辅助函数 ====== */

/**
 * @brief  计算 m 的 n 次幂
 * @param  m 底数
 * @param  n 指数
 * @retval m^n 的值
 */
u32 mypow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)result*=m;
	return result;
}

/**
 * @brief  显示无符号整数
 * @param  x 起始X坐标
 * @param  y 起始Y坐标
 * @param  num 要显示的整数
 * @param  len 显示的总位数（不足位前面补空格）
 * @param  fc 字的前景色（16位RGB565）
 * @param  bc 字的背景色（16位RGB565）
 * @param  sizey 字号大小（12/16/24/32）
 * @retval 无
 */
void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey)
{
	u8 t,temp;
	u8 enshow=0;
	u8 sizex=sizey/2;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;  // 从高位到低位逐位取出数字
		if(enshow==0&&t<(len-1))           // 消隐前导零
		{
			if(temp==0)
			{
				LCD_ShowChar(x+t*sizex,y,' ',fc,bc,sizey,0);  // 显示空格代替前导零
				continue;
			}else enshow=1;

		}
	 	LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);      // 显示数字（ASCII码偏移48）
	}
}

/**
 * @brief  显示两位小数的浮点数（乘以100后作为整数显示，并插入小数点）
 * @param  x 起始X坐标
 * @param  y 起始Y坐标
 * @param  num 要显示的浮点数
 * @param  len 显示的总位数（不含小数点）
 * @param  fc 字的前景色（16位RGB565）
 * @param  bc 字的背景色（16位RGB565）
 * @param  sizey 字号大小（12/16/24/32）
 * @retval 无
 */
void LCD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey)
{
	u8 t,temp,sizex;
	u16 num1;
	sizex=sizey/2;
	num1=num*100;                             // 将浮点数乘以100转为整数，保留两位小数
	for(t=0;t<len;t++)
	{
		temp=(num1/mypow(10,len-t-1))%10;
		if(t==(len-2))                         // 在倒数第二位后插入小数点
		{
			LCD_ShowChar(x+(len-2)*sizex,y,'.',fc,bc,sizey,0);
			t++;
			len+=1;
		}
	 	LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
	}
}

/* ====== 图片显示函数 ====== */

/**
 * @brief  显示图片（逐像素写入）
 * @param  x 起始X坐标
 * @param  y 起始Y坐标
 * @param  length 图片长度（宽度/X方向）
 * @param  width  图片宽度（高度/Y方向）
 * @param  pic 图片数据数组（16位RGB565格式，每像素2字节）
 * @retval 无
 */
void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[])
{
	u16 i,j;
	u32 k=0;
	LCD_Address_Set(x,y,x+length-1,y+width-1);
	for(i=0;i<length;i++)
	{
		for(j=0;j<width;j++)
		{
			LCD_WR_DATA8(pic[k*2]);    // 写入像素高字节
			LCD_WR_DATA8(pic[k*2+1]);  // 写入像素低字节
			k++;
		}
	}
}
