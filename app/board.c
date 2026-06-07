#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx.h"
#include "tim_delay.h"
#include "console.h"
#include "rtc.h"
#include "aht20.h"
#include "st7789.h"

extern void board_lowlevel_init(void);

void board_lowlevel_init(void)
{
    /* 从Boot Config读取活动分区，动态设置VTOR */
    /* Boot Config地址: 0x0800C000 */
    /* active_partition字段偏移: 8字节 (magic=4 + crc32=4) */
    volatile uint32_t boot_config_magic = *(volatile uint32_t *)0x0800C000;
    if (boot_config_magic == 0x424F4F54)  // "BOOT"
    {
        volatile uint8_t active_partition = *(volatile uint8_t *)0x0800C008;
        if (active_partition == 0x01)
        {
            SCB->VTOR = 0x08050100;  // APP1
        }
        else if (active_partition == 0x00)
        {
            SCB->VTOR = 0x08010100;  // APP0
        }
    }
    else
    {
        SCB->VTOR = 0x08010100;  // 默认APP0
    }

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RCC_LSEConfig(RCC_LSE_ON);
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
}

void board_init(void)
{
	console_init();
	
    printf("[SYS] Build Date: %s %s\n", __DATE__, __TIME__);
    
    rtc_init();
	
	tim_delay_init();
	
    st7789_init();
	
	if (!aht20_init())
	{
		printf("[ERROR] aht20 init failed!!!\r\n");
		while (1);
	}
}

int fputc(int ch, FILE *f)
{
    USART_ClearFlag(USART1, USART_FLAG_TC);
    USART_SendData(USART1, (uint8_t)ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    return ch;
}

void vAssertCalled(const char *file, int line)
{
    portDISABLE_INTERRUPTS();
    printf("Assert Called: %s(%d)\n", file, line);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    printf("Stack Overflowed: %s\n", pcTaskName);
    configASSERT(0);
}

void vApplicationMallocFailedHook(void)
{
    printf("Malloc Failed\n");
    configASSERT(0);
}
