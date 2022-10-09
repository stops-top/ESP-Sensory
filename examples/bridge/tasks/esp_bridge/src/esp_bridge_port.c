#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_partition.h"
#include "driver/rtc_io.h"
#include "esp_sleep.h"

#include "bridge_config.h"

#include "esp_bridge_task.h"
#include "esp_bridge_port.h"

static const char *TAG = "bridge port";



/*******************************************************************************
**函数信息 ：
**功能描述 ：
**输入参数 ：
**输出参数 ：
********************************************************************************/
