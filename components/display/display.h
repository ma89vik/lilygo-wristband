#pragma once

#include "stdint.h"
#include "st7735_defs.h"
#include "esp_system.h"

#define SCREEN_HEIGHT SIZE_Y
#define SCREEN_WIDTH SIZE_X

#define BLACK	0x0000	// 
#define WHITE	0xFFFF	// RGB
#define RED	    0xF800	// R
#define GREEN	0x07E0	// G
#define BLUE	0x001F	// B
#define YELLOW	0xFFE0  // RG
#define CYAN	0x07FF	// GB
#define MAGNETA	0xF81F	// RB
#define GREY	0x0821	// 00001 000001 00001

//Initialize the display
void display_draw_rect(int xpos, int xlen, int ypos, int ylen, uint16_t **linedata);
void display_fill_screen(uint16_t color);
void display_draw_fill_rect(int xpos, int xlen, int ypos, int ylen, uint16_t color);
void display_draw_pixel(int xpos, int ypos, uint16_t color);
esp_err_t display_set_border(int xpos, int xlen, int ypos, int ylen);
void display_draw_border(int xpos, int xlen, int ypos, int ylen, uint8_t thickness, uint16_t color);
void display_draw_string(int16_t x, int16_t y, char *str, uint16_t color, uint8_t size_x, uint8_t size_y);

void display_init();