/** @file ui_ComputerPage.c
 *  @brief 计算器页面实现 - 四则运算计算器，使用按钮矩阵输入，支持加减乘除、小数点和退格
 */
#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_ComputerPage.h"

#include "../../../Func/Inc/StrCalculate.h"

/* ====== 页面管理器 ====== */
Page_t Page_Computer = {ui_ComputerPage_screen_init, ui_ComputerPage_screen_deinit, &ui_Computerpage};

/* ====== 变量定义 ====== */

#define TEXT_FULL 10 // 文本框最大字符数

StrStack_t CalStr; // 计算表达式字符串栈
NumStack_t NumStack; // 数字栈（用于表达式求值）
SymStack_t SymStack; // 符号栈（用于表达式求值）
lv_obj_t * ui_Computerpage; // 计算器页面对象
lv_obj_t * ui_CompageBtnM; // 计算器按钮矩阵
lv_obj_t * ui_CompageTextarea; // 计算器文本显示区域
lv_obj_t * ui_CompageBackBtn; // 退格按钮

// 计算器按钮矩阵映射（5列4行，含换行符）
static const char * ui_ComPageBtnmap[] ={"1", "2", "3", "\xEF\xBC\x8B", "\n", // +号
                                         "4", "5", "6", "\xEF\xBC\x8D", "\n", // -号
                                         "7", "8", "9", "\xC3\x97", "\n",      // ×号
                                         ".", "0", "\xEF\xBC\x9D", "\xC3\xB7", ""}; // =号和÷号


/**
 * @brief  计算器按钮矩阵事件回调 - 处理按钮点击和绘制样式
 * @param  e LVGL事件对象
 * @note   处理数字输入、运算符输入和表达式求值
 */
void ui_CompageBtnM_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_DRAW_PART_BEGIN) // 自定义按钮绘制样式
    {

        lv_obj_draw_part_dsc_t * dsc = lv_event_get_param(e);
        if(dsc->id == 3 || dsc->id == 7 || dsc->id == 11 || dsc->id == 14 || dsc->id == 15) // 运算符按钮
        {
            dsc->rect_dsc->radius = LV_RADIUS_CIRCLE; // 圆形按钮
            if (lv_btnmatrix_get_selected_btn(obj) == dsc->id)
            {
                dsc->rect_dsc->bg_color = lv_palette_darken(LV_PALETTE_BLUE,3); // 选中时深色
            }
            else
                dsc->rect_dsc->bg_color = lv_palette_main(LV_PALETTE_BLUE); // 默认蓝色


        }

    }
    if(code == LV_EVENT_DRAW_PART_END)
    {
        lv_obj_draw_part_dsc_t * dsc = lv_event_get_param(e);


    }
    if(code == LV_EVENT_VALUE_CHANGED) // 按钮按下
    {
        uint16_t btn_id = lv_btnmatrix_get_selected_btn(obj); // 获取当前选中的按键id
        const char * txt = lv_btnmatrix_get_btn_text(obj, btn_id); // 获取当前按键文本

        if (txt != NULL)
        {
            if (ui_CompageTextarea != NULL)
            {
							if(lv_textarea_get_cursor_pos(ui_CompageTextarea) <= TEXT_FULL) // 限制输入长度
              {
									lv_textarea_add_text(ui_CompageTextarea, txt); // 文本框追加字符
									switch(btn_id) // 将字符存入计算表达式栈
									{
											case 0: strput(&CalStr,'1'); break;
											case 1: strput(&CalStr,'2'); break;
											case 2: strput(&CalStr,'3'); break;
											case 3: strput(&CalStr,'+'); break;
											case 4: strput(&CalStr,'4'); break;
											case 5: strput(&CalStr,'5'); break;
											case 6: strput(&CalStr,'6'); break;
											case 7: strput(&CalStr,'-'); break;
											case 8: strput(&CalStr,'7'); break;
											case 9: strput(&CalStr,'8'); break;
											case 10: strput(&CalStr,'9'); break;
											case 11: strput(&CalStr,'*'); break;
											case 12: strput(&CalStr,'.'); break;
											case 13: strput(&CalStr,'0'); break;
											case 14: // 等号按钮
													strput(&CalStr,'=');
													lv_textarea_add_text(ui_CompageTextarea,"\n");
													strput(&CalStr,'\n');
													break;
											case 15: strput(&CalStr,'/'); break;
									}
							}
            }
        }

        if(lv_btnmatrix_get_selected_btn(obj) == 14) // 按下等号，执行计算
        {
            if(StrCalculate(CalStr.strque,&NumStack,&SymStack)) // 表达式求值计算
						{lv_textarea_add_text(ui_CompageTextarea,"erro");} // 计算错误
						else
						{
							char strout[10];
							if(isIntNumber(NumStack.data[NumStack.Top_Point-1])) // 整数直接显示
							{sprintf(strout,"%.0f",NumStack.data[NumStack.Top_Point-1]);}
							else // 小数保留4位
							{sprintf(strout,"%.4f",NumStack.data[NumStack.Top_Point-1]);}
							lv_textarea_add_text(ui_CompageTextarea,strout);
						}
            strclear(&CalStr); // 清空表达式栈
						lv_obj_clear_flag(ui_CompageBtnM,LV_OBJ_FLAG_CLICKABLE); // 禁用按钮防止重复输入
        }
    }
}

