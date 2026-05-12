#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include "sys.h"

typedef struct
{
	_Bool b_Beep;
	_Bool bSpace;
	u8 		Mode;
	u8 		Time;
	u8 		Number;
	u8 		Spacetime;
	u8 		BeepCount;
	u8 		Beep10msCount;
	u8 		BeeptimeCount;	
	u8 		BeepspacetimeCount;
}Beep_TypeDef;

extern Beep_TypeDef Beep_Structure;

typedef struct
{
	float temperatureresult[2];
	u8 temperaturebuff1;
	u8 temperaturebuff2;
	u16 temperature;
	u8 	tempcount;
	u8 	tempbuf1current;
	u8	tempbuf1last;
	u8	tempbuf1count;
	u8 	tempbuf2current;
	u8	tempbuf2last;
	u8	tempbuf2count;
	
}Temp_TypeDef;




void Beep_Init(void);
void Beep_Set(u8 mode,u8 time,u8 number,u8 spacetime);
void Beep_Drive(void);
void Display_update(void);
void DisplayWaveform_update(void);
void Sensor_Handle(void);
void LED_Handle(void);
void Usart_Handle(void);
void blood_pressure(u8 point1,u8 point2);


extern u8 temp_xdata;

#endif

