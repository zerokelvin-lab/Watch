/** @file StrCalculate.c
 *  @brief 字符串表达式计算器实现——将中缀数学表达式解析为数字栈和符号栈，按运算符优先级逐步计算得出最终结果
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Inc/StrCalculate.h"

/* ====== 字符串栈操作 ====== */

/**
 * @brief  将字符压入字符串栈
 * @param  st    字符串栈指针
 * @param  strin 要压入的字符
 * @return 0表示成功，-1表示栈已满
 */
uint8_t strput(StrStack_t * st,char strin)
{
    if(st->Top_Point == 15 - 1)
    {return -1;} // 栈满，无法压入

    st->strque[st->Top_Point++] = strin; // 存入字符并移动栈顶指针
    return 0;
}

/**
 * @brief  弹出字符串栈顶字符
 * @param  st 字符串栈指针
 * @return 0表示成功，-1表示栈已空
 */
uint8_t strdel(StrStack_t * st)
{
    if(st->Top_Point == 0)
    {return -1;} // 栈空，无法弹出

    st->strque[--st->Top_Point] = NULL; // 栈顶指针下移并清空
    return 0;
}

/**
 * @brief  判断字符串栈是否为空
 * @param  st 字符串栈指针
 * @return 1表示栈空，0表示栈非空
 */
uint8_t strstack_isEmpty(StrStack_t* st)
{
    if(st->Top_Point == 0)
    {return 1;}

    return 0;
}

/**
 * @brief  清空字符串栈中所有字符
 * @param  sq 字符串栈指针
 * @return 无
 */
void strclear(StrStack_t* sq)
{
    while(!strstack_isEmpty(sq))
    {
        strdel(sq);
    }
}

/* ====== 数字栈操作 ====== */

/**
 * @brief  将浮点数压入数字栈
 * @param  st 数字栈指针
 * @param  in 要压入的浮点数
 * @return 0表示成功，-1表示栈已满
 */
uint8_t NumStackPut(NumStack_t * st, float in)
{
   if(st->Top_Point == CAL_DEPTH - 1)
	{return -1;} // 栈满

	st->data[st->Top_Point++] = in; // 存入数据并移动栈顶
	return 0;
}

/**
 * @brief  弹出数字栈顶元素
 * @param  st 数字栈指针
 * @return 0表示成功，-1表示栈已空
 */
uint8_t NumStackDel(NumStack_t * st)
{
    if(st->Top_Point == 0)
	{return -1;} // 栈空
    st->data[st->Top_Point--] = 0; // 栈顶下移并清零
	return 0;
}

/**
 * @brief  判断数字栈是否为空
 * @param  st 数字栈指针
 * @return 1表示栈空，0表示栈非空
 */
uint8_t NumStack_isEmpty(NumStack_t* st)
{
	if(st->Top_Point == 0)
	{return 1;}

	return 0;
}

/**
 * @brief  清空数字栈中所有数据
 * @param  st 数字栈指针
 * @return 无
 */
void NumStackClear(NumStack_t* st)
{
	while(!NumStack_isEmpty(st))
	{
		NumStackDel(st);
	}
}

/* ====== 符号栈操作 ====== */

/**
 * @brief  将运算符压入符号栈
 * @param  st 符号栈指针
 * @param  in 运算符字符（+、-、*、/）
 * @return 0表示成功，-1表示栈已满
 */
uint8_t SymStackPut(SymStack_t * st, char in)
{
	if(st->Top_Point == CAL_DEPTH - 1)
	{return -1;} // 栈满

	st->data[st->Top_Point++] = in; // 存入符号并移动栈顶
	return 0;
}

/**
 * @brief  弹出符号栈顶元素
 * @param  st 符号栈指针
 * @return 0表示成功，-1表示栈已空
 */
uint8_t SymStackDel(SymStack_t * st)
{
  if(st->Top_Point == 0)
	{return -1;} // 栈空
    st->data[st->Top_Point--] = 0; // 栈顶下移并清零
	return 0;
}

/**
 * @brief  判断符号栈是否为空
 * @param  st 符号栈指针
 * @return 1表示栈空，0表示栈非空
 */
