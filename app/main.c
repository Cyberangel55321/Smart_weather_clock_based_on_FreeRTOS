#include "main.h"

static void main_init(void *param)
{
    board_init();
    ui_init();

    welcome_page_display();
	
	printf("current app version: V1.0\r\n");

    wifi_init();
    wifi_page_display();
    wifi_wait_connect();

    main_page_display();
    app_init();

    vTaskDelete(NULL);
}

int main(void)
{
    board_lowlevel_init();
    workqueue_init();

	xTaskCreate(main_init, "init", 2048, NULL, 9, NULL);
	
//	ui_init();

//    board_init();

//    welcome_page_display();
//	
//	printf("current app version: V1.0\r\n");

//    wifi_init();
//    wifi_page_display();
//    wifi_wait_connect();

//    main_page_display();
//    app_init();

    vTaskStartScheduler();

    while (1)
    {
        ; // code should not run here
    }
}



