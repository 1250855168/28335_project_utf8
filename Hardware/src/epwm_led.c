/*
 * epwm_led.c
 *
 *  Created on: 2023年12月2日
 *      Author: ych
 */

#include <epwm_led.h>


static epwm1_gpio_init(void){

    EALLOW;

    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0;    // Enable pull-up on GPIO0 (EPWM1A)
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;    // Enable pull-up on GPIO1 (EPWM1B)

    GpioCtrlRegs.GPADIR.bit.GPIO0=1;//配置成输出
    GpioCtrlRegs.GPADIR.bit.GPIO1=1;//配置成输出

    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;   // Configure GPIO0 as EPWM1A
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;   // Configure GPIO1 as EPWM1B

    EDIS;
}

void epwm1_led_init(void){

    epwm1_gpio_init();

    EPwm1Regs.TBSTS.all=0;
    EPwm1Regs.TBPHS.half.TBPHS=0;
    EPwm1Regs.TBCTR=0;

    //EPwm1Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
    EPwm1Regs.CMPCTL.bit.LOADAMODE = 2; //计数比较寄存器 A （CMPA） 从影子寄存器装载模式选择位
    EPwm1Regs.CMPCTL.bit.LOADBMODE = 2; // 计数比较寄存器 B （CMPB） 从影子寄存器装载模式选择位
    //EPwm1Regs.CMPCTL.bit.SHDWAFULL    //判断影子寄存器是否加载上去
    //EPwm1Regs.CMPCTL.bit.SHDWAMODE = 0; // 影子模式  1立即装载模式
    //EPwm1Regs.CMPCTL.bit.SHDWBFULL    //判断影子寄存器是否加载上去
    //EPwm1Regs.CMPCTL.bit.SHDWBMODE = 0; // 影子模式  1立即装载模式
    EPwm1Regs.CMPA.half.CMPA =5000;      //时基计数器 （TBCTR） 的值相比较
//    EPwm1Regs.CMPB=500;                 //时基计数器 （TBCTR） 的值相比较


    //EPwm1Regs.AQCTLA.all=0x60;        // EPWMxA = 1 when CTR=CMPA and counter inc
                                      // EPWMxA = 0 when CTR=CMPA and counter dec
    EPwm1Regs.AQCTLA.bit.CAD = 1;
    EPwm1Regs.AQCTLA.bit.CAU = 2;
    //EPwm1Regs.AQCTLB.all=0x60;
    EPwm1Regs.AQCTLB.bit.CAD = 1;
    EPwm1Regs.AQCTLB.bit.CAU = 2;
//  EPwm1Regs.AQCTLB.all=0;
//  EPwm1Regs.AQSFRC.all=0;
//  EPwm1Regs.AQCSFRC.all=0;

//  EPwm1Regs.DBCTL.all=0x23;          // EPWMxB is inverted
    EPwm1Regs.DBRED=0;
    EPwm1Regs.DBFED=0;

    EPwm1Regs.TZSEL.all=0;
    EPwm1Regs.TZCTL.all=0;
    EPwm1Regs.TZEINT.all=0;
    EPwm1Regs.TZFLG.all=0;
    EPwm1Regs.TZCLR.all=0;
    EPwm1Regs.TZFRC.all=0;

    EPwm1Regs.ETSEL.all=0;            // Interrupt when TBCTR = 0x0000
    EPwm1Regs.ETFLG.all=0;
    EPwm1Regs.ETCLR.all=0;
    EPwm1Regs.ETFRC.all=0;

    EPwm1Regs.PCCTL.all=0;

    //EPwm1Regs.TBCTL.all=0x0010+TBCTLVAL;          // Enable Timer
    EPwm1Regs.TBCTL.bit.PHSDIR = 1;
    EPwm1Regs.TBCTL.bit.SYNCOSEL = 1;
    EPwm1Regs.TBCTL.bit.PRDLD = 1;
    EPwm1Regs.TBCTL.bit.PHSEN = 1;
    EPwm1Regs.TBCTL.bit.CTRMODE = 2;
    EPwm1Regs.TBPRD=SP;

    EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;       // 在CTR=TBPRD时，产生中断
    EPwm1Regs.ETSEL.bit.INTEN = 1;                 // 使能中断
    EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;            // 每一次满足中断条件就产生一次中断

}

