/*
 * timer.c
 *
 *  Created on: 2023年12月1日
 *      Author: ych
 */


#include <timer.h>
#include"DSP2833x_Project.h"
#define CPU_FRQ_150MHZ 1

void CPU_Timer_Config(void){
    InitCpuTimers();//CPU初始化

    /*配置CPU参数*/
    #if(CPU_FRQ_150MHZ)
    //ConfigCpuTimer(&CpuTimer0,150,1000000);//<定时器x,系统时钟频率MHZ，时间us>,1s中断一次
   ConfigCpuTimer(&CpuTimer1,150,500000);//<定时器x,系统时钟频率MHZ，时间us>，0.5s中断一次
   //ConfigCpuTimer(&CpuTimer2,150,100000);//<定时器x,系统时钟频率MHZ，时间us>，0.1s中断一次
    #endif

    #if(CPU_FRQ_100MHZ)
    ConfigCpuTimer(&CpuTimer0,100,1000000);//<定时器x,系统时钟频率MHZ，时间us>,1s中断一次
    ConfigCpuTimer(&CpuTimer1,100,500000);//<定时器x,系统时钟频率MHZ，时间us>，0.5s中断一次
    ConfigCpuTimer(&CpuTimer2,100,100000);//<定时器x,系统时钟频率MHZ，时间us>，0.1s中断一次
    #endif

    /*系统定时器开始运行*/
    //CpuTimer0Regs.TCR.all=0x4001;
   CpuTimer1Regs.TCR.all=0x4001;
   // CpuTimer2Regs.TCR.all=0x4001;

    /*使能CPU级中断*/
    //IER|=M_INT1;//使能CPU的INT1中断
    IER|=M_INT13;//使能CPU的INT13中断
   //IER|=M_INT14;//使能CPU的INT14中断

    /*使能PIE级中断，只需Timer0的，其他不需要*/
    //PieCtrlRegs.PIEIER1.bit.INTx7=1;

}
