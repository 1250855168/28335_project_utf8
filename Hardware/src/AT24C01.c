///*
// * AT24C01.c
// *
// *  Created on: 2023年12月13日
// *      Author: ych
// */
//
#include <AT24C01.h>
#include<DSP2833x_I2c.h>

#define READ_CMD                1
#define WRITE_CMD               0

#define x24C01//器件名称，x24C01或x24C02

#define DEV_ADDR                0xA0                    //设备硬件地址

#ifdef x24C01
    #define PAGE_NUM            16                      //页数
    #define PAGE_SIZE           8                       //页面大小(字节)
    #define CAPACITY_SIZE       (PAGE_NUM * PAGE_SIZE)  //总容量(字节)
    #define ADDR_BYTE_NUM       1                       //地址字节个数
#endif

#ifdef x24C02
    #define PAGE_NUM            32                      //页数
    #define PAGE_SIZE           8                       //页面大小(字节)
    #define CAPACITY_SIZE       (PAGE_NUM * PAGE_SIZE)  //总容量(字节)
    #define ADDR_BYTE_NUM       1                       //地址字节个数
#endif


extern __interrupt void i2c_int1a_isr(void);


/**
 *  @brief              AT24C01的初始化
 *  @parameter              无
 *  @return_value           无
 */
void At24c01_Init(){

    EALLOW;
    GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;    // 使能(SDAA)上拉
    GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;    //  使能 (SCLA)上拉
    GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;  // 同步 (SDAA)
    GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;  // 同步 (SCLA)
    GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;   // 配置 GPIO32为 SDAA
    GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;   // 配置GPIO33 为SCLA
    EDIS;

    // Initialize I2C
    I2caRegs.I2CSAR = 0x00A0;        // Slave address - EEPROM control code

    #if (CPU_FRQ_150MHZ)             // Default - For 150MHz SYSCLKOUT
         I2caRegs.I2CPSC.all = 14;   // Prescaler - need 7-12 Mhz on module clk (150/15 = 10MHz)
    #endif
    #if (CPU_FRQ_100MHZ)             // For 100 MHz SYSCLKOUT
      I2caRegs.I2CPSC.all = 9;       // Prescaler - need 7-12 Mhz on module clk (100/10 = 10MHz)
    #endif
    //Master Clock(SCL) = 10MHz / (10 + 5) + (5 + 5) = 400KHz
    I2caRegs.I2CCLKL = 10;           // NOTE: must be non zero
    I2caRegs.I2CCLKH = 5;            // NOTE: must be non zero
                                     // Enable ASS & SCD & ARDY & RRDY interrupts
    I2caRegs.I2CIER.all = 0x6E;      // Enable ASS & SCD & XRDY & ARDY & RRDY & NACK interrupts

    I2caRegs.I2CMDR.all = 0x0020;    // Take I2C out of reset
                                     // Stop I2C when suspended

    I2caRegs.I2CFFTX.all = 0x6000;   // Enable FIFO mode and TXFIFO
    I2caRegs.I2CFFRX.all = 0x2040;   // Enable RXFIFO, clear RXFFINT,

     // Interrupts that are used in this example are re-mapped to
     // ISR functions found within this file.
    EALLOW;  // This is needed to write to EALLOW protected registers
   // PieVectTable.I2CINT1A = &i2c_int1a_isr;
    EDIS;   // This is needed to disable write to EALLOW protected registers

     // Enable I2C interrupt 1 in the PIE: Group 8 interrupt 1
    PieCtrlRegs.PIEIER8.bit.INTx1 = 1;

     // Enable CPU INT8 which is connected to PIE group 8
    IER |= M_INT8;

}
/**
 *  @brief              IIC模块发送准备OK
 *  @parameter              无
 *  @return_value           无
 */
Uint16  I2C_xrdy()
{
    Uint16  t;
    t = I2caRegs.I2CSTR.bit.XRDY;   // IIC模块发送准备OK
    return t;
}



/**
 *  @brief              IIC模块接收准备OK
 *  @parameter              无
 *  @return_value           无
 */
Uint16  I2C_rrdy()
{
    Uint16  t;
    t = I2caRegs.I2CSTR.bit.RRDY;   //  IIC模块接收准备OK
    return t;
}


/**
 *  @brief                  IIC写数据
 *  @parameter              SlaveAddress：从机地址；  RomAddress：寄存器地址； number：写入数据的字节数；  *Wdata：写入数据的地址
 *  @return_value           状态标志
 */