uint8_t SymStack_isEmpty(SymStack_t* st)
{
	if(st->Top_Point == 0)
	{return 1;}

	return 0;
}

/**
 * @brief  清空符号栈中所有数据
 * @param  st 符号栈指针
 * @return 无
 */
void SymStackClear(SymStack_t* st)
{
	while(!SymStack_isEmpty(st))
	{
		SymStackDel(st);
	}
}

/* ====== 表达式计算核心逻辑 ====== */

/**
 * @brief  判断栈顶运算符优先级是否高于当前运算符
 * @param  top     栈顶运算符
 * @param  present 当前运算符
 * @return 1表示栈顶优先级更高（应先计算栈顶运算），0表示当前优先级更高或相等
 */
uint8_t SymisHighPriority(char top, char present)
{
    //乘除的优先级最大，遇到任何运算符都先计算
    if(top == '*' || top == '/')
    {
        return 1;
    }
    else if(top == '+')
    {
        if(present == '-')
        {return 1;} // 栈顶+比当前-优先级高，先算栈顶
        else
        {return 0;}
    }
    else if(top == '-')
    {
        if(present == '+')
        {return 1;} // 栈顶-比当前+优先级高，先算栈顶
        else
        {return 0;}
    }
}

/**
 * @brief  执行一次运算——弹出数字栈顶两个数和符号栈顶一个运算符，计算结果重新压入数字栈
 * @param  numstack 数字栈指针
 * @param  symstack 符号栈指针
 * @return 无
 */
void CalculateOne(NumStack_t * numstack, SymStack_t * symstack)
{
    caldata_t temp;
    temp.datatype = NUMBER_TYPE;
    temp.symbol = NULL;
    //计算数字栈中的顶部两数,结果存到temp中
    if(symstack->data[symstack->Top_Point-1] == '+')
        temp.number = (numstack->data[numstack->Top_Point-2]) + (numstack->data[numstack->Top_Point-1]);

    else if(symstack->data[symstack->Top_Point-1] == '-')
        temp.number = (numstack->data[numstack->Top_Point-2]) - (numstack->data[numstack->Top_Point-1]);

    else if(symstack->data[symstack->Top_Point-1] == '*')
        temp.number = (numstack->data[numstack->Top_Point-2]) * (numstack->data[numstack->Top_Point-1]);

    else if(symstack->data[symstack->Top_Point-1] == '/')
        temp.number = (numstack->data[numstack->Top_Point-2]) / (numstack->data[numstack->Top_Point-1]);

    //运算前两数出栈,运算结果数入栈
    NumStackDel(numstack);
    NumStackDel(numstack);
    NumStackPut(numstack,temp.number); // 计算结果入数字栈
    SymStackDel(symstack);              // 已使用的运算符出栈

}

/**
 * @brief  将字符串表达式解析为数字栈和符号栈（中缀表达式分离）
 * @param  str       输入表达式字符串
 * @param  strlen    字符串长度
 * @param  NumStack  数字栈指针
 * @param  SymStack  符号栈指针
 * @return 0表示解析成功，1-5表示不同错误类型
 */
