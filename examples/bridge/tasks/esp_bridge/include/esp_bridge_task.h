#pragma once
#include "esp_err.h"
#include "sdkconfig.h"
#include "freertos/queue.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"


#define TASK_BRIDGE_PRIO              (configMAX_PRIORITIES-9)
#define TASK_BRIDGE_SIZE              (1024*4)

typedef enum bridge_event_group_bits {
    NFC_BUS_RDY = BIT0,
    NFC_DEV_RDY = BIT1,
    NFC_ACD_IRQ = BIT2,
    NFC_CARD_IN = BIT3,
    NFC_CARD_EX = BIT4,
    NFC_PCD_RUN = BIT5,
    NFC_LE_MARK = BIT6
} bridge_event_group_bits_t;

extern EventGroupHandle_t bridge_event_group;


void esp_bridge_init(void);
void bridge_task_init(void);
void bridge_task_deinit(void);
