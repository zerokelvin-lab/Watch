#ifndef _iic_H
#define _iic_H

#include "sys.h"

//#include "stdio.h"
//#include "usart.h"
//#include "string.h"

//如果移植程序时只要改一下三个地方就行了
/* 定时使用的IO口 */
#define I2C_SCL GPIO_Pin_4	  //PA4
#define I2C_SDA GPIO_Pin_5	  //PA5
#define GPIO_I2C GPIOA

#define I2C_SCL_H GPIO_SetBits(GPIO_I2C,I2C_SCL)
#define I2C_SCL_L GPIO_ResetBits(GPIO_I2C,I2C_SCL)

#define I2C_SDA_H GPIO_SetBits(GPIO_I2C,I2C_SDA)
#define I2C_SDA_L GPIO_ResetBits(GPIO_I2C,I2C_SDA)



/* 声明全局函数 */
void I2C_INIT(void);
void I2C_SDA_OUT(void);
void I2C_SDA_IN(void);
void I2C_StarT(void);
void I2C_StoP(void);
void I2C_Ack(void);
void I2C_NAck(void);
u8   I2C_Wait_Ack(void);
void   I2C_Send_Byte(u8 txd);
u8   I2C_Read_Byte(u8 ack);
void GXHT30_read_result(u8 addr);
void I2C_SDA_in(void);
#endif
