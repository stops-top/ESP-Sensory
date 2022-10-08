#include "lv_demo.h"
#include "./lv_example_func.h"
#include "./lv_example_image.h"

#if LV_BUILD_EXAMPLES

static bool clock_screen_layer_enter_cb(struct lv_layer_t * layer);
static bool clock_screen_layer_exit_cb(struct lv_layer_t * layer);
static void clock_screen_layer_timer_cb(lv_timer_t * tmr);

static lv_layer_t clock_screen_layer ={
    .lv_obj_name    = "clock_screen_layer",
    .lv_obj_layer   = NULL,
    .enter_cb       = clock_screen_layer_enter_cb,
    .exit_cb        = clock_screen_layer_exit_cb,
    .timer_cb       = clock_screen_layer_timer_cb,
};

static bool clock_screen_layer_enter_cb(struct lv_layer_t * layer)
{
	bool ret = false;
    uint8_t timeBuf[20];
    uint32_t temp;

	if(NULL == layer->lv_obj_layer){
		ret = true;
		layer->lv_obj_layer = lv_obj_create(lv_scr_act());
        lv_obj_remove_style_all(layer->lv_obj_layer);
        lv_obj_set_size(layer->lv_obj_layer, 480, 480);

        lv_obj_t * btn_clock_exit = lv_btn_create(layer->lv_obj_layer);
        lv_obj_set_size(btn_clock_exit, 480, 480);
        lv_obj_set_style_radius(btn_clock_exit, 0, 0);
        lv_obj_set_style_bg_color(btn_clock_exit, lv_color_hex(0x0000), 0);//lv_color_hex(0x1F1F1F)
        //lv_obj_add_event_cb(btn_clock_exit, btn_exit_clock_event_cb, LV_EVENT_ALL, btn_clock_exit); 
	}

	return ret;
}

static bool clock_screen_layer_exit_cb(struct lv_layer_t * layer)
{
    LV_LOG_USER("");
}

static void clock_screen_layer_timer_cb(lv_timer_t * tmr)
{
    //uint32_t idle_period_Test;
    int32_t isTmOut;
    uint32_t period_clock_next;

    isTmOut = (lv_tick_get() - (period_clock_next + 9500));
    if(isTmOut > 0){
        period_clock_next = lv_tick_get();
        LV_LOG_USER("new location");
    }
}

#endif