uint8_t NumSymSeparate(char * str, uint8_t strlen, NumStack_t * NumStack, SymStack_t * SymStack)
{
		NumStackClear(NumStack);   // 先清空数字栈
		SymStackClear(SymStack);   // 先清空符号栈
    caldata_t temp,temp_pre;
    char NumBehindPoint_Flag = 0;// 小数点后位数计数，0表示不在小数部分
    temp.datatype = NUMBER_TYPE;
    temp.number = 0;
    temp.symbol = NULL;
    temp_pre = temp;
    temp_pre.datatype = SYMBOL_TYPE;
    if(str[0]>'9' || str[0]<'0')
        return 1;// 错误：表达式必须以数字开头
		int i;
    for(i=0;i<strlen;i++) // 逐字符扫描表达式
    {
        if(str[i]=='.')
        {
            temp.datatype = POINT_TYPE; // 标记当前为小数点类型
            if(temp_pre.datatype == NUMBER_TYPE)
            {} // 小数点前是数字，正常处理
            else
            {return 2;} // 错误：小数点位置不合法
            temp_pre = temp;
        }
        if(str[i]<='9' && str[i]>='0')
        {
            //溢出报错
            if(NumStack->Top_Point>CAL_DEPTH || SymStack->Top_Point>CAL_DEPTH)
            {return 3;} // 错误：计算栈溢出
            //读取当前的字符到temp中
            temp.datatype = NUMBER_TYPE;
            temp.number = (str[i] - '0'); // 字符转数字
            temp.symbol = NULL;
            // 如果为连续数字,需要进行进位,将数字栈顶读出进位，再加上现在位，再入栈
            if(temp_pre.datatype == NUMBER_TYPE)
            {
                if(!NumBehindPoint_Flag)
                {temp.number += NumStack->data[NumStack->Top_Point-1] * 10;} // 整数部分按位累加
                else
                {
                    NumBehindPoint_Flag += 1;
                    char i = NumBehindPoint_Flag;
                    while(i--) // 小数部分除以10的幂次
                    {temp.number /= 10;}
                    temp.number += NumStack->data[NumStack->Top_Point-1]; // 累加到已有小数
                }
                NumStackDel(NumStack);     // 弹出旧值
                NumStackPut(NumStack,temp.number); // 压入新累加值
            }
            // 当前数字刚好是小数点后第一位
            else if(temp_pre.datatype == POINT_TYPE)
            {
                NumBehindPoint_Flag = 1;   // 标记进入小数部分
                temp.number /= 10;          // 第一位小数除以10
                temp.number += NumStack->data[NumStack->Top_Point-1];
                NumStackDel(NumStack);
                NumStackPut(NumStack,temp.number);
            }
            // 前一位不是数字或小数点,现在读取的这一位是数字，直接入栈
            else
            {
                NumStackPut(NumStack,temp.number);
            }
            temp_pre = temp;
        }
        else if(str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/')
        {
            //溢出报错
            if(NumStack->Top_Point>CAL_DEPTH || SymStack->Top_Point>CAL_DEPTH)
            {return 4;} // 错误：计算栈溢出
            //读取当前的字符到temp中
            temp.datatype = SYMBOL_TYPE;
            temp.symbol = str[i];
            temp.number = 0;
            NumBehindPoint_Flag = 0;// 遇到运算符，小数点计数重置
            // 重复输入了运算符号
            if(temp_pre.datatype == SYMBOL_TYPE)
            {
                return 5 ;// 错误：连续两个运算符
            }
            else
            {
                if((!SymStack_isEmpty(SymStack)) && SymisHighPriority(SymStack->data[SymStack->Top_Point-1],temp.symbol))
                {
                    // 栈顶运算符优先级更高，先计算栈顶运算
                    CalculateOne(NumStack, SymStack);
                    SymStackPut(SymStack,temp.symbol); // 当前符号压入符号栈
                }
                else
                {
                    // 当前运算符优先级不低于栈顶，直接压入符号栈
                    SymStackPut(SymStack,temp.symbol);
                }
                temp_pre = temp;
            }
        }
    }
    return 0; // 解析成功
}

/**
 * @brief  计算字符串表达式的最终结果
 * @param  str       输入表达式字符串
 * @param  NumStack  数字栈指针
 * @param  SymStack  符号栈指针
 * @return 0表示计算成功，-1表示表达式有误
 */
uint8_t StrCalculate(char * str,NumStack_t * NumStack, SymStack_t * SymStack)
{
    if(NumSymSeparate(str,strlen(str),NumStack,SymStack)) // 解析表达式分离数字和符号
    {
        //erro, clear all
        NumStackClear(NumStack);   // 出错时清空所有栈
        SymStackClear(SymStack);
        return -1;
    }
    else
    {
        // 符号栈中剩余的运算符按顺序依次计算
        while(!SymStack_isEmpty(SymStack))
        {
            CalculateOne(NumStack,SymStack);
        }
    }
    return 0; // 计算完成，结果在数字栈栈顶
}

/**
 * @brief  判断浮点数是否为整数（无小数部分）
 * @param  number 待判断的浮点数
 * @return 1表示是整数，0表示有小数部分
 */
uint8_t isIntNumber(float number)
{
	if(number == (int)number) // 截断转换后与原值比较
	{return 1;}
	return 0;
}
