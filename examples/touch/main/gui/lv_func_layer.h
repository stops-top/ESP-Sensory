/**
 * @file lv_func_layer.h
 *
 */

#ifndef LV_FUNC_LAYER_H
#define LV_FUNC_LAYER_H

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

typedef bool (*lv_layer_enter_cb)(struct lv_layer_t * layer);
typedef bool (*lv_layer_exit_cb)(struct lv_layer_t * layer);

typedef struct lv_layer_t{
    uint8_t * lv_obj_name;
    uint8_t * lv_obj_user;
    lv_obj_t * lv_obj_layer;
    struct lv_layer_t *lv_show_layer;
    lv_layer_enter_cb enter_cb;
    lv_layer_exit_cb exit_cb;
    lv_timer_cb_t timer_cb;
    lv_timer_t * timer_handle;
}lv_layer_t;

extern void lv_goto_layer(lv_layer_t * src_layer, lv_layer_t * dst_layer);

#endif /*LV_FUNC_LAYER_H*/