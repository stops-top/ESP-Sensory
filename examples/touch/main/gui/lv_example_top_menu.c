#include "lv_demo.h"
#include <time.h>
#include <sys/time.h>
#include "./lv_example_func.h"
#include "./lv_example_image.h"

#if LV_BUILD_EXAMPLES && LV_USE_FLEX

typedef enum{
    SPRITE_TOP_CLOCK,
    SPRITE_TOP_CHECK_OK,
    SPRITE_TOP_CHECK_ERR,
    SPRITE_TOP_MAX,
};

static bool main_layer_enter_cb(struct lv_layer_t * layer);
static bool main_layer_exit_cb(struct lv_layer_t * layer);
static void main_layer_timer_cb(lv_timer_t * tmr);

lv_layer_t finger_touch_layer ={
    .lv_obj_name    = "finger_touch_layer",
    .lv_obj_user    = NULL,
    .lv_obj_layer   = NULL,
    //.lv_show_layer  = &show_public_layer,
    .lv_show_layer  = &show_public_layer,
    .enter_cb       = main_layer_enter_cb,
    .exit_cb        = main_layer_exit_cb,
    .timer_cb       = main_layer_timer_cb,
};


lv_obj_t * top_menu_Event;

lv_obj_t * label_set_time, * labelData, * label_user_check, * label_welcome;
static uint8_t sprite_top_focus;

lv_style_t style_line;
static lv_point_t line_points[2] = { 
            {(0 + 15), 100}, 
            {(320 - 15), 100}
            };


static void clock_date_task_callback()
{
    uint8_t textBuf[30];

    static const char *week_day[7] = { "星期日","星期一","星期二","星期三","星期四","星期五","星期六" };
    static time_t unix_time;
    static struct tm *time_info;
 
    unix_time = time(NULL);
    time_info = localtime(&unix_time);
 
    int year = time_info->tm_year + 1900;
    int month = time_info->tm_mon + 1;
    int day = time_info->tm_mday;
    int weekday = time_info->tm_wday;
    int hour = time_info->tm_hour;
    int minutes = time_info->tm_min;
    int second = time_info->tm_sec;
 
    if(SPRITE_TOP_CLOCK == sprite_top_focus){
        if (label_set_time != NULL){
            //lv_obj_set_style_text_font(label_set_time, &font_KaiTi40, 0);
            sprintf(textBuf, "%02d:%02d:%02d", hour, minutes, second);
            lv_label_set_text_fmt(label_set_time, textBuf);
        }
 
        if (labelData != NULL){
            //lv_label_set_text(labelData, "7月21日 星期四");
            sprintf(textBuf, "%02d月%02d日 %s", month, day, week_day[weekday]);
            lv_label_set_text_fmt(labelData, textBuf);
        }
    }
}

