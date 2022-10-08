#include "bsp_board.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "lv_demo.h"
#include "./lv_example_func.h"

#if LV_BUILD_EXAMPLES && LV_USE_FLEX

/**********************
 *   STATIC FUNCTIONS
 **********************/
static const char *TAG = "lv_func";

uint32_t user_serial;
uint8_t alertFailedList = 1;
static USER_OPERATION_MODE ui_operation_mode;
static tips_show_info_t tips_show_info;

const uint8_t * error_code_tips[11]={
    #if 1
        /*ESP_FP_RC_OK = 0x00,*/            "匹配成功",
        /*ESP_FP_VERIFY_FAIL = 0x01,*/      "匹配失败",         
        /*ESP_FP_FINGER_HOLD = 0x02,*/      "手指没抬起",
        /*ESP_FP_FINGER_NONE = 0x03,*/      "未检测到手指按压",
        /*ESP_FP_ENROLL_FAIL = 0x04,*/      "注册指纹失败", 
        /*ESP_FP_TRY_AGAIN = 0x05,*/        "需要再次采集图像",      
	    /*ESP_FP_SYS_ERR = 0x06, */         "系统故障",
	    /*ESP_FP_ENROLL_SAME = 0x10,  */    "重复注册指纹",
	    /*ESP_FP_MEM_ERR = 0xF0,   */       "分配内存失败",
	    /*ESP_FP_SPIFFS_ERR = 0xF5,  	*/  "数据存储失败",
	    /*ESP_FP_DEVICE_ERR = 0xFF  */      "硬件初始化失败",
    #else
        /*C_OK = 0x00,                           */"成功",           
        /*C_FINGER_NO_UP = 0x01,                 */"手指没抬起",
        /*C_FINGER_DETECT_ERR = 0x02,            */"未检测到手指按压",
        /*C_ENROLLMENT_ERR = 0x03,               */"注册失败", 
        /*C_VERIFY_FAIL = 0x04,                  */"匹配失败",   
        /*C_GET_IMAGE_FAIL = 0x05,               */"采集图像失败",   
        /*C_IMAGE_NG = 0x06,                     */"图片质量差",        
        /*C_SENSOR_INIT_ERR = 0x07,              */"SNESOR初始化失败",
        /*C_EXTRACT_TEMPLATE_FAIL = 0x08,        */"抽取模板失败",
        /*C_CREATE_MULTI_TEMPLATE_FAIL = 0x09,   */"合成多模板失败",
        /*C_IMAGE_AREA_ERR = 0x0A,               */"图像有效面积太小",
        /*C_IMAGE_DRY_ERR = 0x0B,                */"手指太干",        
        /*C_IMAGE_HUMIDITY_ERR = 0x0C,           */"手指太湿",
        /*C_DYNAMIC_UPDATE_ERR = 0x0D,           */"模板动态更新失败",
        /*C_SENSOR_ID_ERR = 0x0E,                */"SENSOR ID 错误",
        /*C_MALLOC_ERR = 0x0F,                   */"内存申请失败",
        /*C_STATIC_PATTERN_ERR = 0x10,           */"SENSOR表面存在\n疑似异物",
        /*C_CRACK_ERR = 0x11,                    */"SENSOR表面存在\n疑似裂纹",
        /*C_MULTI_FINGERS = 0x12,                */"同一个模板录入\n多个手指",
        /*C_RESET_ERR = 0x13,                    */"SENSOR异常复位",
        /*C_IMAGE_STATUS_ERR = 0x14,             */"采图中断标记错误",
        /*C_DEVICE_ERR = 0x15,                   */"设备异常故障",  
        /*C_PARAM_ERROR = 0x16,                  */"输入参数错误",
        /*C_GENERAL_ERROR = 0x17,                */"一般错误",
        /*C_NOT_SUPPORT = 0x18,                  */"不支持该操作",
        /*C_METERIALS_ERROR = 0x19,              */"SENSOR料号错误",
        /*C_MUCH_OVERLAP = 0x1A,                 */"注册时按压位置\n重合度过高",
        /*C_EXIST_ENROLL_ID = 0x1B,              */"手指重复注册",
        /*C_SENSOR_BAD_LINE_ERR = 0x1C,          */"SENSOR存在疑似坏线",
        #endif
};

uint32_t sys_get_Second()
{
    uint32_t sec = (lv_tick_get()+ 1234567)/1000;
    return sec;
}

void create_rand_user_id()
{
    //user_serial = lv_rand(10, 9999);
    LV_LOG_USER("!!!!! RAND_ID:%d", user_serial);
}

uint8_t user_operation_mode_set(USER_OPERATION_MODE mode_set)
{
    ui_operation_mode = mode_set;
    // esp_fp_mode_set(mode_set);
    switch(ui_operation_mode){
        case USER_OPERATION_MODE_NORMAL:
        LV_LOG_USER("####:USER_OPERATION_MODE_NORMAL");
        //esp_fp_mode_set(mode_set);
        break;
        case USER_OPERATION_MODE_ADMIN:
        LV_LOG_USER("####:USER_OPERATION_MODE_ADMIN");
        //esp_fp_mode_set(mode_set);
        break;
        case USER_OPERATION_MODE_USER_ADD:
        LV_LOG_USER("####:USER_OPERATION_MODE_USER_ADD");
        //esp_fp_mode_set(mode_set);
        break;
        case USER_OPERATION_MODE_USRE_DEL:
        LV_LOG_USER("####:USER_OPERATION_MODE_USRE_DEL");
        //esp_fp_mode_set(mode_set);
        break;
        case USER_OPERATION_MODE_UNKNOW:
        LV_LOG_USER("####:USER_OPERATION_MODE_UNKNOW");
        break;
    }
}

uint8_t tips_info_set(tips_action_t action, tips_event_t tipsEvent, uint32_t tipsTmOut, tips_end_func func_cb, uint32_t userdata)
{
    tips_show_info.tipsEnable       = action;
    tips_show_info.tipsEvent        = tipsEvent;
    tips_show_info.tipsTmOut        = tipsTmOut;
    tips_show_info.tips_end_cb      = func_cb;
    tips_show_info.userdata         = userdata;
/*
    LV_LOG_USER("tipsEnable:%d, %d, %d, func:%x", 
        tips_show_info.tipsEnable,
        tips_show_info.tipsEvent,
        tips_show_info.tipsTmOut,
        tips_show_info.tips_end_cb);
*/
}

tips_show_info_t * tips_info_fetch()
{
    return &tips_show_info;
}

void tips_clear_event()
{
    tips_show_info.tipsEnable = TIP_ACTION_OFF;
    return;
}

static QueueHandle_t audio_event_queue = NULL;

void touch_event_init()
{
    /* Audio control event queue */
    audio_event_queue = xQueueCreate(4, sizeof(esp_fp_user_info));
    if (NULL == audio_event_queue) {
        ESP_LOGI(TAG, "audio_event_queue create failed");
    }
}

esp_fp_user_info* touch_event_receive()
{
    static esp_fp_user_info event;
    if (pdPASS == xQueueReceive(audio_event_queue, &event, 0)) {
        ESP_LOGI(TAG, "audio_event_queue receive end");
        return &event;
    }
    else{
        return NULL;
    }
}

void touch_event_send(esp_fp_user_info * event)
{
    if(event){
        BaseType_t ret_val = xQueueSend(audio_event_queue, event, 0);
        //ESP_RETURN_ON_FALSE(pdPASS == ret_val, ESP_ERR_INVALID_STATE, TAG, "The last event has not been processed yet");
    }
    return;
}
#endif
