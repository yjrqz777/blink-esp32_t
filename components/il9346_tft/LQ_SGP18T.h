/********************************************************
【平    台】北京龙丘智能科技淘宝店
【编    写】龙丘科技整理，原创阿呆
【Designed】by Chiu Sir
【E-mail  】chiusir@yahoo.cn
【软件版本】V1.0
【最后更新】2012年9月9日
【dev.env.】KEIL UV3.0
【Target  】STC/AT89C52/STC12C5A60S2/STC12LE5A60S2
【Crystal 】11.0592M/22.1184Mhz

3.3V接线说明：
1.下载本程序到单片机
2.插线(注意单片机的这几个管脚不要有外围电路)：
    GND	地
	VCC 3.3V(裸屏必须用3.3V供电)
	RESET=  P1^0; 
	A0   =  P1^1;
	SDI  =  P1^2;
	SCK  =  P1^3;	 
	CS   =  P1^4;
（5V的单片机需要在信号线上串接1K电阻，电源必须保证3.3V）
********************************************************/

#ifndef __SGP18T_ILI9163B_BASE_H__
#define __SGP18T_ILI9163B_BASE_H__	 

#include "MyType.h"	 

/*******************接口定义******************************/
// sbit	ILI9163_RESET	=	P1^0; 
// sbit	ILI9163_A0		=	P1^1;
// sbit	ILI9163_SDA		=	P1^2;
// sbit	ILI9163_SCK		=	P1^3;
// sbit	ILI9163_CS		=	P1^4;

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

#define		RED			0xf800
#define		GREEN		0x07e0
#define		BLUE		0x001f
#define		PURPLE		0xf81f
#define		YELLOW		0xffe0
#define		CYAN		0x07ff 		//蓝绿色
#define		ORANGE		0xfc08
#define		BLACK		0x0000
#define		WHITE		0xffff

/*****************私有函数声名*********************************/

void ILI9163B_init(void);									//LCD初始化

void ILI9163B_write_command(uint8 cmd);						//发送控制字

void ILI9163B_write_para8(uint8 dat);						//发送数据参数

void ILI9163B_write_para16(uint16 dat);						//发送像素显示参数

void ILI9163B_address_rst(void);							//DDRAM地址重置

void ILI9163B_SetPos(uint8 xs,uint8 ys,uint8 xe,uint8 ye);	//定位显示像素位置

void delay(uint16 t);									   	//延时函数

void ILI9163B_display_full(uint16 color);					//全屏显示某种颜色


void ILI9163B_draw_part(uint8 xs,uint8 ys,uint8 xe,uint8 ye,uint16 color_dat);
															//填充矩形区域，行起始、终止坐标，列起始、终止坐标，颜色

void ILI9163B_draw_line(uint8 xs,uint8 ys,uint8 xe,uint8 ye,uint16 color_dat);
															//画线，行起始、终止坐标，列起始、终止坐标，颜色

void ILI9163B_draw_rectangle(uint8 xs,uint8 ys,uint8 xe,uint8 ye,uint16 color_dat);
															//画矩形边框，行起始、终止坐标，列起始、终止坐标，颜色

void ILI9163B_draw_circle(uint8 x,uint8 y,uint8 r,uint16 color_dat);
															//画圆形边框，圆心横坐标、纵坐标，半径，颜色

void ILI9163B_draw_dot(uint8 x,uint8 y,uint16 color_dat);	//画点，横坐标，纵坐标，颜色



void LCD_printc16(uint8 x, uint8 y, uint8 c_dat,uint16 word_color,uint16 back_color);

void LCD_prints16(uint8 x, uint8 y, uint8 *s_dat,uint16 word_color,uint16 back_color);

void LCD_printc6(uint8 x, uint8 y, uint8 c_dat,uint16 word_color,uint16 back_color);

void LCD_prints6(uint8 x, uint8 y, uint8 *s_dat,uint16 word_color,uint16 back_color);

void LCD_printn6(uint8 x, uint8 y, uint16 num, uint8 num_bit,uint16 word_color,uint16 back_color);

void LCD_printc(uint8 x, uint8 y, uint8 c_dat,uint16 word_color,uint16 back_color);

void LCD_prints(uint8 x, uint8 y, uint8 *s_dat,uint16 word_color,uint16 back_color);

void LCD_printn(uint8 x, uint8 y, uint16 num, uint8 num_bit,uint16 word_color,uint16 back_color);

#endif /*SGP18T_ILI9163B.h*/