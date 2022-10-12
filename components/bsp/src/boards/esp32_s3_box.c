/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "bsp_board.h"
#include "bsp_i2s.h"
#include "bsp_led.h"
#include "bsp_i2c.h"
#include "bsp_codec.h"
#include "../codec/es7210.h"
#include "../codec/es8311.h"
#include "button.h"
#include "bsp_btn.h"
#include "bsp_lcd.h"
#include "../indev/indev.h"

static const char *TAG = "board s3 box";

static const board_button_t g_btns[] = {
    {BOARD_BTN_ID_BOOT, 0,      GPIO_NUM_0,    0},
};

/* 
| MOD1 | 38 BOOT1 | 39 RST1 | 40 RXD1 | 41 TXD1 | 
| MOD1 | 42 TCK | 21 TMS | 19 | 20 |
| MOD2 | 9 TDI  | 43 | 44 | 14 D0 | 
| MOD2 | 10 TDO | 11 CMD | 13 CLK | 12 | 
*/
static const pmod_pins_t g_pmod[2] = {
    {
        {9, 43, 44, 14},
        {10, 11, 13, 12},
    },
    {
        {38, 39, 40, 41},
        {42, 21, 19, 20},
    },
};

static esp_err_t _codec_init(audio_hal_iface_samples_t sample_rate);
static esp_err_t _codec_set_clk(uint32_t rate, uint32_t bits_cfg, uint32_t ch);
static esp_err_t _codec_write(void *dest, size_t size, size_t *bytes_read, TickType_t ticks_to_wait);
static esp_err_t _codec_read(void *dest, size_t size, size_t *bytes_read, TickType_t ticks_to_wait);
static esp_err_t _codec_zero_dma_buffer(void);
static esp_err_t _codec_set_voice_volume(uint8_t volume);
static esp_err_t _codec_set_voice_gain(uint8_t channel_mask, uint8_t volume);
static const board_codec_ops_t g_board_s3_box_codec_ops = {
    .codec_init = _codec_init,
    .codec_set_clk = _codec_set_clk,
    .codec_write = _codec_write,
    .codec_read = _codec_read,
    .codec_zero_dma_buffer = _codec_zero_dma_buffer,
    .codec_set_dac_gain = _codec_set_voice_volume,
    .codec_set_adc_gain = _codec_set_voice_gain,
};

static const board_res_desc_t g_board_s3_box_res = {
    .FUNC_LCD_EN =     (1),
    .LCD_BUS_WIDTH =   (1),
    .LCD_IFACE_SPI =   (1),
    .LCD_DISP_IC_ST =  (1),
    .LCD_WIDTH =       (320),
    .LCD_HEIGHT =      (240),
    .LCD_FREQ =        (40 * 1000 * 1000),
    .LCD_CMD_BITS =    8,
    .LCD_PARAM_BITS =  8,
    .LCD_HOST =        (SPI2_HOST),

    .LCD_SWAP_XY =     (0),
    .LCD_MIRROR_X =    (true),
    .LCD_MIRROR_Y =    (true),
    .LCD_COLOR_INV =   (false),
    .LCD_COLOR_SPACE = ESP_LCD_COLOR_SPACE_BGR,

    .GPIO_LCD_BL =     (GPIO_NUM_45),
    .GPIO_LCD_BL_ON =  (1),
    .GPIO_LCD_CS =     (GPIO_NUM_5),
    .GPIO_LCD_RST =    (GPIO_NUM_48),
    .GPIO_LCD_DC =     (GPIO_NUM_4),
    .GPIO_LCD_WR =     (GPIO_NUM_NC),
    .GPIO_LCD_CLK =    (GPIO_NUM_7),
    .GPIO_LCD_DIN =    (GPIO_NUM_6),
    .GPIO_LCD_DOUT =   (GPIO_NUM_NC),

    .BSP_INDEV_IS_TP =    (1),
    .TOUCH_PANEL_SWAP_XY =     (0),
    .TOUCH_PANEL_INVERSE_X =   (1),
    .TOUCH_PANEL_INVERSE_Y =   (0),
    .TOUCH_PANEL_I2C_ADDR = 0,
    .TOUCH_WITH_HOME_BUTTON = 1,

    .BSP_BUTTON_EN =   (1),
    .BUTTON_ADC_CHAN =  ADC1_CHANNEL_0,
    .BUTTON_TAB =  g_btns,
    .BUTTON_TAB_LEN = sizeof(g_btns) / sizeof(g_btns[0]),

    .FUNC_I2C_EN =     (1),
    .GPIO_I2C_SCL =    (GPIO_NUM_18),
    .GPIO_I2C_SDA =    (GPIO_NUM_8),

    .FUNC_SDMMC_EN =   (1),
    .SDMMC_BUS_WIDTH = (1),
    .GPIO_SDMMC_CLK =  (GPIO_NUM_13),
    .GPIO_SDMMC_CMD =  (GPIO_NUM_11),
    .GPIO_SDMMC_D0 =   (GPIO_NUM_14),
    .GPIO_SDMMC_D1 =   (GPIO_NUM_12),
    .GPIO_SDMMC_D2 =   (GPIO_NUM_10),
    .GPIO_SDMMC_D3 =   (GPIO_NUM_9),
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

    .FUNC_RMT_EN =         (0),
    .GPIO_RMT_IR =         (GPIO_NUM_NC),
    .GPIO_RMT_LED =        (GPIO_NUM_39),

    .FUNC_I2S_EN =         (1),
    .GPIO_I2S_LRCK =       (GPIO_NUM_47),
    .GPIO_I2S_MCLK =       (GPIO_NUM_2),
    .GPIO_I2S_SCLK =       (GPIO_NUM_17),
    .GPIO_I2S_SDIN =       (GPIO_NUM_16),
    .GPIO_I2S_DOUT =       (GPIO_NUM_15),
    .CODEC_I2C_ADDR = 0,
    .AUDIO_ADC_I2C_ADDR = 0,

    .IMU_I2C_ADDR = 0,

    .FUNC_PWR_CTRL =       (1),
    .GPIO_PWR_CTRL =       (GPIO_NUM_46),
    .GPIO_PWR_ON_LEVEL =   (1),

    .GPIO_MUTE_NUM =   GPIO_NUM_1,
    .GPIO_MUTE_LEVEL = 1,

    .PMOD1 = &g_pmod[0],
    .PMOD2 = &g_pmod[1],

    .codec_ops = &g_board_s3_box_codec_ops,
};