void sprite_top_goto(uint8_t sprite, uint32_t user_serial)
{
    uint8_t textBuf[30];

    sprite_top_focus = sprite;

    switch(sprite)
    {
        case SPRITE_TOP_CLOCK:
            lv_obj_add_flag(label_user_check, LV_OBJ_FLAG_HIDDEN);
            #if 0
            uint32_t timenow = sys_get_Second();
            sprintf(textBuf, "%02d:%02d", (timenow/60)%24,timenow%60);
            lv_obj_set_style_text_font(label_set_time, &font_KaiTi40, 0);
            lv_label_set_text(label_set_time, textBuf);

            lv_label_set_text(labelData, "7月21日 星期四");
            #else
            lv_obj_set_style_text_font(label_set_time, &font_KaiTi40, 0);
            clock_date_task_callback();
            #endif
            lv_obj_set_style_text_color(label_welcome, lv_color_hex(COLOUR_BLACK), 0);
            lv_label_set_text(label_welcome, "Hi 欢迎！");
        break;

        case SPRITE_TOP_CHECK_OK:
            lv_obj_clear_flag(label_user_check, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_text_font(label_set_time, &font_KaiTi36, 0);
            sprintf(textBuf, "用户 %d", user_serial);
            lv_label_set_text(label_set_time, textBuf);

            sprintf(textBuf, "登记号码 %06d", user_serial);
            lv_label_set_text(labelData, textBuf);
            lv_obj_set_style_text_color(label_welcome, lv_palette_darken(LV_PALETTE_GREEN, 1), 0);
            lv_label_set_text(label_welcome, "验证成功！");
        break;

        case SPRITE_TOP_CHECK_ERR:
            lv_obj_clear_flag(label_user_check, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_text_font(label_set_time, &font_KaiTi36, 0);
            sprintf(textBuf, "用户 -");
            lv_label_set_text(label_set_time, textBuf);

            sprintf(textBuf, "登记号码 -");
            lv_label_set_text(labelData, textBuf);
            lv_obj_set_style_text_color(label_welcome, lv_palette_darken(LV_PALETTE_RED, 1), 0);
            lv_label_set_text(label_welcome, "验证失败！");
        break;
    }
    // LV_LOG_USER("goto sprite_top_focus:%d", sprite_top_focus);
}

bool top_check_alert_end_cb(uint32_t userdata)
{
    LV_LOG_USER("goto clock spritre");
    sprite_top_goto(SPRITE_TOP_CLOCK, 0xFF);
}

static void top_enter_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * button = lv_event_get_target(e);

    if(code == LV_EVENT_SHORT_CLICKED) {
        LV_LOG_USER("SHORT_CLICKED %d, buttton:%X,%d", code, button, button->state);
        lv_goto_layer(&finger_touch_layer, &administrator_layer);
    }
    else if(LV_EVENT_DRAW_POST_END == code){
    }
}

static void top_test_event_cb(lv_event_t * e)
{
    uint8_t focus;
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * button = lv_event_get_target(e);
    uint8_t touch_event = lv_event_get_user_data(e);
    esp_fp_user_info * param = lv_event_get_param(e);

    if(code == LV_EVENT_SHORT_CLICKED) {
        if(param){/*parse */ 
            #if OFFLINE_TEST
            LV_LOG_USER("touch_event:%d", touch_event);
            #else
            LV_LOG_USER("result:%d, id:%d, stat:%d, level:%d, template:%d", \
                        param->result, param->id,param->stat,param->level,param->template);
            touch_event = param->result;
            #endif
        }
        // LV_LOG_USER("SHORT_CLICKED %d, result:%s", code, touch_event ? "OK": "Err");

        focus = sprite_top_focus;
        switch(focus)
        {
            case SPRITE_TOP_CHECK_ERR:
            case SPRITE_TOP_CHECK_OK:
            case SPRITE_TOP_CLOCK:

                if(SINGAL_VERIFY_OK == touch_event){
                    #if OFFLINE_TEST
                    create_rand_user_id();
                    #else
                    user_serial = param->id;
                    #endif
                    sprite_top_goto(SPRITE_TOP_CHECK_OK, user_serial);
                    tips_info_set(TIP_ACTION_PREPARE, TIP_EVENT_IMPLICIT_INFO, 2000, top_check_alert_end_cb, 0xFF);
                }
                else{
                //else if(SINGAL_VERIFY_ERR == touch_event){
                    //sprite_top_goto(SPRITE_TOP_CHECK_ERR);
                    //tips_info_set(TIP_ACTION_PREPARE, TIP_EVENT_IMPLICIT_INFO, 2000, top_check_alert_end_cb);
                    //tips_info_set(TIP_ACTION_PREPARE, (alertFailedList++)%(BTL_RC_SENSOR_BAD_LINE_ERR + 1), 2000, top_check_alert_end_cb, 0xFF);
                    tips_info_set(TIP_ACTION_PREPARE, param->result, 2000, top_check_alert_end_cb, 0xFF);
                }
        }
    }
}

void lv_example_line(lv_obj_t * parent)
{
     lv_obj_t * line1;

    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 2);
    lv_style_set_line_color(&style_line, lv_color_hex(COLOUR_BG2_GREY));
    lv_style_set_line_rounded(&style_line, true);

    line1 = lv_line_create(parent);
    lv_line_set_points(line1, line_points, 2);     /*Set the points*/
    lv_obj_add_style(line1, &style_line, 0);
    //lv_obj_align(line1, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_pos(line1, 0, 75);
}

