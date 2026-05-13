/** @file ui_HomePage.c
 *  @brief 主页（表盘页面）实现 - 核心页面，显示时间、日期、电池/步数/温湿度/心率等信息，包含下拉快捷面板
 */
#include "../../ui.h"
#include "../../ui_helpers.h"
#include "../Inc/ui_HomePage.h"
#include "../Inc/ui_MenuPage.h"
#include "../Inc/ui_MKS142Page.h"
#include "../Inc/ui_SetPage.h"

#include "../../../Func/Inc/HWDataAccess.h"


/* ====== 变量定义 ====== */
// 主页表盘元素
lv_obj_t * ui_HomePage; // 主页对象
lv_obj_t * ui_TimeHourLabel; // 小时标签
lv_obj_t * ui_TimeColonLabel; // 时间冒号
lv_obj_t * ui_TimeMinuteLabel; // 分钟标签
lv_obj_t * ui_BatArc; // 电池弧形
lv_obj_t * ui_BaticonLabel; // 电池图标
lv_obj_t * ui_BatNumLabel; // 电池百分比数字
lv_obj_t * ui_DateLabel; // 日期标签（MM-DD）
lv_obj_t * ui_DayLabel; // 星期标签
lv_obj_t * ui_StepiconLabel; // 步数图标
lv_obj_t * ui_StepCnLabel; // 步数中文标签
lv_obj_t * ui_StepNumLabel; // 步数数字
lv_obj_t * ui_StepNumBar; // 步数进度条
lv_obj_t * ui_TempArc; // 温度弧形
lv_obj_t * ui_TempiconLabel; // 温度图标
lv_obj_t * ui_TempNumLabel; // 温度数字
lv_obj_t * ui_HumiiconLabel; // 湿度图标
lv_obj_t * ui_HumiArc; // 湿度弧形
lv_obj_t * ui_HumiNumLabel; // 湿度数字
lv_obj_t * ui_HRArc; // 心率弧形
lv_obj_t * ui_HRiconLabel; // 心率图标
lv_obj_t * ui_HRNumLabel; // 心率数字

// 下拉面板元素
lv_obj_t * ui_DropDownPanel; // 下拉面板（可滚动）
lv_obj_t * ui_UpBGPanel; // 上背景面板
lv_obj_t * ui_NFCButton; // NFC开关
lv_obj_t * ui_NFCLabel; // NFC标签
lv_obj_t * ui_BLEButton; // 蓝牙开关
lv_obj_t * ui_BLELabel; // 蓝牙标签
lv_obj_t * ui_PowerButton; // 电源按钮
lv_obj_t * ui_PowerLabel; // 电源标签
lv_obj_t * ui_SetButton; // 设置按钮
lv_obj_t * ui_SetLabel; // 设置标签
lv_obj_t * ui_LightSlider; // 亮度滑块
lv_obj_t * ui_LightLabel; // 亮度标签
lv_obj_t * ui_DownBGPanel; // 下背景面板

// 电源关机页面
lv_obj_t * ui_PowerPage; // 电源页面对象
lv_obj_t * ui_PowerSlider; // 关机滑块
lv_obj_t * ui_PowerDownLabel; // 关机提示标签

lv_timer_t * ui_HomePageTimer; // 主页定时器

/* ====== 状态变量 ====== */
uint8_t ui_TimeHourValue = 11; // 当前小时（默认11）
uint8_t ui_TimeMinuteValue = 59; // 当前分钟（默认59）
const char * ui_Days[7] = {"Mon.", "Tue.", "Wed.", "Thu.", "Fri.", "Sat.", "Sun."}; // 星期缩写
uint8_t ui_DateMonthValue = 11; // 当前月份
uint8_t ui_DateDayValue = 8; // 当前日期
uint8_t ui_DataWeekdayValue = 2; // 当前星期

uint8_t ui_BatArcValue = 70; // 电池电量（默认70%）
uint16_t ui_StepNumValue = 0; // 步数值
uint8_t ui_LightSliderValue = 50; // 亮度值（默认50%）

uint8_t ui_HomePageBLEEN = 0; // 蓝牙使能状态
uint8_t ui_HomePageNFCEN = 0; // NFC使能状态

int8_t ui_HomePageTempValue = 25; // 温度值
int8_t ui_HomePageHumiValue = 67; // 湿度值


