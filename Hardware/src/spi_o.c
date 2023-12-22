#include<spi_o.h>
#include"DSP2833x_Project.h"


//flash type
//W25Q64指令表1
#define W25Q64_Write_Enable						          0x06
#define W25Q64_Write_Disable                              0x04
#define W25Q64_Read_Status_register_1				      0x05
#define W25Q64_Read_Status_register_2				      0x35
#define W25Q64_Write_Status_register				      0x01
#define W25Q64_Page_Program							      0x02
#define W25Q64_Quad_Page_Program				          0x32
#define W25Q64_Block_Erase_64KB						      0xD8
#define W25Q64_Block_Erase_32KB						      0x52
#define W25Q64_Sector_Erase_4KB						      0x20
#define W25Q64_Chip_Erase							      0xC7
#define W25Q64_Erase_Suspend					          0x75
#define W25Q64_Erase_Resume							      0x7A
#define W25Q64_Power_down							      0xB9
#define W25Q64_High_Performance_Mode				      0xA3
#define W25Q64_Continuous_Read_Mode_Reset			      0xFF
#define W25Q64_Release_Power_Down_HPM_Device_ID		      0xAB
#define W25Q64_Manufacturer_Device_ID				      0x90
#define W25Q64_Read_Uuique_ID						      0x4B
#define W25Q64_JEDEC_ID								      0x9F
#define W25Q64_DUMMY_BYTE                                 0x0000

//W25Q64指令集表2(读指令)
#define W25Q64_Read_Data						          0x03
#define W25Q64_Fast_Read						          0x0B
#define W25Q64_Fast_Read_Dual_Output				      0x3B
#define W25Q64_Fast_Read_Dual_IO					      0xBB
#define W25Q64_Fast_Read_Quad_Output				      0x6B
#define W25Q64_Fast_Read_Quad_IO					      0xEB
#define W25Q64_Octal_Word_Read_Quad_IO				      0xE3


void w25q64_gpio_init(){

    EALLOW;
    //初始化为上拉
    GpioCtrlRegs.GPBPUD.bit.GPIO54 = 0;   // Enable pull-up on GPIO54 (SPISIMOA)
     GpioCtrlRegs.GPBPUD.bit.GPIO55 = 0;   // Enable pull-up on GPIO55 (SPISOMIA)
     GpioCtrlRegs.GPBPUD.bit.GPIO56 = 0;   // Enable pull-up on GPIO56 (SPICLKA)
     GpioCtrlRegs.GPBPUD.bit.GPIO57 = 0;   // Enable pull-up on GPIO57 (SPISTEA)

     //配置为GPIO模式
     GpioCtrlRegs.GPBQSEL2.bit.GPIO54 = 3; // Asynch input GPIO54 (SPISIMOA)
     GpioCtrlRegs.GPBQSEL2.bit.GPIO55 = 3; // Asynch input GPIO55 (SPISOMIA)
     GpioCtrlRegs.GPBQSEL2.bit.GPIO56 = 3; // Asynch input GPIO56 (SPICLKA)
     GpioCtrlRegs.GPBQSEL2.bit.GPIO57 = 3; // Asynch input GPIO57 (SPISTEA)


     GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 0; // Configure GPIO54 as SPISIMOA
     GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 0; // Configure GPIO55 as SPISOMIA
     GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 0; // Configure GPIO56 as SPICLKA
     GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0; // Configure GPIO57 as SPISTEA

     GpioCtrlRegs.GPBDIR.bit.GPIO54 = 1;  // GPIO1设置为输出
      GpioCtrlRegs.GPBDIR.bit.GPIO55 = 0;  // GPIO1设置为输入
       GpioCtrlRegs.GPBDIR.bit.GPIO56 = 1;  // GPIO1设置为输出
        GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;  // GPIO1设置为输出
    EDIS;

    //SCL

    //MOSI

    //MISO INPUT

    //CS

}

/*******************************************************************************
 * 函数名：SPI_CS_HIGH
 * 描述  ：CS输出高电平
 * 输入  ：void
 * 输出  ：void
 * 调用  ：内部调用
 * 备注  ：
 *******************************************************************************/
void SPI_CS_HIGH(void)
{
    GpioDataRegs.GPBSET.bit.GPIO57=1;
}