/**
 * @brief  退格按钮事件回调 - 删除最后一个字符，长按清空全部
 * @param  e LVGL事件对象
 */
void ui_CompageBackBtn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) // 短按：删除最后一个字符
    {
        if (ui_CompageTextarea != NULL)
        {
            if(!strstack_isEmpty(&CalStr))
            {
                lv_textarea_del_char(ui_CompageTextarea);
                strdel(&CalStr); // 同步删除表达式栈
            }
            else
            {
                int i = 0;
                for (i = 0; i < (TEXT_FULL*2); i++) // 清空所有字符
                {
                    lv_textarea_del_char(ui_CompageTextarea);
                }
								lv_obj_add_flag(ui_CompageBtnM,LV_OBJ_FLAG_CLICKABLE); // 恢复按钮可用
            }
        }
    }
		if(code == LV_EVENT_LONG_PRESSED) // 长按：清空全部并恢复按钮
		{
			if (ui_CompageTextarea != NULL)
			{
				if(!strstack_isEmpty(&CalStr))
				{
						strclear(&CalStr);
						int i = 0;
						for (i = 0; i < (TEXT_FULL*2); i++)
						{
								lv_textarea_del_char(ui_CompageTextarea);
						}
				}
			}
		}
}


/**
 * @brief  计算器页面初始化 - 创建按钮矩阵、文本显示区和退格按钮
 */
void ui_ComputerPage_screen_init(void)
{
		strclear(&CalStr); // 初始化时清空计算栈
		NumStackClear(&NumStack);
		SymStackClear(&SymStack);
    ui_Computerpage = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Computerpage,LV_OBJ_FLAG_SCROLLABLE);

    // 创建4x5按钮矩阵
    ui_CompageBtnM = lv_btnmatrix_create(ui_Computerpage);
    lv_btnmatrix_set_map(ui_CompageBtnM, ui_ComPageBtnmap);

    lv_obj_set_style_text_font(ui_CompageBtnM, &ui_font_Cuyuan24, 0);
    lv_btnmatrix_set_one_checked(ui_CompageBtnM,true);
    int i = 0;
    for (i = 0; i < 16; i++)
    {
        lv_btnmatrix_set_btn_ctrl(ui_CompageBtnM, i, LV_BTNMATRIX_CTRL_NO_REPEAT); // 禁用长按重复
    }
    lv_obj_clear_flag(ui_CompageBtnM, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_set_style_border_width(ui_CompageBtnM,0,0);
    lv_obj_set_style_bg_opa(ui_CompageBtnM,0,0);
    lv_obj_set_size(ui_CompageBtnM,240,220);
    lv_obj_set_align(ui_CompageBtnM,LV_ALIGN_BOTTOM_MID);

    // 创建文本显示区
    ui_CompageTextarea = lv_textarea_create(ui_Computerpage);
    lv_textarea_set_one_line(ui_CompageTextarea, false); // 多行模式
    lv_textarea_set_max_length(ui_CompageTextarea, TEXT_FULL*2); // 最大字符长度
    lv_obj_add_state(ui_CompageTextarea, LV_STATE_FOCUSED); // 显示光标
    lv_obj_set_style_radius(ui_CompageTextarea, 0, 0); // 无圆角
    lv_obj_set_style_border_width(ui_CompageTextarea, 0, 0); // 无边框
    lv_obj_set_style_bg_color(ui_CompageTextarea, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_CompageTextarea, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_size(ui_CompageTextarea, 200, 60); // 文本区域大小
    lv_obj_align(ui_CompageTextarea, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_clear_flag(ui_CompageTextarea,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_font(ui_CompageTextarea, &ui_font_Cuyuan24, 0);
    lv_textarea_set_align(ui_CompageTextarea, LV_TEXT_ALIGN_RIGHT); // 右对齐

    // 创建退格按钮
    ui_CompageBackBtn = lv_btn_create(ui_Computerpage);
    lv_obj_align(ui_CompageBackBtn,LV_ALIGN_RIGHT_MID,-10,-110);
    lv_obj_set_width(ui_CompageBackBtn,40);
    lv_obj_set_height(ui_CompageBackBtn,30);
    lv_obj_set_style_bg_opa(ui_CompageBackBtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * btnlabel = lv_label_create(ui_CompageBackBtn);
    lv_label_set_text(btnlabel, LV_SYMBOL_BACKSPACE); // 退格图标
    lv_obj_set_style_text_font(btnlabel, &lv_font_montserrat_24, 0);
    lv_obj_center(btnlabel);

    // 注册事件回调
    lv_obj_add_event_cb(ui_CompageBtnM, ui_CompageBtnM_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_CompageBackBtn, ui_CompageBackBtn_event_cb, LV_EVENT_ALL, NULL);

}

/**
 * @brief  计算器页面注销 - 清理资源
 */
void ui_ComputerPage_screen_deinit(void)
{}
