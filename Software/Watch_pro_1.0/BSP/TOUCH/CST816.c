/** @file CST816.c @brief CST816触摸屏驱动 - I2C通信、寄存器配置、触摸坐标读取、手势识别、休眠唤醒 */

#include "CST816.h" 

/* ====== 触摸坐标Y轴偏移 ====== */
#define TOUCH_OFFSET_Y 0                            // Y轴偏移量（用于校准触摸坐标与显示坐标的偏移）

CST816_Info	CST816_Instance;                         // 触摸屏坐标实例（存储当前触摸位置）

/* ====== CST816 I2C总线配置 ====== */
iic_bus_t CST816_dev =
{
	.IIC_SDA_PORT = GPIOB,                         // SDA数据线：PB4
	.IIC_SCL_PORT = GPIOB,                         // SCL时钟线：PB6
	.IIC_SDA_PIN = GPIO_PIN_4,
	.IIC_SCL_PIN = GPIO_PIN_6,
};

/* ====== 触摸屏GPIO初始化 ====== */

/**
 * @brief  CST816触摸屏GPIO口初始化
 * @retval 无
 * @note   初始化复位引脚(PA15)为推挽输出并拉高
 *         初始化I2C通信引脚(PB4/PB6)
 */
void CST816_GPIO_Init(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();                   // 使能GPIOB时钟（I2C引脚）
	__HAL_RCC_GPIOA_CLK_ENABLE();                   // 使能GPIOA时钟（复位引脚）
	
	/* 初始化复位引脚 (PA15: 推挽输出, 上拉, 高速) */
	GPIO_InitStructure.Pin = TOUCH_RST_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(TOUCH_RST_PORT, &GPIO_InitStructure);
	
	HAL_GPIO_WritePin(TOUCH_RST_PORT,TOUCH_RST_PIN,GPIO_PIN_SET); // 复位引脚拉高（不复位）
	
	/* 初始化中断引脚,不用中断的话屏蔽掉（未使用中断方式，使用轮询方式读取触摸数据） */
	
	/* 初始化I2C引脚 */
	IICInit(&CST816_dev);
	
	/* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
	//IICStop(&CST816_dev);
}

/* ====== 触摸屏初始化 ====== */

/**
 * @brief  CST816触摸屏初始化
 * @retval 无
 * @note   初始化GPIO后配置5秒无触摸自动休眠
 */
void CST816_Init(void)
{
	CST816_GPIO_Init();
	CST816_Config_AutoSleepTime(5);                  // 5秒无触摸自动进入低功耗休眠
}

/* ====== I2C寄存器读写 ====== */

/**
 * @brief  读取触摸屏单个寄存器数据
 * @param  addr 寄存器地址
 * @retval 寄存器存储的数据
 */
uint8_t CST816_IIC_ReadREG(uint8_t addr)
{
	return IIC_Read_One_Byte(&CST816_dev,Device_Addr,addr);
}

/**
 * @brief  向触摸屏的寄存器写入数据
 * @param  addr 寄存器地址
 * @param  dat  写入的数据
 * @retval 无
 */
void CST816_IIC_WriteREG(uint8_t addr, uint8_t dat)
{
	IIC_Write_One_Byte(&CST816_dev,Device_Addr,addr,dat);
}

/* ====== 触摸屏复位 ====== */

/**
 * @brief  触摸屏硬件复位
 * @retval 无
 * @note   拉低复位引脚10ms后拉高100ms完成复位
 */
void CST816_RESET(void)
{
	TOUCH_RST_0;                                     // 复位引脚拉低
	HAL_Delay(10);
	TOUCH_RST_1;                                     // 复位引脚拉高
	HAL_Delay(100);                                  // 等待复位完成
}

/* ====== 触摸坐标读取 ====== */

/**
 * @brief  读取触摸屏在触摸时的坐标值
 * @retval 无
 * @note   连续读取4字节触摸数据：
 *         DAT[0]: X坐标高4位（低4位为手势ID）
 *         DAT[1]: X坐标低8位
 *         DAT[2]: Y坐标高4位
 *         DAT[3]: Y坐标低8位
 *         结果存储在全局结构体 CST816_Instance 中
 */
void CST816_Get_XY_AXIS(void)
{
	uint8_t DAT[4];
	IIC_Read_Multi_Byte(&CST816_dev,Device_Addr,XposH,4,DAT); // 连续读取4字节坐标数据
	CST816_Instance.X_Pos=((DAT[0]&0x0F)<<8)|DAT[1];  // X坐标: DAT[0]低4位<<8 | DAT[1]
	CST816_Instance.Y_Pos=((DAT[2]&0x0F)<<8)|DAT[3] + TOUCH_OFFSET_Y; // Y坐标: DAT[2]低4位<<8 | DAT[3] + 偏移
}

/* ====== 触摸屏状态读取 ====== */

/**
 * @brief  读取触摸屏的手指触摸个数
 * @retval 手指触摸个数（0xFF表示芯片处于睡眠状态）
 */
uint8_t CST816_Get_FingerNum(void)
{
	return CST816_IIC_ReadREG(FingerNum);            // 读取 FingerNum 寄存器(0x02)
}

/**
 * @brief  读取触摸屏的芯片ID
 * @retval 芯片ID值
 */
uint8_t CST816_Get_ChipID(void)
{
	return CST816_IIC_ReadREG(ChipID);               // 读取 ChipID 寄存器(0xA7)
}

/* ====== 触摸屏参数配置 ====== */

/**
 * @brief  使能连续动作检测（连续左右滑动、连续上下滑动、双击）
 * @param  mode 动作模式：
 *         0x00=全部禁用, 0x01=连续左右滑动,
 *         0x02=连续上下滑动, 0x03=双击, 0x07=全部使能
 * @retval 无
 * @note   使能连续动作会增加触摸响应时间
 */
void CST816_Config_MotionMask(uint8_t mode)
{
	CST816_IIC_WriteREG(MotionMask,mode);            // 写入 MotionMask 寄存器(0xEC)
}

/**
 * @brief  设置自动休眠时间
 * @param  time 无触摸超时时间（单位：秒）
 * @retval 无
 * @note   在设定时间内无触摸操作，芯片自动进入低功耗模式
 */
void CST816_Config_AutoSleepTime(uint8_t time)
{
	CST816_IIC_WriteREG(AutoSleepTime,time);         // 写入 AutoSleepTime 寄存器(0xF9)
}

/* ====== 休眠/唤醒 ====== */

/**
 * @brief  触摸屏进入睡眠模式（无触摸唤醒功能）
 * @retval 无
 * @note   写入 SleepMode 寄存器(0xE5) = 0x03 进入深度休眠
 */
void CST816_Sleep(void)
{
	CST816_IIC_WriteREG(SleepMode,0x03);             // 深度休眠模式
}

/**
 * @brief  触摸屏从休眠唤醒
 * @retval 无
 * @note   通过硬件复位引脚实现唤醒
 */
void CST816_Wakeup(void)
{
	CST816_RESET();                                  // 硬件复位唤醒
}

/**
 * @brief  手势检测滑动分区角度控制
 * @param  x_right_y_up_angle 角度值
 *         Angle = tan(c)*10，其中c为以X轴正方向为基准的角度
 * @retval 无
 */
void CST816_Config_MotionSlAngle(uint8_t x_right_y_up_angle)
{
	CST816_IIC_WriteREG(MotionSlAngle,x_right_y_up_angle); // 写入 MotionSlAngle 寄存器(0xEF)
}

/**
 * @brief  正常快速检测周期配置
 * @param  Period 周期值（单位10ms，范围1~30，默认值1）
 * @retval 无
 * @note   此值会影响到 LpAutoWakeTime 和 AutoSleepTime
 */
void CST816_Config_NorScanPer(uint8_t Period)
{
	if(Period >= 30)                                 // 限幅保护，最大值30
		Period = 30;
	CST816_IIC_WriteREG(NorScanPer,Period);          // 写入 NorScanPer 寄存器(0xEE)
}

/**
 * @brief  中断低脉冲输出宽度配置
 * @param  Width 脉冲宽度值（范围0~200）
 * @retval 无
 */
void CST816_Config_IrqPluseWidth(uint8_t Width)
{
	if(Width >= 200)                                 // 限幅保护
		Width = 200;
	CST816_IIC_WriteREG(IrqPluseWidth,Width);        // 写入 IrqPluseWidth 寄存器(0xED)
}

/**
 * @brief  低功耗扫描唤醒门限配置
 * @param  TH 门限值（越小越灵敏，默认值48）
 * @retval 无
 */
void CST816_Config_LpScanTH(uint8_t TH)
{
	CST816_IIC_WriteREG(LpScanTH,TH);                // 写入 LpScanTH 寄存器(0xF5)
}
