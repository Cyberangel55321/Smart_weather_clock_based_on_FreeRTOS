#ifndef __MAIN_H__
#define __MAIN_H__

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

#define APP_VERSION "v1.0"
#define WIFI_SSID   "iPhone14"
#define WIFI_PASSWD "19159925209"

void board_lowlevel_init(void);
void board_init(void);
void welcome_page_display(void);

void wifi_init(void);
void wifi_wait_connect(void);
void wifi_page_display(void);

void main_loop_init(void);
void main_loop(void);
void main_page_display(void);

#endif /* __MAIN_H__ */
