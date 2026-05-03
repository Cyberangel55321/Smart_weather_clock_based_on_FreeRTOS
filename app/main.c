#include "main.h"

uint32_t failcount = 0;
float temperature, humidity;

int main(void)
{
    board_lowlevel_init();
    board_init();

    welcome_page_display();

    wifi_init();
    wifi_page_display();
    wifi_wait_connect();
    
    main_loop_init();
    main_page_display();
    
    while (1)
    {
        main_loop();
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



