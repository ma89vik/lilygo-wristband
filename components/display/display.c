#include "display.h"
#include "st7735.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//Initialize the display
void display_draw_rect(int xpos, int xlen, int ypos, int ylen, uint16_t **linedata) {
    for (int i = xpos; i < xpos+xlen; i++) {
        for (int j = ypos; j < ypos+ylen; j++) {
            st7735_draw_pixel(i + MIN_X, j + MIN_Y, linedata[i][j]);
        }    
    }
    st7735_update_screen();
}

void display_fill_screen(uint16_t color) {
    st7735_clear_screen(color);
    st7735_update_screen();

}

void display_draw_fill_rect(int xpos, int xlen, int ypos, int ylen, uint16_t color) {
    for (int i = xpos; i < xpos+xlen; i++) {
        for (int j = ypos; j < ypos+ylen; j++) {
            st7735_draw_pixel(i + MIN_X, j + MIN_Y, color);
        }    
    }
    st7735_update_screen();
}
void display_draw_pixel(int xpos, int ypos, uint16_t color) {
    st7735_draw_pixel(xpos, ypos+ MIN_Y, color);
    st7735_update_screen();
}

void display_draw_border(int xpos, int xlen, int ypos, int ylen, uint8_t thickness, uint16_t color) {
    display_draw_fill_rect(xpos, xlen, ypos , thickness, color);
    printf("xpos %d xlen %d ypos %d ylen %d thick %d\n",xpos,xlen,ypos, ylen, thickness);
    display_draw_fill_rect(xpos, thickness, ypos, ylen, color);

    display_draw_fill_rect(xpos + xlen - thickness, thickness, ypos, ylen, color);    

    display_draw_fill_rect(xpos, xlen, ypos + ylen - thickness, thickness, color);
    st7735_update_screen();


}

void display_draw_string(int16_t x, int16_t y, char *str, uint16_t color, uint8_t size_x, uint8_t size_y) {
    st7735_set_position(x + MIN_X, y  + MIN_Y);
    
    st7735_draw_str(str, color, 0x0A);

    st7735_update_screen();

}


 
void display_init() {
    st7735_init();
    display_fill_screen(0xFFFF);
    display_draw_string(20, 20, "20:10", 0x11, 2,2);
    //display_draw_border(0, SIZE_X , 0, SIZE_Y, 1, 0xFF);

    //display_draw_fill_rect(0, 1, 10, 10, 0xFF);
    //display_draw_fill_rect(SIZE_X-1, 1, 10, 10, 0xFF);
    //display_draw_fill_rect(10, 10, 0, 1, 0xFF);

    //display_draw_string(1, 33, "hei", 0x33, 0,0);
}