#include <spi_o.h>
#include "DSP2833x_Project.h"

/*********************************************
 - W25Q64的操作指令表，MCU通过向W25Q64
  发送以下指令就可以对W25Q64进行以下的操作
*********************************************/
#define W25X_WriteEnable 0x06    // Write Enable
#define W25X_WriteDisable 0x04   // Write Disable
#define W25X_ReadStatusReg1 0x05 // 读状态寄存器1：S7~S0
#define W25X_ReadStatusReg2 0x35 // 读状态寄存器2：S15~S8
#define W25X_WriteStatusReg 0x01 // 写读状态寄存器：BYTE1:S7~S0  BYTE2：S15~S8
#define W25X_PageProgram 0x02    // 单页编程：BYTE1:A23~A16  BYTE2:A15~A8  BYTE3:A7~A0  BYTE4:D7~D0
#define W25X_SectorErase 0x20    // 扇区擦除：4K  BYTE1:A23~A16  BYTE2:A15~A8  BYTE3:A7~A0
#define W25X_BlockErase32K 0x52  // 块擦除：32K  BYTE1:A23~A16  BYTE2:A15~A8  BYTE3:A7~A0
#define W25X_BlockErase64K 0xD8  // 块擦除：64K  BYTE1:A23~A16  BYTE2:A15~A8  BYTE3:A7~A0
#define W25X_ChipErase 0xC7      // 芯片擦除
#define W25X_EraseSuspend 0x75   // 暂停擦除
#define W25X_EraseResume 0x7A    // 恢复擦除
#define W25X_PowerDown 0xB9      // 掉电
#define W25X_ReadData 0x03       // 读数据：BYTE1:A23~A16  BYTE2:A15~A8  BYTE3:A7~A0  BYTE4:D7~D0

#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID 0x9F
// 无效指令数据
#define BYDUMMY 0xFF
/* WIP(busy)标志，FLASH内部正在写入 */
#define WIP_Flag 0x01

#define W25Q64_PAGE_SIZE 256        // 一页的大小，256字节
#define W25Q64_SECTOR_SIZE 4 * 1024 // 扇区大小，字节
#define W25Q64_BLOCK_SIZE 16 * W25Q64_SECTOR_SIZE
#define W25Q16_SIZE 128 * W25Q64_BLOCK_SIZE

#define SPI_MOSI_H GpioDataRegs.GPBSET.bit.GPIO54 = 1
#define SPI_MOSI_L GpioDataRegs.GPBCLEAR.bit.GPIO54 = 1
#define SPI_CLK_H GpioDataRegs.GPBSET.bit.GPIO56 = 1
#define SPI_CLK_L GpioDataRegs.GPBCLEAR.bit.GPIO56 = 1

#define FLASH_CS_SETH GpioDataRegs.GPBSET.bit.GPIO57 = 1
#define FLASH_CS_SETL GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1

#define READ_SPI_MISO GpioDataRegs.GPBDAT.bit.GPIO55

void w25q64_gpio_init()
{

    EALLOW;
    // 初始化为上拉
    GpioCtrlRegs.GPBPUD.bit.GPIO54 = 0; // Enable pull-up on GPIO54 (SPISIMOA)
    GpioCtrlRegs.GPBPUD.bit.GPIO55 = 0; // Enable pull-up on GPIO55 (SPISOMIA)
    GpioCtrlRegs.GPBPUD.bit.GPIO56 = 0; // Enable pull-up on GPIO56 (SPICLKA)
    GpioCtrlRegs.GPBPUD.bit.GPIO57 = 0; // Enable pull-up on GPIO57 (SPISTEA)

    // 配置为GPIO模式
    GpioCtrlRegs.GPBQSEL2.bit.GPIO54 = 3; // Asynch input GPIO54 (SPISIMOA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO55 = 3; // Asynch input GPIO55 (SPISOMIA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO56 = 3; // Asynch input GPIO56 (SPICLKA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO57 = 3; // Asynch input GPIO57 (SPISTEA)

    GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 0; // Configure GPIO54 as SPISIMOA
    GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 0; // Configure GPIO55 as SPISOMIA
    GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 0; // Configure GPIO56 as SPICLKA
    GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0; // Configure GPIO57 as SPISTEA

    GpioCtrlRegs.GPBDIR.bit.GPIO54 = 1; // GPIO1设置为输出
    GpioCtrlRegs.GPBDIR.bit.GPIO55 = 0; // GPIO1设置为输入
    GpioCtrlRegs.GPBDIR.bit.GPIO56 = 1; // GPIO1设置为输出
    GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1; // GPIO1设置为输出
    EDIS;
}