/*******************************************************************************
 * 函数名：SPI_CS_LOW
 * 描述  ：CS输出低电平
 * 输入  ：void
 * 输出  ：void
 * 调用  ：内部调用
 * 备注  ：
 *******************************************************************************/
void SPI_CS_LOW(void)
{
    GpioDataRegs.GPBCLEAR.bit.GPIO57=1;
}

/*******************************************************************************
 * 函数名：SPI_SLK_HIGH
 * 描述  ：SLK输出高电平
 * 输入  ：void
 * 输出  ：void
 * 调用  ：内部调用
 * 备注  ：
 *******************************************************************************/
void SPI_SLK_HIGH(void)
{
    GpioDataRegs.GPBSET.bit.GPIO56=1;
}

/*******************************************************************************
 * 函数名：SPI_SLK_LOW
 * 描述  ：SLK输出低电平
 * 输入  ：void
 * 输出  ：void
 * 调用  ：内部调用
 * 备注  ：
 *******************************************************************************/
void SPI_SLK_LOW(void)
{
    GpioDataRegs.GPBCLEAR.bit.GPIO56=1;
}

/*******************************************************************************
 * 函数名：SPI_SLK_HIGH
 * 描述  ：SI输出高电平
 * 输入  ：void
 * 输出  ：void
 * 调用  ：内部调用
 * 备注  ：
 *******************************************************************************/
void SPI_DI_HIGH(void)
{
    GpioDataRegs.GPBSET.bit.GPIO54=1;
}

/*******************************************************************************
 * 函数名：SPI_DI_LOW
 * 描述  ：SI输出低电平
 * 输入  ：void
 * 输出  ：void
 * 调用  ：内部调用
 * 备注  ：
 *******************************************************************************/
void SPI_DI_LOW(void)
{
    GpioDataRegs.GPBCLEAR.bit.GPIO54=1;
}

/*******************************************************************************
 * 函数名：Read_DO_Level
 * 描述  ：读取DO电平
 * 输入  ：void
 * 输出  ：void
 * 调用  ：内部调用
 * 备注  ：
 *******************************************************************************/
Uint16 Read_DO_Level(void)
{
    return GpioDataRegs.GPBDAT.bit.GPIO55;
}

/*******************************************************************************
 * 函数名：SPI_Send_2Byte
 * 描述  ：SPI读取数据
 * 输入  ：data
 * 输出  ：Outdata
 * 调用  ：内部调用
 * 备注  ：通过发送两个个字节的数据并同时接收两个字节的数据，实现了数据的读取操作。
 *******************************************************************************/
Uint16 SPI_Send_2Byte(Uint16 data)
{
	Uint16 i, Outdata = 0x0000;	
	for (i = 0; i < 16; i ++)
	{
		if(data & (0x8000 >> i))//在下降沿,把数据移到MOSI总线上
		{
			SPI_DI_HIGH();
		}
		else
		{
			SPI_DI_LOW();
		}		
		SPI_SLK_HIGH();	// 上升沿读取数据
		DELAY_US(10);
		if (Read_DO_Level())
		{
		    DELAY_US(10);
			Outdata |= (0x8000 >> i);	//掩码提取数据
		}
		SPI_SLK_LOW();	// 下降沿
	}	
	return Outdata;
}


/*******************************************************************************
 * 函数名：W25Q64_WriteEnable
 * 描述  ：W25Q64发送写使能命令
 * 输入  ：void
 * 输出  ：void
 * 调用  ：内部调用
 * 备注  ：
 *******************************************************************************/
void W25Q64_WriteEnable(void)
{
	SPI_CS_LOW();
    Uint16 data;
    data = W25Q64_Write_Enable;
    data <<= 8;
	SPI_Send_2Byte(data);
	SPI_CS_HIGH();
}

/*******************************************************************************
 * 函数名：W25Q64_WriteDisable
 * 描述  ：W25Q64发送禁止写使能命令
 * 输入  ：void
 * 输出  ：void
 * 调用  ：内部调用
 * 备注  ：
 *******************************************************************************/
void W25Q64_WriteDisable(void)
{
	SPI_CS_LOW();
    Uint16 data;
    data = W25Q64_Write_Disable;
    data <<= 8;
	SPI_Send_2Byte(data);
	SPI_CS_HIGH();
}