/* ====== 页面管理器 ====== */
Page_t Page_Home = {ui_HomePage_screen_init, ui_HomePage_screen_deinit, &ui_HomePage};
Page_t Page_Power = {ui_PowerPage_screen_init, ui_PowerPage_screen_deinit, &ui_PowerPage};

/**
 * @brief  主页定时器回调 - 每500ms刷新时间、日期、电池、步数、温湿度等数据
 * @param  timer LVGL定时器对象
 * @note   仅在该页面获取焦点时执行刷新
 */
static void HomePage_timer_cb(lv_timer_t * timer)
{
  uint8_t value_strbuf[10];
  if(Page_Get_NowPage()->page_obj  == &ui_HomePage) // 仅当前页为首页时刷新
	{
			HW_DateTimeTypeDef DateTime;
      HWInterface.RealTimeClock.GetTimeDate(&DateTime);

			if(ui_TimeMinuteValue != DateTime.Minutes) // 分钟变化
			{
				ui_TimeMinuteValue = DateTime.Minutes;
				sprintf(value_strbuf,"%02d",ui_TimeMinuteValue);
				lv_label_set_text(ui_TimeMinuteLabel, value_strbuf);
			}

			if(ui_TimeHourValue != DateTime.Hours) // 小时变化
			{
				ui_TimeHourValue = DateTime.Hours;
				sprintf(value_strbuf,"%2d",ui_TimeHourValue);
				lv_label_set_text(ui_TimeHourLabel, value_strbuf);
			}

			if(ui_DateDayValue != DateTime.Date) // 日期变化
			{
				ui_DateDayValue = DateTime.Date;
				ui_DataWeekdayValue = DateTime.WeekDay;
				sprintf(value_strbuf,"%2d-%02d",ui_DateMonthValue,ui_DateDayValue);
				lv_label_set_text(ui_DateLabel, value_strbuf);
				lv_label_set_text(ui_DayLabel, ui_Days[ui_DataWeekdayValue-1]);

			}
			if(ui_DateMonthValue != DateTime.Month) // 月份变化
			{
				ui_DateMonthValue = DateTime.Month;
				ui_DateDayValue = DateTime.Date;
				ui_DataWeekdayValue = DateTime.WeekDay;
				sprintf(value_strbuf,"%2d-%02d",ui_DateMonthValue,ui_DateDayValue);
				lv_label_set_text(ui_DateLabel, value_strbuf);
				lv_label_set_text(ui_DayLabel, ui_Days[ui_DataWeekdayValue-1]);
			}

      if(ui_BatArcValue != HWInterface.Power.power_remain) // 电池电量变化
      {
        ui_BatArcValue = HWInterface.Power.power_remain;
        lv_arc_set_value(ui_BatArc, ui_BatArcValue);
				sprintf(value_strbuf,"%2d%%",ui_BatArcValue);
				lv_label_set_text(ui_BatNumLabel, value_strbuf);
      }

      if(ui_StepNumValue != HWInterface.IMU.Steps) // 步数变化
      {
        ui_StepNumValue = HWInterface.IMU.Steps;
        sprintf(value_strbuf,"%d",ui_StepNumValue);
				lv_label_set_text(ui_StepNumLabel, value_strbuf);
      }

      if( ui_HomePageTempValue != (int8_t)HWInterface.AHT21.temperature ) // 温度变化
      {
        ui_HomePageTempValue = (int8_t)HWInterface.AHT21.temperature;
        lv_arc_set_value(ui_TempArc, ui_HomePageTempValue);
        sprintf(value_strbuf,"%d",ui_HomePageTempValue);
				lv_label_set_text(ui_TempNumLabel, value_strbuf);
      }

      if( ui_HomePageHumiValue != (int8_t)HWInterface.AHT21.humidity ) // 湿度变化
      {
        ui_HomePageHumiValue = (int8_t)HWInterface.AHT21.humidity;
        lv_arc_set_value(ui_HumiArc, ui_HomePageHumiValue);
				sprintf(value_strbuf,"%d",ui_HomePageHumiValue);
				lv_label_set_text(ui_HumiNumLabel, value_strbuf);
      }

      /* 心率动态更新（来自MKS142后台测量） */
      {
        static uint8_t last_hr = 0;
        if(last_hr != MKS142_saved_hr)
        {
          last_hr = MKS142_saved_hr;
          if(last_hr > 0)
            sprintf(value_strbuf,"%d", last_hr);
          else
            sprintf(value_strbuf,"--");
          lv_label_set_text(ui_HRNumLabel, value_strbuf);
        }
      }

	}
}


