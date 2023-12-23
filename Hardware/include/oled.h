/*
 * oled.h
 *
 *  Created on: 2023年12月15日
 *      Author: ych
 */

#ifndef HARDWARE_INCLUDE_OLED_H_
#define HARDWARE_INCLUDE_OLED_H_

#include"DSP2833x_Project.h"

/**
 *  @brief             oled写命令
 *  @parameter              无
 *  @return_value           无
 */
void WriteCmd(Uint16 IIC_Command) ;

/**
 *  @brief             oled写数据
 *  @parameter              无
 *  @return_value           无
 */
void WriteDat(Uint16 IIC_Data) ;


/**
*  @brief             OELD屏幕初始化
*  @parameter              无
*  @return_value           无
*/
void OLED_Init(void);


/**
*  @brief             设置起点坐标
*  @parameter              无
*  @return_value           无
*/
void SetPos(unsigned char x,unsigned char y);


/**
*  @brief             全屏填充
*  @parameter              无
*  @return_value           无
*/
void OLED_Fill(unsigned char Fill_Data);


/**
*  @brief              清屏
*  @parameter              无
*  @return_value           无
*/
void OLED_Clean(void);

/**
*  @brief               打开OLED
*  @parameter              无
*  @return_value           无
*/
void OLED_ON(void);

/**
*  @brief               关闭OLED
*  @parameter              无
*  @return_value           无
*/
void OLED_OFF(void);



/**
*  @brief               显示字符串
*  @parameter              无
*  @return_value           无
*/
void OLED_ShowStr( char x, char y, char ch[],char TextSize);


#endif /* HARDWARE_INCLUDE_OLED_H_ */
