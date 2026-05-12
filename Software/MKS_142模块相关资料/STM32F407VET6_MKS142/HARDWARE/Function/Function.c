#include "function.h"
#include "lcd.h"
#include "usart.h"
#include "delay.h"
#include "GX310.h"
#include "key.h"
#include "string.h"
#include <stdlib.h>

Beep_TypeDef Beep_Structure;
Temp_TypeDef Temp_Structure;

u8 temp_xdata;


typedef struct
{
	u8 	systolic_pressure;
	u8 	diastolic_pressure;
	u8 	mean_preaaure;
	u8 	pressure_cnt;
	u8 	s_pres_cnt;
	u8 	d_press_cnt;
	u16 	s_pres_sum;
	u16 	d_pres_sum;
	u8 	s_pres_res;
	u8 	d_pres_res;
	u8 array_dias_buff[3];
	u8 array_sys_buff[3];
	
	
}Pressure_TypeDef;

Pressure_TypeDef	Pressure_Structure;
		
void Display_update(void)
{ 
	
		Temp_Structure.temperature = read_temperature();
		
		Temp_Structure.temperaturebuff1 = Temp_Structure.temperature/100;//温度显示整数部分
		Temp_Structure.temperaturebuff2 = Temp_Structure.temperature%100;//温度显示小数部分
	
		Temp_Structure.tempbuf1current	=	Temp_Structure.temperaturebuff1;
		Temp_Structure.tempbuf2current	=	Temp_Structure.temperaturebuff2;
		if(Temp_Structure.tempbuf1last!=Temp_Structure.tempbuf1current)
		{
			Temp_Structure.tempbuf1count++;
			if(Temp_Structure.tempbuf1count>=80)
			{
				Temp_Structure.temperature = 0;
				Temp_Structure.tempbuf1last=Temp_Structure.tempbuf1current;
				Temp_Structure.temperatureresult[0] = Temp_Structure.temperaturebuff1;//体温整数部分
			}
		}
		if(Temp_Structure.tempbuf2last!=Temp_Structure.tempbuf2current)
		{
			Temp_Structure.tempbuf2count++;
			if(Temp_Structure.tempbuf2count>=100)
			{
				Temp_Structure.tempbuf2count = 0;
				Temp_Structure.tempbuf2last=Temp_Structure.tempbuf2current;
				Temp_Structure.temperatureresult[1] = Temp_Structure.temperaturebuff2;//体温小数部分
			}
		}

	
		POINT_COLOR=RED;												//更新传感器参数

		if(b_Tempwork)		//开始检测
		{
//			LCD_ShowChar(125,120,'.',16,0);
//			LCD_ShowNum(108,120,Temp_Structure.temperatureresult[0],2,16);//体温整数部分
//			LCD_ShowNum(132,120,Temp_Structure.temperatureresult[1],2,16);//体温小数部分
//	

			
			LCD_ShowNum(54,100,Data50buf[65],2,16);//心率
			LCD_ShowNum(110,100,Data50buf[66],2,16);//血氧
			LCD_ShowNum(54,120,Data50buf[67],2,16);//微循环
			LCD_ShowNum(46,140,Data50buf[71],3,16);//收缩压
			LCD_ShowNum(125,140,Data50buf[72],2,16);//舒张压
			LCD_ShowNum(125,160,Data50buf[75],2,16);//RR期间
			LCD_ShowNum(210,140,Pressure_Structure.mean_preaaure,3,16);//平均血压
			
			
			LCD_ShowNum(54,160,Data50buf[68],2,16);//疲劳值
			LCD_ShowNum(217,100,Data50buf[73],2,16);//心输出量
			LCD_ShowNum(217,120,Data50buf[74],2,16);//外周阻力
			LCD_ShowNum(224,160,Data50buf[77],2,16);//RMSSD  心率变异性
		
		}
		else							//停止
		{
			LCD_ShowChar(125,120,' ',16,0);
			LCD_ShowChar(132,120,' ',16,0);
			LCD_ShowChar(140,120,' ',16,0);
			LCD_ShowNum(108,120,00,2,16);
			
			LCD_ShowNum(54,100,00,2,16);//心率
			LCD_ShowNum(110,100,00,2,16);//血氧
			LCD_ShowNum(54,120,00,2,16);//微循环
			LCD_ShowNum(46,140,0,3,16);//收缩压
			LCD_ShowNum(125,140,00,2,16);//舒张压
			LCD_ShowNum(125,160,00,2,16);//RR期间
			LCD_ShowNum(210,140,0,3,16);//平均血压
			
			LCD_ShowNum(54,160,00,2,16);//疲劳值
			LCD_ShowNum(217,100,00,2,16);//疲劳值
			LCD_ShowNum(217,120,00,2,16);//疲劳值
			LCD_ShowNum(224,160,00,2,16);//RMSSD  心率变异性
			
			memset(Data50buf,0,sizeof(Data50buf));
			
			Port_DrawCurve(temp_xdata,2);
			temp_xdata++;
			if(temp_xdata >= 240)
			{
					temp_xdata = 0;
			}
		}	
}