/* ====== 事件处理函数 ====== */

/**
 * @brief  主页手势事件 - 右滑进入菜单页面
 * @param  e LVGL事件对象
 */
void ui_event_HomePage(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);

    if(event_code == LV_EVENT_GESTURE)
    {
			if(lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) // 右滑进入菜单
      {
				Page_Load(&Page_Menu);
      }
		}
}

/**
 * @brief  NFC按钮事件 - 开关NFC功能
 * @param  e LVGL事件对象
 */
void ui_event_NFCButton(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED &&  lv_obj_has_state(target, LV_STATE_CHECKED))
    {
        ui_HomePageNFCEN=1; // 开启NFC
    }
    if(event_code == LV_EVENT_VALUE_CHANGED &&  !lv_obj_has_state(target, LV_STATE_CHECKED))
    {
        ui_HomePageNFCEN=0; // 关闭NFC
    }
}

/**
 * @brief  蓝牙按钮事件 - 开关蓝牙功能
 * @param  e LVGL事件对象
 */
void ui_event_BLEButton(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED &&  lv_obj_has_state(target, LV_STATE_CHECKED))
    {
        HWInterface.BLE.Enable(); // 开启蓝牙
        ui_HomePageBLEEN=1;

    }
    if(event_code == LV_EVENT_VALUE_CHANGED &&  !lv_obj_has_state(target, LV_STATE_CHECKED))
    {
				HWInterface.BLE.Disable(); // 关闭蓝牙
        ui_HomePageBLEEN=0;
    }
}

/**
 * @brief  电源按钮事件 - 进入关机页面
 * @param  e LVGL事件对象
 */
void ui_event_PowerButton(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
      Page_Load(&Page_Power); // 打开关机滑块页面
    }
}

/**
 * @brief  关机滑块事件 - 滑动到>=90时执行关机
 * @param  e LVGL事件对象
 */
void ui_event_PowerSlider(lv_event_t * e)
{
		lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED)
    {

    }
		if(event_code == LV_EVENT_CLICKED)
		{
			if(lv_slider_get_value(ui_PowerSlider) >=90) // 滑到90%以上关机
			{
        HWInterface.Power.Shutdown();
			}
		}
}

/**
 * @brief  设置按钮事件 - 进入设置页面
 * @param  e LVGL事件对象
 */
void ui_event_SetButton(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED)
    {
			Page_Load(&Page_Set); // 打开设置页面
    }
}

/**
 * @brief  亮度滑块事件 - 调整屏幕亮度
 * @param  e LVGL事件对象
 */
void ui_event_LightSlider(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED)
    {
        ui_LightSliderValue = lv_slider_get_value(ui_LightSlider);
        HWInterface.LCD.SetLight(ui_LightSliderValue); // 设置LCD背光亮度
    }
}

/* ====== 页面初始化 ====== */

/**
 * @brief  主页初始化 - 创建表盘全部元素：时间、日期、电池、步数、温湿度、心率及下拉面板
 */
