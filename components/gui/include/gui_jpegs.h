#ifndef GUI_JPEGS_H_
#define GUI_JPEGS_H_


#include <stdint.h>

//Define the height and width of the jpeg file. Make sure this matches the actual jpeg
//dimensions.
#define IMAGE_W 146
#define IMAGE_H 86

#define JPEG_HEART_W 82
#define JPEG_HEART_H 74

typedef struct jpeg_image_t {
    uint8_t* file;
    int height;
    int width;
} jpeg_image_t;

extern const uint8_t image_jpg_start[] asm("_binary_image_jpg_start");
extern const uint8_t image_jpg_end[] asm("_binary_image_jpg_end");



#endif