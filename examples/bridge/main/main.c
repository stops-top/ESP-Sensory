#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_bridge_task.h"

static const char *TAG = "app_main";


void app_main(void)
{
    esp_bridge_init();
}