void ui_HomePage_screen_init(void)
{
		ui_MenuScrollY = 0; // 重置菜单滚动位置
    uint8_t value_strbuf[10];

    ui_HomePage = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_HomePage, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    /* ====== 时间显示区域 ====== */
    // 小时标签（大字号）
    ui_TimeHourLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_TimeHourLabel, LV_SIZE_CONTENT);   /// 10
    lv_obj_set_height(ui_TimeHourLabel, LV_SIZE_CONTENT);    /// 10
    lv_obj_set_x(ui_TimeHourLabel, -105);
    lv_obj_set_y(ui_TimeHourLabel, -70);
    lv_obj_set_align(ui_TimeHourLabel, LV_ALIGN_RIGHT_MID);
    sprintf(value_strbuf,"%2d",ui_TimeHourValue);
    lv_label_set_text(ui_TimeHourLabel, value_strbuf);
    lv_obj_set_style_text_font(ui_TimeHourLabel, &ui_font_Cuyuan48, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 时间冒号
    ui_TimeColonLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_TimeColonLabel, LV_SIZE_CONTENT);   /// 10
    lv_obj_set_height(ui_TimeColonLabel, LV_SIZE_CONTENT);    /// 10
    lv_obj_set_x(ui_TimeColonLabel, -82);
    lv_obj_set_y(ui_TimeColonLabel, -70);
    lv_obj_set_align(ui_TimeColonLabel, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_TimeColonLabel, ":");
    lv_obj_set_style_text_color(ui_TimeColonLabel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_TimeColonLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_TimeColonLabel, &ui_font_Cuyuan48, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 分钟标签
    ui_TimeMinuteLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_TimeMinuteLabel, LV_SIZE_CONTENT);   /// 10
    lv_obj_set_height(ui_TimeMinuteLabel, LV_SIZE_CONTENT);    /// 10
    lv_obj_set_x(ui_TimeMinuteLabel, -20);
    lv_obj_set_y(ui_TimeMinuteLabel, -70);
    lv_obj_set_align(ui_TimeMinuteLabel, LV_ALIGN_RIGHT_MID);
    sprintf(value_strbuf,"%02d",ui_TimeMinuteValue);
    lv_label_set_text(ui_TimeMinuteLabel, value_strbuf);
    lv_obj_set_style_text_font(ui_TimeMinuteLabel, &ui_font_Cuyuan48, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 电池显示 ====== */
    ui_BatArc = lv_arc_create(ui_HomePage);
    lv_obj_set_width(ui_BatArc, 50);
    lv_obj_set_height(ui_BatArc, 50);
    lv_obj_set_x(ui_BatArc, -75);
    lv_obj_set_y(ui_BatArc, -68);
    lv_obj_set_align(ui_BatArc, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_BatArc, LV_OBJ_FLAG_CLICKABLE);      /// Flags
    lv_arc_set_value(ui_BatArc, ui_BatArcValue);
    lv_obj_set_style_arc_width(ui_BatArc, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_BatArc, lv_color_hex(0x19C819), LV_PART_INDICATOR | LV_STATE_DEFAULT); // 绿色
    lv_obj_set_style_arc_opa(ui_BatArc, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_BatArc, 5, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_BatArc, lv_color_hex(0x19C819), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BatArc, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_BatArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_BatArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_BatArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_BatArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    ui_BaticonLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_BaticonLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_BaticonLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_BaticonLabel, -75);
    lv_obj_set_y(ui_BaticonLabel, -50);
    lv_obj_set_align(ui_BaticonLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_BaticonLabel, "\xEE\x9A\x8F");
    lv_obj_set_style_text_color(ui_BaticonLabel, lv_color_hex(0x19C819), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_BaticonLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_BaticonLabel, &ui_font_iconfont16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_BatNumLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_BatNumLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_BatNumLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_BatNumLabel, -75);
    lv_obj_set_y(ui_BatNumLabel, -68);
    lv_obj_set_align(ui_BatNumLabel, LV_ALIGN_CENTER);
    sprintf(value_strbuf,"%2d%%",ui_BatArcValue);
    lv_label_set_text(ui_BatNumLabel, value_strbuf);
    lv_obj_set_style_text_font(ui_BatNumLabel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 日期显示 ====== */
    ui_DateLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_DateLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_DateLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_DateLabel, -75);
    lv_obj_set_y(ui_DateLabel, -110);
    lv_obj_set_align(ui_DateLabel, LV_ALIGN_RIGHT_MID);
    sprintf(value_strbuf,"%2d-%02d",ui_DateMonthValue,ui_DateDayValue);
    lv_label_set_text(ui_DateLabel, value_strbuf);
    lv_obj_set_style_text_color(ui_DateLabel, lv_color_hex(0xFF461E), LV_PART_MAIN | LV_STATE_DEFAULT); // 橙色
    lv_obj_set_style_text_opa(ui_DateLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_DateLabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_DayLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_DayLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_DayLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_DayLabel, -20);
    lv_obj_set_y(ui_DayLabel, -110);
    lv_obj_set_align(ui_DayLabel, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_DayLabel, ui_Days[ui_DataWeekdayValue-1]);
    lv_obj_set_style_text_color(ui_DayLabel, lv_color_hex(0xFF461E), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_DayLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_DayLabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* ====== 步数显示 ====== */
    ui_StepiconLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_StepiconLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_StepiconLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_StepiconLabel, -90);
    lv_obj_set_y(ui_StepiconLabel, -5);
    lv_obj_set_align(ui_StepiconLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_StepiconLabel, "\xEE\x99\xB6");
    lv_obj_set_style_text_color(ui_StepiconLabel, lv_color_hex(0x3278FF), LV_PART_MAIN | LV_STATE_DEFAULT); // 蓝色
    lv_obj_set_style_text_opa(ui_StepiconLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_StepiconLabel, &ui_font_iconfont24, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_StepCnLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_StepCnLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_StepCnLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_StepCnLabel, -40);
    lv_obj_set_y(ui_StepCnLabel, -5);
    lv_obj_set_align(ui_StepCnLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_StepCnLabel, "\xE4\xBB\x8A\xE6\x97\xA5\xE6\xAD\xA5\xE6\x95\xB0"); // 今日步数
    lv_obj_set_style_text_color(ui_StepCnLabel, lv_color_hex(0x3278FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_StepCnLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_StepCnLabel, &ui_font_Cuyuan18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_StepNumLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_StepNumLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_StepNumLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_StepNumLabel, -80);
    lv_obj_set_y(ui_StepNumLabel, 20);
    lv_obj_set_align(ui_StepNumLabel, LV_ALIGN_CENTER);
    sprintf(value_strbuf,"%d",ui_StepNumValue);
    lv_label_set_text(ui_StepNumLabel, value_strbuf);
    lv_obj_set_style_text_font(ui_StepNumLabel, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_StepNumBar = lv_bar_create(ui_HomePage);
    lv_bar_set_value(ui_StepNumBar, 40, LV_ANIM_OFF);
    lv_obj_set_width(ui_StepNumBar, 200);
    lv_obj_set_height(ui_StepNumBar, 10);
    lv_obj_set_x(ui_StepNumBar, 0);
    lv_obj_set_y(ui_StepNumBar, 40);
    lv_obj_set_align(ui_StepNumBar, LV_ALIGN_CENTER);

    lv_obj_set_style_bg_color(ui_StepNumBar, lv_color_hex(0x3278FF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_StepNumBar, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    /* ====== 温度显示（弧形+图标+数字） ====== */
    ui_TempArc = lv_arc_create(ui_HomePage);
    lv_obj_set_width(ui_TempArc, 50);
    lv_obj_set_height(ui_TempArc, 50);
    lv_obj_set_x(ui_TempArc, -75);
    lv_obj_set_y(ui_TempArc, 90);
    lv_obj_set_align(ui_TempArc, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_TempArc, LV_OBJ_FLAG_CLICKABLE);      /// Flags
    lv_arc_set_range(ui_TempArc, -20, 60); // 温度范围-20~60℃
    lv_arc_set_value(ui_TempArc, ui_HomePageTempValue);
    lv_obj_set_style_arc_width(ui_TempArc, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_TempArc, lv_color_hex(0xF5A73A), LV_PART_INDICATOR | LV_STATE_DEFAULT); // 橙色
    lv_obj_set_style_arc_opa(ui_TempArc, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_TempArc, 5, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_TempArc, lv_color_hex(0xF5A73A), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_TempArc, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_TempArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_TempArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_TempArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_TempArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    ui_TempiconLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_TempiconLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_TempiconLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_TempiconLabel, -75);
    lv_obj_set_y(ui_TempiconLabel, 90);
    lv_obj_set_align(ui_TempiconLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_TempiconLabel, "\xEE\x99\x99");
    lv_obj_set_style_text_color(ui_TempiconLabel, lv_color_hex(0xF5A73A), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_TempiconLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_TempiconLabel, &ui_font_iconfont30, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_TempNumLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_TempNumLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_TempNumLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_TempNumLabel, -73);
    lv_obj_set_y(ui_TempNumLabel, 125);
    lv_obj_set_align(ui_TempNumLabel, LV_ALIGN_CENTER);
    sprintf(value_strbuf,"%d",ui_HomePageTempValue);
    lv_label_set_text(ui_TempNumLabel, value_strbuf);

    /* ====== 湿度显示（弧形+图标+数字） ====== */
    ui_HumiiconLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_HumiiconLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_HumiiconLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_HumiiconLabel, 0);
    lv_obj_set_y(ui_HumiiconLabel, 89);
    lv_obj_set_align(ui_HumiiconLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_HumiiconLabel, "\xEE\x99\xB3");
    lv_obj_set_style_text_color(ui_HumiiconLabel, lv_color_hex(0x14C8E1), LV_PART_MAIN | LV_STATE_DEFAULT); // 青色
    lv_obj_set_style_text_opa(ui_HumiiconLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_HumiiconLabel, &ui_font_iconfont28, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_HumiArc = lv_arc_create(ui_HomePage);
    lv_obj_set_width(ui_HumiArc, 50);
    lv_obj_set_height(ui_HumiArc, 50);
    lv_obj_set_x(ui_HumiArc, 0);
    lv_obj_set_y(ui_HumiArc, 90);
    lv_obj_set_align(ui_HumiArc, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_HumiArc, LV_OBJ_FLAG_CLICKABLE);      /// Flags
    lv_arc_set_value(ui_HumiArc, ui_HomePageHumiValue);
    lv_arc_set_bg_angles(ui_HumiArc, 270, 620); // 270度起始
    lv_obj_set_style_arc_width(ui_HumiArc, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_HumiArc, lv_color_hex(0x14C8E1), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_HumiArc, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_HumiArc, 5, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_HumiArc, lv_color_hex(0x14C8E1), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_HumiArc, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_HumiArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_HumiArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_HumiArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_HumiArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    ui_HumiNumLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_HumiNumLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_HumiNumLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_HumiNumLabel, 0);
    lv_obj_set_y(ui_HumiNumLabel, 125);
    lv_obj_set_align(ui_HumiNumLabel, LV_ALIGN_CENTER);
    sprintf(value_strbuf,"%d",ui_HomePageHumiValue);
    lv_label_set_text(ui_HumiNumLabel, value_strbuf);

    /* ====== 心率显示（弧形+图标+数字） ====== */
		ui_HRArc = lv_arc_create(ui_HomePage);
    lv_obj_set_width(ui_HRArc, 50);
    lv_obj_set_height(ui_HRArc, 50);
    lv_obj_set_x(ui_HRArc, 70);
    lv_obj_set_y(ui_HRArc, 90);
    lv_obj_set_align(ui_HRArc, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_HRArc, LV_OBJ_FLAG_CLICKABLE);      /// Flags
    lv_arc_set_value(ui_HRArc, 100); // 心率弧形默认满值
    lv_arc_set_bg_angles(ui_HRArc, 270, 620);
    lv_obj_set_style_arc_width(ui_HRArc, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_HRArc, lv_color_hex(0xE11432), LV_PART_INDICATOR | LV_STATE_DEFAULT); // 红色
    lv_obj_set_style_arc_opa(ui_HRArc, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_HRArc, 5, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_HRArc, lv_color_hex(0xE11432), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_HRArc, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_HRArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_HRArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_HRArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_HRArc, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    ui_HRiconLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_HRiconLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_HRiconLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_HRiconLabel, 70);
    lv_obj_set_y(ui_HRiconLabel, 90);
    lv_obj_set_align(ui_HRiconLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_HRiconLabel, "\xEE\x9D\xA2");
    lv_obj_set_style_text_color(ui_HRiconLabel, lv_color_hex(0xE11432), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_HRiconLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_HRiconLabel, &ui_font_iconfont32, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_HRNumLabel = lv_label_create(ui_HomePage);
    lv_obj_set_width(ui_HRNumLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_HRNumLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_HRNumLabel, 70);
    lv_obj_set_y(ui_HRNumLabel, 125);
    lv_obj_set_align(ui_HRNumLabel, LV_ALIGN_CENTER);
    sprintf(value_strbuf,"%d", MKS142_saved_hr > 0 ? MKS142_saved_hr : 0);
    lv_label_set_text(ui_HRNumLabel, value_strbuf);

    /* ====== 下拉快捷面板 ====== */
    ui_DropDownPanel = lv_obj_create(ui_HomePage);
    lv_obj_set_width(ui_DropDownPanel, 240);
    lv_obj_set_height(ui_DropDownPanel, 420);
    lv_obj_set_x(ui_DropDownPanel, 0);
    lv_obj_set_y(ui_DropDownPanel, -10);
    lv_obj_set_align(ui_DropDownPanel, LV_ALIGN_TOP_MID);
    lv_obj_scroll_by(ui_DropDownPanel,0,-130,LV_ANIM_OFF); // 初始向上滚动偏移
    lv_obj_clear_flag(ui_DropDownPanel, LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_CHAIN);      /// Flags
    lv_obj_set_scroll_dir(ui_DropDownPanel, LV_DIR_VER); // 仅垂直滚动
    lv_obj_set_style_bg_color(ui_DropDownPanel, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_DropDownPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_DropDownPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_DropDownPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_DropDownPanel, 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_DropDownPanel, 0, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);

    // 上半部分背景
    ui_UpBGPanel = lv_obj_create(ui_DropDownPanel);
    lv_obj_set_width(ui_UpBGPanel, 240);
    lv_obj_set_height(ui_UpBGPanel, 130);
    lv_obj_set_x(ui_UpBGPanel, 0);
    lv_obj_set_y(ui_UpBGPanel, -10);
    lv_obj_set_align(ui_UpBGPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_UpBGPanel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_UpBGPanel, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_UpBGPanel, 200, LV_PART_MAIN | LV_STATE_DEFAULT);

    // NFC按钮（左侧，可选中状态）
    ui_NFCButton = lv_btn_create(ui_DropDownPanel);
    lv_obj_set_width(ui_NFCButton, 50);
    lv_obj_set_height(ui_NFCButton, 50);
		lv_obj_set_x(ui_NFCButton, 0);
    lv_obj_set_y(ui_NFCButton, 5);
    lv_obj_add_flag(ui_NFCButton, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_NFCButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_NFCButton, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_NFCButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_NFCButton, lv_color_hex(0x3264C8), LV_PART_MAIN | LV_STATE_CHECKED); // 选中变蓝
    lv_obj_set_style_bg_opa(ui_NFCButton, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    if(ui_HomePageNFCEN)
    {lv_obj_add_state(ui_NFCButton, LV_STATE_CHECKED);}

    ui_NFCLabel = lv_label_create(ui_NFCButton);
    lv_obj_set_width(ui_NFCLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_NFCLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_NFCLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_NFCLabel, "\xEE\xA6\x86");
    lv_obj_set_style_text_color(ui_NFCLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_NFCLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_NFCLabel, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 蓝牙按钮（左侧第二行）
    ui_BLEButton = lv_btn_create(ui_DropDownPanel);
    lv_obj_set_width(ui_BLEButton, 50);
    lv_obj_set_height(ui_BLEButton, 50);
    lv_obj_set_x(ui_BLEButton, 0);
    lv_obj_set_y(ui_BLEButton, 65);
    lv_obj_add_flag(ui_BLEButton, LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_BLEButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_BLEButton, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BLEButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_BLEButton, lv_color_hex(0x3264C8), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_BLEButton, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    if(ui_HomePageBLEEN)
    {lv_obj_add_state(ui_BLEButton, LV_STATE_CHECKED);}

    ui_BLELabel = lv_label_create(ui_BLEButton);
    lv_obj_set_width(ui_BLELabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_BLELabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_BLELabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_BLELabel, "\xEE\x98\x87");
    lv_obj_set_style_text_color(ui_BLELabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_BLELabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_BLELabel, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 电源按钮（中央上方）
    ui_PowerButton = lv_btn_create(ui_DropDownPanel);
    lv_obj_set_width(ui_PowerButton, 50);
    lv_obj_set_height(ui_PowerButton, 50);
	lv_obj_set_x(ui_PowerButton, 0);
    lv_obj_set_y(ui_PowerButton, 5);
    lv_obj_set_align(ui_PowerButton, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_PowerButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_PowerButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_PowerButton, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_PowerButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_PowerLabel = lv_label_create(ui_PowerButton);
    lv_obj_set_width(ui_PowerLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_PowerLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_PowerLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_PowerLabel, "\xEE\x98\xAC");
    lv_obj_set_style_text_color(ui_PowerLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_PowerLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_PowerLabel, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 设置按钮（中央下方）
    ui_SetButton = lv_btn_create(ui_DropDownPanel);
    lv_obj_set_width(ui_SetButton, 50);
    lv_obj_set_height(ui_SetButton, 50);
    lv_obj_set_x(ui_SetButton, 0);
    lv_obj_set_y(ui_SetButton, 65);
    lv_obj_set_align(ui_SetButton, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_SetButton, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_SetButton, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_SetButton, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_SetButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_SetLabel = lv_label_create(ui_SetButton);
    lv_obj_set_width(ui_SetLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_SetLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_SetLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_SetLabel, "\xEE\x98\x80");
    lv_obj_set_style_text_color(ui_SetLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_SetLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_SetLabel, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 亮度滑块（右侧竖条）
    ui_LightSlider = lv_slider_create(ui_DropDownPanel);
    lv_slider_set_value(ui_LightSlider, ui_LightSliderValue, LV_ANIM_OFF);
    if(lv_slider_get_mode(ui_LightSlider) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(ui_LightSlider, 0, LV_ANIM_OFF);
    lv_obj_set_width(ui_LightSlider, 50);
    lv_obj_set_height(ui_LightSlider, 110);
	lv_obj_set_x(ui_LightSlider, 0);
    lv_obj_set_y(ui_LightSlider, 5);
    lv_obj_set_align(ui_LightSlider, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_style_radius(ui_LightSlider, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_LightSlider, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_LightSlider, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_radius(ui_LightSlider, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_LightSlider, lv_color_hex(0x3264C8), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_LightSlider, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_LightSlider, lv_color_hex(0x000000), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_LightSlider, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    ui_LightLabel = lv_label_create(ui_LightSlider);
    lv_obj_set_width(ui_LightLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LightLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_LightLabel, 0);
    lv_obj_set_y(ui_LightLabel, 30);
    lv_obj_set_align(ui_LightLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_LightLabel, "\xEE\x99\xBA");
    lv_obj_set_style_text_color(ui_LightLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LightLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LightLabel, &ui_font_iconfont34, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 下半部分背景
    ui_DownBGPanel = lv_obj_create(ui_DropDownPanel);
    lv_obj_set_width(ui_DownBGPanel, 240);
    lv_obj_set_height(ui_DownBGPanel, 130);
    lv_obj_set_x(ui_DownBGPanel, 0);
    lv_obj_set_y(ui_DownBGPanel, 420);
    lv_obj_set_align(ui_DownBGPanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_DownBGPanel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_DownBGPanel, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_DownBGPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_DownBGPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_DownBGPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

		// 注册所有事件回调
		lv_obj_add_event_cb(ui_HomePage, ui_event_HomePage, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_NFCButton, ui_event_NFCButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_BLEButton, ui_event_BLEButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_PowerButton, ui_event_PowerButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_SetButton, ui_event_SetButton, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_LightSlider,ui_event_LightSlider, LV_EVENT_ALL, NULL);

    // 创建定时器，每500ms刷新
    ui_HomePageTimer = lv_timer_create(HomePage_timer_cb, 500,  NULL);

}


/**
 * @brief  关机页面初始化 - 创建滑动关机滑块
 */
void ui_PowerPage_screen_init(void)
{
    ui_PowerPage = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_PowerPage, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_PowerSlider = lv_slider_create(ui_PowerPage);
    lv_obj_set_width(ui_PowerSlider, 220);
    lv_obj_set_height(ui_PowerSlider, 50);
    lv_obj_set_align(ui_PowerSlider, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(ui_PowerSlider, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_PowerSlider, 128, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_PowerSlider, lv_color_hex(0x800000), LV_PART_INDICATOR | LV_STATE_DEFAULT); // 暗红色指示
    lv_obj_set_style_bg_opa(ui_PowerSlider, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_PowerSlider, lv_color_hex(0x800000), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_PowerSlider, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    ui_PowerDownLabel = lv_label_create(ui_PowerSlider);
    lv_obj_set_width(ui_PowerDownLabel, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_PowerDownLabel, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_PowerDownLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_PowerDownLabel, "\xE6\xBB\x91\xE5\x8A\xA8\xE5\x85\xB3\xE6\x9C\xBA"); // 滑动关机
    lv_obj_set_style_text_color(ui_PowerDownLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_PowerDownLabel, 200, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_PowerDownLabel, &ui_font_Cuyuan20, LV_PART_MAIN | LV_STATE_DEFAULT);

		lv_obj_add_event_cb(ui_PowerSlider, ui_event_PowerSlider, LV_EVENT_ALL, NULL);

}

/* ====== 页面注销 ====== */

/**
 * @brief  主页注销 - 删除定时器
 */
void ui_HomePage_screen_deinit(void)
{
  lv_timer_del(ui_HomePageTimer);
}

/**
 * @brief  关机页面注销 - 清理资源
 */
void ui_PowerPage_screen_deinit(void)
{}
