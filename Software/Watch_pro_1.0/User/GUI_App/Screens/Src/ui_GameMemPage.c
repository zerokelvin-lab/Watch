/** @file ui_GameMemPage.c
 *  @brief 记忆方块游戏页面实现 - 翻牌记忆配对游戏，点击按钮查找相同数字对，全部配对成功显示重玩按钮
 */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_MenuPage.h"
#include "../Inc/ui_GameMemPage.h"

#include "../../../Func/Inc/HWDataAccess.h"

/* ====== 游戏宏定义 ====== */
#define MATRIX_SIZE 4 // 4x4矩阵

// 记忆游戏状态结构
typedef struct {
    uint8_t matrix[MATRIX_SIZE * MATRIX_SIZE + 1]; // 数字矩阵（存储0~8）
    char    * btnm_map[MATRIX_SIZE * MATRIX_SIZE + MATRIX_SIZE]; // 按钮映射
    uint8_t label_opa_map[MATRIX_SIZE * MATRIX_SIZE]; // 每格的透明度（控制显示/隐藏）
    uint8_t previous_id; // 上一次点击的按钮ID
    uint8_t present_id; // 当前点击的按钮ID
} memory_game_t;

// 数字字符列表
static char * num_list[] = {"0","1","2","3","4","5","6","7","8"};

/* ====== 游戏函数声明 ====== */
void Game_Mem_Init(void); // 游戏初始化（随机排列数字对）
void list_rand_number(uint8_t arry[], uint8_t max_count, uint8_t count); // 随机排列数组
void updata_btnm(char * btnm_map[], uint8_t matrix[MATRIX_SIZE * MATRIX_SIZE + 1]); // 更新按钮文字
uint8_t Game_Mem_Finish(void); // 判断游戏是否完成

/* ====== 页面管理器 ====== */
Page_t Page_GameMem = {ui_GameMemPage_screen_init, ui_GameMemPage_screen_deinit, &ui_GameMem_Page};

/* ====== 变量定义 ====== */
memory_game_t Game_Mem; // 记忆游戏状态

lv_obj_t * ui_GameMem_Page; // 游戏页面对象
lv_obj_t * ui_GameMem_BtnM; // 游戏按钮矩阵
lv_obj_t * ui_new_game_btn; // 重新开始按钮

/**
 * @brief  按钮矩阵事件回调 - 处理翻牌逻辑和配对判断
 * @param  e LVGL事件对象
 */
void ui_event_GameMemBtnM_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_VALUE_CHANGED) // 按钮点击
    {
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        Game_Mem.present_id = (uint8_t)id;
        if(Game_Mem.present_id != Game_Mem.previous_id) // 不和上次点同一个格子
        {

            if(Game_Mem.matrix[Game_Mem.present_id] == Game_Mem.matrix[Game_Mem.previous_id]) // 配对成功
            {

                lv_btnmatrix_set_btn_ctrl(ui_GameMem_BtnM,Game_Mem.present_id,LV_BTNMATRIX_CTRL_HIDDEN); // 隐藏配对的按钮
                lv_btnmatrix_set_btn_ctrl(ui_GameMem_BtnM,Game_Mem.previous_id,LV_BTNMATRIX_CTRL_HIDDEN);
                Game_Mem.matrix[Game_Mem.present_id] = 0; // 标记为已消除
                Game_Mem.matrix[Game_Mem.previous_id] = 0;
                if(Game_Mem_Finish()) // 全部配对完成
                {
                    lv_obj_clear_flag(ui_new_game_btn,LV_OBJ_FLAG_HIDDEN); // 显示重新开始按钮
                }
                Game_Mem.previous_id = Game_Mem.present_id;

            }
            else // 配对失败
            {
								if(Game_Mem.previous_id >= 0 && Game_Mem.previous_id < 17 ) // 隐藏之前显示的数字
									Game_Mem.label_opa_map[Game_Mem.previous_id] = LV_OPA_0;
                Game_Mem.label_opa_map[Game_Mem.present_id] = LV_OPA_100; // 显示当前点击的数字
                Game_Mem.previous_id = Game_Mem.present_id;
            }

        }
    }

    if(code == LV_EVENT_DRAW_PART_BEGIN) // 自定义按钮绘制样式
    {
        lv_obj_draw_part_dsc_t * dsc = lv_event_get_param(e);


        if((dsc->id >= 0) && (dsc->label_dsc))
        {
            dsc->rect_dsc->bg_color = lv_color_hex(0xB07010); // 金色背景
            dsc->label_dsc->color = lv_color_white(); // 白色文字
            dsc->label_dsc->opa = Game_Mem.label_opa_map[dsc->id]; // 根据透明度控制是否显示数字
        }
    }
}

/**
 * @brief  重新开始按钮事件 - 重置游戏状态
 * @param  e LVGL事件对象
 */
void ui_event_NewGameBtn_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED)
    {
        Game_Mem_Init(); // 重新初始化游戏
        lv_btnmatrix_set_map(ui_GameMem_BtnM, Game_Mem.btnm_map); // 刷新显示
        lv_btnmatrix_clear_btn_ctrl_all(ui_GameMem_BtnM,LV_BTNMATRIX_CTRL_HIDDEN); // 清除隐藏状态
        lv_obj_add_flag(ui_new_game_btn,LV_OBJ_FLAG_HIDDEN); // 隐藏重新开始按钮
    }
}

