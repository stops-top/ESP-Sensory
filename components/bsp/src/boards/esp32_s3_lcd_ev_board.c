/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_log.h"
#include "bsp_board.h"
#include "bsp_led.h"
#include "bsp_i2s.h"
#include "bsp_i2c.h"
#include "bsp_codec.h"
#include "../codec/es7210.h"
#include "../codec/es8311.h"
#include "button.h"
#include "bsp_btn.h"
#include "tca9554.h"
#include "lcd_panel_config.h"
#include "bsp_lcd.h"
#include "../indev/indev.h"
#include "../indev/indev_tp.h"

static const char *TAG = "board lcd evb";

#define BIGENDIAN_DATA 0

#define BOARD_TCA9554_ADDR 0x20
#define EXPANDER_IO_PA_CTRL 0
#define EXPANDER_IO_SPI_CS 1
#define EXPANDER_IO_SPI_SCK 2
#define EXPANDER_IO_SPI_MOSI 3

static const board_button_t g_btns[] = {
    {BOARD_BTN_ID_BOOT, 0,      GPIO_NUM_0,    0},
};

static esp_err_t _codec_init(audio_hal_iface_samples_t sample_rate);
static esp_err_t _codec_set_clk(uint32_t rate, uint32_t bits_cfg, uint32_t ch);
static esp_err_t _codec_write(void *dest, size_t size, size_t *bytes_read, TickType_t ticks_to_wait);
static esp_err_t _codec_read(void *dest, size_t size, size_t *bytes_read, TickType_t ticks_to_wait);
static esp_err_t _codec_zero_dma_buffer(void);
static esp_err_t _codec_set_voice_volume(uint8_t volume);
static esp_err_t _codec_set_voice_gain(uint8_t channel_mask, uint8_t volume);
static const board_codec_ops_t g_board_lcd_evb_codec_ops = {
    .codec_init = _codec_init,
    .codec_set_clk = _codec_set_clk,
    .codec_write = _codec_write,
    .codec_read = _codec_read,
    .codec_zero_dma_buffer = _codec_zero_dma_buffer,
    .codec_set_dac_gain = _codec_set_voice_volume,
    .codec_set_adc_gain = _codec_set_voice_gain,
};

