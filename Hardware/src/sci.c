/*
 * sci.c
 *
 *  Created on: 2023年12月1日
 *      Author: ych
 */

#include <sci.h>

static Sci_Gpio_Init(void){
    EALLOW;

 /* Enable internal pull-up for the selected pins */
 // Pull-ups can be enabled or disabled disabled by the user.
 // This will enable the pullups for the specified pins.

     GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;    // Enable pull-up for GPIO28 (SCIRXDA)
     GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;    // Enable pull-up for GPIO29 (SCITXDA)

 //  GpioCtrlRegs.GPBPUD.bit.GPIO36 = 0;    // Enable pull-up for GPIO36 (SCIRXDA)
 //  GpioCtrlRegs.GPBPUD.bit.GPIO35 = 0;    // Enable pull-up for GPIO35 (SCITXDA)

 /* Set qualification for selected pins to asynch only */
 // Inputs are synchronized to SYSCLKOUT by default.
 // This will select asynch (no qualification) for the selected pins.

     GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;  // Asynch input GPIO28 (SCIRXDA)
 //  GpioCtrlRegs.GPBQSEL1.bit.GPIO36 = 3;  // Asynch input GPIO28 (SCIRXDA)

 /* Configure SCI-A pins using GPIO regs*/
 // This specifies which of the possible GPIO pins will be SCI functional pins.

     GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;   // Configure GPIO28 for SCIRXDA operation
     GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;   // Configure GPIO29 for SCITXDA operation
 //  GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 1;   // Configure GPIO36 for SCIRXDA operation
 //  GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 1;   // Configure GPIO35 for SCITXDA operation
     EDIS;
}

void Sci_Init(void){

    Sci_Gpio_Init();

    SciaRegs.SCICCR.all=0x07;//一个位置停止位 无校验位 8个数据为

    SciaRegs.SCICTL1.all=0x03;//使能 RX TX SCICLK

    #if(CPU_FRQ_150MHZ)
    SciaRegs.SCIHBAUD=0x0001;// 9600 baud @LSPCLK = 37.5MHz.
    SciaRegs.SCILBAUD=0x00E7;
    #endif

    #if(CPU_FRQ_100MHZ)
    SciaRegs.SCIHBAUD=0x0001;// 9600 baud @LSPCLK = 20MHz.
    SciaRegs.SCILBAUD=0x0044;
    #endif

    SciaRegs.SCICTL2.bit.RXBKINTENA=1;//接受数据寄存器使能
    SciaRegs.SCICTL2.bit.TXINTENA=1;//发送数据寄存器使能
    SciaRegs.SCICTL1.bit.SWRESET=1;//SCI 软件复位位

    PieCtrlRegs.PIECTRL.bit.ENPIE=1;//开启PIE中断
    IER|=M_INT9;
    PieCtrlRegs.PIEIER9.bit.INTx1=1;//开启SCIA接受数据中断


}

void scia_xmit(int a){

    while (SciaRegs.SCICTL2.bit.TXRDY == 0) {}//发送缓冲寄存器准备就绪标志位 0表示满了
    SciaRegs.SCITXBUF=a;

}


void scia_msg(char * msg){

    int i;
    i = 0;
    SciaRegs.SCICTL2.bit.TXINTENA=0;//发送缓冲寄存器 （ SCITXBUF） 中断禁用
    while(msg[i] != '\0')
    {
        scia_xmit(msg[i]);
        i++;
    }
    SciaRegs.SCICTL2.bit.TXINTENA=1;//发送缓冲寄存器 （ SCITXBUF） 中断使能

}