__attribute__((weak)) void mute_btn_handler(void *arg)
{
    if (g_board_s3_box_res.GPIO_MUTE_LEVEL == gpio_get_level(g_board_s3_box_res.GPIO_MUTE_NUM)) {
        esp_rom_printf(DRAM_STR("Mute Off"));
    } else {
        esp_rom_printf(DRAM_STR("Mute On"));
    }
}

esp_err_t bsp_board_s3_box_detect(void)
{
    esp_err_t ret = bsp_i2c_probe_addr(0x18); // probe es8311
    ret |= bsp_i2c_probe_addr(0x40); // probe es7210

    return ESP_OK == ret ? ESP_OK : ESP_FAIL;
}

esp_err_t bsp_board_s3_box_init(void)
{
    /*!< Mute_Button on ESP32-S3-Box */
    gpio_config_t io_conf_key;
    io_conf_key.intr_type = GPIO_INTR_ANYEDGE;
    io_conf_key.mode = GPIO_MODE_INPUT;
    io_conf_key.pin_bit_mask = 1ULL << g_board_s3_box_res.GPIO_MUTE_NUM;
    io_conf_key.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf_key.pull_up_en = GPIO_PULLUP_ENABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf_key));

    /* Install GPIO ISR service to enable GPIO ISR callback */
    gpio_install_isr_service(0);
    ESP_ERROR_CHECK(gpio_isr_handler_add(g_board_s3_box_res.GPIO_MUTE_NUM, mute_btn_handler, NULL));
    // bsp_sdcard_init_default();
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
    vTaskDelay(pdMS_TO_TICKS(100));
    bsp_led_set_rgb(0, 0, 0, 0);
    return ESP_OK;
}

esp_err_t bsp_board_s3_box_power_ctrl(power_module_t module, bool on)
{
    /* Config power control IO */
    static esp_err_t bsp_io_config_state = ESP_FAIL;
    if (ESP_OK != bsp_io_config_state) {
        gpio_config_t io_conf;
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_OUTPUT;
        io_conf.pin_bit_mask = 1ULL << g_board_s3_box_res.GPIO_PWR_CTRL;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        bsp_io_config_state = gpio_config(&io_conf);
    }

    /* Checko IO config result */
    if (ESP_OK != bsp_io_config_state) {
        ESP_LOGE(TAG, "Failed initialize power control IO");
        return bsp_io_config_state;
    }

    /* Control independent power domain */
    switch (module) {
    case POWER_MODULE_LCD:
        gpio_set_level(g_board_s3_box_res.GPIO_LCD_BL, on ? (g_board_s3_box_res.GPIO_LCD_BL_ON) : (!g_board_s3_box_res.GPIO_LCD_BL_ON));
        break;
    case POWER_MODULE_AUDIO:
    case POWER_MODULE_ALL:
        gpio_set_level(g_board_s3_box_res.GPIO_PWR_CTRL, on ? (g_board_s3_box_res.GPIO_PWR_ON_LEVEL) : (!g_board_s3_box_res.GPIO_PWR_ON_LEVEL));
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

const board_res_desc_t *bsp_board_s3_box_get_res_desc(void)
{
    return &g_board_s3_box_res;
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
    esp_err_t ret = es7210_init_default(0x40);
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