static const board_res_desc_t g_board_lcd_evb_res = {

    .FUNC_LCD_EN =     (1),
    .LCD_BUS_WIDTH =   (16),
    .LCD_IFACE_SPI =   (0),
    .LCD_DISP_IC_ST =  (1),
    .LCD_WIDTH =       (480),
    .LCD_HEIGHT =      (480),
    .LCD_FREQ =        (18 * 1000 * 1000),
    .LCD_CMD_BITS =    0,
    .LCD_PARAM_BITS =  0,
    .LCD_HOST =        (SPI2_HOST),

    .LCD_SWAP_XY =     (false),
    .LCD_MIRROR_X =    (false),
    .LCD_MIRROR_Y =    (false),
    .LCD_COLOR_INV =   (false),
    .LCD_COLOR_SPACE = ESP_LCD_COLOR_SPACE_RGB,

    .GPIO_LCD_BL =     (GPIO_NUM_NC),
    .GPIO_LCD_BL_ON =  (0),

    // RGB interface GPIOs for LCD panel
    .GPIO_LCD_DISP_EN = GPIO_NUM_NC,
    .GPIO_LCD_VSYNC   = GPIO_NUM_3,
    .GPIO_LCD_HSYNC   = GPIO_NUM_46,
    .GPIO_LCD_DE      = GPIO_NUM_17,
    .GPIO_LCD_PCLK    = GPIO_NUM_9,
#if BIGENDIAN_DATA
    .GPIO_LCD_DATA0   = GPIO_NUM_10, // B0
    .GPIO_LCD_DATA1   = GPIO_NUM_11, // B1
    .GPIO_LCD_DATA2   = GPIO_NUM_12, // B2
    .GPIO_LCD_DATA3   = GPIO_NUM_13, // B3
    .GPIO_LCD_DATA4   = GPIO_NUM_14, // B4
    .GPIO_LCD_DATA5   = GPIO_NUM_21, // G0
    .GPIO_LCD_DATA6   = GPIO_NUM_47, // G1
    .GPIO_LCD_DATA7   = GPIO_NUM_48, // G2
    .GPIO_LCD_DATA8   = GPIO_NUM_45, // G3
    .GPIO_LCD_DATA9   = GPIO_NUM_38, // G4
    .GPIO_LCD_DATA10  = GPIO_NUM_39, // G5
    .GPIO_LCD_DATA11  = GPIO_NUM_40,  // R0
    .GPIO_LCD_DATA12  = GPIO_NUM_41,  // R1
    .GPIO_LCD_DATA13  = GPIO_NUM_42, // R2
    .GPIO_LCD_DATA14  = GPIO_NUM_2, // R3
    .GPIO_LCD_DATA15  = GPIO_NUM_1, // R4
#else//LITTLE_ENDIAN_DATA
    .GPIO_LCD_DATA0   = GPIO_NUM_45, // B0
    .GPIO_LCD_DATA1   = GPIO_NUM_38, // B1
    .GPIO_LCD_DATA2   = GPIO_NUM_39, // B2
    .GPIO_LCD_DATA3   = GPIO_NUM_40, // B3
    .GPIO_LCD_DATA4   = GPIO_NUM_41, // B4
    .GPIO_LCD_DATA5   = GPIO_NUM_42, // G0
    .GPIO_LCD_DATA6   = GPIO_NUM_2,  // G1
    .GPIO_LCD_DATA7   = GPIO_NUM_1,  // G2
    .GPIO_LCD_DATA8   = GPIO_NUM_10, // G3
    .GPIO_LCD_DATA9   = GPIO_NUM_11, // G4
    .GPIO_LCD_DATA10  = GPIO_NUM_12, // G5
    .GPIO_LCD_DATA11  = GPIO_NUM_13,  // R0
    .GPIO_LCD_DATA12  = GPIO_NUM_14,  // R1
    .GPIO_LCD_DATA13  = GPIO_NUM_21, // R2
    .GPIO_LCD_DATA14  = GPIO_NUM_47, // R3
    .GPIO_LCD_DATA15  = GPIO_NUM_48, // R4
#endif
    .HSYNC_BACK_PORCH = 10,
    .HSYNC_FRONT_PORCH = 40,
    .HSYNC_PULSE_WIDTH = 8,
    .VSYNC_BACK_PORCH = 10,
    .VSYNC_FRONT_PORCH = 40,
    .VSYNC_PULSE_WIDTH = 8,
    .PCLK_ACTIVE_NEG = 0,

    .BSP_INDEV_IS_TP =         (1),
    .TOUCH_PANEL_SWAP_XY =     (0),
    .TOUCH_PANEL_INVERSE_X =   (0),
    .TOUCH_PANEL_INVERSE_Y =   (0),
    .TOUCH_PANEL_I2C_ADDR = 0,
    .TOUCH_WITH_HOME_BUTTON = 0,

    .BSP_BUTTON_EN =   (1),
    .BUTTON_TAB =  g_btns,
    .BUTTON_TAB_LEN = sizeof(g_btns) / sizeof(g_btns[0]),

    .FUNC_I2C_EN =     (1),
    .GPIO_I2C_SCL =    (GPIO_NUM_18),
    .GPIO_I2C_SDA =    (GPIO_NUM_8),

    .FUNC_SDMMC_EN =   (0),
    .SDMMC_BUS_WIDTH = (1),
    .GPIO_SDMMC_CLK =  (GPIO_NUM_NC),
    .GPIO_SDMMC_CMD =  (GPIO_NUM_NC),
    .GPIO_SDMMC_D0 =   (GPIO_NUM_NC),
    .GPIO_SDMMC_D1 =   (GPIO_NUM_NC),
    .GPIO_SDMMC_D2 =   (GPIO_NUM_NC),
    .GPIO_SDMMC_D3 =   (GPIO_NUM_NC),
    .GPIO_SDMMC_DET =  (GPIO_NUM_NC),

    .FUNC_SDSPI_EN =       (0),
    .SDSPI_HOST =          (SPI2_HOST),
    .GPIO_SDSPI_CS =       (GPIO_NUM_NC),
    .GPIO_SDSPI_SCLK =     (GPIO_NUM_NC),
    .GPIO_SDSPI_MISO =     (GPIO_NUM_NC),
    .GPIO_SDSPI_MOSI =     (GPIO_NUM_NC),

    .FUNC_SPI_EN =         (0),
    .GPIO_SPI_CS =         (GPIO_NUM_NC),
    .GPIO_SPI_MISO =       (GPIO_NUM_NC),
    .GPIO_SPI_MOSI =       (GPIO_NUM_NC),
    .GPIO_SPI_SCLK =       (GPIO_NUM_NC),

    .FUNC_RMT_EN =         (1),
    .GPIO_RMT_IR =         (GPIO_NUM_NC),
    .GPIO_RMT_LED =        (GPIO_NUM_4),
    .RMT_LED_NUM  =        1,

    .FUNC_I2S_EN =         (1),
    .GPIO_I2S_LRCK =       (GPIO_NUM_7),
    .GPIO_I2S_MCLK =       (GPIO_NUM_5),
    .GPIO_I2S_SCLK =       (GPIO_NUM_16),
    .GPIO_I2S_SDIN =       (GPIO_NUM_15),
    .GPIO_I2S_DOUT =       (GPIO_NUM_6),
    .CODEC_I2C_ADDR = 0,
    .AUDIO_ADC_I2C_ADDR = 0,

    .IMU_I2C_ADDR = 0,

    .FUNC_PWR_CTRL =       (0),
    .GPIO_PWR_CTRL =       (GPIO_NUM_NC),
    .GPIO_PWR_ON_LEVEL =   (1),

    .GPIO_MUTE_NUM =   GPIO_NUM_NC,
    .GPIO_MUTE_LEVEL = 1,

    .PMOD1 = NULL,
    .PMOD2 = NULL,

    .codec_ops = &g_board_lcd_evb_codec_ops,
};

