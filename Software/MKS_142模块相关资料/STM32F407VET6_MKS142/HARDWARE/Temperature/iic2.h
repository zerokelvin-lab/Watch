#ifndef __IIC2_H__
#define __IIC2_H__


#include "sys.h"


#define I2C_SCL2 GPIO_Pin_2	  //PA2
#define I2C_SDA2 GPIO_Pin_3	  //PA3
#define GPIO_I2C2 GPIOA

#define I2C_SCL_H_2 GPIO_SetBits(GPIO_I2C2,I2C_SCL2)
#define I2C_SCL_L_2 GPIO_ResetBits(GPIO_I2C2,I2C_SCL2)

#define I2C_SDA_H_2 GPIO_SetBits(GPIO_I2C2,I2C_SDA2)
#define I2C_SDA_L_2 GPIO_ResetBits(GPIO_I2C2,I2C_SDA2)


void I2C_INIT_2(void);
void I2C_SDA_OUT_2(void);
void I2C_SDA_IN_2(void);
void I2C_SDA_in_2(void);
void I2C_StarT_2(void);
void I2C_StoP_2(void);
void I2C_Ack_2(void);
void I2C_NAck_2(void);
u8   I2C_Wait_Ack_2(void);
void 	 I2C_Send_Byte_2(u8 txd);
u8   I2C_Read_Byte_2(u8 ack);


#endif
