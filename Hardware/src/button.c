/*
 * button.c
 *
 *  Created on: 2023年12月1日
 *      Author: ych
 */
#include <button.h>
#include"DSP2833x_Project.h"

extern void delay_1ms(Uint32 t);

void InitExInt(void){

    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;//复用为GPIO功能
    GpioCtrlRegs.GPADIR.bit.GPIO13 = 0;//gpio输入功能
    GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 2;//采用6次采样
    GpioCtrlRegs.GPACTRL.bit.QUALPRD1 = 255;//设置采样时间
    GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 13;//选择GPIO13为外部输入XINT输入引脚
    XIntruptRegs.XINT1CR.bit.POLARITY= 0;     //下降沿触发中断 0是下降沿 1是上升沿
    XIntruptRegs.XINT1CR.bit.ENABLE = 1;      //使能XINT中断
    EDIS;


}



void Button_Matrix_Config(void){

    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK=1;//开启GPIO时钟

    GpioCtrlRegs.GPAMUX2.bit.GPIO21=0;//普通IO模式
    GpioCtrlRegs.GPAPUD.bit.GPIO21=0;//使能内部上拉
    GpioCtrlRegs.GPADIR.bit.GPIO21=1;//配置成输出

    GpioCtrlRegs.GPAMUX2.bit.GPIO20=0;//普通IO模式
    GpioCtrlRegs.GPAPUD.bit.GPIO20=0;//使能内部上拉
    GpioCtrlRegs.GPADIR.bit.GPIO20=1;//配置成输出

    GpioCtrlRegs.GPAMUX2.bit.GPIO27=0;//普通IO模式
    GpioCtrlRegs.GPAPUD.bit.GPIO27=0;//使能内部上拉
    GpioCtrlRegs.GPADIR.bit.GPIO27=1;//配置成输出


    GpioCtrlRegs.GPAMUX2.bit.GPIO26=0;//普通IO模式
    GpioCtrlRegs.GPAPUD.bit.GPIO26=0;//使能内部上拉
    GpioCtrlRegs.GPADIR.bit.GPIO26=0;//配置成输入



    GpioCtrlRegs.GPAMUX2.bit.GPIO25=0;//普通IO模式
    GpioCtrlRegs.GPAPUD.bit.GPIO25=0;//使能内部上拉
    GpioCtrlRegs.GPADIR.bit.GPIO25=0;//配置成输入


    GpioCtrlRegs.GPBMUX2.bit.GPIO48=0;//普通IO模式
    GpioCtrlRegs.GPBPUD.bit.GPIO48=0;//使能内部上拉
    GpioCtrlRegs.GPBDIR.bit.GPIO48=0;//配置成输入


    GpioCtrlRegs.GPAMUX2.bit.GPIO24=0;//普通IO模式
    GpioCtrlRegs.GPAPUD.bit.GPIO24=0;//使能内部上拉
    GpioCtrlRegs.GPADIR.bit.GPIO24=0;//配置成输入

    EDIS;

    GpioDataRegs.GPASET.bit.GPIO21=1;//设置输出高电平
    GpioDataRegs.GPASET.bit.GPIO20=1;//设置输出高电平
    GpioDataRegs.GPASET.bit.GPIO27=1;//设置输出高电平


}

/*
 * 行列扫描法检测时， 先送一列为低电平， 其余几列全为高电平(此时我们确定了列数)，
 * 然后立即轮流检测一次各行是否有低电平， 若检测到某一行为低电平(这时我们又确定了行数)，
 * 则我们便可确认当前被按下的键是哪一行哪一列的， 用同样方法轮流送各列一次低电平,
 * 再轮流检测一次各行是否变为低电平，这样即可检测完所有的按键，
 * 当有键被按下时便可判断出按下的键是哪一个键。当然我们也可以将行线置低电平，
 * 扫描列是否有低电平。 从而达到整个键盘的检测。
 * */
unsigned char Button_Martix_Scan(void){

    V1_L;
    V2_H;
    V3_H;
    if(L2==0||L3==0||L4==0||L5==0){
        DELAY_US(10000);
        if(L2==0){
            return 0x21;
        }else if(L3==0){
            return 0x31;
        }else if(L4==0){
            return 0x41;
        }else if(L5==0){
            return 0x51;
        }
    }

    V2_L;
    V1_H;
    V3_H;
    if(L2==0||L3==0||L4==0||L5==0){
        DELAY_US(10000);
        if(L2==0){
            return 0x22;
        }else if(L3==0){
            return 0x32;
        }else if(L4==0){
            return 0x42;
        }else if(L5==0){
            return 0x52;
        }
    }

    V3_L;
    V1_H;
    V2_H;
    if(L2==0||L3==0||L4==0||L5==0){
        DELAY_US(10000);
        if(L2==0){
            return 0x23;
        }else if(L3==0){
            return 0x33;
        }else if(L4==0){
            return 0x43;
        }else if(L5==0){
            return 0x53;
        }
    }

    return 0x00;
}