esp_err_t bsp_board_lcd_evb_detect(void)
{
    esp_err_t ret = bsp_i2c_probe_addr(0x18); // probe es8311
    ret |= bsp_i2c_probe_addr(0x20); // probe tca9554
    ret |= bsp_i2c_probe_addr(0x41); // probe es7210

    return ESP_OK == ret ? ESP_OK : ESP_FAIL;
}

esp_err_t bsp_board_lcd_evb_init(void)
{
    ESP_ERROR_CHECK(tca9554_init(BOARD_TCA9554_ADDR));
    tca9554_set_direction(EXPANDER_IO_PA_CTRL, 1);
    bsp_btn_init_default();
    ESP_ERROR_CHECK(bsp_lcd_init());
    bsp_led_init();
    bsp_led_set_rgb(0, 255, 0, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    bsp_led_set_rgb(0, 0, 255, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_ERROR_CHECK(indev_init_default());
    ESP_ERROR_CHECK(bsp_i2s_init(I2S_NUM_0, 16000));
    ESP_ERROR_CHECK(bsp_codec_init(AUDIO_HAL_16K_SAMPLES));
    bsp_led_set_rgb(0, 0, 0, 255);
    char *name;
    uint8_t addr;
    indev_tp_get_dev(&name, &addr);
    if (strstr(name, "Focal")) {
        lcd_panel_gc9503np_init();
    } else if (strstr(name, "Goodix")) {
        lcd_panel_st7701s_init();
    }
    bsp_led_set_rgb(0, 0, 0, 0);
    return ESP_OK;
}

esp_err_t bsp_board_lcd_evb_power_ctrl(power_module_t module, bool on)
{
    /* Control independent power domain */
    switch (module) {
    case POWER_MODULE_LCD:
        break;
    case POWER_MODULE_AUDIO:
        tca9554_set_level(EXPANDER_IO_PA_CTRL, on);
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

const board_res_desc_t *bsp_board_lcd_evb_get_res_desc(void)
{
    return &g_board_lcd_evb_res;
}

static esp_err_t es7210_init_default(uint8_t i2c_addr)
{
    esp_err_t ret_val = ESP_OK;
    audio_hal_codec_config_t cfg = {
        .codec_mode = AUDIO_HAL_CODEC_MODE_ENCODE,
        .adc_input = AUDIO_HAL_ADC_INPUT_ALL,
        .i2s_iface = {
            .bits = AUDIO_HAL_BIT_LENGTH_16BITS,
            .fmt = AUDIO_HAL_I2S_NORMAL,
            .mode = AUDIO_HAL_MODE_SLAVE,
            .samples = AUDIO_HAL_16K_SAMPLES,
        },
    };

    ret_val |= es7210_adc_init(i2c_addr, &cfg);
    ret_val |= es7210_adc_config_i2s(cfg.codec_mode, &cfg.i2s_iface);
    ret_val |= es7210_adc_set_gain(ES7210_INPUT_MIC1 | ES7210_INPUT_MIC2, GAIN_37_5DB);
    ret_val |= es7210_adc_set_gain(ES7210_INPUT_MIC3 | ES7210_INPUT_MIC4, GAIN_0DB);
    ret_val |= es7210_adc_ctrl_state(cfg.codec_mode, AUDIO_HAL_CTRL_START);

    if (ESP_OK != ret_val) {
        ESP_LOGE(TAG, "Failed initialize es7210");
    }

    return ret_val;
}

static esp_err_t es8311_init_default(uint8_t i2c_addr)
{
    esp_err_t ret_val = ESP_OK;
    audio_hal_codec_config_t cfg = {
        .codec_mode = AUDIO_HAL_CODEC_MODE_DECODE,
        .dac_output = AUDIO_HAL_DAC_OUTPUT_LINE1,
        .i2s_iface = {
            .bits = AUDIO_HAL_BIT_LENGTH_16BITS,
            .fmt = AUDIO_HAL_I2S_NORMAL,
            .mode = AUDIO_HAL_MODE_SLAVE,
            .samples = AUDIO_HAL_16K_SAMPLES,
        },
    };

    ret_val |= es8311_codec_init(i2c_addr, &cfg);
    ret_val |= es8311_set_bits_per_sample(cfg.i2s_iface.bits);
    ret_val |= es8311_config_fmt(cfg.i2s_iface.fmt);
    ret_val |= es8311_codec_set_voice_volume(75);
    ret_val |= es8311_codec_ctrl_state(cfg.codec_mode, AUDIO_HAL_CTRL_START);
    if (ESP_OK != ret_val) {
        ESP_LOGE(TAG, "Failed initialize es8311");
    }

    return ret_val;
}

static esp_err_t _codec_init(audio_hal_iface_samples_t sample_rate)
{
    esp_err_t ret = es7210_init_default(0x41);
    ret |= es8311_init_default(0x18);
    return ESP_OK == ret ? ESP_OK : ESP_FAIL;
}

static esp_err_t _codec_set_clk(uint32_t rate, uint32_t bits_cfg, uint32_t ch)
{
    return i2s_set_clk(I2S_NUM_0, rate, bits_cfg, ch);
}

static esp_err_t _codec_write(void *dest, size_t size, size_t *bytes_read, TickType_t ticks_to_wait)
{
    return i2s_write(I2S_NUM_0, dest, size, bytes_read, ticks_to_wait);
}

static esp_err_t _codec_read(void *dest, size_t size, size_t *bytes_read, TickType_t ticks_to_wait)
{
    return i2s_read(I2S_NUM_0, dest, size, bytes_read, ticks_to_wait);
}

static esp_err_t _codec_zero_dma_buffer(void)
{
    return i2s_zero_dma_buffer(I2S_NUM_0);
}

static esp_err_t _codec_set_voice_volume(uint8_t volume)
{
    // reduce volume
    float v = volume;
    v *= 0.6f;
    v = -0.01f * (v * v) + 2.0f * v;
    return es8311_codec_set_voice_volume((int)v);

}

static esp_err_t _codec_set_voice_gain(uint8_t channel_mask, uint8_t volume)
{
    return ESP_ERR_NOT_SUPPORTED;
}
