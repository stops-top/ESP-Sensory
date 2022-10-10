#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "esp_err.h"


esp_err_t esp_bridge_port_init(void);
esp_err_t esp_bridge_port_deinit(void);

#ifdef __cplusplus
}
#endif
