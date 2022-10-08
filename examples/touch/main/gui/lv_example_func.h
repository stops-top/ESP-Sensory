/**
 * @file lv_example_scroll.h
 *
 */

#ifndef LV_EXAMPLE_FUNC_H
#define LV_EXAMPLE_FUNC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#include "lv_func_layer.h"

#define OFFLINE_TEST            0
#define USER_MAINMENU_MAX       3
#define USER_CTRLBTN_MAX        7

#define TIME_ENTER_CLOCK_2MIN    (2*60*1000)/1//2min (2*60*1000)/(50 ms)

#define COLOUR_BLACK            0x000000
#define COLOUR_GREY             0xF1F1F1
#define COLOUR_WHITE            0xFFFFFF
#define COLOUR_YELLOW           0xE9BD85
#define COLOUR_BG_GREY          0x5F5F5F
#define COLOUR_BG2_GREY         0xBFBFBF
#define COLOUR_BG1_GREY         0x2F2F2F

#define USER_SERIAL_NUM  654

typedef bool (*tips_end_func)(uint32_t userdata);

#if 1

typedef struct fp_user_info
{
    unsigned int    id;             
    unsigned char   stat; 	      
    unsigned char   level; 	        
	unsigned int    template;     
} esp_user_info; 				    //用户信息

typedef enum btl_bep_rc_e{
        BTL_RC_OK = 0x00,                           /* 成功 */           
        BTL_RC_FINGER_NO_UP = 0x01,                 /* 手指没抬起 */
        BTL_RC_FINGER_DETECT_ERR = 0x02,            /* 未检测到手指按压 */
        BTL_RC_ENROLLMENT_ERR = 0x03,               /* 注册失败 */  
        BTL_RC_VERIFY_FAIL = 0x04,                  /* 匹配失败 */     
        BTL_RC_GET_IMAGE_FAIL = 0x05,               /* 采集图像失败 */     
        BTL_RC_IMAGE_NG = 0x06,                     /* 图片质量差 */         
        BTL_RC_SENSOR_INIT_ERR = 0x07,              /* SNESOR初始化失败 */
        BTL_RC_EXTRACT_TEMPLATE_FAIL = 0x08,        /* 抽取模板失败 */
        BTL_RC_CREATE_MULTI_TEMPLATE_FAIL = 0x09,   /* 合成多模板失败 */
        BTL_RC_IMAGE_AREA_ERR = 0x0A,               /* 图像有效面积太小 */
        BTL_RC_IMAGE_DRY_ERR = 0x0B,                /* 手指太干 */         
        BTL_RC_IMAGE_HUMIDITY_ERR = 0x0C,           /* 手指太湿 */
        BTL_RC_DYNAMIC_UPDATE_ERR = 0x0D,           /* 模板动态更新失败 */
        BTL_RC_SENSOR_ID_ERR = 0x0E,                /* SENSOR ID 错误 */
        BTL_RC_MALLOC_ERR = 0x0F,                   /* 内存申请失败 */
        BTL_RC_STATIC_PATTERN_ERR = 0x10,           /* SENSOR表面存在疑似异物 */
        BTL_RC_CRACK_ERR = 0x11,                    /* SENSOR表面存在疑似裂纹 */
        BTL_RC_MULTI_FINGERS = 0x12,                /* 同一个模板录入多个手指 */
        BTL_RC_RESET_ERR = 0x13,                    /* SENSOR异常复位, V3.2.0版本及之前为0xFC */ 
        BTL_RC_IMAGE_STATUS_ERR = 0x14,             /* 采图中断标记错误 */
        BTL_RC_DEVICE_ERR = 0x15,                   /* 设备异常故障 */   
        BTL_RC_PARAM_ERROR = 0x16,                  /* 输入参数错误 */
        BTL_RC_GENERAL_ERROR = 0x17,                /* 一般错误 */
        BTL_RC_NOT_SUPPORT = 0x18,                  /* 不支持该操作 */
        BTL_RC_METERIALS_ERROR = 0x19,              /* SENSOR料号错误 */
        BTL_RC_MUCH_OVERLAP = 0x1A,                 /* 注册时按压位置重合度过高 */
        BTL_RC_EXIST_ENROLL_ID = 0x1B,              /* 手指重复注册 */
        BTL_RC_SENSOR_BAD_LINE_ERR = 0x1C,          /* SENSOR存在疑似坏线 */ 
} btl_rc_t;
#endif

typedef enum{
    SINGAL_VERIFY_OK,
    SINGAL_VERIFY_ERR,
    SINGAL_TOUCH_DOWN,
    SINGAL_TOUCH_LIFTOFF,
}SINGAL_TOUCH_EVENT;

typedef enum{
    TIP_EVENT_VERITY_SUCCESS = 1,   //验证成功！
    TIP_EVENT_VERITY_FAILED,    //验证失败！
    TIP_EVENT_INPUT_SUCCESS,    //录入成功！
    TIP_EVENT_USER_NOT_EXIST,   //查无此用户！
    TIP_EVENT_USER_DELETE_END,  //00601删除成功
    TIP_EVENT_IMPLICIT_INFO,
    TIP_EVENT_MAX,
}tips_event_t;

typedef enum{
    TIP_ACTION_OFF,
    TIP_ACTION_PREPARE,
    TIP_ACTION_SHOWING,
}tips_action_t;

typedef enum{
    USER_OPERATION_MODE_NORMAL,
    USER_OPERATION_MODE_ADMIN,
    USER_OPERATION_MODE_USER_ADD,
    USER_OPERATION_MODE_USRE_DEL,
    USER_OPERATION_MODE_UNKNOW,
}USER_OPERATION_MODE;

typedef struct{
    tips_action_t   tipsEnable;
    tips_event_t    tipsEvent;
    uint32_t        tipsTmOut;
    uint32_t        userdata;
    tips_end_func   tips_end_cb;
}tips_show_info_t;

extern lv_layer_t show_public_layer;
extern lv_layer_t finger_touch_layer;
extern lv_layer_t administrator_layer;
extern lv_layer_t test_layer;

extern lv_style_t style_line;

extern uint32_t user_serial;
extern uint8_t alertFailedList;
extern const uint8_t * error_code_tips[11];

extern void create_rand_user_id();

extern void tips_clear_event();

extern uint32_t sys_get_Second();

extern uint8_t user_operation_mode_set(USER_OPERATION_MODE mode_set);

extern uint8_t tips_info_set(tips_action_t action, tips_event_t tipsEvent, uint32_t tipsTmOut, tips_end_func func_cb, uint32_t userdata);

extern tips_show_info_t * tips_info_fetch();

extern void touch_event_init();

extern esp_user_info* touch_event_receive();

extern void touch_event_send(esp_user_info * event);

#endif /*LV_EXAMPLE_FUNC_H*/
