/*
 * main.c
 *
 *  Created on: 2023年12月1日
 *      Author: ych
 */

#include <button.h>
#include <epwm_led.h>
#include <led.h>
#include <sci.h>
#include <WatchDog.h>
#include<oled.h>
#include"DSP2833x_Project.h"
#include<spi_o.h>
#include<w25q64.h>
#include<stdio.h>


#define true 1
#define false 0
#define MAX_BUFFER_SIZE 256

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;

Uint16 PWM_count=0;
Uint16 PWM_direction=0;

#define BufferSize 			16
 
 
#define  FLASH_WriteAddress     0x00000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress
 
void arr_split(Uint32 *p,Uint32 *q);
 
//发送缓冲区初始化
Uint32 Tx_Buffer[BufferSize] = {0};
Uint32 Rx_Buffer[BufferSize] = {0};
 
//Uint32 Tx_Buffer1[BufferSize*2] = {0};
//Uint32 Rx_Buffer1[BufferSize*2] = {0};
 
 
Uint32 DeviceID=0;
Uint32 FlashID=0;




/**
*   @brief                   1ms延迟函数
 *  @parameter                  t
 *  @return_value               无
 */
//void delay_1ms(Uint32 t){
//
//    while(t--){
//
//        DELAY_US(1000);
//
//    }
//
//}

//interrupt void xint1_isr(void){
//
//    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;//清除第一组所有得中断  不接受其他中断来
//    DELAY_US(1000);//消抖
//    if(GpioDataRegs.GPADAT.bit.GPIO13 == 0){
//
//        //操作
//
//    }
//
//}

//定时器0中断服务子程序
//interrupt void cpu_timer0_isr(void);{
//
//    //操作
//
//    CpuTimer0.InterruptCount++;
//    PieCtrlRegs.PIEACK.all=PIEACK_GROUP1;//中断已应答，可以从组1接收更多中断
//
//}

////定时器1中断服务子程序
//interrupt void cpu_timer1_isr(void){
//
//    //操作
//
//    CpuTimer1.InterruptCount++;
//    EDIS;//直接确认，无需PIE
//
//}
//interrupt void cpu_timer2_isr(void);//定时器2中断服务子程序

Uint32 WakeCount;
Uint32 LoopCount;

