#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"


void TIM2_Init(u16 arr,u16 psc);
void TIM3_Init(u16 arr,u16 psc);


typedef struct
{
	_Bool bT2ms;
	_Bool bT10ms;
	_Bool bT50ms;
	_Bool bTimer1s_Flag;
	u8 Time2msCnt;
	u8 Time10msCnt;
	u8 Time50msCnt;
	
}Timer_TypeDef;

extern Timer_TypeDef Timer_Structure;

#endif
