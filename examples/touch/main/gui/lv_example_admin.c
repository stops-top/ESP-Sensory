#include "lv_demo.h"
#include "./lv_example_func.h"
#include "./lv_example_image.h"

#if LV_BUILD_EXAMPLES

typedef enum{
    SPRITE_ADMIN_ENTER,

    SPRITE_ADMIN_OPERATION,

    SPRITE_ADMIN_ADD_USER_FIRST,
    SPRITE_ADMIN_ADD_USER_RELEASE,
    SPRITE_ADMIN_ADD_USER_SECOND,
    SPRITE_ADMIN_END_RETURN,

    SPRITE_ADMIN_DEL_USER_FIRST,
    SPRITE_ADMIN_DEL_USER_RELEASE,
    SPRITE_ADMIN_DEL_USER_SECOND,

    SPRITE_ADMIN_MAX,
};

static bool administrator_layer_enter_cb(struct lv_layer_t * layer);
static bool administrator_layer_exit_cb(struct lv_layer_t * layer);
static void administrator_layer_timer_cb(lv_timer_t * tmr);

lv_layer_t administrator_layer ={
    .lv_obj_name    = "administrator_layer",
    .lv_obj_layer   = NULL,
    .lv_show_layer  = &show_public_layer,
    .enter_cb       = administrator_layer_enter_cb,
    .exit_cb        = administrator_layer_exit_cb,
    .timer_cb       = administrator_layer_timer_cb,
};

lv_obj_t * label_record_serial, * label_record_info, * img_finger_focus, * label_end_serial, *label_delete_serial;
lv_obj_t * sprite_admin_enter, * sprite_admin_operation, *sprite_admin_press, * sprite_admin_return, * sprite_admin_delete;
lv_obj_t * list_adddel_select, * label_back_time;

lv_obj_t * admin_menu_Event;

static uint8_t end_back_time;
static uint8_t sprite_admin_focus;
static uint32_t period_clock_next;

