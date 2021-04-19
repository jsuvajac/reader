﻿#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include <stdlib.h> // malloc() free()
#include <time.h> 

#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"
#include "ImageData.h"
#include "Debug.h"
#include "EPD_5in65f.h"


int EPD_5in65f_test(void)
{
    printf("EPD_5in65F_test Demo\r\n");
    if(DEV_Module_Init()!=0){
        return -1;
    }

    printf("e-Paper Init and Clear...\r\n");
    EPD_5IN65F_Init();
	struct timespec start={0,0}, finish={0,0}; 
    clock_gettime(CLOCK_REALTIME,&start);
    EPD_5IN65F_Clear(EPD_5IN65F_WHITE);
	clock_gettime(CLOCK_REALTIME,&finish);
    printf("%ld S\r\n",finish.tv_sec-start.tv_sec);
    DEV_Delay_ms(100);
	
    UBYTE *BlackImage;
    /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
    UDOUBLE Imagesize = ((EPD_5IN65F_WIDTH % 2 == 0)? (EPD_5IN65F_WIDTH / 2 ): (EPD_5IN65F_WIDTH / 2 + 1)) * EPD_5IN65F_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    Paint_NewImage(BlackImage, EPD_5IN65F_WIDTH, EPD_5IN65F_HEIGHT, 0, EPD_5IN65F_WHITE);
    Paint_SetScale(7);
	
#if 0
    printf("Maryland flag\r\n");
	EPD_5IN65F_Display(flagimage);
    DEV_Delay_ms(4000);
#endif

#if 0
    printf("show image for array\r\n");
    Paint_Clear(EPD_5IN65F_WHITE);
    GUI_ReadBmp_RGB_7Color("./pic/5in65f.bmp", 0, 0);
	EPD_5IN65F_Display(BlackImage);
    DEV_Delay_ms(4000);
#endif

#if 1
    Paint_Clear(EPD_5IN65F_WHITE);
    printf("Drawing:BlackImage\r\n");
    // Paint_DrawPoint(10, 80, EPD_5IN65F_BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    // Paint_DrawPoint(10, 90, EPD_5IN65F_BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    // Paint_DrawPoint(10, 100, EPD_5IN65F_BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
    // Paint_DrawLine(20, 70, 70, 120, EPD_5IN65F_BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    // Paint_DrawLine(70, 70, 20, 120, EPD_5IN65F_BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    // Paint_DrawRectangle(20, 70, 70, 120, EPD_5IN65F_BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    // Paint_DrawRectangle(80, 70, 130, 120, EPD_5IN65F_BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    // Paint_DrawCircle(45, 95, 20, EPD_5IN65F_BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    // Paint_DrawCircle(105, 95, 20, EPD_5IN65F_WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    // Paint_DrawLine(85, 95, 125, 95, EPD_5IN65F_BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    // Paint_DrawLine(105, 75, 105, 115, EPD_5IN65F_BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);

    Paint_DrawString_EN(10, 0, " !\"%&'()*+,-.", &Font32, EPD_5IN65F_BLACK, EPD_5IN65F_WHITE);
    Paint_DrawString_EN(10, 32, "hello world!", &Font32, EPD_5IN65F_WHITE, EPD_5IN65F_BLACK);
    Paint_DrawNum(10, 100, 123456789, &Font32, EPD_5IN65F_BLACK, EPD_5IN65F_WHITE);
    Paint_DrawNum(10, 150, 987654321, &Font32, EPD_5IN65F_WHITE, EPD_5IN65F_BLACK);

    EPD_5IN65F_Display(BlackImage);
    DEV_Delay_ms(4000); 
#endif
    // printf("e-Paper Clear...\r\n");
    // EPD_5IN65F_Clear(EPD_5IN65F_WHITE);
    // EPD_5IN65F_Clear(EPD_5IN65F_WHITE);
    // DEV_Delay_ms(1000); 
	
	printf("e-Paper Sleep...\r\n");
    EPD_5IN65F_Sleep();

    free(BlackImage);
    BlackImage = NULL;
    DEV_Delay_ms(2000);//important, at least 2s
    // close 5V
    printf("close 5V, Module enters 0 power consumption ...\r\n");
    DEV_Module_Exit();
    return 0;
}

void  Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:exit\r\n");
    DEV_Module_Exit();

    exit(0);
}

int main(void)
{
    // Exception handling:ctrl + c
    signal(SIGINT, Handler);
    EPD_5in65f_test();
	
    return 0;
}
