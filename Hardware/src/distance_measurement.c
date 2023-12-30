/*
 * distance_measurement.c
 *
 *  Created on: 2023年12月23日
 *      Author: ych
 */

#include <distance_measurement.h>
#include<sci.h>

extern Uint32 cap_val;

void InitECap1Gpio(){

    EALLOW;
 /* Enable internal pull-up for the selected pins */
 // Pull-ups can be enabled or disabled by the user.
 // This will enable the pullups for the specified pins.
 // Comment out other unwanted lines.

    GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;      // Enable pull-up on GPIO5 (CAP1)
   //GpioCtrlRegs.GPAPUD.bit.GPIO24 = 0;     // Enable pull-up on GPIO24 (CAP1)
 // GpioCtrlRegs.GPBPUD.bit.GPIO34 = 0;     // Enable pull-up on GPIO34 (CAP1)


 // Inputs are synchronized to SYSCLKOUT by default.
 // Comment out other unwanted lines.

    GpioCtrlRegs.GPAQSEL1.bit.GPIO5 = 0;    // Synch to SYSCLKOUT GPIO5 (CAP1)
  //GpioCtrlRegs.GPAQSEL2.bit.GPIO24 = 0;   // Synch to SYSCLKOUT GPIO24 (CAP1)
 // GpioCtrlRegs.GPBQSEL1.bit.GPIO34 = 0;   // Synch to SYSCLKOUT GPIO34 (CAP1)

 /* Configure eCAP-1 pins using GPIO regs*/
 // This specifies which of the possible GPIO pins will be eCAP1 functional pins.
 // Comment out other unwanted lines.

     GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 3;     // Configure GPIO5 as CAP1
    // GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 1;    // Configure GPIO24 as CAP1
 // GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 1;    // Configure GPIO24 as CAP1

     EDIS;

}

/**
 * 初始化ECAP
 *
 */
void Init_ECap1(void)
{
    InitECap1Gpio();
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0; // 普通IO模式
    GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;  // 配置成输出
    EDIS;

    ECap1Regs.ECEINT.all = 0x0000; // 关闭所有的CAP中断
    ECap1Regs.ECCLR.all = 0xFFFF;  // 清除所有的CAP中断标志位
    ECap1Regs.ECCTL1.all = 0x0000; // 复位控制寄存器2
    ECap1Regs.ECCTL2.all = 0x0000; // 复位控制寄存器2

    ECap1Regs.TSCTR = 0;  // 计数清零
    ECap1Regs.CTRPHS = 0; // 计数相位寄存器清零

    ECap1Regs.ECCTL1.bit.CAP2POL = 1;   // 捕获事件2下降沿触发
    ECap1Regs.ECCTL1.bit.CTRRST2 = 1;   // 捕获事件2发生时计数器复位
    ECap1Regs.ECCTL1.bit.CAP4POL = 1;   // 捕获事件4下降沿触发
    ECap1Regs.ECCTL1.bit.CTRRST4 = 1;   // 捕获事件4发生时计数器复位
    ECap1Regs.ECCTL1.bit.CAPLDEN = 1;   // 使能在捕获事件发生时装载CAP1~CAP4
    ECap1Regs.ECCTL1.bit.FREE_SOFT = 2; // TSCTR不受仿真影响

    ECap1Regs.ECCTL2.bit.STOP_WRAP = 1; // 在捕获事件2发生后停止（单次模式）
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1; // 开始计数
    ECap1Regs.ECCTL2.bit.SYNCO_SEL = 2; // 禁止SYNC_OUT输出信号

    ECap1Regs.ECEINT.bit.CEVT2 = 1; // 使能捕获事件2发生中断
}

/**
 * 读取测量距离
 */
Uint32 ReadDistance(void)
{
    GpioDataRegs.GPASET.bit.GPIO1 = 1;   // Trig引脚输出高电平
    DELAY_US(1);                        // 至少10us，本实验给15us
    GpioDataRegs.GPACLEAR.bit.GPIO1 = 1; // Trig引脚输出低
    Uint32 Distan;
    Distan = cap_val * 170 / 150000;     // 计算距离，此处单位为mm
    return Distan;
}
