/** @file HrAlgorythm.c @brief 心率计算算法实现，使用队列缓存PPG数据，通过峰值检测和均值滤波计算心率 */
#include "HrAlgorythm.h"
#include "string.h"

/** @brief 数据队列：存储最近7个PPG原始数据 */
Queue datas;
/** @brief 时间队列：存储最近7个PPG采样时间戳 */
Queue times;
/** @brief 心率队列：存储最近7次计算的心率值(用于均值滤波) */
Queue HR_List;

/**
  * @brief 心率算法初始化，初始化3个循环队列
  */
void HR_AlgoInit(void)
{
	initQueue(&datas);
	initQueue(&times);
	initQueue(&HR_List);
}

/**
  * @brief 心率均值滤波（对7个历史心率值取平均）
  * @param HrList 心率值数组(长度=7)
  * @param lenth 数组长度(固定为7)
  * @return 平均心率值
  */
uint8_t Hr_Ave_Filter(uint32_t *HrList, uint8_t lenth)
{
	uint32_t ave;
	uint8_t i;
	for(i = 0;i<lenth;i++)
	{
		ave += HrList[i];
	}
	ave /= lenth;
}

/**
  * @brief 心率计算核心算法：滑动窗口峰值检测 + 均值滤波
  * @param present_dat 当前PPG原始采样数据
  * @param present_time 当前采样时间戳(ms)
  * @return 当前心率值(bpm)，首次计算前返回0
  * @note 算法流程：
  *   1. 维护7元素队列(PPG数据 + 时间戳)
  *   2. 检测datas[3]是否为局部峰值(大于相邻点)
  *   3. 若峰值间隔>425ms(对应<141bpm上限)，计算瞬时心率
  *   4. 对最近7次心率做均值滤波输出
  */
uint16_t HR_Calculate(uint16_t present_dat,uint32_t present_time)
{

	static uint16_t peaks_time[]={0,0};  // 记录最近两次峰值时间戳[最近, 前次]
	static uint8_t HR=0;                 // 静态心率值

	// 若队列已满则先出队，维护滑动窗口大小为7
	if(isQueueFull(&datas))
	{dequeue(&datas);}
	if(isQueueFull(&times))
	{dequeue(&times);}
	if(isQueueFull(&HR_List))
	{dequeue(&HR_List);}

	// 将新数据入队
	enqueue(&datas,present_dat);
	enqueue(&times,present_time);

	// 峰值检测：判断datas[3](窗口中心)是否为局部最大值
	// 检查条件：中心点 >= 相邻两点，且严格大于最远两点(确保是明显峰值)
	if((datas.data[3]>=datas.data[2]) && (datas.data[3]>=datas.data[1]) && (datas.data[3]>datas.data[0]) 
		&& (datas.data[3]>=datas.data[4]) && (datas.data[3]>=datas.data[5]) && (datas.data[3]>datas.data[6]))
	{
			// 时间间隔滤波：峰值间隔必须>425ms(避免高频噪声误判，对应心率上限约141bpm)
			if((times.data[3]-peaks_time[0])>425)
			{
					// 更新峰值时间戳
					peaks_time[1] = peaks_time[0];
					peaks_time[0] = times.data[3];
					// 计算瞬时心率: HR(bpm) = 60000(ms) / 峰峰间隔(ms)
					enqueue(&HR_List,60000/(peaks_time[0]-peaks_time[1]));
					// 队列满7个后开始均值滤波输出
					if(HR_List.data[6]!=0)
					{HR = Hr_Ave_Filter(HR_List.data,7);}
			}
	}
	return HR;
}
