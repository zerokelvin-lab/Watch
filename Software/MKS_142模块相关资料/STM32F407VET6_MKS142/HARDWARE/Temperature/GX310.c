#include "GX310.h"
#include "iic.h"
#include "iic2.h"

#define write 0
#define read  1

float t_sensor = 0;         //经过补偿后的皮肤温度
//float t_sensor_temp = 0;    //传感器测量温度缓存
float t_display = 0;        //app显示的温度值
float t_predict = 0;        //预测的人体温度值
float t_temp80 = 0;         //预测的人体温度值
float t_environment =0;     //环境温度
float t_watch = 0;          //皮肤温度；
int n=0;    //预测值计数



void GX310_Write_conf(u8 add)   //
{
    I2C_StarT();              //开始
    I2C_Send_Byte(add|write);
    I2C_Send_Byte(GX310_Conf);
    I2C_Send_Byte(Conf_def_MSB);
    I2C_Send_Byte(Conf_def_LSB);
    I2C_StoP();               //停止
}

float GX310_Read_Res(u8 add)
{
    uint8_t date[2];
    uint16_t sensor_res;
    float result;
	
    I2C_StarT();
    I2C_Send_Byte(add|write); //写地址 10010110 =0x90  
    I2C_Send_Byte(GX310_Temp);  //读区域 0x00

    I2C_StarT();
    I2C_Send_Byte(add|read); //读地址 10010001 =0x91 

    date[0] = I2C_Read_Byte(1);
    date[1] = I2C_Read_Byte(0);

	
    I2C_StoP();

    sensor_res = (date[0]<<8)|date[1];

    result = sensor_res*0.0078125;
 
    return result;
 
}

void GX310_Write_pc(u8 add)
{
    I2C_StarT_2();              //开始
    I2C_Send_Byte_2(add|write);
    I2C_Send_Byte_2(GX310_Conf);
    I2C_Send_Byte_2(Conf_def_MSB);
    I2C_Send_Byte_2(Conf_def_LSB);
    I2C_StoP_2();               //停止
}

float GX310_Read_pc(u8 add)
{
    uint8_t date[2];
    uint16_t sensor_res;
    float result;
	
    I2C_StarT_2();
    I2C_Send_Byte_2(add|write); //写地址 10010110 =0x90  
    I2C_Send_Byte_2(GX310_Temp);  //读区域 0x00

    I2C_StarT_2();
    I2C_Send_Byte_2(add|read); //读地址 10010001 =0x91 

    date[0] = I2C_Read_Byte_2(1);
    date[1] = I2C_Read_Byte_2(0);

    I2C_StoP_2();

    sensor_res = (date[0]<<8)|date[1];

    result = sensor_res*0.0078125;
    return result;//2700
}


uint16_t read_temperature(void){
	
	t_watch= GX310_Read_Res(GX310X0_address);//皮肤温度,IIC1
	t_environment= GX310_Read_pc(GX310X0_address);//环境温度,IIC2
	
	if(t_watch - t_environment > 2.7)
        t_sensor = t_watch + 0.15*(t_watch - t_environment - 2.7)*(t_watch - t_environment - 2.7);
      else 
        t_sensor = t_watch - 0.27*(t_watch - t_environment - 2.7)*(t_watch - t_environment - 2.7); 

    if(t_watch > 28)           //如果上一次测温大于28，那么开始计数
     {
       n = n + 1 ;
     }
     else if (n == 2000)
     {
         n = 2000;
     }
     else
     {
       n = 0;
     }
     if(n==0)
     {
      t_display = t_watch; 
      t_predict = 0;
     }
     else if((n > 0) && (n <20))
     {
       t_display = t_watch;
     }
     else if (n==20)
     {
       t_display = t_watch;
       t_temp80   = t_watch;
       if ((36 < t_watch )&& (t_watch <= 41 ))
	        t_predict = 36.9 + (t_watch-36)*4.1/5;
       else if ((30 < t_watch)&& (t_watch <= 36 ))
	        t_predict = 36.2 + (t_watch-30)*0.7/6; 
       else if ((28 < t_watch)&& (t_watch <= 30 ))
             t_predict = 35 + (t_watch - 28)*1.2/2;
       else 
             t_predict = t_watch;
       
     }
     else if((n> 20) && (n<= 25))
     {
      t_display = ((n-20)*0.8*(t_predict - t_temp80))/5 + t_temp80;  
      
     }
     
     else if((n> 25) && (n<= 30))
     {
      t_display = t_predict-((30-n)*0.2*(t_predict - t_temp80))/5;
     }

     else
     {
       if ((36 < t_sensor )&& (t_sensor <= 41 ))
	        t_display = 36.9 + (t_sensor-36)*4.1/5;
       else if ((30 < t_sensor)&& (t_sensor <= 36 ))
	        t_display = 36.2 + (t_sensor-30)*0.7/6; 
       else if ((28 < t_sensor)&& (t_sensor <= 30 ))
             t_display = 35 + (t_sensor - 28)*1.2/2;
       else 
             t_display = t_sensor;
	   
     }
			return (int)(t_display*100);
}