static void sprite_admin_goto(uint8_t sprite, uint32_t serial)
{
    uint8_t textBuf[30];

    lv_obj_add_flag(sprite_admin_enter, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(sprite_admin_operation, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(sprite_admin_press, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(sprite_admin_return, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(sprite_admin_delete, LV_OBJ_FLAG_HIDDEN);

    switch(sprite)
    {
        case SPRITE_ADMIN_ENTER:
            lv_obj_clear_flag(sprite_admin_enter, LV_OBJ_FLAG_HIDDEN);
        break;
        case SPRITE_ADMIN_OPERATION:
            lv_obj_clear_flag(sprite_admin_operation, LV_OBJ_FLAG_HIDDEN);
        break;

        case SPRITE_ADMIN_ADD_USER_FIRST:
            //sprintf(textBuf, "登记号码 %06d", serial);
            sprintf(textBuf, "登记号码");
            lv_label_set_text(label_record_serial, textBuf);

            lv_label_set_text(label_record_info, "请按压手指");
            lv_img_set_src(img_finger_focus, &press_small);
            lv_obj_clear_flag(sprite_admin_press, LV_OBJ_FLAG_HIDDEN);
        break;
        case SPRITE_ADMIN_DEL_USER_FIRST:
            lv_label_set_text(label_record_serial, "删除用户");
            lv_label_set_text(label_record_info, "请按压手指");
            lv_img_set_src(img_finger_focus, &press_small);
            lv_obj_clear_flag(sprite_admin_press, LV_OBJ_FLAG_HIDDEN);
        break;

        case SPRITE_ADMIN_ADD_USER_RELEASE:
            sprintf(textBuf, "登记号码 %06d", serial);
            lv_label_set_text(label_record_serial, textBuf);

            lv_label_set_text(label_record_info, "请抬手");
            lv_img_set_src(img_finger_focus, &liftOff_small);
            lv_obj_clear_flag(sprite_admin_press, LV_OBJ_FLAG_HIDDEN);
        break;

        case SPRITE_ADMIN_DEL_USER_RELEASE:
            sprintf(textBuf, "登记号码 %06d", serial);
            lv_label_set_text(label_delete_serial, textBuf);
            lv_obj_clear_flag(sprite_admin_delete, LV_OBJ_FLAG_HIDDEN);
        break;

        case SPRITE_ADMIN_ADD_USER_SECOND:
            sprintf(textBuf, "登记号码 %06d", serial);
            lv_label_set_text(label_record_serial, textBuf);
        
            lv_label_set_text(label_record_info, "请再次按压");

            lv_img_set_src(img_finger_focus, &press_small);
            lv_obj_clear_flag(sprite_admin_press, LV_OBJ_FLAG_HIDDEN);
        break;

        case SPRITE_ADMIN_DEL_USER_SECOND:
            sprintf(textBuf, "删除用户 %06d", serial);
            lv_label_set_text(label_record_serial, textBuf);

            lv_obj_set_style_text_font(label_record_info, &font_KaiTi24, 0);
            lv_label_set_text(label_record_info, "请再次按压手指，确认删除");

            lv_img_set_src(img_finger_focus, &press_small);
            lv_obj_clear_flag(sprite_admin_press, LV_OBJ_FLAG_HIDDEN);
        break;

        case SPRITE_ADMIN_END_RETURN:
            sprintf(textBuf, "登记号码 %06d", serial);
            lv_label_set_text(label_end_serial, textBuf);
            lv_obj_clear_flag(sprite_admin_return, LV_OBJ_FLAG_HIDDEN);
        break;
    }

    sprite_admin_focus = sprite;
}

static void admin_back_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * button = lv_event_get_target(e);

    if(code == LV_EVENT_SHORT_CLICKED) {
        //LV_LOG_USER("SHORT_CLICKED %d, buttton:%X,%d", code, button, button->state);
        lv_goto_layer(&administrator_layer, &finger_touch_layer);
    }
}

bool admin_enter_verify_success_cb(uint32_t userdata)
{
    //LV_LOG_USER("goto SPRITE_ADMIN_OPERATION");
    sprite_admin_goto(SPRITE_ADMIN_OPERATION, userdata);
}

bool admin_record_end_success_cb(uint32_t userdata)
{
    uint8_t leftTimeBuf[30];

    if(end_back_time >1){
        end_back_time--;
        memset(leftTimeBuf, 0, sizeof(leftTimeBuf));
        sprintf(leftTimeBuf, "%ds后自动返回", end_back_time);
        lv_label_set_text(label_back_time, leftTimeBuf);
        tips_info_set(TIP_ACTION_PREPARE, TIP_EVENT_IMPLICIT_INFO, 1000, admin_record_end_success_cb, 0xFF);
    }
    else{
        lv_goto_layer(&administrator_layer, &finger_touch_layer);
    }
}

bool admin_record_wait_liftoff_cb(uint32_t userdata)
{
    if(end_back_time >1){
        end_back_time--;
        LV_LOG_USER("wait liftoff event:%d", end_back_time);
        tips_info_set(TIP_ACTION_PREPARE, TIP_EVENT_IMPLICIT_INFO, 500, admin_record_wait_liftoff_cb, userdata);
    }
    else{
        
        if(SPRITE_ADMIN_ADD_USER_RELEASE == sprite_admin_focus){
            sprite_admin_goto(SPRITE_ADMIN_ADD_USER_SECOND, userdata);
            LV_LOG_USER("wait liftoff no warn, ->SPRITE_ADMIN_ADD_USER_SECOND");
        }
        else{
            sprite_admin_goto(SPRITE_ADMIN_DEL_USER_SECOND, userdata);
            LV_LOG_USER("wait liftoff no warn, ->SPRITE_ADMIN_DEL_USER_SECOND");
        }
    }
}

bool admin_add_step2_success_cb(uint32_t userdata)
{
    //LV_LOG_USER("goto SPRITE_ADMIN_END_RETURN");
    sprite_admin_goto(SPRITE_ADMIN_END_RETURN, userdata);
    end_back_time = 3;
    tips_info_set(TIP_ACTION_PREPARE, TIP_EVENT_IMPLICIT_INFO, 1000, admin_record_end_success_cb, 0xFF);
}

bool admin_del_step2_success_cb(uint32_t userdata)
{
    //LV_LOG_USER("goto main menu");
    lv_goto_layer(&administrator_layer, &finger_touch_layer);
}

bool admin_del_user_not_exist_cb(uint32_t userdata)
{
    //LV_LOG_USER("user isn't exist");
    sprite_admin_goto(SPRITE_ADMIN_DEL_USER_FIRST, userdata);
}

static void admin_test_event_cb(lv_event_t * e)
{
    uint8_t focus;
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * button = lv_event_get_target(e);
    uint8_t touch_event = lv_event_get_user_data(e);
    esp_user_info * param = lv_event_get_param(e);

    if(code == LV_EVENT_SHORT_CLICKED) {
        if(param){/*parse */ 
        #if OFFLINE_TEST
            LV_LOG_USER("touch_event:%d", touch_event);
            #else
            LV_LOG_USER("id:%d, stat:%d, level:%d, template:%d, result:%d", \
                        param->id,param->stat,param->level,param->template, param->result);
            touch_event = param->result;
            #endif
        }

        focus = sprite_admin_focus;
        switch(focus)
        {
            case SPRITE_ADMIN_ENTER:
                if(SINGAL_VERIFY_OK == touch_event){
                    user_operation_mode_set(USER_OPERATION_MODE_UNKNOW);
                    tips_info_set(TIP_ACTION_PREPARE, TIP_EVENT_VERITY_SUCCESS, 2000, admin_enter_verify_success_cb, 0xFF);
                }
                else{
                    tips_info_set(TIP_ACTION_PREPARE, param->result, 2000, NULL, 0xFF);
                }
            break;
            /*user choose*/
            case SPRITE_ADMIN_OPERATION:
            break;

            #if 0 
            case SPRITE_ADMIN_ADD_USER_FIRST:
                if(SINGAL_VERIFY_OK == touch_event){
                    LV_LOG_USER("touch input ok, create a new user num.........");
                    #if OFFLINE_TEST
                    create_rand_user_id();
                    #else
                    user_serial = param->id;
                    #endif
                    sprite_admin_goto(SPRITE_ADMIN_ADD_USER_RELEASE, user_serial);
                    end_back_time = 6; //6*500ms == 3s
                    tips_info_set(TIP_ACTION_PREPARE, TIP_EVENT_IMPLICIT_INFO, 500, admin_record_wait_liftoff_cb, user_serial);
                }
                else{
                    tips_info_set(TIP_ACTION_PREPARE, param->result, 2000, NULL, 0xFF);
                }
            break;
            #endif
            
            case SPRITE_ADMIN_DEL_USER_FIRST:
                if(SINGAL_VERIFY_OK == touch_event){
                    LV_LOG_USER("touch recongnise ok, response a current user num.........");
                    #if OFFLINE_TEST
                    create_rand_user_id();
                    #else
                    user_serial = param->id;
                    #endif
                    sprite_admin_goto(SPRITE_ADMIN_DEL_USER_RELEASE, user_serial);
                    end_back_time = 6; //6*500ms == 3s
                    tips_info_set(TIP_ACTION_PREPARE, TIP_EVENT_IMPLICIT_INFO, 500, admin_record_wait_liftoff_cb, user_serial);
                }
                else{
                //else if(SINGAL_VERIFY_ERR == touch_event){
                    LV_LOG_USER("SINGAL_TOUCH_LIFTOFF, no user");
                    tips_info_set(TIP_ACTION_PREPARE, param->result, 2000, admin_del_user_not_exist_cb, 0xFF);
                }
            break;

            /* If no warn of touch liftoff, goto next when timeout( how long??)
            */
           case SPRITE_ADMIN_ADD_USER_RELEASE:
           case SPRITE_ADMIN_DEL_USER_RELEASE:
                if(SINGAL_TOUCH_DOWN == touch_event){ //tell me "手指没抬起"
                    LV_LOG_USER("[warn]rinse up, step stop here..");
                    //sprite_admin_goto(SPRITE_ADMIN_ADD_USER_SECOND);
                    tips_info_set(TIP_ACTION_OFF, TIP_EVENT_IMPLICIT_INFO, 500, NULL, 0xFF);
                }
                else if(SINGAL_TOUCH_LIFTOFF == touch_event){
                    LV_LOG_USER("liftoff ready -> next");
                    sprite_admin_goto((SPRITE_ADMIN_ADD_USER_RELEASE == focus) ? \
                                    SPRITE_ADMIN_ADD_USER_SECOND: SPRITE_ADMIN_DEL_USER_SECOND, user_serial);
                }
            break;

            case SPRITE_ADMIN_ADD_USER_FIRST:
            case SPRITE_ADMIN_ADD_USER_SECOND:
                #if OFFLINE_TEST
                    create_rand_user_id();
                #else
                    user_serial = param->id;
                #endif
                sprite_admin_goto(SPRITE_ADMIN_ADD_USER_SECOND, user_serial);

                if(SINGAL_VERIFY_OK == touch_event){
                    user_operation_mode_set(USER_OPERATION_MODE_UNKNOW);
                    tips_info_set(TIP_ACTION_PREPARE, TIP_EVENT_INPUT_SUCCESS, 2000, admin_add_step2_success_cb, user_serial);
                }
                else{
                    tips_info_set(TIP_ACTION_PREPARE, param->result, 2000, NULL, 0xFF);
                }
            break;

            case SPRITE_ADMIN_DEL_USER_SECOND:
                if(SINGAL_VERIFY_OK == touch_event){
                    user_operation_mode_set(USER_OPERATION_MODE_UNKNOW);
                    tips_info_set(TIP_ACTION_PREPARE, TIP_EVENT_USER_DELETE_END, 2000, admin_del_step2_success_cb, user_serial);
                }
                else{
                    tips_info_set(TIP_ACTION_PREPARE, param->result, 2000, NULL, 0xFF);
                }
            break;

            /*6s 自动返回，计时，跳转*/
            case SPRITE_ADMIN_END_RETURN:
                LV_LOG_USER("End, monitor no event:%d", touch_event);
                tips_info_set(TIP_ACTION_PREPARE, TIP_EVENT_IMPLICIT_INFO, 1000, admin_record_end_success_cb, user_serial);
            break;
        }
    }
}

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    uint8_t add_user = lv_event_get_user_data(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked: %s, %d", lv_list_get_btn_text(list_adddel_select, obj), add_user);
        if(add_user){
            sprite_admin_goto(SPRITE_ADMIN_ADD_USER_FIRST, 0xFF);
            user_operation_mode_set(USER_OPERATION_MODE_USER_ADD);
        }
        else{
            sprite_admin_goto(SPRITE_ADMIN_DEL_USER_FIRST, 0xFF);
            user_operation_mode_set(USER_OPERATION_MODE_USRE_DEL);
        }
    }
}

static void lv_create_admin_back(lv_obj_t * parent)
{
    static lv_coord_t grid_2_col_dsc[] = {LV_GRID_FR(1),LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_2_row_dsc[] = {
                40,
                50,
                50,  
                100,  
                LV_GRID_TEMPLATE_LAST};

    sprite_admin_return = lv_list_create(parent);
    lv_obj_set_style_opa(sprite_admin_return, LV_OPA_100, 0);
    lv_obj_set_size(sprite_admin_return, LV_PCT(100), LV_PCT(100));
    lv_obj_align(sprite_admin_return, LV_ALIGN_TOP_LEFT, 0, 0);
    
    lv_obj_set_style_bg_color(sprite_admin_return, lv_color_hex(COLOUR_GREY), LV_PART_MAIN);
    lv_obj_set_style_border_width(sprite_admin_return, 0, 0);
    lv_obj_set_style_radius(sprite_admin_return, 0, 0);

    lv_obj_set_grid_dsc_array(sprite_admin_return, grid_2_col_dsc, grid_2_row_dsc);

    lv_obj_t * label_title1 = lv_label_create(sprite_admin_return);
    lv_obj_remove_style_all(label_title1);
    lv_obj_set_style_text_color(label_title1, lv_color_hex(COLOUR_BLACK), 0);
    lv_obj_set_style_text_font(label_title1, &font_KaiTi36, 0);

    lv_label_set_text(label_title1, "Hi 欢迎！");
    lv_obj_center(label_title1);
    lv_obj_set_grid_cell(label_title1, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    lv_obj_t * label_title2 = lv_label_create(sprite_admin_return);
    lv_obj_remove_style_all(label_title2);
    lv_obj_set_style_text_color(label_title2, lv_color_hex(COLOUR_BG_GREY), 0);
    lv_obj_set_style_text_font(label_title2, &font_KaiTi24, 0);

    lv_label_set_text(label_title2, "登记号码 000601");
    lv_obj_center(label_title2);
    lv_obj_set_grid_cell(label_title2, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    label_end_serial = label_title2;

    lv_obj_t * label_title3 = lv_label_create(sprite_admin_return);
    lv_obj_remove_style_all(label_title3);
    lv_obj_set_style_text_color(label_title3, lv_color_hex(COLOUR_BG_GREY), 0);
    lv_obj_set_style_text_font(label_title3, &font_KaiTi24, 0);

    lv_label_set_text(label_title3, "3s后自动返回");
    lv_obj_center(label_title3);
    lv_obj_set_grid_cell(label_title3, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    label_back_time = label_title3;

    static lv_point_t line_points1[2] = { {(0 + 15), 160}, {(320 - 40), 160}};

    lv_obj_t * line1 = lv_line_create(sprite_admin_return);
    lv_line_set_points(line1, line_points1, 2);     /*Set the points*/
    lv_obj_add_style(line1, &style_line, 0);
    //lv_obj_align(line1, LV_ALIGN_TOP_LEFT, 0, 0);
    //lv_obj_set_pos(line1, 0, 20);

    return;
}

static void lv_create_admin_delete_confirm(lv_obj_t * parent)
{
    static lv_coord_t grid_2_col_dsc[] = {LV_GRID_FR(1),LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_2_row_dsc[] = {
                80,
                80,  
                70,  
                LV_GRID_TEMPLATE_LAST};

    sprite_admin_delete = lv_list_create(parent);
    lv_obj_set_style_opa(sprite_admin_delete, LV_OPA_100, 0);
    lv_obj_set_size(sprite_admin_delete, LV_PCT(100), LV_PCT(100));
    lv_obj_align(sprite_admin_delete, LV_ALIGN_TOP_LEFT, 0, 0);
    
    lv_obj_set_style_bg_color(sprite_admin_delete, lv_color_hex(COLOUR_GREY), LV_PART_MAIN);
    lv_obj_set_style_border_width(sprite_admin_delete, 0, 0);
    lv_obj_set_style_radius(sprite_admin_delete, 0, 0);
    lv_obj_clear_flag(sprite_admin_delete, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_grid_dsc_array(sprite_admin_delete, grid_2_col_dsc, grid_2_row_dsc);

    lv_obj_t * label_title1 = lv_label_create(sprite_admin_delete);
    lv_obj_remove_style_all(label_title1);
    lv_obj_set_style_text_color(label_title1, lv_color_hex(COLOUR_BG_GREY), 0);
    lv_obj_set_style_text_font(label_title1, &font_KaiTi24, 0);

    lv_label_set_text(label_title1, "删除用户");
    lv_obj_center(label_title1);
    lv_obj_set_grid_cell(label_title1, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_t * label_title2 = lv_label_create(sprite_admin_delete);
    lv_obj_remove_style_all(label_title2);
    lv_obj_set_style_text_color(label_title2, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_text_font(label_title2, &font_KaiTi36, 0);

    lv_label_set_text(label_title2, "登记号码 000601");
    lv_obj_center(label_title2);
    lv_obj_set_grid_cell(label_title2, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    label_delete_serial = label_title2;

    lv_obj_t * label_title3 = lv_label_create(sprite_admin_delete);
    lv_obj_remove_style_all(label_title3);
    lv_obj_set_style_text_color(label_title3, lv_color_hex(COLOUR_BLACK), 0);
    lv_obj_set_style_text_font(label_title3, &font_KaiTi36, 0);

    lv_label_set_text(label_title3, "请抬手");
    lv_obj_center(label_title3);
    lv_obj_set_grid_cell(label_title3, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    static lv_point_t line_points1[2] = { {(0 + 15), 80}, {(320 - 40), 80}};

    lv_obj_t * line1 = lv_line_create(sprite_admin_delete);
    lv_line_set_points(line1, line_points1, 2);     /*Set the points*/
    lv_obj_add_style(line1, &style_line, 0);
    //lv_obj_align(line1, LV_ALIGN_TOP_LEFT, 0, 0);
    //lv_obj_set_pos(line1, 0, 20);

    static lv_point_t line_points2[2] = { {(0 + 15), 160}, {(320 - 40), 160}};

    lv_obj_t * line2 = lv_line_create(sprite_admin_delete);
    lv_line_set_points(line2, line_points2, 2);     /*Set the points*/
    lv_obj_add_style(line2, &style_line, 0);
    //lv_obj_align(line1, LV_ALIGN_TOP_LEFT, 0, 0);
    //lv_obj_set_pos(line2, 0, 20);

    return;
}

static void lv_create_admin_press(lv_obj_t * parent)
{
    static lv_coord_t grid_2_col_dsc[] = {LV_GRID_FR(1),LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_2_row_dsc[] = {
                50,
                130,  
                60,  
                LV_GRID_TEMPLATE_LAST};

    sprite_admin_press = lv_list_create(parent);
    lv_obj_set_style_opa(sprite_admin_press, LV_OPA_100, 0);
    lv_obj_set_size(sprite_admin_press, LV_PCT(100), LV_PCT(100));
    lv_obj_align(sprite_admin_press, LV_ALIGN_TOP_LEFT, 0, 0);
    
    lv_obj_set_style_bg_color(sprite_admin_press, lv_color_hex(COLOUR_GREY), LV_PART_MAIN);
    lv_obj_set_style_border_width(sprite_admin_press, 0, 0);
    lv_obj_set_style_radius(sprite_admin_press, 0, 0);

    lv_obj_set_grid_dsc_array(sprite_admin_press, grid_2_col_dsc, grid_2_row_dsc);


    lv_obj_t * label_title1 = lv_label_create(sprite_admin_press);
    lv_obj_remove_style_all(label_title1);
    lv_obj_set_style_text_color(label_title1, lv_color_hex(COLOUR_BG_GREY), 0);
    lv_obj_set_style_text_font(label_title1, &font_KaiTi24, 0);

    lv_label_set_text(label_title1, "登记号码 00601");
    lv_obj_center(label_title1);
    lv_obj_set_grid_cell(label_title1, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    label_record_serial = label_title1;

    lv_obj_t * img_Finger = lv_img_create(sprite_admin_press);
    lv_img_set_src(img_Finger, &press_small);
    lv_obj_set_grid_cell(img_Finger, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    img_finger_focus = img_Finger;

    lv_obj_t * label_title2 = lv_label_create(sprite_admin_press);
    lv_obj_remove_style_all(label_title2);
    lv_obj_set_style_text_color(label_title2, lv_color_hex(COLOUR_BLACK), 0);
    lv_obj_set_style_text_font(label_title2, &font_KaiTi36, 0);

    lv_label_set_text(label_title2, "请按压手指");
    lv_obj_center(label_title2);
    lv_obj_set_grid_cell(label_title2, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    label_record_info = label_title2;

    return;
}

static void lv_create_admin_operation(lv_obj_t * parent)
{
    sprite_admin_operation = lv_list_create(parent);
    lv_obj_set_style_opa(sprite_admin_operation, LV_OPA_100, 0);
    lv_obj_set_size(sprite_admin_operation, LV_PCT(100), 200);
    lv_obj_align(sprite_admin_operation, LV_ALIGN_TOP_LEFT, 0, 40);
    
    lv_obj_set_style_bg_color(sprite_admin_operation, lv_color_hex(COLOUR_WHITE), LV_PART_MAIN);
    lv_obj_set_style_border_width(sprite_admin_operation, 0, 0);
    lv_obj_set_style_radius(sprite_admin_operation, 0, 0);

    /*Add buttons to the list*/
    lv_obj_t * btn, * label_enter, *label_enter2;

    list_adddel_select = lv_list_create(sprite_admin_operation);
    lv_obj_set_size(list_adddel_select, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_border_width(list_adddel_select, 0, 0);
    lv_obj_set_style_radius(list_adddel_select, 0, 0);
    lv_obj_set_style_text_font(list_adddel_select, &font_KaiTi24, 0);
    lv_obj_align(list_adddel_select, LV_ALIGN_TOP_LEFT, 0, 0);

    //lv_list_add_text(list_adddel_select, "File");
    //lv_label_set_text(hint, LV_SYMBOL_UP" 17% growth this week");
    btn = lv_list_add_btn(list_adddel_select, LV_SYMBOL_FILE, "新增用户");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, 1);

    label_enter = lv_label_create(btn);
    lv_obj_set_style_text_font(label_enter, \
        lv_obj_get_style_text_font(sprite_admin_operation, 0), 0);
    lv_obj_set_style_text_color(label_enter, lv_palette_darken(LV_PALETTE_GREY, 2), 0);

    lv_label_set_text(label_enter, LV_SYMBOL_RIGHT);
    lv_obj_align(label_enter, LV_ALIGN_RIGHT_MID, 30, 10);

    btn = lv_list_add_btn(list_adddel_select, LV_SYMBOL_DIRECTORY, "删除用户");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, 0);

    label_enter2 = lv_label_create(btn);
    lv_obj_set_style_text_font(label_enter2, \
        lv_obj_get_style_text_font(sprite_admin_operation, 0), 0);
    lv_obj_set_style_text_color(label_enter2, lv_palette_darken(LV_PALETTE_GREY, 2), 0);

    lv_label_set_text(label_enter2, LV_SYMBOL_RIGHT);
    lv_obj_align(label_enter2, LV_ALIGN_RIGHT_MID, 0, 0);

    return;
}

static void lv_create_admin_enter(lv_obj_t * parent)
{
    static lv_coord_t grid_2_col_dsc[] = {LV_GRID_FR(1),LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_2_row_dsc[] = {
                30,
                40,  
                40,  
                LV_GRID_TEMPLATE_LAST};

    sprite_admin_enter = lv_list_create(parent);
    lv_obj_set_style_opa(sprite_admin_enter, LV_OPA_100, 0);
    lv_obj_set_size(sprite_admin_enter, LV_PCT(100), 200);
    lv_obj_align(sprite_admin_enter, LV_ALIGN_TOP_LEFT, 0, 40);
    
    lv_obj_set_style_bg_color(sprite_admin_enter, lv_color_hex(COLOUR_WHITE), LV_PART_MAIN);
    lv_obj_set_style_border_width(sprite_admin_enter, 0, 0);
    lv_obj_set_style_radius(sprite_admin_enter, 0, 0);

    lv_obj_set_grid_dsc_array(sprite_admin_enter, grid_2_col_dsc, grid_2_row_dsc);

    lv_obj_t * label_title1 = lv_label_create(sprite_admin_enter);
    lv_obj_remove_style_all(label_title1);
    lv_obj_set_style_text_color(label_title1, lv_color_hex(COLOUR_BLACK), 0);
    lv_obj_set_style_text_font(label_title1, &font_KaiTi36, 0);

    lv_label_set_text(label_title1, "管理员模式");
    lv_obj_center(label_title1);
    lv_obj_set_grid_cell(label_title1, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    lv_obj_t * label_title2 = lv_label_create(sprite_admin_enter);
    lv_obj_remove_style_all(label_title2);
    lv_obj_set_style_text_color(label_title2, lv_color_hex(COLOUR_BG_GREY), 0);
    lv_obj_set_style_text_font(label_title2, &font_KaiTi24, 0);

    lv_label_set_text(label_title2, "按压指纹,确认管理员权限");
    lv_obj_center(label_title2);
    lv_obj_set_grid_cell(label_title2, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    return;
}

static void lv_create_administrator_page(lv_obj_t * parent)
{
    lv_create_admin_enter(parent);
    lv_create_admin_operation(parent);
    lv_create_admin_press(parent);
    lv_create_admin_back(parent);
    lv_create_admin_delete_confirm(parent);

    lv_obj_t * btn_enter = lv_btn_create(parent);
    //lv_obj_remove_style_all(btn_enter);
    //lv_obj_set_size(btn_enter, 50, 40);
    lv_obj_set_style_radius(btn_enter, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(btn_enter, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn_enter, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(btn_enter, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_obj_add_event_cb(btn_enter, admin_back_event_cb, LV_EVENT_SHORT_CLICKED, parent);

    lv_obj_t * label_enter = lv_label_create(btn_enter);
    lv_obj_remove_style_all(label_enter);
    lv_obj_set_style_text_color(label_enter, lv_palette_darken(LV_PALETTE_GREY, 2), 0);
    //lv_obj_set_style_text_font(label_enter, &font_lanting16, 0);
    //lv_label_set_text(label_enter, "<-");LV_SYMBOL_PREV
    lv_label_set_text(label_enter, LV_SYMBOL_LEFT);
    lv_obj_center(label_enter);

#if OFFLINE_TEST
    lv_obj_t * testBtn, * label_test;
    
    testBtn = lv_btn_create(parent);
    lv_obj_set_style_opa(testBtn, LV_OPA_20, 0);
    lv_obj_align(testBtn, LV_ALIGN_TOP_MID, 50, 0);
    lv_obj_add_event_cb(testBtn, admin_test_event_cb, LV_EVENT_SHORT_CLICKED, SINGAL_VERIFY_OK);

    label_test = lv_label_create(testBtn);
    lv_obj_remove_style_all(label_test);
    lv_obj_set_style_text_color(label_test, lv_color_black(), 0);
    lv_obj_set_style_text_font(label_test, &font_KaiTi24, 0);
    lv_label_set_text(label_test, "OK");
    lv_obj_center(label_test);

    testBtn = lv_btn_create(parent);
    lv_obj_set_style_opa(testBtn, LV_OPA_20, 0);
    lv_obj_align(testBtn, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_event_cb(testBtn, admin_test_event_cb, LV_EVENT_SHORT_CLICKED, SINGAL_VERIFY_ERR);

    label_test = lv_label_create(testBtn);
    lv_obj_remove_style_all(label_test);
    lv_obj_set_style_text_color(label_test, lv_color_black(), 0);
    lv_obj_set_style_text_font(label_test, &font_KaiTi24, 0);
    lv_label_set_text(label_test, "ER");
    lv_obj_center(label_test);

    admin_menu_Event = testBtn;
    
    testBtn = lv_btn_create(parent);
    lv_obj_set_style_opa(testBtn, LV_OPA_20, 0);
    lv_obj_align(testBtn, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(testBtn, admin_test_event_cb, LV_EVENT_SHORT_CLICKED, SINGAL_TOUCH_DOWN);

    label_test = lv_label_create(testBtn);
    lv_obj_remove_style_all(label_test);
    lv_obj_set_style_text_color(label_test, lv_color_black(), 0);
    lv_obj_set_style_text_font(label_test, &font_KaiTi24, 0);
    lv_label_set_text(label_test, "Dw");
    lv_obj_center(label_test);

    testBtn = lv_btn_create(parent);
    lv_obj_set_style_opa(testBtn, LV_OPA_20, 0);
    lv_obj_align(testBtn, LV_ALIGN_BOTTOM_MID, 50, 0);
    lv_obj_add_event_cb(testBtn, admin_test_event_cb, LV_EVENT_SHORT_CLICKED, SINGAL_TOUCH_LIFTOFF);

    label_test = lv_label_create(testBtn);
    lv_obj_remove_style_all(label_test);
    lv_obj_set_style_text_color(label_test, lv_color_black(), 0);
    lv_obj_set_style_text_font(label_test, &font_KaiTi24, 0);
    lv_label_set_text(label_test, "UP");
    lv_obj_center(label_test);
#else
    lv_obj_t * testBtn;
    testBtn = lv_btn_create(parent);
    lv_obj_set_size(testBtn, 0, 0);
    lv_obj_set_style_opa(testBtn, LV_OPA_20, 0);
    lv_obj_align(testBtn, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_event_cb(testBtn, admin_test_event_cb, LV_EVENT_SHORT_CLICKED, SINGAL_VERIFY_ERR);  
    admin_menu_Event = testBtn;  
#endif
    return;
}

static bool administrator_layer_enter_cb(struct lv_layer_t * layer)
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
        lv_create_administrator_page(layer->lv_obj_layer);

        sprite_admin_goto(SPRITE_ADMIN_ENTER, 0xFF);
        //sprite_admin_goto(SPRITE_ADMIN_END_RETURN);
        user_operation_mode_set(USER_OPERATION_MODE_ADMIN);

        period_clock_next = lv_tick_get()+ 8000;  
	}

	return ret;
}

static bool administrator_layer_exit_cb(struct lv_layer_t * layer)
{
    LV_LOG_USER("");
}

static void administrator_layer_timer_cb(lv_timer_t * tmr)
{
    int32_t isTmOut;

    isTmOut = (lv_tick_get() - (period_clock_next + 2000));
    if(isTmOut > 0){
        period_clock_next = lv_tick_get();
    }
    
    esp_user_info * touch_event;
    touch_event = touch_event_receive();
    if(touch_event){
        lv_event_send(admin_menu_Event, LV_EVENT_SHORT_CLICKED, touch_event);
    }
}
#endif
