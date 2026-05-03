#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "stm32f4xx.h"
#include "cpu_tick.h"
#include "st7789.h"
#include "font.h"
#include "image.h"

// CLK ！！ PB13
// MOSI ！！ PC3
// MISO ！！ PC2
// CS ！！ PE2
// RESET ！！ PE3
// DC ！！ PE4
// BL ！！ PE5

#define CS_PORT     GPIOE
#define CS_PIN      GPIO_Pin_2
#define RESET_PORT  GPIOE
#define RESET_PIN   GPIO_Pin_3
#define DC_PORT     GPIOE
#define DC_PIN      GPIO_Pin_4
#define BL_PORT     GPIOE
#define BL_PIN      GPIO_Pin_5

static void st7789_init_display(void);
static void st7789_write_register(uint8_t reg, uint8_t data[], uint16_t length);
static void st7789_reset(void);
static void st7789_set_backlight(bool on);
static bool in_screen_range(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
static void st7789_set_range_and_prepare_gram(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
static void st7789_draw_font(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *model, uint16_t color, uint16_t bg_color);
static void st7789_write_ascii(uint16_t x, uint16_t y, char ch, uint16_t color, uint16_t bg_color, const font_t *font);
static void st7789_write_chinese(uint16_t x, uint16_t y, char *ch, uint16_t color, uint16_t bg_color, const font_t *font);
static bool is_gb2312(char ch);

void st7789_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    
    GPIO_ResetBits(BL_PORT, BL_PIN);
    GPIO_SetBits(GPIOE, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_SPI2);
    
    SPI_InitTypeDef SPI_InitStruct;
    SPI_StructInit(&SPI_InitStruct);
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI2, &SPI_InitStruct);
    SPI_Cmd(SPI2, ENABLE);
    
    st7789_init_display();
}

static void st7789_write_register(uint8_t reg, uint8_t data[], uint16_t length)
{
    GPIO_ResetBits(CS_PORT, CS_PIN);
    
    GPIO_ResetBits(DC_PORT, DC_PIN);
    SPI_SendData(SPI2, reg);
    while (SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET);
    while (SPI_GetFlagStatus(SPI2, SPI_FLAG_BSY) != RESET);
    
    GPIO_SetBits(DC_PORT, DC_PIN);
    for (uint16_t i = 0; i < length; i++)
    {
        SPI_SendData(SPI2, data[i]);
        while (!SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE));
    }
    while (SPI_GetFlagStatus(SPI2, SPI_FLAG_BSY) != RESET);
    
    GPIO_SetBits(CS_PORT, CS_PIN);
}

static void st7789_reset(void)
{
    GPIO_ResetBits(RESET_PORT, RESET_PIN);
    cpu_delay_us(20);
    GPIO_SetBits(RESET_PORT, RESET_PIN);
    cpu_delay_ms(120);
}

static void st7789_set_backlight(bool on)
{
    GPIO_WriteBit(BL_PORT, BL_PIN, on ? Bit_SET : Bit_RESET);
}


static void st7789_init_display(void)
{
    st7789_reset();

    st7789_write_register(0x01, NULL, 0); // Software reset
    cpu_delay_ms(120);

    st7789_write_register(0x11, NULL, 0); // Sleep out
    cpu_delay_ms(120);

    st7789_write_register(0x3A, (uint8_t[]){0x05}, 1); // RGB565
    st7789_write_register(0x36, (uint8_t[]){0x00}, 1); 
    st7789_write_register(0xC5, (uint8_t[]){0x1A}, 1);
    st7789_write_register(0xB2, (uint8_t[]){0x05,0x05,0x00,0x33,0x33}, 5);
    st7789_write_register(0xB7, (uint8_t[]){0x05}, 1);
    st7789_write_register(0xBB, (uint8_t[]){0x3F}, 1);
    st7789_write_register(0xC0, (uint8_t[]){0x2C}, 1);
    st7789_write_register(0xC2, (uint8_t[]){0x01}, 1);
    st7789_write_register(0xC3, (uint8_t[]){0x0F}, 1);
    st7789_write_register(0xC4, (uint8_t[]){0x20}, 1);
    st7789_write_register(0xC6, (uint8_t[]){0x01}, 1);
    st7789_write_register(0xD0, (uint8_t[]){0xA4,0xA1}, 2);
    st7789_write_register(0xE8, (uint8_t[]){0x03}, 1);
    st7789_write_register(0xE9, (uint8_t[]){0x09,0x09,0x08}, 3);
    st7789_write_register(0xE0, (uint8_t[]){
        0xD0,0x05,0x09,0x09,0x08,0x14,0x28,
        0x33,0x3F,0x07,0x13,0x14,0x28,0x30
    }, 14);
    st7789_write_register(0xE1, (uint8_t[]){
        0xD0,0x05,0x09,0x09,0x08,0x03,0x24,
        0x32,0x32,0x3B,0x14,0x13,0x28,0x2F
    }, 14);
    st7789_write_register(0x20, NULL, 0);
    cpu_delay_ms(120);
    st7789_write_register(0x29, NULL, 0);
    st7789_fill_color(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1, 0x0000);
    st7789_set_backlight(true);
}

static bool in_screen_range(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    if (x1 >= ST7789_WIDTH || y1 >= ST7789_HEIGHT)
        return false;
    if (x2 >= ST7789_WIDTH || y2 >= ST7789_HEIGHT)
        return false;
    if (x1 > x2 || y1 > y2)
        return false;

    return true;
}

static void st7789_set_range_and_prepare_gram(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    st7789_write_register(0x2A, (uint8_t[]){(x1 >> 8) & 0xff, x1 & 0xff, (x2 >> 8) & 0xff, x2 & 0xff}, 4);
    st7789_write_register(0x2B, (uint8_t[]){(y1 >> 8) & 0xff, y1 & 0xff, (y2 >> 8) & 0xff, y2 & 0xff}, 4);
    st7789_write_register(0x2C, NULL, 0);
}

