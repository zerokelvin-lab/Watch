/** @file PageManager.c
 *  @brief 页面管理器实现——基于栈结构的页面导航，支持页面压入、弹出、回退和动画切换
 */

#include "../Inc/PageManager.h"
#include "../../GUI_App/Screens/Inc/ui_HomePage.h"
#include "../../GUI_App/Screens/Inc/ui_MenuPage.h"

PageStack_t PageStack; // 全局页面栈实例定义

/**
 * @brief  初始化页面栈，将栈顶指针置零
 * @param  stack 页面栈指针
 * @return 无
 */
static void page_stack_init(PageStack_t* stack) {
    stack->top = 0; // 栈顶归零，栈变为空
}

/**
 * @brief  将页面压入栈顶
 * @param  stack 页面栈指针
 * @param  page  要压入的页面指针
 * @return 0表示成功，-1表示栈已满
 */
static uint8_t page_stack_push(PageStack_t* stack, Page_t* page) {
    if (stack->top >= MAX_DEPTH)
		return -1; // 栈已满，无法压入
    stack->pages[stack->top++] = page; // 存入页面并栈顶+1
	return 0;
}

/**
 * @brief  弹出栈顶页面，并调用其反初始化函数
 * @param  stack 页面栈指针
 * @return 0表示成功，-1表示栈已空
 */
static uint8_t page_stack_pop(PageStack_t* stack) {
    if (stack->top <= 0)
		return -1;                  // 栈已空，无法弹出
    stack->pages[--stack->top]->deinit(); // 栈顶-1，并调用旧栈顶页面的反初始化
    return 0;
}

/**
 * @brief  判断页面栈是否为空
 * @param  stack 页面栈指针
 * @return 1表示栈空，0表示栈非空
 */
static uint8_t page_stack_is_empty(const PageStack_t* stack) {
    return stack->top == 0;
}

/**
 * @brief  获取栈顶页面指针
 * @param  stack 页面栈指针
 * @return 栈顶页面指针，栈空时返回NULL
 */
static Page_t* get_top_page(PageStack_t* stack) {
    // 检查栈是否为空
    if (stack->top == 0) {
        return NULL; // 如果栈为空，返回NULL
    }

    // 返回栈顶页面的指针
    return stack->pages[stack->top - 1];
}


/**
 * @brief  获取当前页面（栈顶页面）的指针
 * @param  无
 * @return 返回当前页面的指针，如果栈为空则返回NULL
 */
Page_t* Page_Get_NowPage(void) {
    return get_top_page(&PageStack);
}


/**
 * @brief  返回上一页——弹出当前页面，若栈空则回退到主菜单
 * @param  无
 * @return 无
 */
void Page_Back(void) {
	if (page_stack_is_empty(&PageStack)) {
        // 栈为空时，不应发生
        return;
    }

    // 弹出当前页面
    page_stack_pop(&PageStack);

    if (page_stack_is_empty(&PageStack)) {
        // 如果栈为空，入栈并切换到MenuPage（首页菜单）
        page_stack_push(&PageStack, &Page_Home);
        page_stack_push(&PageStack, &Page_Menu);
        Page_Menu.init();
        lv_scr_load_anim(*Page_Menu.page_obj, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 100, 0, true); // 从右侧滑入动画
    } else {
        // 切换到上一个页面
        Page_t *previous_page = PageStack.pages[PageStack.top - 1];
        previous_page->init();                                                               // 重新初始化上一页
        lv_scr_load_anim(*previous_page->page_obj, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 100, 0, true); // 从右侧滑入动画
    }
}

/**
 * @brief  返回栈底首页——弹出当前页上方所有页面，直接回到首页
 * @param  无
 * @return 无
 */
void Page_Back_Bottom(void) {

    if (page_stack_is_empty(&PageStack)) {
        // 栈为空时，不应发生
        return;
    }

    // 弹出除栈底的所有页面
    while(PageStack.top > 1)
        page_stack_pop(&PageStack);
    PageStack.pages[PageStack.top - 1]->init(); // 初始化新页面
    lv_scr_load_anim(*PageStack.pages[PageStack.top - 1]->page_obj, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 100, 0, true); // 加载并应用动画（从右侧滑入）
}

/**
 * @brief  加载新页面到栈顶——反初始化当前页，压入新页面并播放进入动画
 * @param  newPage 要加载的新页面指针
 * @return 无
 */
void Page_Load(Page_t *newPage) {
	// 检查堆栈是否已满
    if (PageStack.top >= MAX_DEPTH - 1) {
        // 错误处理：堆栈满
        return;
    }

    // 如果堆栈非空，反初始化当前页面
    if (PageStack.top > 0) {
        PageStack.pages[PageStack.top - 1]->deinit();
    }

    // 将新页面推入堆栈
    page_stack_push(&PageStack, newPage);
    newPage->init(); // 初始化新页面
    lv_scr_load_anim(*newPage->page_obj, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 100, 0, true); // 加载并应用动画（从右侧滑入）
}

/**
 * @brief  初始化页面管理器——创建栈并加载首页
 * @param  无
 * @return 无
 */
void Pages_init(void) {
    page_stack_init(&PageStack);      // 初始化栈结构
    page_stack_push(&PageStack, &Page_Home); // 首页压入栈底
    Page_Home.init();                  // 初始化首页
    lv_disp_load_scr(*Page_Home.page_obj); // 加载首页到屏幕显示
}