void SPI_Init()
{
    w25q64_gpio_init();

    FLASH_CS_SETH; // 拉高片选信号
    SPI_CLK_L;     // 时钟空闲时为高电平
    SPI_MOSI_H;    // SPI输出管脚设置为高
}

unsigned int SPI_ReadWriteByte(unsigned int data)
{
    unsigned int i = 0;
    unsigned int ReceiveData = 0;

    for (i = 0; i < 8; i++)
    {
        if ((data & 0x80) != 0)
        {
            SPI_MOSI_H;
        }
        else
        {
            SPI_MOSI_L;
        }

        data <<= 1;
        SPI_CLK_L;
        DELAY_US(1);
        SPI_CLK_H;
        ReceiveData <<= 1; // 此处先左移一位是为了防止在接收最后一位时丢失接收第一位的数据
        ReceiveData |= READ_SPI_MISO;
        DELAY_US(1);
    }

    return ReceiveData;
}

Uint32 FLASH_ReadJEDE_ID()
{
    Uint32 JEDE_ID = 0, temp0 = 0, temp1 = 0, temp2 = 0;

    FLASH_CS_SETL;

    SPI_ReadWriteByte(0x9F);
    temp0 = SPI_ReadWriteByte(BYDUMMY);
    temp1 = SPI_ReadWriteByte(BYDUMMY);
    temp2 = SPI_ReadWriteByte(BYDUMMY);

    JEDE_ID = (temp0 << 16) | (temp1 << 8) | temp2;

    FLASH_CS_SETH;
    return JEDE_ID;
}

Uint32 FLASH_ReadDeviceID()
{
    Uint16 DeviceID = 0, temp0 = 0, temp1 = 0;

    FLASH_CS_SETL;
    SPI_ReadWriteByte(0X90);
    SPI_ReadWriteByte(BYDUMMY);
    SPI_ReadWriteByte(BYDUMMY);
    SPI_ReadWriteByte(0x00);

    temp0 = SPI_ReadWriteByte(BYDUMMY);
    temp1 = SPI_ReadWriteByte(BYDUMMY);

    DeviceID = temp0 << 8 | temp1;

    FLASH_CS_SETH;
    return DeviceID;
}

void SPI_FLASH_WriteEnable()
{
    FLASH_CS_SETL;

    SPI_ReadWriteByte(W25X_WriteEnable);

    FLASH_CS_SETH;
}

void SPI_FLASH_WaitForWriteEnd()
{
    Uint16 FLASH_Status = 0;

    FLASH_CS_SETL;

    SPI_ReadWriteByte(0x05);

    do
    {
        FLASH_Status = SPI_ReadWriteByte(BYDUMMY);
    } while ((FLASH_Status & WIP_Flag) == 1);

    FLASH_CS_SETH;
}

// 页写入
void FLASH_PageWrite(Uint32 *pBuffer, Uint32 WriteAddr, Uint16 NumByteToWrite)
{

    SPI_FLASH_WriteEnable();

    FLASH_CS_SETL;

    SPI_ReadWriteByte(W25X_PageProgram);

    SPI_ReadWriteByte((WriteAddr & 0xFF0000) >> 16);
    SPI_ReadWriteByte((WriteAddr & 0xFF00) >> 8);
    SPI_ReadWriteByte(WriteAddr & 0xFF);

    if (NumByteToWrite > W25Q64_PAGE_SIZE)
    {
        NumByteToWrite = W25Q64_PAGE_SIZE;
    }

    while (NumByteToWrite--)
    {
        SPI_ReadWriteByte(*pBuffer);
        pBuffer++;
    }

    FLASH_CS_SETH;

    SPI_FLASH_WaitForWriteEnd();
}