void Beep_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOE时钟

  //GPIOE6初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//蜂鸣器对应IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIO  PE6
}
/*---------------------------
4KHZ蜂鸣器驱动
// 	mode ：模式选择-- 1是一次性响，2是连续响，3是持续响
//time ：取值1--255，鸣响的时间 time*100ms。 
//	number ：连续响的次数。
//	spacetime : 间隔时间
------------------------------*/


void Beep_Set(u8 mode,u8 time,u8 number,u8 spacetime)  //设置鸣响参数
{
	Beep_Structure.b_Beep = 1;
	Beep_Structure.Mode = mode;
	Beep_Structure.Time = time;
	Beep_Structure.Number = number;
	Beep_Structure.Spacetime = spacetime;
	Beep_Structure.BeepCount = 0;
	Beep_Structure.BeeptimeCount = 0;
	Beep_Structure.BeepspacetimeCount = 0;
}

void Beep_Drive(void)  //蜂鸣器驱动 在时间函数 10ms 里调用
{
	if(Beep_Structure.b_Beep||Beep_Structure.bSpace )
	{
		Beep_Structure.Beep10msCount++;
		if( Beep_Structure.Beep10msCount>=10 )
		{
			Beep_Structure.Beep10msCount = 0;
			if(Beep_Structure.b_Beep)
			{
				if( Beep_Structure.Mode == 3 ) // 持续响
				{
					return;  // 持续响的要在其他函数执行关闭 bBuzzer 标志位
				}
				else if( Beep_Structure.Mode == 1 ) // 一次性响
				{
					Beep_Structure.BeeptimeCount ++;
					if( Beep_Structure.BeeptimeCount >= Beep_Structure.Time )
					{
						Beep_Structure.Time = 0;
						Beep_Structure.b_Beep = 0;
					}
				}
				else if( Beep_Structure.Mode == 2 ) // 连续响
				{
					Beep_Structure.BeeptimeCount ++;
					if( Beep_Structure.BeeptimeCount >= Beep_Structure.Time )
					{
						Beep_Structure.BeeptimeCount = 0;
						Beep_Structure.BeepCount ++;
						Beep_Structure.b_Beep = 0;
						if( Beep_Structure.BeepCount >= Beep_Structure.Number )
						{
							Beep_Structure.BeepCount = 0;
							Beep_Structure.Number = 0;			
						}
						else
						{
							Beep_Structure.bSpace = 1;	
						}
					}
				}
			}
			if( Beep_Structure.bSpace )
			{
				Beep_Structure.BeepspacetimeCount ++;
				if( Beep_Structure.BeepspacetimeCount >= Beep_Structure.Spacetime )
				{
					Beep_Structure.BeepspacetimeCount = 0;
					Beep_Structure.bSpace = 0;
					Beep_Structure.b_Beep = 1;
				}
			}
		}
	}
	else
	{
		Beep_Structure.Beep10msCount = 0;
	}
}




void DisplayWaveform_update(void)//更新脉搏波数据
{
	static u8 	i=1;
	static u16 	temp_x,buff_y;
	
	
	if(!b_Tempwork)			//判断是否开始检测
	{
		return;
	}
	
	if(b_Star)
		{
			if(Data50buf[66]<=89)
			{
				temp_x = 0;
				Port_DrawCurve(temp_x,0);
				return;
			}
			
					temp_x++;
					i++;
					if(i>=60)
					{
						i = 2;	
					}
					if(temp_x >= 240)
					{
							temp_x = 0;
					}
					if(Data50buf[i]>=250)
					{
						Data50buf[i] = 128;
					}

					buff_y=(int8_t)Data50buf[i]+140;

						Port_DrawCurve(temp_x,buff_y);

		}
}