void st7789_fill_color(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    if (!in_screen_range(x1, y1, x2, y2))
        return;
    
    st7789_set_range_and_prepare_gram(x1, y1, x2, y2);
    
    uint8_t color_data[2] = {(color >> 8) & 0xff, color & 0xff};
    uint32_t pixels = (x2 - x1 + 1) * (y2 - y1 + 1);

    GPIO_ResetBits(CS_PORT, CS_PIN);
    GPIO_SetBits(DC_PORT, DC_PIN);
    for (uint32_t i = 0; i < pixels; i++)
    {
        SPI_SendData(SPI2, color_data[0]);
        while (SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET);
        SPI_SendData(SPI2, color_data[1]);
        while (SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET);
    }
    while (SPI_GetFlagStatus(SPI2, SPI_FLAG_BSY) != RESET);
    GPIO_SetBits(CS_PORT, CS_PIN);
}


static void st7789_draw_font(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *model, uint16_t color, uint16_t bg_color)
{
    uint16_t bytes_per_row = (width + 7) / 8;
    uint8_t color_data[2] = {(color >> 8) & 0xff, color & 0xff};
    uint8_t bg_color_data[2] = {(bg_color >> 8) & 0xff, bg_color & 0xff};
    
    st7789_set_range_and_prepare_gram(x, y, x + width - 1, y + height - 1);
    
    GPIO_ResetBits(CS_PORT, CS_PIN);
    GPIO_SetBits(DC_PORT, DC_PIN);
	for (uint16_t row = 0; row < height; row++)
	{
		const uint8_t *row_data = model + row * bytes_per_row;
		for (uint16_t col = 0; col < width; col++)
		{
			uint8_t pixel = row_data[col / 8] & (1 << (7 - col % 8));
			if (pixel)
            {
                SPI_SendData(SPI2, color_data[0]);
                while (SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET);
                SPI_SendData(SPI2, color_data[1]);
                while (SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET);
            }
            else
            {
                SPI_SendData(SPI2, bg_color_data[0]);
                while (SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET);
                SPI_SendData(SPI2, bg_color_data[1]);
                while (SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET);
            }
		}
	}
    while (SPI_GetFlagStatus(SPI2, SPI_FLAG_BSY) != RESET);
    GPIO_SetBits(CS_PORT, CS_PIN);
}

static void st7789_write_ascii(uint16_t x, uint16_t y, char ch, uint16_t color, uint16_t bg_color, const font_t *font)
{
    if (font == NULL)
        return;
    
    uint16_t fheight = font->size, fwidth = font->size / 2;
    if (!in_screen_range(x, y, x + fwidth - 1, y + fheight - 1))
        return;
    
    if (ch < 0x20 || ch > 0x7E)
        return;
    
    uint16_t bytes_per_row = (fwidth + 7) / 8;
	const uint8_t *model = font->ascii_model + (ch - ' ') * fheight * bytes_per_row;
    st7789_draw_font(x, y, fwidth, fheight, model, color, bg_color);
}

static void st7789_write_chinese(uint16_t x, uint16_t y, char *ch, uint16_t color, uint16_t bg_color, const font_t *font)
{
    if (ch == NULL || font == NULL)
        return;

    uint16_t fheight = font->size, fwidth = font->size;
    if (!in_screen_range(x, y, x + fwidth - 1, y + fheight - 1))
        return;
    
    const font_chinese_t *c = font->chinese;
    for (; c->name != NULL; c++)
    {
        if (strcmp(c->name, ch) == 0)
            break;
    }
    if (c->name == NULL)
        return;
    
    st7789_draw_font(x, y, fwidth, fheight, c->model, color, bg_color);
}

static bool is_gb2312(char ch)
{
    return ((unsigned char)ch >= 0xA1 && (unsigned char)ch <= 0xF7);
}

void st7789_write_string(uint16_t x, uint16_t y, char *str, uint16_t color, uint16_t bg_color, const font_t *font)
{
    while (*str)
    {
        // int len = utf8_char_length(*str);
        int len = is_gb2312(*str) ? 2 : 1;
        if (len <= 0)
        {
            str++;
            continue;
        }
        else if (len == 1)
        {
            st7789_write_ascii(x, y, *str, color, bg_color, font);
            str++;
            x += font->size / 2;
        }
        else
        {
            char ch[5];
            strncpy(ch, str, len);
            st7789_write_chinese(x, y, ch, color, bg_color, font);
            str += len;
            x += font->size;
        }
    }
}

void st7789_draw_image(uint16_t x, uint16_t y, const image_t *image)
{
    if (x >= ST7789_WIDTH || y >= ST7789_HEIGHT || 
        x + image->width - 1 >= ST7789_WIDTH || y + image->height + 1 >= ST7789_HEIGHT)
        return;
    
    st7789_set_range_and_prepare_gram(x, y, x + image->width - 1, y + image->height - 1);
    
    GPIO_ResetBits(CS_PORT, CS_PIN);
    GPIO_SetBits(DC_PORT, DC_PIN);
    uint32_t size = image->width * image->height * 2;
    const uint8_t *data = image->data;
	for (uint32_t i = 0; i < size; i += 2)
	{
        SPI_SendData(SPI2, data[i + 1]);
        while (SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET);
        SPI_SendData(SPI2, data[i]);
        while (SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET);
	}
    while (SPI_GetFlagStatus(SPI2, SPI_FLAG_BSY) != RESET);
    GPIO_SetBits(CS_PORT, CS_PIN);
}

