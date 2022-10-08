#include "lv_demo.h"
#include "./lv_example_func.h"
#include "./lv_example_image.h"

#if LV_BUILD_EXAMPLES

#define LV_DEMO_MUSIC_HANDLE_SIZE  20

static bool show_public_layer_enter_cb(struct lv_layer_t * layer);
static bool show_public_layer_exit_cb(struct lv_layer_t * layer);
static void show_public_layer_timer_cb(lv_timer_t * tmr);

lv_layer_t show_public_layer ={
    .lv_obj_name    = "show_public_layer",
    .lv_obj_layer   = NULL,
    .enter_cb       = show_public_layer_enter_cb,
    .exit_cb        = show_public_layer_exit_cb,
    .timer_cb       = show_public_layer_timer_cb,
};


lv_obj_t * label_alert_info;

static void lv_create_show_page(lv_obj_t * parent)
{
    lv_obj_t * btn_enter = lv_btn_create(parent);
    //lv_obj_remove_style_all(btn_enter);
    lv_obj_set_size(btn_enter, 220, 80);
    lv_obj_align(btn_enter, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_opa(btn_enter, LV_OPA_100, 0);
    lv_obj_set_style_bg_color(btn_enter, lv_color_hex(COLOUR_WHITE), 0);  
    //lv_obj_add_event_cb(btn_enter, admin_back_event_cb, LV_EVENT_SHORT_CLICKED, parent);

    label_alert_info = lv_label_create(btn_enter);
    lv_obj_remove_style_all(label_alert_info);
    lv_label_set_long_mode(label_alert_info, LV_LABEL_LONG_WRAP);
    //lv_obj_set_width(label_alert_info, LV_PCT(100));
    lv_obj_set_style_text_color(label_alert_info, lv_color_hex(COLOUR_BLACK), 0);
    lv_obj_set_style_text_font(label_alert_info, &font_KaiTi22, 0);
    lv_label_set_text(label_alert_info, "验证成功");
    lv_obj_center(label_alert_info);

    return;
}

static bool show_public_layer_enter_cb(struct lv_layer_t * layer)
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
        lv_obj_set_style_bg_color(layer->lv_obj_layer, lv_color_hex(COLOUR_BLACK), 0);  
        lv_obj_set_style_opa(layer->lv_obj_layer, LV_OPA_80, 0);

        lv_create_show_page(layer->lv_obj_layer);

        lv_obj_add_flag(show_public_layer.lv_obj_layer, LV_OBJ_FLAG_HIDDEN);
	}

	return ret;
}

static bool show_public_layer_exit_cb(struct lv_layer_t * layer)
{
    tips_clear_event();
    LV_LOG_USER("");
}

static void show_public_layer_timer_cb(lv_timer_t * tmr)
{
    int32_t isTmOut;
    uint8_t infoTips[30];
    static uint32_t period_clock_next, tips_end_time;

    isTmOut = (lv_tick_get() - (period_clock_next + 3000));
    if(isTmOut > 0){
        period_clock_next = lv_tick_get();

        // esp_fp_user_info touchEvent;
        // touchEvent.id = 333;
        // touchEvent.result = 1;
        // touch_event_send(&touchEvent);
    }

    tips_show_info_t * Event = tips_info_fetch();
    if((NULL != Event) && (TIP_ACTION_PREPARE == Event->tipsEnable)){
        switch(Event->tipsEvent)
        {
            case TIP_EVENT_VERITY_SUCCESS:
            {
                sprintf(infoTips, "%s", "验证成功！");
            }
            break;
            case TIP_EVENT_VERITY_FAILED:
            {
                sprintf(infoTips, "%s", "验证失败！");
            }
            break;
            case TIP_EVENT_INPUT_SUCCESS:
            {
                sprintf(infoTips, "%s", "录入成功！");
            }
            break;
            case TIP_EVENT_USER_NOT_EXIST:
            {
                sprintf(infoTips, "%s", "查无此用户！");
            }
            break;
            case TIP_EVENT_USER_DELETE_END:
            {
                sprintf(infoTips, "%06d删除成功！", Event->userdata);
            }
            break;
            case TIP_EVENT_IMPLICIT_INFO:
            {
                // LV_LOG_USER("TIP_EVENT_IMPLICIT_INFO");
            }
            break;

            default:
            {
                sprintf(infoTips, "%s", error_code_tips[(Event->tipsEvent)]);
            }
            break;
        }

        Event->tipsEnable = TIP_ACTION_SHOWING;
        tips_end_time = Event->tipsTmOut + lv_tick_get();

        if(TIP_EVENT_IMPLICIT_INFO != Event->tipsEvent){
            lv_label_set_text(label_alert_info, infoTips);
            lv_obj_clear_flag(show_public_layer.lv_obj_layer, LV_OBJ_FLAG_HIDDEN);
        }
    }
    else if((NULL != Event) && (TIP_ACTION_SHOWING == Event->tipsEnable)){
        isTmOut = (lv_tick_get() - tips_end_time);
        if(isTmOut > 0){
            Event->tipsEnable = TIP_ACTION_OFF;
            lv_obj_add_flag(show_public_layer.lv_obj_layer, LV_OBJ_FLAG_HIDDEN);

            if(Event->tips_end_cb){
                LV_LOG_USER("excute tips_end_cb");
                Event->tips_end_cb(Event->userdata);
            }
        }
    }
    else if((NULL != Event) && (TIP_ACTION_OFF == Event->tipsEnable)){
        lv_obj_add_flag(show_public_layer.lv_obj_layer, LV_OBJ_FLAG_HIDDEN);
    }
}
#endif