void epwm1_test_init(void){

    epwm1_gpio_init(); //150Mhz
    EALLOW;
    EPwm1Regs.TBSTS.all=0;
   // EPwm1Regs.TBPHS.half.TBPHS=500;//ePWM1模块的时基计数器将从500开始计数，而不是从0开始计数
    EPwm1Regs.TBCTR=0x0000;          //寄存器可以设置时基计数器的值
   // EPwm1Regs.TBCTL.all=0x0010+TBCTLVAL;            // Enable Timer
    EPwm1Regs.TBCTL.bit.CLKDIV =  1;                 //用于配置时基计数器的时钟分频系数
    EPwm1Regs.TBCTL.bit.CTRMODE = 0;      //当CTRMODE设置为0时，表示选择"上计数"模式。在这种模式下，时基计数器（TBCTR）从0开始向上计数
   // EPwm1Regs.TBCTL.bit.FREE_SOFT = 0x10;
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 5;        //用于配置高速时钟分频系数
   //EPwm1Regs.TBCTL.bit.PHSDIR = 1;  // 相位方向位。 当时基计数器 （TBCTR） 0设置为增减计数模式时 当同步事件发生时减计数
    //EPwm1Regs.TBCTL.bit.PHSEN = 0;  //1在多个PWM信号之间保持固定的相位差 开启多通道的同步
    EPwm1Regs.TBCTL.bit.PRDLD = 0;  // 活跃寄存器将从影子寄存器装载新值
    //EPwm1Regs.TBCTL.bit.SWFSYNC = 0;//设置 EPwm1Regs.TBCTL.bit.SWFSYNC = 1，以使软件同步功能启用
    //EPwm1Regs.TBCTL.bit.SYNCOSEL = 0;   //禁用同步输出适用于那些不需要与其他ePWM模块同步的应用，每个ePWM模块可以独立地控制其输出
    EPwm1Regs.TBPRD=1000;              //确定  PWM 的输出频率  1000

   //EPwm1Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
    EPwm1Regs.CMPCTL.bit.LOADAMODE = 2; //计数比较寄存器 A （CMPA） 从影子寄存器装载模式选择位
    EPwm1Regs.CMPCTL.bit.LOADBMODE = 2; // 计数比较寄存器 B （CMPB） 从影子寄存器装载模式选择位
  //  EPwm1Regs.CMPCTL.bit.SHDWAFULL    //判断影子寄存器是否加载上去
   // EPwm1Regs.CMPCTL.bit.SHDWAMODE = 0; // 影子模式  1立即装载模式
   // EPwm1Regs.CMPCTL.bit.SHDWBFULL    //判断影子寄存器是否加载上去
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = 0; // 影子模式  1立即装载模式
    EPwm1Regs.CMPA.half.CMPA =500;      //时基计数器 （TBCTR） 的值相比较
    EPwm1Regs.CMPB=500;                 //时基计数器 （TBCTR） 的值相比较

   //EPwm1Regs.AQCTLA.all=0x60;        // EPWMxA = 1 when CTR=CMPA and counter inc
                                      // EPWMxA = 0 when CTR=CMPA and counter dec
    //EPwm1Regs.AQCTLA.bit.CAD = 2;              //且数器处于减计数时， 控制输出动作
    EPwm1Regs.AQCTLA.bit.CAU = 2;             //且数器处于增计数时， 控制输出动作 2为高电平
    //EPwm1Regs.AQCTLA.bit.CBD = 2;            //且计器处于减计数时， 控制输出动作
    //EPwm1Regs.AQCTLA.bit.CBU = 2;          //且计器处于增计数时， 控制输出动作
   // EPwm1Regs.AQCTLA.bit.PRD             //当时基计数器 （TBCTR） 的值等于周期值时， 控制输出动作
   // EPwm1Regs.AQCTLA.bit.ZRO             // 当时基计数器 （TBCTR） 的值为 0 时， 控制输出动作
   //EPwm1Regs.AQCTLB.all=0x60;
    //EPwm1Regs.AQCTLB.bit.CAD = 2;               //且数器处于减计数时， 控制输出动作
    //EPwm1Regs.AQCTLB.bit.CAU = 2;              //且数器处于增计数时， 控制输出动作
    //EPwm1Regs.AQCTLB.bit.CBD = 2;              //且计器处于减计数时， 控制输出动作
    EPwm1Regs.AQCTLB.bit.CBU = 2;              //且计器处于增计数时， 控制输出动作
   // EPwm1Regs.AQCTLB.bit.PRD
   // EPwm1Regs.AQCTLB.bit.ZRO
   //EPwm1Regs.AQSFRC.all=0;
    //EPwm1Regs.AQSFRC.bit.ACTSFA = 0;           //当 A 输出一次软件强制事件发生时， 对动作的影响
    //EPwm1Regs.AQSFRC.bit.ACTSFB = 0;           //当 B 输出一次软件强制事件发生时， 对动作的影响
   // EPwm1Regs.AQSFRC.bit.OTSFA = 0;            //A 输出一次软件强制事件
   // EPwm1Regs.AQSFRC.bit.OTSFB = 0;            //B 输出一次软件强制事件
   // EPwm1Regs.AQSFRC.bit.RLDCSF = 2;           // AQSFRC 寄存器从影子寄存器重新装载值
   //EPwm1Regs.AQCSFRC.all=0;
   // EPwm1Regs.AQCSFRC.bit.CSFA = 0;            //连续软件强制事件输出
  //  EPwm1Regs.AQCSFRC.bit.CSFB = 0;            //连续软件强制事件输出


/*
    //EPwm1Regs.DBCTL.all=0x23;          // EPWMxB is inverted 0000 0000 0010 0011
    EPwm1Regs.DBCTL.bit.IN_MODE = 2;         // 死区输入模式控制位
    EPwm1Regs.DBCTL.bit.OUT_MODE = 3;         //死区输出模式控制位
    EPwm1Regs.DBCTL.bit.POLSEL = 0;           //极性选择控制位
    EPwm1Regs.DBRED = 100;                      //位 9 ～ 0， DEL： 该位域设置上升沿延迟时间
    EPwm1Regs.DBFED = 100;                     //位 9 ～ 0， DEL： 该位域设置上升沿延迟时间
*/
    /*
    //EPwm1Regs.TZSEL.all=0;
    EPwm1Regs.TZSEL.bit.CBC1 = 0; // 1： 使能TZ1作为重复脱开信号源， 即当TZ1引脚为低时， 表示发生重复脱开
    EPwm1Regs.TZSEL.bit.OSHT1 = 0;// 1： 使能TZ1作为单发脉冲脱开信号源， 即当TZ1 引脚为低时， 表示发生单发脉冲脱开

    //EPwm1Regs.TZCTL.all=0;
    EPwm1Regs.TZCTL.bit.TZA = 0;//当一个脱开事件发生时， 如何控制 ePWMxB 输出信号， 哪个引脚产生的 脱开事件由 TZSEL 寄存器确定
    EPwm1Regs.TZCTL.bit.TZB = 0;// 当一个脱开事件发生时， 如何控制 ePWMxA 输出信号， 哪个引脚产生的 脱开事件由 TZSEL 寄存器确定
    //EPwm1Regs.TZEINT.all=0;
    EPwm1Regs.TZEINT.bit.CBC = 0;//重复脱开保护中断使能位
    EPwm1Regs.TZEINT.bit.OST = 0;//单发脉冲脱开保护中断使能位
    //EPwm1Regs.TZFLG.all=0;
    EPwm1Regs.TZFLG.bit.CBC = 0;
    EPwm1Regs.TZFLG.bit.INT = 0;
    EPwm1Regs.TZFLG.bit.OST = 0;
    //EPwm1Regs.TZCLR.all=0;
    EPwm1Regs.TZCLR.bit.CBC = 0;//写 1 清重复脱开事件标志位
    EPwm1Regs.TZCLR.bit.INT = 0;//写1 清 INT 中断标志位
    EPwm1Regs.TZCLR.bit.OST = 0;//写 1 清单发脉冲脱开事件标志位
    //EPwm1Regs.TZFRC.all=0;
    EPwm1Regs.TZFRC.bit.CBC = 0;//写 1 强制一个重复脱开事件产生， 同时置位重复脱开事件标志位
    EPwm1Regs.TZFRC.bit.OST = 0;//写 1 强制一个单发脉冲脱开事件产生
*/
    //EPwm1Regs.ETSEL.all=0;            // Interrupt when TBCTR = 0x0000
    EPwm1Regs.ETSEL.bit.INTEN = 1;//ePWM 中断使能位
    EPwm1Regs.ETSEL.bit.INTSEL = 4;// 产生中断选择位
   // EPwm1Regs.ETSEL.bit.SOCAEN = 0;//当有 ePWMxSOCA 脉冲时启动 A - D 转换。
   // EPwm1Regs.ETSEL.bit.SOCASEL = 0;//ePWMxSOCA 脉冲选择位
   // EPwm1Regs.ETSEL.bit.SOCBEN = 0;//当有 ePWMxSOCB 脉冲时启动 A - D 转换。
   // EPwm1Regs.ETSEL.bit.SOCBSEL = 0;//ePWMxSOCB 脉冲选择位
    //EPwm1Regs.ETFLG.all=0;
    EPwm1Regs.ETFLG.bit.INT = 0;//中断标志位
   // EPwm1Regs.ETFLG.bit.SOCA = 0;//ePWMxSOCA 启动 A - D 转换标志位
   // EPwm1Regs.ETFLG.bit.SOCB = 0;//ePWMxSOCB 启动 A - D 转换标志位
    //EPwm1Regs.ETCLR.all=0;
   EPwm1Regs.ETCLR.bit.INT = 0;// 中断标志清除位
   // EPwm1Regs.ETCLR.bit.SOCA = 0;//ePWMxSOCA 启动 A - D 转换标志清除位
   // EPwm1Regs.ETCLR.bit.SOCB = 0;//ePWMxSOCB 启动 A - D 转换标志清除位
    //EPwm1Regs.ETFRC.all=0;
   // EPwm1Regs.ETFRC.bit.INT = 0;//强制ePWM 中断使能位
   // EPwm1Regs.ETFRC.bit.SOCA = 0;//强制当有 ePWMxSOCA 脉冲时启动 A - D 转换。
   // EPwm1Regs.ETFRC.bit.SOCB = 0;//强制当有 ePWMxSOCA 脉冲时启动 A - D 转换。

   //EPwm1Regs.PCCTL.all=0;         //不怎么使用斩波
    EPwm1Regs.PCCTL.bit.CHPDUTY = 0;//斩波时钟占空比
    EPwm1Regs.PCCTL.bit.CHPEN = 0;//斩波使能位
    EPwm1Regs.PCCTL.bit.CHPFREQ = 0;//斩波时钟频率
    EPwm1Regs.PCCTL.bit.OSHTWTH = 0;//单发脉冲宽度

    EDIS;

}
