/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bsp_lcd.h"
#include "bsp_board.h"
#include "esp_compiler.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "soc/soc_memory_layout.h"

static const char *TAG = "bsp_lcd";

static esp_lcd_panel_io_handle_t io_handle = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;

static void *p_user_data = NULL;
static bool (*p_on_trans_done_cb)(void *) = NULL;


static bool lcd_trans_done_cb(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *user_data, void *event_data)
{
    (void) panel_io;
    (void) user_data;
    (void) event_data;

    /* Call user registered function */
    if (NULL != p_on_trans_done_cb) {
        return p_on_trans_done_cb(p_user_data);
    }

    return false;
}

static esp_err_t screen_clear(uint16_t color)
{
    const board_res_desc_t *brd = bsp_board_get_description();
    int width = brd->LCD_WIDTH;
    int height = brd->LCD_HEIGHT;
    uint16_t *buffer = malloc(width * sizeof(uint16_t));
    ESP_RETURN_ON_FALSE(NULL != buffer, ESP_FAIL, TAG,  "no memory for screen clear");

    for (size_t i = 0; i < width; i++) {
        buffer[i] = color;
    }
    for (int y = 0; y < height; y++) {
        esp_lcd_panel_draw_bitmap(panel_handle, 0, y, width, y + 1, buffer);
    }

    free(buffer);
    return ESP_OK;
}

esp_err_t bsp_lcd_init(void)
{
    esp_err_t ret_val = ESP_OK;
    const board_res_desc_t *brd = bsp_board_get_description();
    if (brd->LCD_IFACE_SPI) {
        spi_bus_config_t buscfg = {
            .sclk_io_num = brd->GPIO_LCD_CLK,
            .mosi_io_num = brd->GPIO_LCD_DIN,
            .miso_io_num = GPIO_NUM_NC,
            .quadwp_io_num = GPIO_NUM_NC,
            .quadhd_io_num = GPIO_NUM_NC,
            .max_transfer_sz = brd->LCD_WIDTH * brd->LCD_HEIGHT * sizeof(uint16_t)
        };
        ret_val |= spi_bus_initialize(brd->LCD_HOST, &buscfg, SPI_DMA_CH_AUTO);

        esp_lcd_panel_io_spi_config_t io_config = {
            .dc_gpio_num = brd->GPIO_LCD_DC,
            .cs_gpio_num = brd->GPIO_LCD_CS,
            .pclk_hz = brd->LCD_FREQ,
            .spi_mode = 0,
            .trans_queue_depth = 10,
            .lcd_cmd_bits = brd->LCD_CMD_BITS,
            .lcd_param_bits = brd->LCD_PARAM_BITS,
            .on_color_trans_done = lcd_trans_done_cb,
            .user_ctx = NULL,
        };
        ret_val |= esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t) brd->LCD_HOST, &io_config, &io_handle);

        esp_lcd_panel_dev_config_t panel_config = {
            .reset_gpio_num = brd->GPIO_LCD_RST,
            .color_space = brd->LCD_COLOR_SPACE,
            .bits_per_pixel = 16,
        };

        if (!brd->LCD_DISP_IC_ST) {
            ESP_ERROR_CHECK(esp_lcd_new_panel_nt35510(io_handle, &panel_config, &panel_handle));
        } else  {
            ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
        }

        ret_val |= esp_lcd_panel_reset(panel_handle);
        ret_val |= esp_lcd_panel_init(panel_handle);
        ret_val |= esp_lcd_panel_invert_color(panel_handle, brd->LCD_COLOR_INV);
        ret_val |= esp_lcd_panel_set_gap(panel_handle, 0, 0);
        ret_val |= esp_lcd_panel_swap_xy(panel_handle, brd->LCD_SWAP_XY);
        ret_val |= esp_lcd_panel_mirror(panel_handle, brd->LCD_MIRROR_X, brd->LCD_MIRROR_Y);
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
        ret_val |= esp_lcd_panel_disp_on_off(panel_handle, true);
