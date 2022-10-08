
#include "bsp_board.h"
#include "bsp_btn.h"
#include "bsp_lcd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "lv_port.h"
#include "lvgl.h"
#include "gui/lv_example_func.h"

#include <esp_log.h>
#include "esp_log.h"
#include "app_wifi.h"
#include "app_sntp.h"


static const char *TAG = "app_main";

static TaskHandle_t g_lvgl_task_handle;


static void lvgl_task(void *pvParam)
{
    do {
        lv_task_handler();
    } while (vTaskDelay(1), true);
}

static void rmaker_task(void *args)
{
    /* Start the Wi-Fi. */
    app_wifi_enable();
    app_sntp_init();

    vTaskDelete(NULL);
}

static void wifi_credential_reset(void *arg)
{
    ESP_LOGI(TAG, "WiFi credential reset");
}

void app_rmaker_start(void)
{
    bsp_btn_register_callback(BOARD_BTN_ID_BOOT, BUTTON_LONG_PRESS_START, wifi_credential_reset, NULL);
    
    BaseType_t ret_val = xTaskCreatePinnedToCore(rmaker_task, "RMaker Task", 6 * 1024, NULL, 1, NULL, 0);
    ESP_ERROR_CHECK_WITHOUT_ABORT((pdPASS == ret_val) ? ESP_OK : ESP_FAIL);
}
/*******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：
**输出参数 ：
********************************************************************************/
void gui_main_start(void)
{
    /**
     * @brief Demos provided by LVGL.
     * 
     * @note Only enable one demo every time.
     * 
     */
    lv_example_fingerTouch();
    //lv_demo_widgets();      /* A widgets example. This is what you get out of the box */
    //lv_demo_music();        /* A modern, smartphone-like music player demo. */
    //lv_demo_stress();       /* A stress test for LVGL. */
    //lv_demo_benchmark();    /* A demo to measure the performance of LVGL or to compare different settings. */

    BaseType_t ret_val = xTaskCreatePinnedToCore(lvgl_task, "lvgl_Task", 6 * 1024, NULL, configMAX_PRIORITIES - 3, &g_lvgl_task_handle, 0);
    ESP_ERROR_CHECK((pdPASS == ret_val) ? ESP_OK : ESP_FAIL);
}
/*******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：
**输出参数 ：
********************************************************************************/
// EventGroupHandle_t sys_event_group;
void app_main(void)
{
    LV_LOG_USER("start up");
    // sys_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(bsp_board_init());
    ESP_ERROR_CHECK(lv_port_init());
    bsp_lcd_set_backlight(true);
    touch_event_init();
    gui_main_start();
    // app_rmaker_start();
    while(1){
        vTaskDelay(pdMS_TO_TICKS(10));
        // EventBits_t uxBits = xEventGroupWaitBits(
        //     fp_task_events,
        //     FP_CHECKED | FP_TOUCHON | FP_AGAIN,
        //     pdFALSE,
        //     pdFALSE,
        //     portMAX_DELAY
        // ); 
        // ESP_LOGI(TAG, "result %d ID %d\r\n", fp_user.result, fp_user.id);
        // if (uxBits & FP_CHECKED) {
        //     xEventGroupClearBits(fp_task_events, FP_CHECKED);
        // }
        // if (uxBits & FP_TOUCHON) {
        //     xEventGroupClearBits(fp_task_events, FP_TOUCHON);
        // } 
        // if (uxBits & FP_AGAIN) {
        //     xEventGroupClearBits(fp_task_events, FP_AGAIN);
        // } 
        // touch_event_send(&fp_user);
    }
}
