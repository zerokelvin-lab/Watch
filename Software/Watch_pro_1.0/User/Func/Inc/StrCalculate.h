/** @file StrCalculate.h
 *  @brief 字符串表达式计算器——将中缀数学表达式字符串解析并计算出结果，支持加减乘除四则运算
 */

#include <stdint.h>

#define NUMBER_TYPE 0 // 数据类型标记：数字
#define SYMBOL_TYPE 1 // 数据类型标记：运算符号（+、-、*、/）
#define POINT_TYPE 3  // 数据类型标记：小数点
#define CAL_DEPTH 5   // 计算栈最大深度，限制表达式复杂度

// 字符串栈结构体，用于存储和操作字符序列
typedef struct
{
    char strque[10];    // 字符队列缓冲，最多存15个字符
    uint8_t Top_Point;  // 栈顶指针

}StrStack_t;

// ====== 字符串栈操作函数 ======
uint8_t strput(StrStack_t * st,char strin);    // 入栈：压入一个字符
uint8_t strdel(StrStack_t * st);               // 出栈：移除栈顶字符
uint8_t strstack_isEmpty(StrStack_t* st);      // 判断栈是否为空
void strclear(StrStack_t* sq);                 // 清空栈中所有元素

// 计算数据类型，统一表示数字或符号
typedef struct
{
    char datatype;  // 数据类型标记（NUMBER_TYPE/SYMBOL_TYPE/POINT_TYPE）
    float number;   // 数字值（当datatype为NUMBER_TYPE时有效）
    char symbol;    // 符号字符（当datatype为SYMBOL_TYPE时有效）
}caldata_t;

// 数字栈结构体，用于存储表达式中的数字
typedef struct
{
    float data[CAL_DEPTH]; // 浮点数数组
    uint8_t Top_Point;     // 栈顶指针

}NumStack_t;

// 符号栈结构体，用于存储表达式中的运算符
typedef struct
{
    char data[CAL_DEPTH]; // 字符数组
    uint8_t Top_Point;    // 栈顶指针

}SymStack_t;

// ====== 计算器核心API ======
void NumStackClear(NumStack_t* st);                                                   // 清空数字栈
void SymStackClear(SymStack_t* st);                                                   // 清空符号栈
uint8_t NumSymSeparate(char * str, uint8_t strlen, NumStack_t * NumStack, SymStack_t * SymStack); // 解析字符串表达式，分离数字和符号到各自栈中
uint8_t StrCalculate(char * str, NumStack_t * NumStack, SymStack_t * SymStack);       // 计算字符串表达式的值，结果存入数字栈
uint8_t isIntNumber(float number);                                                    // 判断浮点数是否为整数（无小数部分）