#else
        ret_val |= esp_lcd_panel_disp_off(panel_handle, false);
#endif
    }
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    else if (0) {
        esp_lcd_i80_bus_handle_t i80_bus = NULL;
        esp_lcd_i80_bus_config_t bus_config = {
            .clk_src = LCD_CLK_SRC_DEFAULT,
            .dc_gpio_num = brd->GPIO_LCD_DC,
            .wr_gpio_num = brd->GPIO_LCD_WR,
            .data_gpio_nums = {
                brd->GPIO_LCD_DATA0,
                brd->GPIO_LCD_DATA1,
                brd->GPIO_LCD_DATA2,
                brd->GPIO_LCD_DATA3,
                brd->GPIO_LCD_DATA4,
                brd->GPIO_LCD_DATA5,
                brd->GPIO_LCD_DATA6,
                brd->GPIO_LCD_DATA7,
                brd->GPIO_LCD_DATA8,
                brd->GPIO_LCD_DATA9,
                brd->GPIO_LCD_DATA10,
                brd->GPIO_LCD_DATA11,
                brd->GPIO_LCD_DATA12,
                brd->GPIO_LCD_DATA13,
                brd->GPIO_LCD_DATA14,
                brd->GPIO_LCD_DATA15,
            },
            .bus_width = brd->LCD_BUS_WIDTH,
            .max_transfer_bytes = brd->LCD_WIDTH * brd->LCD_HEIGHT * sizeof(uint16_t),
            .psram_trans_align = 64,
            .sram_trans_align = 4,
        };
        ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));
        esp_lcd_panel_io_handle_t io_handle = NULL;
        esp_lcd_panel_io_i80_config_t io_config = {
            .cs_gpio_num = brd->GPIO_LCD_CS,
            .pclk_hz = brd->LCD_FREQ,
            .trans_queue_depth = 10,
            .dc_levels = {
                .dc_idle_level = 0,
                .dc_cmd_level = 0,
                .dc_dummy_level = 0,
                .dc_data_level = 1,
            },
            // .on_color_trans_done = example_notify_lvgl_flush_ready,
            // .user_ctx = &disp_drv,
            .lcd_cmd_bits = 8,
            .lcd_param_bits = 8,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

        esp_lcd_panel_handle_t panel_handle = NULL;
        esp_lcd_panel_dev_config_t panel_config = {
            .reset_gpio_num = brd->GPIO_LCD_RST,
            .color_space = ESP_LCD_COLOR_SPACE_RGB,
            .bits_per_pixel = 16,
        };
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

        esp_lcd_panel_reset(panel_handle);
        esp_lcd_panel_init(panel_handle);
        // Set inversion, x/y coordinate order, x/y mirror according to your LCD module spec
        esp_lcd_panel_invert_color(panel_handle, true);
        esp_lcd_panel_swap_xy(panel_handle, true);
        esp_lcd_panel_mirror(panel_handle, true, false);

        // user can flush pre-defined pattern to the screen before we turn on the screen or backlight
        ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    } else {
        esp_lcd_rgb_panel_config_t panel_config = {
            .clk_src = LCD_CLK_SRC_PLL160M,
            .data_width = brd->LCD_BUS_WIDTH,
            .disp_gpio_num = brd->GPIO_LCD_DISP_EN,
            .pclk_gpio_num = brd->GPIO_LCD_PCLK,
            .vsync_gpio_num = brd->GPIO_LCD_VSYNC,
            .hsync_gpio_num = brd->GPIO_LCD_HSYNC,
            .de_gpio_num = brd->GPIO_LCD_DE,
            .data_gpio_nums = {
                brd->GPIO_LCD_DATA0,
                brd->GPIO_LCD_DATA1,
                brd->GPIO_LCD_DATA2,
                brd->GPIO_LCD_DATA3,
                brd->GPIO_LCD_DATA4,
                brd->GPIO_LCD_DATA5,
                brd->GPIO_LCD_DATA6,
                brd->GPIO_LCD_DATA7,
                brd->GPIO_LCD_DATA8,
                brd->GPIO_LCD_DATA9,
                brd->GPIO_LCD_DATA10,
                brd->GPIO_LCD_DATA11,
                brd->GPIO_LCD_DATA12,
                brd->GPIO_LCD_DATA13,
                brd->GPIO_LCD_DATA14,
                brd->GPIO_LCD_DATA15,
            },
            .timings = {
                .pclk_hz = brd->LCD_FREQ,
                .h_res = brd->LCD_WIDTH,
                .v_res = brd->LCD_HEIGHT,
                .hsync_back_porch = brd->HSYNC_BACK_PORCH,
                .hsync_front_porch = brd->HSYNC_FRONT_PORCH,
                .hsync_pulse_width = brd->HSYNC_PULSE_WIDTH,
                .vsync_back_porch = brd->VSYNC_BACK_PORCH,
                .vsync_front_porch = brd->VSYNC_FRONT_PORCH,
                .vsync_pulse_width = brd->VSYNC_PULSE_WIDTH,
                .flags.pclk_active_neg = brd->PCLK_ACTIVE_NEG,
            },
            .flags.fb_in_psram = 1,
        };
        esp_lcd_new_rgb_panel(&panel_config, &panel_handle);
        esp_lcd_panel_reset(panel_handle);
        esp_lcd_panel_init(panel_handle);
    }
