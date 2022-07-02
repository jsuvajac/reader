#include "../lib/Config/DEV_Config.h"
#include "example.h"
#include "../lib/GUI/GUI_BMPfile.h"
#include "../lib/GUI/GUI_Paint.h"
#include "../lib/Fonts/fonts.h"

#include <math.h>
#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include <termios.h>

// value for my screen -1.45 (default = -2.51)
uint16_t VCOM = 1450;

IT8951_Dev_Info Dev_Info;
UWORD Panel_Width;
UWORD Panel_Height;
UDOUBLE Init_Target_Memory_Addr;
int epd_mode = 0;	//0: no rotate, no mirror
					//1: no rotate, horizontal mirror, for 10.3inch
					//2: no totate, horizontal mirror, for 5.17inch
					//3: no rotate, no mirror, isColor, for 6inch color
					
struct termios start_term_attr;


void  Handler(int signo){
    Debug("\r\nHandler:exit\r\n");
    if(Refresh_Frame_Buf != NULL){
        free(Refresh_Frame_Buf);
        Debug("free Refresh_Frame_Buf\r\n");
        Refresh_Frame_Buf = NULL;
    }
    if(Panel_Frame_Buf != NULL){
        free(Panel_Frame_Buf);
        Debug("free Panel_Frame_Buf\r\n");
        Panel_Frame_Buf = NULL;
    }
    if(Panel_Area_Frame_Buf != NULL){
        free(Panel_Area_Frame_Buf);
        Debug("free Panel_Area_Frame_Buf\r\n");
        Panel_Area_Frame_Buf = NULL;
    }
    if(bmp_src_buf != NULL){
        free(bmp_src_buf);
        Debug("free bmp_src_buf\r\n");
        bmp_src_buf = NULL;
    }
    if(bmp_dst_buf != NULL){
        free(bmp_dst_buf);
        Debug("free bmp_dst_buf\r\n");
        bmp_dst_buf = NULL;
    }
    Debug("Going to sleep\r\n");
    EPD_IT8951_Sleep();
    DEV_Delay_ms(5000);
    DEV_Module_Exit();
    tcsetattr(fileno(stdin), TCSANOW, &start_term_attr);
    exit(0);
}


// non-blocking get key, ignore queued
int getkey() {
    int character = -1;
    struct termios new_term_attr;
    struct termios orig_term_attr;

    // set the terminal to raw mode
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSAFLUSH, &new_term_attr);

    // read a character from the stdin stream without blocking, ignore queued key presses
    // returns EOF (-1) if no character is available
    int prev = -1;
    do {
        prev = character;
        character = fgetc(stdin);
    } while(character != -1);
    character = prev;

    /* restore the original terminal attributes */
    // tcsetattr(fileno(stdin), TCSAFLUSH, &orig_term_attr);

    return character;
}
void refresh(uint8_t BitsPerPixel, uint8_t *buffer, uint16_t width, uint16_t height, uint32_t Init_Target_Memory_Addr) {
    switch(BitsPerPixel){
        case BitsPerPixel_8:{
            EPD_IT8951_8bp_Refresh(buffer, 0, 0, width,  height, false, Init_Target_Memory_Addr);
            break;
        }
        case BitsPerPixel_4:{
            EPD_IT8951_4bp_Refresh(buffer, 0, 0, width,  height, false, Init_Target_Memory_Addr,false);
            break;
        }
        case BitsPerPixel_2:{
            EPD_IT8951_2bp_Refresh(buffer, 0, 0, width,  height, false, Init_Target_Memory_Addr,false);
            break;
        }
        case BitsPerPixel_1:{
            EPD_IT8951_1bp_Refresh(buffer, 0, 0, width,  height, A2_Mode, Init_Target_Memory_Addr,false);
            break;
        }
    }
        
}

void draw_point(uint16_t x, uint16_t y, uint16_t size, uint16_t color) {
    for (int i = 0; i < size; ++i) {
        for (int ii = 0; ii < size; ++ii) {
            Paint_SetPixel(x + i - size/2, y + ii- size/2, color);
        }
    }
}

int main(int argc, char *argv[]) {
    struct termios orig_term_attr;

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&start_term_attr, &orig_term_attr, sizeof(struct termios));

    //Exception handling:ctrl + c
    signal(SIGINT, Handler);

    //Init the BCM2835 Device
    if(DEV_Module_Init()!=0){
        return -1;
    }

    // get dimensions
    Dev_Info = EPD_IT8951_Init(VCOM);
    // 1872 x 1404;
    Panel_Width = Dev_Info.Panel_W;
    Panel_Height = Dev_Info.Panel_H;


    uint8_t BitsPerPixel = BitsPerPixel_8;

    Init_Target_Memory_Addr = Dev_Info.Memory_Addr_L | (Dev_Info.Memory_Addr_H << 16);
    // char* LUT_Version = (char*)Dev_Info.LUT_Version;
    //7.8inch e-Paper HAT(1872,1404)
    A2_Mode = 6;

	EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, INIT_Mode);

    // examples
#if 0
    run_all_examples(Dev_Info, Init_Target_Memory_Addr);
