#include "../../lv_examples.h"
#if LV_USE_FREETYPE && LV_BUILD_EXAMPLES
/**
 * Load a font with FreeType
 */

//#define BASE_PATH   "S:/spiffs"
//static const char *font_name = BASE_PATH "/KaiTi.ttf";

#define FONT_CNT    (1)
#define FONT_SIZE   (24)
// #define BASE_PATH   "S:/spiffs"
#define BASE_PATH   "/spiffs"

static lv_font_t font;
static lv_obj_t *label = NULL;
static const char *font_name = BASE_PATH "/KaiTi.ttf";
static uint8_t presstest = 0;

static void ctrlBtn_set_event_cb(lv_event_t * e)
{
    uint8_t i;
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * button = lv_event_get_target(e);
    uint8_t pressShow[40];

    if(code == LV_EVENT_SHORT_CLICKED) {
        presstest++;
        sprintf(pressShow, "Press num:%d", presstest);
        LV_LOG_USER("Start [%d],%s", presstest, pressShow);
        lv_label_set_text(label, pressShow);
        LV_LOG_USER("End [%d],%s", presstest, pressShow);
    }
}

void lv_example_freetype_1(void)
{
    bool bRet = false;
    uint8_t TFTname[40];
    #if 0
    lv_freetype_init(FONT_CNT);
    lv_freetype_font_init(&font, font_name, FONT_SIZE);
    #else
    /*Create a font*/
    static lv_ft_info_t info;
    /*FreeType uses C standard file system, so no driver letter is required.*/
    //info.name = "./lvgl/examples/libs/freetype/arial.ttf";
    info.name = font_name;
    info.weight = 30;
    info.style = FT_FONT_STYLE_NORMAL;
    bRet = lv_ft_font_init(&info);
    if(true == bRet){
        LV_LOG_USER("lv_ft_font_init ok:%s, LV_FREETYPE_CACHE_SIZE:%d", info.name, LV_FREETYPE_CACHE_SIZE);
    }
    else{
        LV_LOG_USER("lv_ft_font_init failed:%s, LV_FREETYPE_CACHE_SIZE:%d", info.name, LV_FREETYPE_CACHE_SIZE);
        return;
    }
    
    /*Create style with the new font*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, info.font);
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);
    #endif
    
    lv_obj_t * btn_del = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn_del, ctrlBtn_set_event_cb, LV_EVENT_SHORT_CLICKED, NULL);
    lv_obj_align(btn_del, LV_ALIGN_BOTTOM_MID, 0, 20);
    lv_obj_set_size(btn_del, 160, 70);

    lv_obj_t * label_del = lv_label_create(btn_del);
    lv_label_set_text(label_del, "Press");
    lv_obj_center(label_del);


    /*Create a label with the new style*/
    label = lv_label_create(lv_scr_act());
    LV_LOG_USER("info.name:%s", info.name);
        
    if (true == bRet) {
        LV_LOG_USER("lv_obj_set_style_text_font start");
        //lv_obj_set_style_text_font(label, &style, 0);
        lv_obj_set_style_text_font(label, info.font, 0);
        LV_LOG_USER("lv_obj_set_style_text_font end");
    }

    sprintf(TFTname, "Font Test:%s", info.name);
    lv_label_set_text(label, TFTname);
    lv_obj_center(label);
    LV_LOG_USER("create end");
}

#else


static lv_obj_t *label = NULL;
static uint8_t presstest;

static void ctrlBtn_set_event_cb(lv_event_t * e)
{
    uint8_t i;
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * button = lv_event_get_target(e);
    uint8_t pressShow[40];

    if(code == LV_EVENT_SHORT_CLICKED) {
        presstest++;
        sprintf(pressShow, "Press %d\nI'm a font created with FreeType", presstest);
        LV_LOG_USER("[%d],%s", presstest, pressShow);
        lv_label_set_text(label, pressShow);
    }
}

void lv_example_freetype_1(void)
{
    /*TODO
     *fallback for online examples*/

    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "FreeType is not installed");
    lv_obj_center(label);

    lv_obj_t * btn_del = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn_del, ctrlBtn_set_event_cb, LV_EVENT_SHORT_CLICKED, NULL);
    lv_obj_align(btn_del, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_size(btn_del, 200, 70);
}

#endif