#endif

    /**
     * @brief Configure LCD backlight IO.
     *
     */
    if (GPIO_NUM_NC != brd->GPIO_LCD_BL) {
        gpio_config_t bk_gpio_config = {
            .mode = GPIO_MODE_OUTPUT,
            /*!< Prevent left shift negtive value warning */
            .pin_bit_mask = brd->GPIO_LCD_BL > 0 ? 1ULL << brd->GPIO_LCD_BL : 0ULL,
        };
        gpio_config(&bk_gpio_config);
        gpio_set_level(brd->GPIO_LCD_BL, !brd->GPIO_LCD_BL_ON);
    }

    //screen_clear(0x00ff);
    screen_clear(0x00);
    return ret_val;
}

esp_err_t bsp_lcd_deinit(void)
{
    esp_err_t ret_val = ESP_OK;

    ret_val |= esp_lcd_panel_del(panel_handle);
    ret_val |= esp_lcd_panel_io_del(io_handle);

    const board_res_desc_t *brd = bsp_board_get_description();
    if (brd->LCD_IFACE_SPI) {
        ret_val |= spi_bus_free(brd->LCD_HOST);
    }

    return ret_val;
}

esp_err_t bsp_lcd_flush(int x1, int y1, int x2, int y2, const void *p_data)
{
    esp_lcd_panel_draw_bitmap(panel_handle, x1, y1, x2, y2, p_data);

    const board_res_desc_t *brd = bsp_board_get_description();
    if (!brd->LCD_IFACE_SPI) { // call trans_done_cb for RGB LCD panel
        lcd_trans_done_cb(NULL, NULL, NULL);
    }
    return ESP_OK;
}

esp_err_t bsp_lcd_set_cb(bool (*trans_done_cb)(void *), void *data)
{
    if (esp_ptr_executable(trans_done_cb)) {
        p_on_trans_done_cb = trans_done_cb;
        p_user_data = data;
    } else {
        ESP_LOGE(TAG, "Invalid function pointer");
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t bsp_lcd_set_backlight(bool en)
{
    const board_res_desc_t *brd = bsp_board_get_description();
    if (GPIO_NUM_NC == brd->GPIO_LCD_BL) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return gpio_set_level(brd->GPIO_LCD_BL, en ? brd->GPIO_LCD_BL_ON : !brd->GPIO_LCD_BL_ON);
}