Uint16 WriteData( Uint16 SlaveAddress, Uint16 RomAddress,Uint16 number, Uint8 *Wdata)
{
   Uint16 i;
   if (I2caRegs.I2CSTR.bit.BB == 1)
   {
      return I2C_BUS_BUSY_ERROR;   //返回总线忙错误状态
   }
   while(!I2C_xrdy());      //等待数据发送就绪，XRDY=1,表明发送寄存器已经准备好接受新的数据
   I2caRegs.I2CSAR = SlaveAddress&0xff;    //设备从地址
   I2caRegs.I2CCNT = number + 1;   //需要发送的字节数
   I2caRegs.I2CDXR = RomAddress&0xff;    //第一个发送字节为发送数据的目标寄存器地址
   DELAY_US(1);//等待数据完全赋值到I2CDXR，再使能IIC，必须要加上！
   I2caRegs.I2CMDR.all = 0x6E20;    //发送起始信号，内部数据计数器减到0时，发送停止信号，主机发送模式，使能IIC模式，
   for (i=0; i<number; i++)
   {
      while(!I2C_xrdy());   //等待数据发送就绪，发送下一个数据
      I2caRegs.I2CDXR = *Wdata&0xFF;
      Wdata++;
      if (I2caRegs.I2CSTR.bit.NACK == 1)    //送到无应答信号返回错误
          return    I2C_BUS_BUSY_ERROR;
   }
   return I2C_SUCCESS;         //发送成功
}

void IIC_Ack() {
    // 清除应答位
    I2caRegs.I2CMDR.bit.NACKMOD = 0;
}

void IIC_NoAck() {
    I2caRegs.I2CMDR.bit.NACKMOD = 1;
}



/**
 *  @brief                  IIC写数据
 *  @parameter              SlaveAddress：从机地址；  RomAddress：寄存器地址； number：写入数据的字节数；  *RamAddr：读出数据存放的地址
 *  @return_value           状态标志
 */
Uint16 ReadData( Uint16 SlaveAddress, Uint16 RomAddress,Uint16 number, Uint16  *RamAddr)
{
   Uint16  i,Temp;

   if (I2caRegs.I2CSTR.bit.BB == 1)  //返回总线忙错误状态
   {
       return I2C_BUS_BUSY_ERROR;
   }
   while(!I2C_xrdy());
   I2caRegs.I2CSAR = SlaveAddress&0xff;      //设备从地址
   I2caRegs.I2CCNT = 1;        //发送一个字节为要读取数据的寄存器地址
   I2caRegs.I2CDXR = RomAddress&0xff;
   DELAY_US(1);//等待数据完全赋值到I2CDXR，再使能IIC，必须要加上！
   I2caRegs.I2CMDR.all = 0x6620;   //主机发送模式，自动清除停止位不产生停止信号
   if (I2caRegs.I2CSTR.bit.NACK == 1)    //送到无应答信号返回错误
        return  I2C_BUS_BUSY_ERROR;
   DELAY_US(50);
   while(!I2C_xrdy());                  //调试过程中有时会卡在这一步,表示通讯不正常
   I2caRegs.I2CSAR = SlaveAddress&0xff;
   I2caRegs.I2CCNT = number;
   DELAY_US(10);//等待数据完全赋值到I2CDXR，再使能IIC，必须要加上！
   I2caRegs.I2CMDR.all = 0x6C20;   //主机接受模式，包含有停止信号
   if (I2caRegs.I2CSTR.bit.NACK == 1)
        return  I2C_BUS_BUSY_ERROR;
   for(i=0;i<number;i++)
   {
         //while(!I2C_rrdy());
      Temp = I2caRegs.I2CDRR;
      if(i==number-1){
              IIC_NoAck();
              *RamAddr = Temp;
              RamAddr++;
              return   I2C_SUCCESS;

      }
              IIC_Ack();
//      if (I2caRegs.I2CSTR.bit.NACK == 1)
//          return    I2C_BUS_BUSY_ERROR;
      *RamAddr = Temp;
      RamAddr++;
   }
   return I2C_SUCCESS;    //接受数据成功
}



/**
 *  @brief                 IIC向寄存器的某一位写数据
 *  @parameter              SlaveAddress：从机地址；  regaddress：寄存器地址； bitNum：某一位；  data：写入数据
 *  @return_value           无
 */
void IICwriteBit(Uint16 slaveaddress, Uint16 regaddress, Uint8 bitNum, Uint8 data)
{
    Uint16 a;
    Uint8 b;
    DELAY_US(50);
    ReadData(slaveaddress,regaddress,1,&a);
    b=(Uint8 )(a&0xff);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    DELAY_US(50);
    WriteData(slaveaddress,regaddress,1,&b);
}



/**
 *  @brief                 IIC向寄存器的某几位写数据
 *  @parameter              SlaveAddress：从机地址；  regaddress：寄存器地址； bitStart：开始位；  length：长度；data：写入数据
 *  @return_value           无
 */
void IICwriteBits(Uint16 slaveaddress,Uint16 regaddress,Uint8 bitStart,Uint8 length,Uint8 data)
{

    Uint8 b,mask;
    Uint16 a;
    DELAY_US(50);
    ReadData(slaveaddress,regaddress,1,&a);
    b=(Uint8 )(a&0xff);
    mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
    data <<= (8 - length);
    data >>= (7 - bitStart);
    b &= mask;
    b |= data;
    DELAY_US(50);
    WriteData(slaveaddress,regaddress,1, &b);
}