// 不定量写入
void SPI_FLASH_BufferWrite(Uint32 *pBuffer, Uint32 WriteAddr, Uint16 NumByteToWrite)
{
    Uint16 NumOfPage = 0;   // 要写多少整数页
    Uint16 NumOfSingle = 0; // 剩余不满一页的字节数
    Uint16 Addr = 0;        // 判断要写入的地址是不是一个完整页的哪一个位置
    Uint16 count = 0;       // 差count个数据值，刚好可以对齐到页首地址
    Uint16 temp = 0;

    Addr = WriteAddr / W25Q64_PAGE_SIZE;
    NumOfPage = NumByteToWrite / W25Q64_PAGE_SIZE;
    NumOfSingle = NumByteToWrite % W25Q64_PAGE_SIZE;
    count = W25Q64_PAGE_SIZE - Addr;

    if (Addr == 0)
    {
        if (NumOfPage == 0)
        {
            FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
        }
        else
        {
            while (NumOfPage--)
            {
                FLASH_PageWrite(pBuffer, WriteAddr, W25Q64_PAGE_SIZE);
                pBuffer += W25Q64_PAGE_SIZE;
                WriteAddr += W25Q64_PAGE_SIZE;
            }

            FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        }
    }
    else
    {
        if (NumOfPage == 0)
        {
            if (NumOfSingle > count)
            {
                temp = NumOfSingle - count;

                FLASH_PageWrite(pBuffer, WriteAddr, count);
                pBuffer += count;
                WriteAddr += count;

                FLASH_PageWrite(pBuffer, WriteAddr, temp);
            }
            else
            {
                FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
            }
        }
        else
        {
            FLASH_PageWrite(pBuffer, WriteAddr, count);
            pBuffer += count;
            WriteAddr += count;

            NumByteToWrite = NumByteToWrite - count;
            NumOfPage = NumByteToWrite / W25Q64_PAGE_SIZE;
            NumOfSingle = NumByteToWrite % W25Q64_PAGE_SIZE;

            while (NumOfPage--)
            {
                FLASH_PageWrite(pBuffer, WriteAddr, W25Q64_PAGE_SIZE);
                pBuffer += W25Q64_PAGE_SIZE;
                WriteAddr += W25Q64_PAGE_SIZE;
            }

            if (NumOfSingle != 0)
            {
                FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
            }
        }
    }
}

void SPI_FLASH_BufferRead(Uint32 *pBuffer, Uint32 ReadAddr, Uint16 NumByteToRead)
{

    FLASH_CS_SETL;

    SPI_ReadWriteByte(0x03);

    SPI_ReadWriteByte((ReadAddr & 0xFF0000) >> 16);
    SPI_ReadWriteByte((ReadAddr & 0xFF00) >> 8);
    SPI_ReadWriteByte(ReadAddr & 0xFF);

    while (NumByteToRead--)
    {
        *pBuffer++ = SPI_ReadWriteByte(BYDUMMY);
        //		pBuffer++;
    }

    FLASH_CS_SETH;
}

// 扇区擦除
void SPI_FLASH_SectorErase(Uint32 SectorAddr)
{
    SPI_FLASH_WriteEnable();
    SPI_FLASH_WaitForWriteEnd();

    FLASH_CS_SETL;

    SPI_ReadWriteByte(W25X_SectorErase);

    SPI_ReadWriteByte((SectorAddr & 0xFF0000) >> 16);
    SPI_ReadWriteByte((SectorAddr & 0xFF00) >> 8);
    SPI_ReadWriteByte(SectorAddr & 0xFF);

    FLASH_CS_SETH;

    SPI_FLASH_WaitForWriteEnd();
}

// 32K擦除
void FLASH_32KErase(Uint32 addr)
{
    SPI_FLASH_WriteEnable();

    FLASH_CS_SETL;

    SPI_ReadWriteByte(W25X_BlockErase32K);

    SPI_ReadWriteByte((addr & 0xFF0000) >> 16);
    SPI_ReadWriteByte((addr & 0xFF00) >> 8);
    SPI_ReadWriteByte(addr & 0xFF);

    FLASH_CS_SETH;

    SPI_FLASH_WaitForWriteEnd();
}

// 64K擦除
void FLASH_64KErase(Uint32 addr)
{
    SPI_FLASH_WriteEnable();

    FLASH_CS_SETL;

    SPI_ReadWriteByte(W25X_BlockErase64K);

    SPI_ReadWriteByte((addr & 0xFF0000) >> 16);
    SPI_ReadWriteByte((addr & 0xFF00) >> 8);
    SPI_ReadWriteByte(addr & 0xFF);

    FLASH_CS_SETH;

    SPI_FLASH_WaitForWriteEnd();
}

// 芯片擦除
void SPI_FLASH_ChipErase()
{
    SPI_FLASH_WriteEnable();

    FLASH_CS_SETL;

    SPI_ReadWriteByte(W25X_ChipErase);

    FLASH_CS_SETH;

    SPI_FLASH_WaitForWriteEnd();
}

void SPI_Flash_WAKEUP()
{
    FLASH_CS_SETL;

    SPI_ReadWriteByte(0xAB);

    FLASH_CS_SETH;
}

