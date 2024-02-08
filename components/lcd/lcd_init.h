#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "driver/gpio.h"
// #include "freertos/task.h"

#define u8 unsigned char
#define u16 unsigned short int
#define u32 unsigned int

#define USE_HORIZONTAL 0  //设置横屏或者竖屏显示 
                          //0:正向
                          //1:旋转180度
													//2:旋转90度
													//3:旋转270度
#if USE_HORIZONTAL<2
#define LCD_W 128
#define LCD_H 64
#else
#define LCD_W 64
#define LCD_H 128
#endif


//-----------------LCD端口定义---------------- 

#define LCD_SCLK_Clr() gpio_set_level(GPIO_NUM_14, 0);//SCL=SCLK
#define LCD_SCLK_Set() gpio_set_level(GPIO_NUM_14, 1);

#define LCD_MOSI_Clr() gpio_set_level(GPIO_NUM_13, 0);//SDA=MOSI
#define LCD_MOSI_Set() gpio_set_level(GPIO_NUM_13, 1);

#define LCD_RES_Clr()  gpio_set_level(GPIO_NUM_4, 0);//RES
#define LCD_RES_Set()  gpio_set_level(GPIO_NUM_4, 1);

#define LCD_DC_Clr()   gpio_set_level(GPIO_NUM_2, 0);//DC
#define LCD_DC_Set()   gpio_set_level(GPIO_NUM_2, 1);

#define LCD_BLK_Clr()  gpio_set_level(GPIO_NUM_15, 0);//BLK
#define LCD_BLK_Set()  gpio_set_level(GPIO_NUM_15, 1);


// #define LCD_SCLK_Clr() gpio_set_level(GPIO_NUM_14, 1);//SCL=SCLK
// #define LCD_SCLK_Set() gpio_set_level(GPIO_NUM_14, 0);

// #define LCD_MOSI_Clr() gpio_set_level(GPIO_NUM_13, 1);//SDA=MOSI
// #define LCD_MOSI_Set() gpio_set_level(GPIO_NUM_13, 0);

// #define LCD_RES_Clr()  gpio_set_level(GPIO_NUM_4, 1);//RES
// #define LCD_RES_Set()  gpio_set_level(GPIO_NUM_4, 0);

// #define LCD_DC_Clr()   gpio_set_level(GPIO_NUM_2, 1);//DC
// #define LCD_DC_Set()   gpio_set_level(GPIO_NUM_2, 0);

// #define LCD_BLK_Clr()  gpio_set_level(GPIO_NUM_15, 1);//BLK
// #define LCD_BLK_Set()  gpio_set_level(GPIO_NUM_15, 0);




void LCD_GPIO_Init(void);//初始化GPIO
void LCD_Writ_Bus(u8 dat);//模拟SPI时序
void LCD_WR_DATA8(u8 dat);//写入一个字节
void LCD_WR_DATA(u16 dat);//写入两个字节
void LCD_WR_REG(u8 dat);//写入一个指令
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置坐标函数
void LCD_Init(void);//LCD初始化
#endif




