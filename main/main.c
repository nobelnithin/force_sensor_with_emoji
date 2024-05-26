#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"

#include "ssd1306.h"
#include "font8x8_basic.h"
#include "images.h"
/*
 You have to set this config value with menuconfig
 CONFIG_INTERFACE

 for i2c
 CONFIG_MODEL
 CONFIG_SDA_GPIO
 CONFIG_SCL_GPIO
 CONFIG_RESET_GPIO

 for SPI
 CONFIG_CS_GPIO
 CONFIG_DC_GPIO
 CONFIG_RESET_GPIO
*/

#define TAG "SSD1306"
#define FORCE_SENSOR_PIN ADC1_CHANNEL_7

// https://www.mischianti.org/2021/07/14/ssd1306-oled-display-draw-images-splash-and-animations-2/

// https://www.mischianti.org/2021/07/14/ssd1306-oled-display-draw-images-splash-and-animations-2/

void app_main(void)
{
	SSD1306_t dev;

#if CONFIG_I2C_INTERFACE
	ESP_LOGI(TAG, "INTERFACE is i2c");
	ESP_LOGI(TAG, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	ESP_LOGI(TAG, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	ESP_LOGI(TAG, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_I2C_INTERFACE



#if CONFIG_FLIP
	dev._flip = true;
	ESP_LOGW(TAG, "Flip upside down");
#endif

#if CONFIG_SSD1306_128x64
	ESP_LOGI(TAG, "Panel is 128x64");
	ssd1306_init(&dev, 128, 64);
#endif // CONFIG_SSD1306_128x64
#if CONFIG_SSD1306_128x32
	ESP_LOGI(TAG, "Panel is 128x32");
	ssd1306_init(&dev, 128, 32);
#endif // CONFIG_SSD1306_128x32

	ssd1306_contrast(&dev, 0xff);
	ssd1306_clear_screen(&dev, false);
    ssd1306_bitmaps(&dev, 0, 0, ElectroHub, 128, 64, false);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    ssd1306_bitmaps(&dev, 0, 0, no_press, 128, 64, false);
    vTaskDelay(250/portTICK_PERIOD_MS);
    ssd1306_bitmaps(&dev, 0, 0, light_press, 128, 64, false);
    vTaskDelay(250/portTICK_PERIOD_MS);
    ssd1306_bitmaps(&dev, 0, 0, light_squeez, 128, 64, false);
    vTaskDelay(250/portTICK_PERIOD_MS);
    ssd1306_bitmaps(&dev, 0, 0, medium, 128, 64, false);
    vTaskDelay(250/portTICK_PERIOD_MS);
    ssd1306_bitmaps(&dev, 0, 0, final, 128, 64, false);
    vTaskDelay(250/portTICK_PERIOD_MS);
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(FORCE_SENSOR_PIN, ADC_ATTEN_DB_0);
	while(1) {
        int analogReading = adc1_get_raw(FORCE_SENSOR_PIN);
        printf("The force sensor value = %d", analogReading);

        // Determine the pressure level based on the reading
        if (analogReading < 10)
        {
            printf(" -> no pressure\n");
            ssd1306_bitmaps(&dev, 0, 0, no_press, 128, 64, false);
        }

        else if (analogReading < 200)
        {
            printf(" -> light touch\n");
            ssd1306_bitmaps(&dev, 0, 0, light_press, 128, 64, false);
        }
        else if (analogReading < 500)
        {
            printf(" -> light squeeze\n");
            ssd1306_bitmaps(&dev, 0, 0, light_squeez, 128, 64, false);
        }
            
        else if (analogReading < 800)
        {
            printf(" -> medium squeeze\n");
            ssd1306_bitmaps(&dev, 0, 0, medium, 128, 64, false);
        }
            
        else
        {
            printf(" -> big squeeze\n");
            ssd1306_bitmaps(&dev, 0, 0, final, 128, 64, false);
        }
            

        // Delay for 1000 ms
        vTaskDelay(pdMS_TO_TICKS(500));
	}
}
