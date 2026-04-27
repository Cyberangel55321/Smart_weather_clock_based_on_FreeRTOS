#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "aht20.h"
#include "usart.h"
#include "cpu_delay.h"
#include "st7789.h"
#include "font.h"


int main(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	usart_init();
	st7789_init();
	if (!aht20_init())
	{
		printf("[ERROR] aht20 init failed!!!\r\n");
		while (1);
	}

	const uint16_t Pink = mkcolor(241, 158, 194);
	const uint16_t Green = mkcolor(0, 255, 0);
	const uint16_t Blue = mkcolor(0, 0, 255);
	const uint16_t White = mkcolor(255, 255, 255);
	const uint16_t Black = mkcolor(0, 0, 0);

	st7789_write_string(0, 0, "Hello!", Pink, Black, &font16);
    st7789_write_string(0, 16, "Hello!", Pink, Black, &font32);
    st7789_write_string(0, 48, "Hello!", Pink, Black, &font48);
    
    st7789_write_string(0, 200, "梅花嵌入式", Pink, Black, &font16);
    st7789_write_string(0, 216, "梅花嵌入式", Pink, Black, &font32);
    st7789_write_string(0, 248, "梅花嵌入式", Pink, Black, &font48);
	
	uint32_t failcount = 0;
    float temperature, humidity;
	while (1)
	{
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
	}

    while (1)
    {
        printf("system failed, please check log history\r\n");
        cpu_delay_ms(1000);
    }	
}