/**
 * @brief  记忆游戏页面初始化 - 创建4x4按钮矩阵和重新开始按钮
 */
void ui_GameMemPage_screen_init(void)
{

    Game_Mem_Init(); // 初始化游戏数据
    ui_GameMem_Page = lv_obj_create(NULL);;
    ui_GameMem_BtnM = lv_btnmatrix_create(ui_GameMem_Page);
    lv_btnmatrix_set_map(ui_GameMem_BtnM, Game_Mem.btnm_map);
    lv_obj_set_style_text_font(ui_GameMem_BtnM, &ui_font_Cuyuan24, 0);
    lv_btnmatrix_set_one_checked(ui_GameMem_BtnM,true);
    lv_obj_set_style_border_width(ui_GameMem_BtnM,0,0);
    lv_obj_set_style_bg_opa(ui_GameMem_BtnM,0,0);
    lv_obj_set_size(ui_GameMem_BtnM,240,240);
    lv_obj_set_align(ui_GameMem_BtnM,LV_ALIGN_CENTER);

    // 重新开始按钮（初始隐藏）
    ui_new_game_btn = lv_btn_create(ui_GameMem_Page);
    lv_obj_align(ui_new_game_btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(ui_new_game_btn,90,45);
    lv_obj_t * new_game_btn_label = lv_label_create(ui_new_game_btn);
    lv_label_set_text(new_game_btn_label,"\xE9\x87\x8D \xE7\x8E\xA9"); // 重玩
    lv_obj_align(new_game_btn_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(new_game_btn_label, &ui_font_Cuyuan24, 0);
    lv_obj_add_flag(ui_new_game_btn,LV_OBJ_FLAG_HIDDEN); // 初始隐藏

    lv_obj_add_event_cb(ui_GameMem_BtnM, ui_event_GameMemBtnM_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_new_game_btn, ui_event_NewGameBtn_handler, LV_EVENT_ALL, NULL);
}

/**
 * @brief  记忆游戏页面注销 - 清理资源
 */
void ui_GameMemPage_screen_deinit(void)
{}

/* ====== 记忆游戏核心函数 ====== */

/**
 * @brief  游戏数据初始化 - 随机生成8对数字（1~8），初始化透明度和按钮映射
 */
void Game_Mem_Init(void)
{
    uint8_t x;

    for (x = 0; x < MATRIX_SIZE * MATRIX_SIZE + 1; x++) {
            Game_Mem.matrix[x] = 0; // 清空矩阵
    }

    for (x = 0; x < MATRIX_SIZE * MATRIX_SIZE; x++) {
        Game_Mem.label_opa_map[x] = LV_OPA_0; // 初始全部不可见
    }
    Game_Mem.previous_id = -1; // 无上次选择
    Game_Mem.present_id = -1; // 无当前选择
    list_rand_number(Game_Mem.matrix,MATRIX_SIZE * MATRIX_SIZE,MATRIX_SIZE*MATRIX_SIZE/2); // 随机生成8对数字
    updata_btnm(Game_Mem.btnm_map,Game_Mem.matrix); // 更新按钮文字

}

/**
 * @brief  随机排列数组 - 在max_count个位置中均匀填充count种数字（每种出现两次）
 * @param  arry      目标数组
 * @param  max_count 数组大小
 * @param  count     数字种类数
 */
void list_rand_number(uint8_t arry[], uint8_t max_count, uint8_t count)
{
    int w, t;
	int time=0;
	HW_DateTimeTypeDef nowtime;
	HWInterface.RealTimeClock.GetTimeDate(&nowtime);
	time = nowtime.Seconds; // 用当前秒数作随机种子
    srand(time);

    for (int i = 0; i < max_count; i++)
        arry[i] = (i % count) + 1; // 均匀填充数字
    for (int i = 0; i < max_count; i++) // Fisher-Yates洗牌算法
    {
        w = rand() % (count - (i % count)) + i;
        if (w > max_count)  w = max_count - 1;

        t = arry[i];
        arry[i] = arry[w];
        arry[w] = t;
    }
}

/**
 * @brief  更新按钮映射 - 将矩阵中的数字映射到按钮显示
 * @param  btnm_map 按钮映射数组
 * @param  matrix   数字矩阵
 */
void updata_btnm(char * btnm_map[], uint8_t matrix[MATRIX_SIZE * MATRIX_SIZE + 1])
{
    uint8_t x,index;
    index = 0;
    for(x=0;x<MATRIX_SIZE * MATRIX_SIZE;x++)
    {
        if((index + 1) % (MATRIX_SIZE + 1) == 0) // 每4个按钮插入换行
        {
            btnm_map[index] = "\n";
            index++;
        }
        btnm_map[index] = num_list[matrix[x]]; // 设置按钮文字
        index++;
    }
    btnm_map[MATRIX_SIZE * MATRIX_SIZE + MATRIX_SIZE - 1] = ""; // 结尾空字符串
}

/**
 * @brief  判断游戏是否完成 - 检查是否所有数字对都已消除
 * @return 1=完成，0=未完成
 */
uint8_t Game_Mem_Finish(void)
{
    for(uint16_t x = 0; x < MATRIX_SIZE*MATRIX_SIZE; x++)
    {
        if(Game_Mem.matrix[x]!=0) // 还有未消除的数字
            return 0;
    }
    return 1;
}
