#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int gt911_pos_read(uint16_t *xpos, uint16_t *ypos);

int gt911_init(void);

#ifdef __cplusplus
}
#endif