void Sensor_Handle(void)  //数据异常报警检测
{
	static u8 heartcount,sopcount;
	static u8 heartcurrent,heartlast;
	static u8 sopcurrent,soplast;
	
	
	if(!b_Tempwork)			//判断是否开始检测
	{
		return;
	}
	POINT_COLOR=RED;
	
//	if(Temp_Structure.temperatureresult[0]>=38)//体温异常
//	{
//		Temp_Structure.tempcount++;
//		if(Temp_Structure.tempcount>=20)
//		{
//			Temp_Structure.tempcount = 0;
//			Beep_Set(2,255,0,0);

//			LCD_ShowChinese(40,190,6,16,0);//体温异常
//			LCD_ShowChinese(40+16,190,7,16,0);
//			LCD_ShowChinese(40+2*16,190,14,16,0);
//			LCD_ShowChinese(40+3*16,190,15,16,0);
//		}
//	
//	}
	
	if(Data50buf[65]>20)
	{
		if(Data50buf[65]<50||Data50buf[65]>=100)//心率异常
		{
			heartcurrent = Data50buf[40];
			if(heartlast!=heartcurrent)
			{
				heartcount++;
				heartlast=heartcurrent;
			}
			if(heartcount>=4)
			{
				heartcount = 0;
				Beep_Set(2,255,0,0);
				LCD_ShowChinese(40,190,0,16,0);//心率异常
				LCD_ShowChinese(40+16,190,1,16,0);
				LCD_ShowChinese(40+2*16,190,14,16,0);
				LCD_ShowChinese(40+3*16,190,15,16,0);
			}
		}
	
	}
	
	
	
	if(Data50buf[66]<=92&&Data50buf[66]>=20)//血氧异常
	{
		sopcurrent = Data50buf[66];
		if(soplast!=sopcurrent)
		{
			sopcount++;
			soplast=sopcurrent;
		}
			
		if(sopcount>=20)
		{
			sopcount = 0;
			Beep_Set(2,255,0,0);
		
			LCD_ShowChinese(40,190,2,16,0);//血氧过低
			LCD_ShowChinese(40+16,190,3,16,0);
			LCD_ShowChinese(40+2*16,190,33,16,0);
			LCD_ShowChinese(40+3*16,190,34,16,0);
		}
	}
	
}

void LED_Handle(void)
{
	if(b_WorkFlag)
	{
			b_WorkFlag = 0;
			GPIO_WriteBit(GPIOA,GPIO_Pin_7,(BitAction)(1-GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_7)));
	}
}



void Usart_Handle(void)
{
	static u8 i;
	static u8 databuflast,databufcurrent;
	
	databufcurrent = Data50buf[1];
	
		if(databuflast==databufcurrent)
		{
			i++;
		}
		else
		{
			databuflast=databufcurrent;
			i=0;
		}
		if(i>=200)
		{
			i = 0;
			b_Star = 0;
		}
}

int compare(const void *a, const void *b)
{
    return (*(int*)a - *(int*)b);
}



void blood_pressure(u8 point1,u8 point2)
{
	static u8 i,j;

	
	Pressure_Structure.systolic_pressure = point1;//Data50buf[71];//收缩压
	Pressure_Structure.diastolic_pressure = point2;//Data50buf[72];//舒张压
	
	
	if((Pressure_Structure.systolic_pressure>=100)&&(Pressure_Structure.systolic_pressure<=140))  //收缩压有效区间
	{
		Pressure_Structure.array_sys_buff[i]=Pressure_Structure.systolic_pressure;
		i++;
		if(i>=3)
		{
			 qsort(Pressure_Structure.array_sys_buff,3,sizeof(int),compare);
				i=0;
				Pressure_Structure.s_pres_cnt++;
				Pressure_Structure.s_pres_sum+=Pressure_Structure.array_sys_buff[1];  //取中位值
		}
		if(Pressure_Structure.s_pres_cnt>=4)
		{
			Pressure_Structure.s_pres_res = Pressure_Structure.s_pres_sum >> 2; //平均
			Pressure_Structure.s_pres_sum = 0;
			Pressure_Structure.s_pres_cnt = 0;
		}
	}	
	
	if((Pressure_Structure.diastolic_pressure>=70)&&(Pressure_Structure.diastolic_pressure<=100))//舒张压有效区间
	{
		Pressure_Structure.array_dias_buff[j] = Pressure_Structure.diastolic_pressure;
		j++;
		if(j>=3)
		{
			qsort(Pressure_Structure.array_dias_buff,3,sizeof(int),compare);
			j = 0;
			Pressure_Structure.d_press_cnt++;
			Pressure_Structure.d_pres_sum +=Pressure_Structure.array_dias_buff[1];
		}
		if(Pressure_Structure.d_press_cnt>=4)
		{
			Pressure_Structure.d_press_cnt = 0;
			Pressure_Structure.d_pres_res = Pressure_Structure.d_pres_sum >> 2;
			Pressure_Structure.d_pres_sum = 0;
		}
	}
	
	Pressure_Structure.mean_preaaure = ((Pressure_Structure.d_pres_res*2)+Pressure_Structure.s_pres_res)/3;
	if((Data50buf[65]==0)&&(Data50buf[66]==0))
	{
		Pressure_Structure.mean_preaaure = 0;
	}
	
	
	//printf("mean_preaaure=%d\r\n",Pressure_Structure.mean_preaaure);
}
