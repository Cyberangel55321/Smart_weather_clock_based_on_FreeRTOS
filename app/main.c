#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "aht20.h"
#include "usart.h"
#include "cpu_tick.h"
#include "st7789.h"
#include "font.h"
#include "image.h"
#include "esp_at.h"
#include "weather.h"

uint32_t failcount = 0;
float temperature, humidity;
static const char *weather_url = "https://api.seniverse.com/v3/weather/now.json?key=SOi5-h3NaxmEoye66&location=huainan&language=en&unit=c";

int main(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    /******************************  system init ******************************/
    cpu_tick_init();
	usart_init();
	st7789_init();
	if (!aht20_init())
	{
		printf("[ERROR] aht20 init failed!!!\r\n");
		while (1);
	}
    printf("[SYS] startup\n");
    /******************************  system init ******************************/


    /********************************  AT init ********************************/
    if (!esp_at_init())
    {
        printf("[AT] init failed\n");
        goto err;
    }
    printf("[AT] inited\n");
    
    if (!esp_at_wifi_init())
    {
        printf("[WIFI] init failed\n");
        goto err;
    }
    printf("[WIFI] inited\n");
    
    if (!esp_at_connect_wifi("iPhone14", "19159925209", NULL))
    {
        printf("[WIFI] connect failed\n");
        goto err;
    }
    printf("[WIFI] connecting\n");
    
    if (!esp_at_sntp_init())
    {
        printf("[SNTP] init failed\n");
        goto err;
    }
    printf("[SNTP] inited\n");
    /********************************  AT init ********************************/

    st7789_draw_image(0, 0, &image_touxiang);

	while (1)
	{
    /********************************* AHT20 *********************************/
        if (failcount >= 10)
        {
            printf("[ERROR] aht20 failed %u times! program stop!!!\r\n", failcount);
            break;
        }
        if (!aht20_start_measurement())
        {
            printf("[ERROR] aht20 start measurement failed!!!\r\n");
            failcount++;
            continue;
        }
        if (!aht20_wait_for_measurement())
        {
            printf("[ERROR] aht20 measurement timeout!!!\r\n");
            failcount++;
            continue;
        }
        if (!aht20_read_measurement(&temperature, &humidity))
        {
            printf("[ERROR] aht20 read measurement failed!!!\r\n");
            failcount++;
            continue;
        }
        
        failcount = 0;
        printf("temperature: %.2f, humidity:%.2f\r\n", temperature, humidity);
        cpu_delay_ms(1000);
    /********************************* AHT20 *********************************/


    /*********************************** AT **********************************/
        esp_wifi_info_t wifi = { 0 };

        if (!esp_at_get_wifi_info(&wifi))
        {
            printf("[AT] wifi info get failed\n");
            continue;
        }
        
        if (!wifi.connected)
        {
            printf("[WIFI] disconnected\n");
            continue;
        }
        
        printf("[WIFI] SSID: %s, BSSID: %s, Channel: %d, RSSI: %d\n",
                wifi.ssid, wifi.bssid, wifi.channel, wifi.rssi);
        
        esp_date_time_t date = { 0 };
        if (!esp_at_sntp_get_time(&date))
        {
            printf("[SNTP] get time failed\n");
            continue;
        }
        
        if (date.year > 2000)
        {
            printf("[SNTP] %04u-%02u-%02u %02u:%02u:%02u (%s)\n",
                date.year, date.month, date.day, date.hour, date.minute, date.second,
                date.weekday == 1 ? "Monday":
                date.weekday == 2 ? "Tuesday":
                date.weekday == 3 ? "Wednesday":
                date.weekday == 4 ? "Thursday":
                date.weekday == 5 ? "Friday":
                date.weekday == 6 ? "Saturday":
                date.weekday == 7 ? "Sunday" : "Unknown");
        }
        
        weather_info_t weather = { 0 };
        const char *weather_http_response = esp_at_http_get(weather_url);
        if (weather_http_response == NULL)
        {
            printf("[WEAHTER] http error\n");
            continue;
        }
        
        if (!parse_seniverse_response(weather_http_response, &weather))
        {
            printf("[WEAHTER] parse failed\n");
            continue;
        }
        
        printf("[WEATHER] %s, %s, %.1f\n", weather.city, weather.weather, weather.temperature);
        printf("\r\n");
    /*********************************** AT **********************************/
	}

err:
    while (1)
    {
        printf("system failed, please check log history\r\n");
        printf("AT Error\r\n");
        cpu_delay_ms(1000);
    }
}









// const uint16_t Pink = mkcolor(241, 158, 194);
// const uint16_t Green = mkcolor(0, 255, 0);
// const uint16_t Blue = mkcolor(0, 0, 255);
// const uint16_t White = mkcolor(255, 255, 255);
// const uint16_t Black = mkcolor(0, 0, 0);

// st7789_write_string(0, 0, "Hello!", Pink, Black, &font16);
// st7789_write_string(0, 16, "Hello!", Pink, Black, &font32);
// st7789_write_string(0, 48, "Hello!", Pink, Black, &font48);

// st7789_write_string(0, 200, "学习嵌入式", Pink, Black, &font16);
// st7789_write_string(0, 216, "学习嵌入式", Pink, Black, &font32);
// st7789_write_string(0, 248, "学习嵌入式", Pink, Black, &font48);



