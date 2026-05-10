/** @file PageManager.h
 *  @brief 页面管理器——基于栈结构的页面导航系统，支持页面压栈、出栈、回退等操作
 */

#ifndef PAGE_STACK_H
#define PAGE_STACK_H

#include "../../GUI_App/ui.h"

#define MAX_DEPTH 6 // 页面栈最大深度，即最多支持6层页面嵌套

// 页面结构体，封装一个页面的生命周期和LVGL对象
typedef struct {
    void (*init)(void);     // 页面初始化函数，进入页面时调用
    void (*deinit)(void);   // 页面反初始化函数，离开页面时调用
    lv_obj_t **page_obj;    // 指向页面LVGL对象指针的二级指针
} Page_t;


// 页面堆栈结构体，用数组模拟栈存储页面指针
typedef struct {
    Page_t* pages[MAX_DEPTH]; // 页面指针数组，模拟栈空间
    uint8_t top;               // 栈顶指针，指向下一个空位
} PageStack_t;

extern PageStack_t PageStack; // 全局页面栈实例，在PageManager.c中定义


Page_t* Page_Get_NowPage(void);               // 获取当前页面（栈顶页面）
void Page_Back(void);                         // 返回上一页，弹出当前页面并加载前一页
void Page_Back_Bottom(void);                  // 返回栈底（首页），弹出中间所有页面
void Page_Load(Page_t *newPage);              // 加载新页面，压入栈顶并初始化
void Pages_init(void);                        // 初始化页面管理器并启动首页


#endif // PAGE_STACK_H