/*******************************************************************************
 * 函数名：W25Q64_WaitForBusyStatus
 * 描述  ：等待 W25Q64 芯片的忙状态结束
 * 输入  ：void
 * 输出  ：void
 * 调用  ：内部调用
 * 备注  ：
 *******************************************************************************/
void W25Q64_WaitForBusyStatus(void)
{
    Uint16 Timeout = 0xFFFF;
    SPI_CS_LOW();
    Uint16  data;
    data = W25Q64_Read_Status_register_1;
    data <<= 8;
    SPI_Send_2Byte(W25Q64_Read_Status_register_1);    
    while (Timeout > 0)
    {
        Uint16 status = SPI_Send_2Byte(W25Q64_DUMMY_BYTE);
        if ((status & 0x01) == 0)// 检查忙状态是否结束
        {
            break;
        }    
        Timeout--;
        if (Timeout == 0)
        {
            //printf("W25Q64 ERROR \r\n");
            break;
        }
    }    
    SPI_CS_HIGH();
}


/*******************************************************************************
 * 函数名：W25Q64_SectorErase
 * 描述  ：扇区擦除函数
 * 输入  ：Sector_Address		
 * 输出  ：void
 * 调用  ：内部调用
 * 备注  ：在执行写入操作前要进行擦除
 *******************************************************************************/
void W25Q64_SectorErase(Uint32 Sector_Address)
{
	W25Q64_WriteEnable();//W25Q64写使能
	SPI_CS_LOW();
    Uint16 data;
    data = W25Q64_Sector_Erase_4KB;
    data <<= 8;
	SPI_Send_2Byte(W25Q64_Sector_Erase_4KB);
	SPI_Send_2Byte(Sector_Address >> 16);//24位扇区地址
	SPI_Send_2Byte(Sector_Address >> 8);
	SPI_Send_2Byte(Sector_Address);
	SPI_CS_HIGH();
	W25Q64_WaitForBusyStatus();//等待 W25Q64 芯片的忙状态结束
	DELAY_US(10);
	W25Q64_WriteDisable();//W25Q64禁止写使能
}


/*******************************************************************************
 * 函数名：W25Q64_BlockErase
 * 描述  ：块擦除函数
 * 输入  ：Address		
 * 输出  ：void
 * 调用  ：内部调用
 * 备注  ：在执行写入操作前要进行擦除
 *******************************************************************************/
//void W25Q64_BlockErase(uint32_t Block_Address)
//{
//    W25Q64_WriteEnable();//W25Q64写使能
//    SPI_CS_LOW();
//    SPI_Send_2Byte(W25Q64_Block_Erase_64KB);
//    SPI_Send_2Byte((Block_Address & 0xFF0000) >> 16);//24位扇区地址
//    SPI_Send_2Byte((Block_Address & 0xFF00) >> 8);
//    SPI_Send_2Byte(Block_Address & 0xFF);
//    SPI_CS_HIGH();
//    W25Q64_WaitForBusyStatus();//等待 W25Q64 芯片的忙状态结束
//	W25Q64_WriteDisable();//W25Q64禁止写使能
//}


/*******************************************************************************
 * 函数名：W25Q64_ChipErase
 * 描述  ：芯片擦除函数
 * 输入  ：void		
 * 输出  ：void
 * 调用  ：内部调用
 * 备注  ：在执行写入操作前要进行擦除
 *******************************************************************************/
void W25Q64_ChipErase(void)
{
    W25Q64_WriteEnable();//W25Q64写使能	 
    SPI_CS_LOW();
    Uint16 data ;
    data = W25Q64_Chip_Erase;
    data <<= 8;
    SPI_Send_2Byte(data);
    SPI_CS_HIGH();
    W25Q64_WaitForBusyStatus();//等待 W25Q64 芯片的忙状态结束	
	W25Q64_WriteDisable();//W25Q64禁止写使能
}


/*******************************************************************************
 * 函数名：W25Q64_PageProgram
 * 描述  ：W25Q64页写操作
 * 输入  ：address	 要写入的起始地址
           *data	 要写入的数据缓冲区指针。
           dataSize	 要写入的数据大小，单位为字节。
 * 输出  ：void
 * 调用  ：内部调用
 * 备注  ：
 *******************************************************************************/