#endif


    // allocate buffer
    uint32_t image_size  = ((Panel_Width * BitsPerPixel % 8 == 0)?
        (Panel_Width * BitsPerPixel / 8 ):
        (Panel_Width * BitsPerPixel / 8 + 1)) * Panel_Height;

    uint8_t *buffer = NULL;

    if((buffer = (uint8_t *)malloc(image_size)) == NULL) {
        Debug("Failed to apply for image memory...\r\n");
        return -1;
    }



    uint16_t width = Dev_Info.Panel_W;
    uint16_t height = Dev_Info.Panel_H;
    int tile_size = 50;
    int grid_size = 26;
    int start_offset_x = 1;
    int start_offset_y = 2;

    int x = 0;
    int y = 0;
    int prev_x = x;
    int prev_y = y;


    int is_running = true;

    // printf("%d, %d\n", Paint.Width, Paint.Height);

    printf("\n");

    printf("starting snake\n");

    // setup new image
    Paint_NewImage(buffer, width, height, 0, 0);
    Paint_SelectImage(buffer);

    Paint_SetRotate(0);
    Paint_SetMirroring(MIRROR_NONE);

    Paint_SetBitsPerPixel(BitsPerPixel);
    Paint_Clear(0xFF);


    // draw grid
    printf("draw grid\n");

    Paint_DrawRectangle(25 * start_offset_x, 25 * start_offset_y, tile_size * grid_size + 25, tile_size * grid_size + 25, 0x00, 3, DRAW_FILL_EMPTY);

    for (int i = 0; i < grid_size; ++i) {
        for (int ii = 0; ii < grid_size; ++ii) {
            // Paint_DrawLine(x * tile_size, y * tile_size, i * tile_size, ii * tile_size, 0x10, DOT_PIXEL_3X3, LINE_STYLE_SOLID);

            Paint_DrawPoint(
                start_offset_x * tile_size + ii * tile_size,
                start_offset_y * tile_size + i  * tile_size,
                0x1, DOT_PIXEL_3X3, DOT_STYLE_DFT);
        }
    }

    // debug log
    Paint_DrawString_EN(
        (grid_size + 2) * tile_size, 100,
        "Started", &Font24, 0x30, 0xD0);
    Paint_DrawString_EN(
        (grid_size + 2) * tile_size, 120,
        "1234567890abcdefghijklmnopqrstuvwxyz"
        , &Font32, 0x00, 0xFF);

    // printf("draw point\n");
    // draw_point(
    //     x * 100 + 50,
    //     y * 100 + 50,
    //     30, 0x1);
    // if (prev_x != x || prev_y != y) {
    //     draw_point(
    //         x * 100 + 50,
    //         y * 100 + 50,
    //         65, 0x8);
    // }

    printf("\nrefresh\n"); // slow
    refresh(BitsPerPixel, buffer, width, height, Init_Target_Memory_Addr);
    printf("done\n");

    while(is_running) {
        Paint_NewImage(buffer, tile_size, tile_size, 0, 0);
        Paint_SelectImage(buffer);
        Paint_SetBitsPerPixel(BitsPerPixel);

        printf("(%d, %d) <- (%d, %d)\n", x, y, prev_x, prev_y);

        // clear previous field
        if (prev_x != x || prev_y != y) {
            Paint_Clear(WHITE);
            // reset cleared corners
            int s = tile_size;
            Paint_DrawPoint(0, 0, 0, 3, DOT_STYLE_DFT);
            Paint_DrawPoint(0, s, 0, 3, DOT_STYLE_DFT);
            Paint_DrawPoint(s, 0, 0, 3, DOT_STYLE_DFT);
            Paint_DrawPoint(s, s, 0, 3, DOT_STYLE_DFT);

            EPD_IT8951_8bp_Refresh(buffer, (prev_x + 1)* tile_size, (prev_y + 1)* tile_size, tile_size,  tile_size, false, Init_Target_Memory_Addr);
        }

        // draw new tile
        Paint_Clear(WHITE);
        int border = 10;
        Paint_DrawRectangle(border, border, tile_size-border, tile_size-border, 0x00, 3, DRAW_FILL_EMPTY);
        Paint_DrawString_EN(
            (tile_size - 2)/2 - 17/2, 
            (tile_size - 2)/2 - 24/2,
            "S", &Font24, 0x30, 0xD0);

        // Paint_Clear(WHITE);
        printf("refresh\n");
        EPD_IT8951_8bp_Refresh(buffer, (x + 1) * tile_size, (y + 1) * tile_size, tile_size,  tile_size, false, Init_Target_Memory_Addr);
        printf("done\n");

        prev_x = x;
        prev_y = y;
    
        // printf("key\n");
        while(true) {
            int key = getkey();
            if(key != -1) {
                printf("--- %d %c\n", key, (char)key);

                if ((char)key == 'h' || (char)key == 'a') x -= 1;
                if ((char)key == 'l' || (char)key == 'd') x += 1;
                if ((char)key == 'j' || (char)key == 's') y += 1;
                if ((char)key == 'k' || (char)key == 'w') y -= 1;

                if (key == 27) is_running = false;
                break;
            }
        }
        if (is_running == false) break;

        // printf("\n");
    }


    printf("cleanup\n");
    tcsetattr(fileno(stdin), TCSANOW, &start_term_attr);

    if(buffer != NULL){
        free(buffer);
        buffer = NULL;
    }

    //We recommended refresh the panel to white color before storing in the warehouse.
    EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, INIT_Mode);

    //EPD_IT8951_Standby();
    EPD_IT8951_Sleep();

    //In case RPI is transmitting image in no hold mode, which requires at most 10s
    DEV_Delay_ms(5000);

    DEV_Module_Exit();
    return 0;
}
 