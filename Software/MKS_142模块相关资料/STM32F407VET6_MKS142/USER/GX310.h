#ifndef __GX310_H__
#define __GX310_H__

#include "sys.h"



#define GX310X0_address  0x90


#define GX310_Temp  0x00
#define GX310_Conf  0x01
#define GX310_Tlow  0x02
#define GX310_Thigh 0x03

#define Conf_def_MSB  0x00  //1000 0100
#define Conf_def_LSB  0x84  //1000 0000


void GX310_Write_conf(u8 add);
float GX310_Read_Res(u8 add);
void GX310_Write_pc(u8 add);
float GX310_Read_pc(u8 add);
uint16_t read_temperature(void);

#endif