interrupt void wakeint_isr(void)
{
    WakeCount++;

    // Acknowledge this interrupt to get more from group 1
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

//BRR = LSPCLK/(BAUD × 8)-1波特率

Uint16 ReceivedData[MAX_BUFFER_SIZE];
Uint16 DataLength = 0;
Uint16 DataIndex = 0;
Uint16 IsReceiving = false;


/**
 * 接受的格式为16进制  如：01 11  发送11
 *                  如: 02  11 22 发送 11 22
 *
 *
 */
interrupt void scia_Rx_isr()
{
    Uint16 receivedByte = SciaRegs.SCIRXBUF.all;

    if (!IsReceiving)
    {
        // 接收数据长度
        DataLength = receivedByte;
        DataIndex = 0;
        IsReceiving = true;
    }
    else
    {
        // 接收实际数据
        ReceivedData[DataIndex++] = receivedByte;

        if (DataIndex >= DataLength)
        {
            // 接收完全
            IsReceiving = false;
        }
    }

    // 清除中断标志
    SciaRegs.SCIFFRX.bit.RXFFOVRCLR = 1;
    SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;

    PieCtrlRegs.PIEACK.all = M_INT9;
}


interrupt void epwm1_isr(void){


    if(EPwm1Regs.TBSTS.bit.CTRDIR==1)//当前PWM_count方向是否=0 (=0表示向上，=1表示向下)
    {
        PWM_count=PWM_count+50;//PWM_count是用于下面更新CMPA比较值。 增加PWM_count
    }else{
        PWM_count=PWM_count-50;
    }

   EPwm1Regs.CMPA.half.CMPA = PWM_count;//更新EPWM1的CMPA比较值

   // 清除中断标志位，防止重复进入中断
   EPwm1Regs.ETCLR.bit.INT = 1;

   // 应答此中断，使得从第3组接收更多其他中断（关于中断第几组，具体查看中断向量表）
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}


//__interrupt void i2c_int1a_isr(void)     // I2C-A
//{
//   Uint16 IntSource;
//
//   // Read interrupt source
//   IntSource = I2caRegs.I2CISRC.bit.INTCODE & 0x7;  //获取IIC中断的状态
//
// Uint32  Interrupts_Mask= IER;//只保存实时操作系统的这个定时器中断  把其他的中断全部挂起
//   IER &= 0x2000;
//   IFR &= ~0x2000;
//   EINT;  //可屏蔽中断使能
//
//   switch(IntSource)
//      {
//          case I2C_NO_ISRC:   // =0  无
//              break;
//
//          case I2C_ARB_ISRC:  // =1 仲裁丢失
//              break;
//
//          case I2C_NACK_ISRC: // =2 检测到不应答条件
//              break;
//
//          case I2C_ARDY_ISRC: // =3 寄存器存取准备完成
//              break;
//
//          case I2C_RX_ISRC:   // =4 接收数据准备完成
//              break;
//
//          case I2C_TX_ISRC:   // =5 发送数据准备完成
//              break;
//
//          case I2C_SCD_ISRC:  // =6 检测到停止(STOP)条件
//              break;
//
//          case I2C_AAS_ISRC:  // =7   //设置为从
//              break;
//
//      }
//
//   DINT;//开启中断
//   IER = Interrupts_Mask;//恢复之前的中断标志
//
//   // Enable future I2C (PIE Group 8) interrupts
//   PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
//}



void main(void){


//第一步初始化系统时钟
    InitSysCtrl();

//第二部初始化GPIO口
   // InitGpio();

//第三步清除所有中断和初始化PIE中断向量表
    DINT;//禁用CPU中断
    InitPieCtrl();//PIE 控制寄存器到默认状态
    IER = 0X0000;
    IFR = 0X0000;
    InitPieVectTable();//初始化中断向量表 初始化

//注册中断程序入口

#if 0
    MemCopy(&RamfuncsLoadStart,&RamfuncsLoadEnd,&RamfuncsRunStart);
    InitFlash();
#endif

//第四步初始化外设
    //InitPeripherals();

//用户功能配置

    //led
    //LED_GPIO_Config();


    //button  exteral interrupt
//    EALLOW;
//    PieVectTable.XINT1 = &xint1_isr;
//    EDIS;
//
//    EALLOW;
//    PieVectTable.SCIRXINTA=&scia_Rx_isr;
//    EDIS;
//
//    IER |=  M_INT1;//使能第一组中断
//    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;//使能总中断
//    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;//使能第一组第四个
//
//    EINT;//中断使能
//    ERTM;//使能总时钟中断
//
//
//
//    //button 3*4
//    Button_Matrix_Config();
//
//
//
//    //timer
//    EALLOW;
//    //PieVectTable.TINT0=&cpu_timer0_isr;//将定时器中断服务子程序的地址存放到相应的向量地址中
//    PieVectTable.XINT13=&cpu_timer1_isr;
//    //PieVectTable.TINT2=&cpu_timer2_isr;
//    EDIS;
//
//
//    //WatchDog
//    InitWatchDog();
//
//
//    //sci
//    Sci_Init();


    //epwm  fan led

//    EALLOW;
//    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
//    SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;     // 使能ePWM1
//    EDIS;
//
//    EALLOW;
//    PieVectTable.EPWM1_INT = &epwm1_isr;
//    EDIS;
//
//    EALLOW;
//    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;//停止时基计时器
//    EDIS;
//
//    epwm1_led_init();
//
//    EALLOW;
//    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;//开启时基计时器
//    EDIS;
//
//    IER |= M_INT3;//使能INT3中断
//    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;//使能ePWM1中断
//
//    EINT;
//    ERTM;


    //ecap distance_measurement  cap_graph


    //i2c

   //AT42C0x
//    Uint16 temp1[2];
//
//    Uint8 temp2[2]={0X7,0x8};
//
//    At24c01_Init();
//
//    WriteData(0x50,0X00,2,temp2);
//
//    ReadData(0x50,0X00,2,temp1);

    //oled


//    OLED_Init();
//
//    OLED_ON();
//
//    OLED_Fill(0);
//
//    unsigned char ch[] = "1234567";
//
//    OLED_ShowStr(1,1,ch,1);
//    DELAY_US(100000);
//    OLED_ShowStr(1,2,ch,2);


    //adc-DMA  Temp Lighting



    //spi  oled  flash

    Uint16 i=0;
 
	SPI_Init();
 
 
	/* 获取 Flash Device ID */
	DeviceID = FLASH_ReadDeviceID();
	DELAY_US(20);
	FlashID = FLASH_ReadJEDE_ID();

	for(i=0;i<BufferSize;i++)
	{
		Tx_Buffer[i]=1;
		Rx_Buffer[i]=0;
	}
 
	if(FlashID == SPI_FLASH_ID)
	{
		//扇区擦除
		FLASH_32KErase(FLASH_SectorToErase);

		SPI_FLASH_BufferWrite(Tx_Buffer,FLASH_WriteAddress,BufferSize);

		SPI_FLASH_BufferRead(Rx_Buffer,FLASH_ReadAddress,BufferSize);

	}



}



