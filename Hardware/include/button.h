/*
 * button.h
 *
 *  Created on: 2023年12月1日
 *      Author: ych
 */

#ifndef SRC_BUTTON_H_
#define SRC_BUTTON_H_


//键盘行的GPIO宏定义
#define   L5     GpioDataRegs.GPADAT.bit.GPIO26  //宏定义
#define   L4    GpioDataRegs.GPADAT.bit.GPIO25  //宏定义
#define   L2    GpioDataRegs.GPBDAT.bit.GPIO48  //宏定义
#define   L3    GpioDataRegs.GPADAT.bit.GPIO24  //宏定义

#define  V1_H  GpioDataRegs.GPASET.bit.GPIO21=1;//设置输出高电平
#define  V2_H  GpioDataRegs.GPASET.bit.GPIO20=1;//设置输出高电平
#define  V3_H  GpioDataRegs.GPASET.bit.GPIO27=1;//设置输出高电平

#define  V1_L GpioDataRegs.GPACLEAR.bit.GPIO21=1;//设置输出低电平
#define  V2_L GpioDataRegs.GPACLEAR.bit.GPIO20=1;//设置输出低电平
#define  V3_L GpioDataRegs.GPACLEAR.bit.GPIO27=1;//设置输出低电平

void InitExInt(void);

void Button_Matrix_Config(void);

unsigned char Button_Martix_Scan(void);


#endif /* SRC_BUTTON_H_ */
