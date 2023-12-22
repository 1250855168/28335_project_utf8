/*
 * led.c
 *
 *  Created on: 2023年12月1日
 *      Author: ych
 */

#include <led.h>


void LED_GPIO_Config(void){
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0; // GPIO0复用为GPIO功能
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;  // GPIO0设置为输出
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0; // GPIO1复用为GPIO功能
    GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;  // GPIO1设置为输出
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0; // GPIO2复用为GPIO功能
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;  // GPIO2设置为输出
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0; // GPIO3复用为GPIO功能
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;  // GPIO3设置为输出
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0; // GPIO4复用为GPIO功能
    GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;  // GPIO4设置为输出
    EDIS;
}