void W25Q64_PageWrite(Uint32 address, Uint16 *data, Uint16 dataSize)
{
  W25Q64_WriteEnable();//W25Q64写使能
  SPI_CS_LOW();
  Uint16 Data;
  Data = W25Q64_Page_Program;
  Data <<= 8;
  Data |= ((address & 0xFF0000) >> 16);
  SPI_Send_2Byte(Data);
Uint16 addr;
addr = ((address & 0xFF00) >> 8)| (address & 0xFF);
  SPI_Send_2Byte(addr);
  Uint16 i;
  if (dataSize <= 128)
  {
      for (i = 0; i < dataSize; i++)
      {
          SPI_Send_2Byte(data[i]);
      }
  }
  else
  {
      for (i = 0; i < 128; i++)
      {
          SPI_Send_2Byte(data[i]);
      }
      dataSize -= 128;
      data += 128;
      // 写入剩余数据
      W25Q64_WaitForBusyStatus();//等待 W25Q64 芯片的忙状态结束
      address += 128;
      while (dataSize > 0)
      {
          W25Q64_WriteEnable();//W25Q64写使能
          SPI_CS_LOW();
          Uint16 Data;
          Data = W25Q64_Page_Program;
          Data <<= 8;
          Data |= ((address & 0xFF0000) >> 16);
          SPI_Send_2Byte(Data);
            Uint16 addr;
            addr = ((address & 0xFF00) >> 8)|(address & 0xFF);
          SPI_Send_2Byte(addr);
          Uint16 chunkSize = (dataSize > 128) ? 128 : dataSize;
          for (i = 0; i < chunkSize; i++)
          {
              SPI_Send_2Byte(data[i]);
          }
          SPI_CS_HIGH();
          W25Q64_WaitForBusyStatus();//等待 W25Q64 芯片的忙状态结束
          dataSize -= chunkSize;
          data += chunkSize;
          address += chunkSize;
      }
  }
  SPI_CS_HIGH();
  //W25Q64_WaitForBusyStatus();//等待 W25Q64 芯片的忙状态结束
	DELAY_US(10);
  W25Q64_WriteDisable();//W25Q64禁止写使能
}

/*******************************************************************************
 * 函数名：W25Q64_ReadData
 * 描述  ：W25Q64读数据
 * 输入  ：void
 * 输出  ：void
 * 调用  ：
 * 备注  ：
 *******************************************************************************/
void W25Q64_ReadData(Uint32 address, Uint16 *Data, Uint16 dataSize)
{
	Uint16 i;
   SPI_CS_LOW();
   Uint16 data;
   data = W25Q64_Read_Data;
   data <<= 8;
   data |= ((address & 0xFF0000) >> 16);
   SPI_Send_2Byte(data);
   Uint16 addr;
   addr = ((address & 0xFF00) >> 8)| (address & 0xFF);
   SPI_Send_2Byte(addr);
   for (i = 0; i < dataSize; i++)
   {
       Data[i] = SPI_Send_2Byte(W25Q64_DUMMY_BYTE);
   }
   SPI_CS_HIGH();
}

/*******************************************************************************
 * 函数名：W25Q64_ReadID
 * 描述  ：W25Q64读取设备ID
 * 输入  ：*ID	存储ID的变量	
 * 输出  ：void
 * 调用  ：内部调用
 * 备注  ：EF4017
 *******************************************************************************/
// void W25Q64_ReadID(Uint32 *ID)
// {
//     SPI_CS_LOW();
//     Uint16 data;
//     data = W25Q64_JEDEC_ID;
//     data <<= 8;

//     Uint16 x[4];
//     x[0] = SPI_Send_2Byte(W25Q64_JEDEC_ID);         // 读ID号指令
//     *ID <<= 16;
//     x[1] = SPI_Send_2Byte(W25Q64_DUMMY_BYTE); // 厂商ID，默认为0xEF
//     *ID <<= 16;
//     x[2]= SPI_Send_2Byte(W25Q64_DUMMY_BYTE); // 设备ID，表示存储类型，默认为0x40
//     *ID <<= 16;
//     x[3]= SPI_Send_2Byte(W25Q64_DUMMY_BYTE); // 设备ID，表示容量，默认为0x17
//     SPI_CS_HIGH();
// }