static void lv_create_set_clock_page(lv_obj_t * parent)
{
    int i;
    uint8_t timeBuf[30];

    static lv_coord_t grid_2_col_dsc[] = {LV_GRID_FR(1),LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_2_row_dsc[] = {
                20,
                60,
                40,  
                80,  
                LV_GRID_TEMPLATE_LAST};

    lv_obj_t * btn_enter = lv_btn_create(parent);
    //lv_obj_remove_style_all(btn_enter);
    //lv_obj_set_size(btn_enter, 50, 40);
    lv_obj_set_style_radius(btn_enter, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn_enter, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn_enter, lv_color_white(), LV_PART_MAIN);

    lv_obj_align(btn_enter, LV_ALIGN_TOP_RIGHT, -5, 5);
    lv_obj_add_event_cb(btn_enter, top_enter_event_cb, LV_EVENT_SHORT_CLICKED, parent);

    lv_obj_t * label_enter = lv_label_create(btn_enter);
    lv_obj_remove_style_all(label_enter);
    lv_obj_set_style_text_color(label_enter, lv_palette_darken(LV_PALETTE_GREY, 2), 0);
    lv_label_set_text(label_enter, LV_SYMBOL_LIST);
    lv_obj_center(label_enter);

    label_user_check = lv_label_create(parent);
    lv_obj_remove_style_all(label_user_check);
    //lv_obj_set_style_bg_opa(label_user_check, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_user_check, lv_color_black(), 0);
    lv_obj_set_style_text_font(label_user_check, &font_KaiTi24, 0);
    lv_label_set_text(label_user_check, "用户验证");
    lv_obj_align(label_user_check, LV_ALIGN_TOP_LEFT, 0, 10);

    lv_obj_t * bg_setpage = lv_list_create(parent);
    lv_obj_set_style_opa(bg_setpage, LV_OPA_100, 0);
    lv_obj_set_size(bg_setpage, LV_PCT(100), 200);
    lv_obj_align(bg_setpage, LV_ALIGN_TOP_LEFT, 0, 40);
    
    lv_obj_set_style_bg_color(bg_setpage, lv_color_hex(COLOUR_WHITE), LV_PART_MAIN);
    lv_obj_set_style_border_width(bg_setpage, 0, 0);
    lv_obj_set_style_radius(bg_setpage, 0, 0);

    lv_obj_set_grid_dsc_array(bg_setpage, grid_2_col_dsc, grid_2_row_dsc);

    label_set_time = lv_label_create(bg_setpage);
    lv_obj_remove_style_all(label_set_time);
    lv_obj_set_style_text_color(label_set_time, lv_color_hex(COLOUR_BLACK), 0);
    lv_obj_set_style_text_font(label_set_time, &font_KaiTi40, 0);

    uint32_t timenow = sys_get_Second();
    sprintf(timeBuf, "%02d:%02d", (timenow/60)%24,timenow%60);
    lv_label_set_text(label_set_time, timeBuf);
    lv_obj_center(label_set_time);
    lv_obj_set_grid_cell(label_set_time, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    LV_LOG_USER("timeBuf:%s, %d",timeBuf, timenow);

    labelData = lv_label_create(bg_setpage);
    lv_obj_remove_style_all(labelData);
    lv_obj_set_style_text_color(labelData, lv_color_hex(COLOUR_BG_GREY), 0);
    lv_obj_set_style_text_font(labelData, &font_KaiTi24, 0);

    lv_label_set_text(labelData, "7月 21日 星期四");
    lv_obj_center(labelData);
    lv_obj_set_grid_cell(labelData, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    label_welcome = lv_label_create(bg_setpage);
    lv_obj_remove_style_all(label_welcome);
    lv_obj_set_style_text_color(label_welcome, lv_color_hex(COLOUR_BLACK), 0);
    lv_obj_set_style_text_font(label_welcome, &font_KaiTi24, 0);

    lv_label_set_text(label_welcome, "Hi 欢迎！");
    lv_obj_center(label_welcome);
    lv_obj_set_grid_cell(label_welcome, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    lv_example_line(parent);
}

static bool main_layer_enter_cb(struct lv_layer_t * layer)
{
	bool ret = false;

	LV_LOG_USER("Enter:%s", layer->lv_obj_name);
	if(NULL == layer->lv_obj_layer){
		ret = true;
		layer->lv_obj_layer = lv_obj_create(lv_scr_act());
		lv_obj_set_size(layer->lv_obj_layer, LV_HOR_RES, LV_VER_RES);
		lv_obj_set_style_border_width(layer->lv_obj_layer, 0, 0);
		lv_obj_set_style_pad_all(layer->lv_obj_layer, 0, 0);
		lv_obj_set_style_radius(layer->lv_obj_layer, 0, 0);
		lv_obj_set_style_bg_color(layer->lv_obj_layer, lv_color_hex(COLOUR_GREY), 0);  

		lv_create_set_clock_page(layer->lv_obj_layer);  

        sprite_top_goto(SPRITE_TOP_CLOCK, 0xFF);
        user_operation_mode_set(USER_OPERATION_MODE_NORMAL);
#if OFFLINE_TEST
		lv_obj_t * testBtn, * label_test;

		testBtn = lv_btn_create(layer->lv_obj_layer);
		lv_obj_set_style_opa(testBtn, LV_OPA_20, 0);
		lv_obj_align(testBtn, LV_ALIGN_TOP_MID, 50, 0);
		lv_obj_add_event_cb(testBtn, top_test_event_cb, LV_EVENT_SHORT_CLICKED, SINGAL_VERIFY_OK);

		label_test = lv_label_create(testBtn);
		lv_obj_remove_style_all(label_test);
		lv_obj_set_style_text_color(label_test, lv_color_black(), 0);
		lv_obj_set_style_text_font(label_test, &font_KaiTi24, 0);
		lv_label_set_text(label_test, "OK");
		lv_obj_center(label_test);

		testBtn = lv_btn_create(layer->lv_obj_layer);
		lv_obj_set_style_opa(testBtn, LV_OPA_20, 0);
		lv_obj_align(testBtn, LV_ALIGN_TOP_MID, 0, 0);
		lv_obj_add_event_cb(testBtn, top_test_event_cb, LV_EVENT_SHORT_CLICKED, SINGAL_VERIFY_ERR);

		label_test = lv_label_create(testBtn);
		lv_obj_remove_style_all(label_test);
		lv_obj_set_style_text_color(label_test, lv_color_black(), 0);
		lv_obj_set_style_text_font(label_test, &font_KaiTi24, 0);
		lv_label_set_text(label_test, "ER");
		lv_obj_center(label_test);

        top_menu_Event = testBtn;
#else
        lv_obj_t * testBtn = lv_btn_create(layer->lv_obj_layer);
        lv_obj_set_size(testBtn, 0, 0);
		lv_obj_set_style_opa(testBtn, LV_OPA_20, 0);
		lv_obj_align(testBtn, LV_ALIGN_TOP_MID, 0, 0);
		lv_obj_add_event_cb(testBtn, top_test_event_cb, LV_EVENT_SHORT_CLICKED, SINGAL_VERIFY_ERR);  
        top_menu_Event = testBtn;  
#endif
    }
	return ret;
}

static bool main_layer_exit_cb(struct lv_layer_t * layer)
{
    LV_LOG_USER("");
}

static void main_layer_timer_cb(lv_timer_t * tmr)
{
    uint8_t timeBuf[30];
    int32_t temp, isTmOut = 0;
    static uint32_t period_clock_add;

    esp_fp_user_info * touch_event;
    touch_event = touch_event_receive();
    if(touch_event){
        lv_event_send(top_menu_Event, LV_EVENT_SHORT_CLICKED, touch_event);
    }

    isTmOut = (lv_tick_get() - (period_clock_add + 1000));
    if(isTmOut > 0){
        period_clock_add = lv_tick_get();
        //clock_date_task_callback();
        temp = sys_get_Second();
        sprintf(timeBuf, "%02d:%02d", (temp/60)%24,temp%60);
        if(label_set_time && (SPRITE_TOP_CLOCK == sprite_top_focus)){
            lv_label_set_text(label_set_time, timeBuf);
        }
    }
}

void lv_example_fingerTouch(void)
{
    lv_goto_layer(NULL, &finger_touch_layer);
    //lv_goto_layer(NULL, &test_layer);
}
#endif