//
// /*******************************************************************************
//  * 函数名：W25Q64_WriteEnable
//  * 描述  ：W25Q64发送写使能命令
//  * 输入  ：void
//  * 输出  ：void
//  * 调用  ：内部调用
//  * 备注  ：
//  *******************************************************************************/
// void W25Q64_WriteEnable(void)
// {
// 	SPI_CS_LOW();
// 	SPI_Send_2Byte(W25Q64_Write_Enable);
// 	SPI_CS_HIGH();
// }
//
// /*******************************************************************************
//  * 函数名：W25Q64_WriteDisable
//  * 描述  ：W25Q64发送禁止写使能命令
//  * 输入  ：void
//  * 输出  ：void
//  * 调用  ：内部调用
//  * 备注  ：
//  *******************************************************************************/
// void W25Q64_WriteDisable(void)
// {
// 	SPI_CS_LOW();
// 	SPI_Send_2Byte(W25Q64_Write_Disable);
// 	SPI_CS_HIGH();
// }
//
// /*******************************************************************************
//  * 函数名：W25Q64_WaitForBusyStatus
//  * 描述  ：等待 W25Q64 芯片的忙状态结束
//  * 输入  ：void
//  * 输出  ：void
//  * 调用  ：内部调用
//  * 备注  ：
//  *******************************************************************************/
// void W25Q64_WaitForBusyStatus(void)
// {
//     Uint16 Timeout = 0xFFFF;
//     SPI_CS_LOW();
//     SPI_Send_2Byte(W25Q64_Read_Status_register_1);
//     while (Timeout > 0)
//     {
//         Uint16 status = SPI_Send_2Byte(W25Q64_DUMMY_BYTE);
//         if ((status & 0x01) == 0)// 检查忙状态是否结束
//         {
//             break;
//         }
//         Timeout--;
//         if (Timeout == 0)
//         {
//             //printf("W25Q64 ERROR \r\n");
//             break;
//         }
//     }
//     SPI_CS_HIGH();
// }
//
//
// /*******************************************************************************
//  * 函数名：W25Q64_SectorErase
//  * 描述  ：扇区擦除函数
//  * 输入  ：Sector_Address
//  * 输出  ：void
//  * 调用  ：内部调用
//  * 备注  ：在执行写入操作前要进行擦除
//  *******************************************************************************/
// void W25Q64_SectorErase(Uint32 Sector_Address)
// {
// 	W25Q64_WriteEnable();//W25Q64写使能
// 	SPI_CS_LOW();
// 	SPI_Send_2Byte(W25Q64_Sector_Erase_4KB);
// 	SPI_Send_2Byte(Sector_Address >> 16);//24位扇区地址
// 	SPI_Send_2Byte(Sector_Address >> 8);
// 	SPI_Send_2Byte(Sector_Address);
// 	SPI_CS_HIGH();
// 	W25Q64_WaitForBusyStatus();//等待 W25Q64 芯片的忙状态结束
// 	DELAY_US(10);
// 	W25Q64_WriteDisable();//W25Q64禁止写使能
// }
//
//
// /*******************************************************************************
//  * 函数名：W25Q64_BlockErase
//  * 描述  ：块擦除函数
//  * 输入  ：Address
//  * 输出  ：void
//  * 调用  ：内部调用
//  * 备注  ：在执行写入操作前要进行擦除
//  *******************************************************************************/
// //void W25Q64_BlockErase(uint32_t Block_Address)
// //{
// //    W25Q64_WriteEnable();//W25Q64写使能
// //    SPI_CS_LOW();
// //    SPI_Send_2Byte(W25Q64_Block_Erase_64KB);
// //    SPI_Send_2Byte((Block_Address & 0xFF0000) >> 16);//24位扇区地址
// //    SPI_Send_2Byte((Block_Address & 0xFF00) >> 8);
// //    SPI_Send_2Byte(Block_Address & 0xFF);
// //    SPI_CS_HIGH();
// //    W25Q64_WaitForBusyStatus();//等待 W25Q64 芯片的忙状态结束
// //	W25Q64_WriteDisable();//W25Q64禁止写使能
// //}
//
//
// /*******************************************************************************
//  * 函数名：W25Q64_ChipErase
//  * 描述  ：芯片擦除函数
//  * 输入  ：void
//  * 输出  ：void
//  * 调用  ：内部调用
//  * 备注  ：在执行写入操作前要进行擦除
//  *******************************************************************************/
// void W25Q64_ChipErase(void)
// {
//     W25Q64_WriteEnable();//W25Q64写使能
//     SPI_CS_LOW();
//     SPI_Send_2Byte(W25Q64_Chip_Erase);
//     SPI_CS_HIGH();
//     W25Q64_WaitForBusyStatus();//等待 W25Q64 芯片的忙状态结束
// 	W25Q64_WriteDisable();//W25Q64禁止写使能
// }
//
//
// /*******************************************************************************
//  * 函数名：W25Q64_PageProgram
//  * 描述  ：W25Q64页写操作
//  * 输入  ：address	 要写入的起始地址
//            *data	 要写入的数据缓冲区指针。
//            dataSize	 要写入的数据大小，单位为字节。
//  * 输出  ：void
//  * 调用  ：内部调用
//  * 备注  ：
//  *******************************************************************************/
// void W25Q64_PageWrite(Uint32 address, Uint16 *data, Uint16 dataSize)
// {
//   W25Q64_WriteEnable();//W25Q64写使能
//   SPI_CS_LOW();
//   Data |= ;
//   SPI_Send_2Byte(W25Q64_Page_Program);
//   SPI_Send_2Byte((address & 0xFF0000) >> 16);
// Uint16 addr;
// SPI_Send_2Byte((address & 0xFF00) >> 8);
// SPI_Send_2Byte(address & 0xFF);
//   Uint16 i;
//   if (dataSize <= 128)
//   {
//       for (i = 0; i < dataSize; i++)
//       {
//           SPI_Send_2Byte(data[i]);
//       }
//   }
//   else
//   {
//       for (i = 0; i < 128; i++)
//       {
//           SPI_Send_2Byte(data[i]);
//       }
//       dataSize -= 128;
//       data += 128;
//       // 写入剩余数据
//       W25Q64_WaitForBusyStatus();//等待 W25Q64 芯片的忙状态结束
//       address += 128;
//       while (dataSize > 0)
//       {
//           W25Q64_WriteEnable();//W25Q64写使能
//           SPI_CS_LOW();
//           SPI_Send_2Byte(W25Q64_Page_Program);
//             SPI_Send_2Byte((address & 0xFF0000) >> 16);
//             SPI_Send_2Byte((address & 0xFF00) >> 8);
//             SPI_Send_2Byte(address & 0xFF);
//           Uint16 chunkSize = (dataSize > 128) ? 128 : dataSize;
//           for (i = 0; i < chunkSize; i++)
//           {
//               SPI_Send_2Byte(data[i]);
//           }
//           SPI_CS_HIGH();
//           W25Q64_WaitForBusyStatus();//等待 W25Q64 芯片的忙状态结束
//           dataSize -= chunkSize;
//           data += chunkSize;
//           address += chunkSize;
//       }
//   }
//   SPI_CS_HIGH();
//   W25Q64_WaitForBusyStatus();//等待 W25Q64 芯片的忙状态结束
//   W25Q64_WriteDisable();//W25Q64禁止写使能
// }
//
// /*******************************************************************************
//  * 函数名：W25Q64_ReadData
//  * 描述  ：W25Q64读数据
//  * 输入  ：void
//  * 输出  ：void
//  * 调用  ：
//  * 备注  ：
//  *******************************************************************************/
// void W25Q64_ReadData(Uint32 address, Uint16 *Data, Uint16 dataSize)
// {
// 	  Uint16 i;
//    SPI_CS_LOW();
//    Uint16 data;
//    data = W25Q64_Read_Data;
//    data <<= 8;
//    data |= ((address & 0xFF0000) >> 16);
//    SPI_Send_2Byte(W25Q64_Read_Data);
//     SPI_Send_2Byte((address & 0xFF0000) >> 16);
//     SPI_Send_2Byte((address & 0xFF00) >> 8);
//     SPI_Send_2Byte(address & 0xFF);
//    for (i = 0; i < dataSize; i++)
//    {
//        Data[i] = SPI_Send_2Byte(W25Q64_DUMMY_BYTE);
//    }
//    SPI_CS_HIGH();
// }
//
// /*******************************************************************************
//  * 函数名：W25Q64_ReadID
//  * 描述  ：W25Q64读取设备ID
//  * 输入  ：*ID	存储ID的变量
//  * 输出  ：void
//  * 调用  ：内部调用
//  * 备注  ：EF4017
//  *******************************************************************************/
//  void W25Q64_ReadID(Uint32 *ID)
//  {
//        Uint32 JEDE_ID=0,temp0=0,temp1=0,temp2=0;
//      SPI_CS_LOW();
//      SPI_Send_2Byte(0x9F);         // 读ID号指令
//      temp0 = SPI_Send_2Byte(W25Q64_DUMMY_BYTE); // 厂商ID，默认为0xEF
//      temp1 = SPI_Send_2Byte(W25Q64_DUMMY_BYTE); // 设备ID，表示存储类型，默认为0x40
//      temp2 = SPI_Send_2Byte(W25Q64_DUMMY_BYTE); // 设备ID，表示容量，默认为0x17
//            ID = (temp0<<16) | (temp1<<8) | temp2;
//      SPI_CS_HIGH();
//  }
