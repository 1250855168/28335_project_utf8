#include"DSP2833x_Project.h"
#include"w25q64.h"

//flash type
#define WINBOND_FLASH       0x55

#define SPI_FAILED          0xFF
#define SPI_SUCCESS         0x00
#define SPI_COMM_TIMEOUT    0xFFFE

#define     DUMMY_DATA          0x00    //
#define     FLASH_WRSR          0x01    // write STATUS register instruction
#define     FLASH_WRITE_PAGE    0x02    // page program instruction
#define     FLASH_READ          0x03    // read data from memory instruction
#define     FLASH_WRDI          0x04    // write disable instruction
#define     FLASH_RDSR          0x05    // read STATUS register instruction
#define     FLASH_WREN          0x06    // write enable instruction
#define     FLASH_READ_FAST     0x0B    // fast read,Read Data Bytes at Higher Speed
#define     FLASH_ERASE_SECTOR  0x20    // sector erase instruction

#define     FLASH_READ_MFR          0x90    // read manufacture/device id instruction
#define     FLASH_ERASE_CHIP        0xC7    // chip erase instruction
#define     FLASH_ERASE_BLOCK_64K   0xD8    // block erase 64K instruction

//#define       FLASH_BaoHu_Boot    0x0E //02 不保护， 2E保护用户区数据 0E保护BOOT区数据
#define     FLASH_BaoHu_User    0x2A
#define     FLASH_BaoHu_No      0x02

int gSpiTimeoutCounts;

void InitSPIGpio()
{

    EALLOW;
 /* Enable internal pull-up for the selected pins */
 // Pull-ups can be enabled or disabled by the user.
 // This will enable the pullups for the specified pins.
 // Comment out other unwanted lines.

 //    GpioCtrlRegs.GPBPUD.bit.GPIO16 = 0;   // Enable pull-up on GPIO16 (SPISIMOA)
 //    GpioCtrlRegs.GPBPUD.bit.GPIO17 = 0;   // Enable pull-up on GPIO17 (SPISOMIA)
 //    GpioCtrlRegs.GPBPUD.bit.GPIO18 = 0;   // Enable pull-up on GPIO18 (SPICLKA)
 //    GpioCtrlRegs.GPBPUD.bit.GPIO19 = 0;   // Enable pull-up on GPIO19 (SPISTEA)


     GpioCtrlRegs.GPBPUD.bit.GPIO54 = 0;   // Enable pull-up on GPIO54 (SPISIMOA)
     GpioCtrlRegs.GPBPUD.bit.GPIO55 = 0;   // Enable pull-up on GPIO55 (SPISOMIA)
     GpioCtrlRegs.GPBPUD.bit.GPIO56 = 0;   // Enable pull-up on GPIO56 (SPICLKA)
     GpioCtrlRegs.GPBPUD.bit.GPIO57 = 0;   // Enable pull-up on GPIO57 (SPISTEA)

 /* Set qualification for selected pins to asynch only */
 // This will select asynch (no qualification) for the selected pins.
 // Comment out other unwanted lines.

 //    GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3; // Asynch input GPIO16 (SPISIMOA)
 //    GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3; // Asynch input GPIO17 (SPISOMIA)
 //    GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3; // Asynch input GPIO18 (SPICLKA)
 //    GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3; // Asynch input GPIO19 (SPISTEA)

     GpioCtrlRegs.GPBQSEL2.bit.GPIO54 = 3; // Asynch input GPIO54 (SPISIMOA)
     GpioCtrlRegs.GPBQSEL2.bit.GPIO55 = 3; // Asynch input GPIO55 (SPISOMIA)
     GpioCtrlRegs.GPBQSEL2.bit.GPIO56 = 3; // Asynch input GPIO56 (SPICLKA)
     GpioCtrlRegs.GPBQSEL2.bit.GPIO57 = 3; // Asynch input GPIO57 (SPISTEA)


 /* Configure SPI-A pins using GPIO regs*/
 // This specifies which of the possible GPIO pins will be SPI functional pins.
 // Comment out other unwanted lines.

 //    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1; // Configure GPIO16 as SPISIMOA
 //    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1; // Configure GPIO17 as SPISOMIA
 //    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1; // Configure GPIO18 as SPICLKA
 //    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 1; // Configure GPIO19 as SPISTEA

     GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 1; // Configure GPIO54 as SPISIMOA
     GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 1; // Configure GPIO55 as SPISOMIA
     GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 1; // Configure GPIO56 as SPICLKA
     GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 1; // Configure GPIO57 as SPISTEA

     EDIS;
}

void spi_init()
{
    InitSPIGpio();
    SpiaRegs.SPIFFTX.all=0xE040;
    SpiaRegs.SPIFFRX.all=0x204F;
    SpiaRegs.SPIFFCT.all=0x0;
    //PCLKCR0
    //SPI Configuration Control Register (SPICCR)
    //clear the spi software reset bit to 0 to force the spi module
    // to the reset state
    SpiaRegs.SPICCR.bit.SPISWRESET = 0;
    //choose spiclk polarity
    //1:Data is output on rising edge and input on falling edge
    //0:Data is output on falling edge and input on rising edge.
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 1;
    //spi character length
    //0-1bit word,1-2bit word,2-3bit word,3-4bit word,4-5bit word
    //5-6bit word,6-7bit word,7-8bit word,8-9bit word,9-10bit word
    //A-11bit word,B-12bit word,C-13bit word,D-14bit word,E-15bit word
    //F-16bit word.
    SpiaRegs.SPICCR.bit.SPICHAR = 0x0F;
    //lookback off
    SpiaRegs.SPICCR.bit.SPILBK = 1;

    //SPI Operation Control Register (SPICTL)
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;//1,master,0,slave
    //choose soiclk phase
    //0:Normal SPI clocking scheme, depending on the CLOCK POLARITY bit (SPICCR.6)
    //1:SPICLK signal delayed by one half-cycle; polarity determined by the CLOCK POLARITY bit
    SpiaRegs.SPICTL.bit.CLK_PHASE = 0;
    //enable 3-wire mode(TRIWIRE)
    //1:Enables transmission For the 4-pin option, ensure to enable the receiver’s SPISTE input pin
    SpiaRegs.SPICTL.bit.TALK=1;
    //disable spi interrupt
    SpiaRegs.SPICTL.bit.SPIINTENA = 0;
    //disable reciever overrun interrupt
    SpiaRegs.SPICTL.bit.OVERRUNINTENA = 0;

    //SPI Status Register (SPIST)
    SpiaRegs.SPISTS.all = 0;//clear spi flags:overrun_flag,int_flag
    //SPI Baud Rate Register (SPIBRR)

    SpiaRegs.SPIBRR = 20;       // 60M/4 / (20+1) = 194805.2//baud rate//76
    //SPI Priority Control Register (SPIPRI)
    //enable SPISTE inverson (STEINV)
    SpiaRegs.SPIPRI.bit.FREE = 1;
    SpiaRegs.SPIPRI.bit.SOFT = 1;
    //SpiaRegs.SPIPRI.bit.TRIWIRE = 0;
    //FIFO enhancements config
    //interrupts config
    SpiaRegs.SPICCR.bit.SPISWRESET = 1;//set SPISWRESET to 1 to release the spi from the reset state
}

void WriteDataSpi(Uint16 iData)
{   //MSB First
    SpiaRegs.SPITXBUF=iData;
}

unsigned char ReadMfrDeviceID_SPI(void)
{//test ok
    unsigned char i=0;
    Uint16 iDataBuff[4];
    unsigned char iRCounts=0;
    Uint16 iSendCmd = 0x00;
    iSendCmd = FLASH_READ_MFR;
    iSendCmd <<= 8;
    WriteDataSpi(iSendCmd);//cmd+address
    WriteDataSpi(DUMMY_DATA);//address
    WriteDataSpi(DUMMY_DATA);//data 1
    WriteDataSpi(DUMMY_DATA);//data 2
    gSpiTimeoutCounts = 0;
    while ( SpiaRegs.SPIFFRX.bit.RXFFST <=3 )
    {
        gSpiTimeoutCounts++;
        if ( SPI_COMM_TIMEOUT <= gSpiTimeoutCounts )
        {
            gSpiTimeoutCounts = 0;
            return SPI_FAILED;
        }
    }
    iRCounts = SpiaRegs.SPIFFRX.bit.RXFFST;
    for ( i=0;i< iRCounts;i++ )
    {
        iDataBuff[i] = SpiaRegs.SPIRXBUF;
    }
     if ( ( iDataBuff[2] == 0xEF10 )&&
          ( iDataBuff[3] == 0xEF10 ) )
     {
        return WINBOND_FLASH;
     }else{
         return SPI_FAILED;
     }
}

unsigned char SPI_WriteEnable_GW(void)
{//test ok
    unsigned char i=0;
    unsigned char iDataBuff[4];
    unsigned char iRCounts=0;
    Uint16 iSendCmd = 0x00;
    iSendCmd = FLASH_WREN;
    iSendCmd <<= 8;
    SpiaRegs.SPICCR.bit.SPICHAR = 0x07;
    WriteDataSpi(iSendCmd);
    gSpiTimeoutCounts = 0;
    while ( SpiaRegs.SPIFFRX.bit.RXFFST <=0 )
    {
        gSpiTimeoutCounts++;
        if ( SPI_COMM_TIMEOUT <= gSpiTimeoutCounts )
        {
            gSpiTimeoutCounts = 0;
            return SPI_FAILED;
        }
    }
    iRCounts = SpiaRegs.SPIFFRX.bit.RXFFST;
    for ( i=0;i< iRCounts;i++ )
    {
        iDataBuff[i] = SpiaRegs.SPIRXBUF;
    }
    SpiaRegs.SPICCR.bit.SPICHAR = 0x0F;
    return SPI_SUCCESS;
}


//
//unsigned char SPI_ReadStatusREG_GW(unsigned char *iStatus)
//{//test ok
//    unsigned char i=0;
//    Uint16 iDataBuff[4];
//    unsigned char iRCounts=0;
//    Uint16 iSendCmd = 0x00;
//    iSendCmd = FLASH_RDSR;
//    iSendCmd <<= 8;
//    WriteDataSpi(iSendCmd);
//    gSpiTimeoutCounts = 0;
//    while ( SpiaRegs.SPIFFRX.bit.RXFFST <=0 )
//    {
//        gSpiTimeoutCounts++;
//        if ( SPI_COMM_TIMEOUT <= gSpiTimeoutCounts )
//        {
//            gSpiTimeoutCounts = 0;
//            return SPI_FAILED;
//        }
//    }
//    iRCounts = SpiaRegs.SPIFFRX.bit.RXFFST;
//    for ( i=0;i< iRCounts;i++ )
//    {
//        iDataBuff[i] = SpiaRegs.SPIRXBUF;
//    }
//    *iStatus = iDataBuff[0]&0xFF;
//    return SPI_SUCCESS;
//}
//
//unsigned char SPI_WriteStatusREG_GW(unsigned char iValue)
//{//test ok
//    unsigned char i=0;
//    Uint16 iDataBuff[4];
//    unsigned char iRCounts=0;
//    Uint16 iSendCmd = 0x00;
//    iSendCmd = FLASH_WRSR;
//    iSendCmd <<= 8;
//    iSendCmd += iValue;
//    WriteDataSpi(iSendCmd);
//    gSpiTimeoutCounts = 0;
//    while ( SpiaRegs.SPIFFRX.bit.RXFFST <=0 )
//    {
//        gSpiTimeoutCounts++;
//        if ( SPI_COMM_TIMEOUT <= gSpiTimeoutCounts )
//        {
//            gSpiTimeoutCounts = 0;
//            return SPI_FAILED;
//        }
//    }
//    iRCounts = SpiaRegs.SPIFFRX.bit.RXFFST;
//    for ( i=0;i< iRCounts;i++ )
//    {
//        iDataBuff[i] = SpiaRegs.SPIRXBUF;
//    }
//    return SPI_SUCCESS;
//}
//
//unsigned char SPI_WriteEnable_GW(void)
//{//test ok
//    unsigned char i=0;
//    unsigned char iDataBuff[4];
//    unsigned char iRCounts=0;
//    Uint16 iSendCmd = 0x00;
//    iSendCmd = FLASH_WREN;
//    iSendCmd <<= 8;
//    SpiaRegs.SPICCR.bit.SPICHAR = 0x07;
//    WriteDataSpi(iSendCmd);
//    gSpiTimeoutCounts = 0;
//    while ( SpiaRegs.SPIFFRX.bit.RXFFST <=0 )
//    {
//        gSpiTimeoutCounts++;
//        if ( SPI_COMM_TIMEOUT <= gSpiTimeoutCounts )
//        {
//            gSpiTimeoutCounts = 0;
//            return SPI_FAILED;
//        }
//    }
//    iRCounts = SpiaRegs.SPIFFRX.bit.RXFFST;
//    for ( i=0;i< iRCounts;i++ )
//    {
//        iDataBuff[i] = SpiaRegs.SPIRXBUF;
//    }
//    SpiaRegs.SPICCR.bit.SPICHAR = 0x0F;
//    return SPI_SUCCESS;
//}
//
//unsigned char SPI_WriteDisable_GW(void)
//{//test ok
//    unsigned char i=0;
//    unsigned char iDataBuff[4];
//    unsigned char iRCounts=0;
//    Uint16 iSendCmd = 0x00;
//    iSendCmd = FLASH_WRDI;
//    iSendCmd <<= 8;
//    SpiaRegs.SPICCR.bit.SPICHAR = 0x07;
//    WriteDataSpi(iSendCmd);
//    gSpiTimeoutCounts = 0;
//    while ( SpiaRegs.SPIFFRX.bit.RXFFST <=0 )
//    {
//        gSpiTimeoutCounts++;
//        if ( SPI_COMM_TIMEOUT <= gSpiTimeoutCounts )
//        {
//            gSpiTimeoutCounts = 0;
//            return SPI_FAILED;
//        }
//    }
//    iRCounts = SpiaRegs.SPIFFRX.bit.RXFFST;
//    for ( i=0;i< iRCounts;i++ )
//    {
//        iDataBuff[i] = SpiaRegs.SPIRXBUF;
//    }
//    SpiaRegs.SPICCR.bit.SPICHAR = 0x0F;
//    return SPI_SUCCESS;
//}
//
//unsigned char SPI_EraseChip_GW(void)
//{//test ok
//    unsigned char iStatus=0x00;
//    unsigned char i=0;
//    unsigned char iDataBuff[4];
//    Uint16 iSendCmd=0;
//    unsigned char iRCounts=0;
//    if ( SPI_WriteEnable_GW() == SPI_SUCCESS )
//    {
//        if ( SPI_ReadStatusREG_GW(&iStatus) == SPI_SUCCESS )
//        {
//            //if ( ( iStatus&0x3E ) != FLASH_BaoHu_Boot )
//            if ( ( iStatus&0x3E ) != FLASH_BaoHu_No )
//            {
//                if ( SPI_WriteEnable_GW() == SPI_SUCCESS )
//                {
//                    //if ( SPI_WriteStatusREG_GW(FLASH_BaoHu_Boot) == SPI_SUCCESS )
//                    if( SPI_WriteStatusREG_GW(FLASH_BaoHu_No) == SPI_SUCCESS )
//                    {
//                        if ( SPI_ReadStatusREG_GW(&iStatus) == SPI_SUCCESS )
//                        {
//                            //if ( FLASH_BaoHu_Boot != (iStatus&0x3E) )
//                            if ( FLASH_BaoHu_No != (iStatus&0x3E) )
//                            {
//                                return SPI_FAILED;
//                            }
//                        }else{
//                            return SPI_FAILED;
//                        }
//                    }else{
//                        return SPI_FAILED;
//                    }
//                }else{
//                    return SPI_FAILED;
//                }
//            }
//            if ( SPI_WaitBusy_GW() != SPI_SUCCESS )
//            {
//                return SPI_FAILED;
//            }
//            iSendCmd = FLASH_ERASE_CHIP;
//            iSendCmd <<= 8;
//            SpiaRegs.SPICCR.bit.SPICHAR = 0x07;
//            WriteDataSpi(iSendCmd);
//            gSpiTimeoutCounts = 0;
//            while ( SpiaRegs.SPIFFRX.bit.RXFFST <=0 )
//            {
//                gSpiTimeoutCounts++;
//                if ( SPI_COMM_TIMEOUT <= gSpiTimeoutCounts )
//                {
//                    gSpiTimeoutCounts = 0;
//                    return SPI_FAILED;
//                }
//            }
//            iRCounts = SpiaRegs.SPIFFRX.bit.RXFFST;
//            for ( i=0;i< iRCounts;i++ )
//            {
//                iDataBuff[i] = SpiaRegs.SPIRXBUF;
//            }
//            SpiaRegs.SPICCR.bit.SPICHAR = 0x0F;
//            if ( SPI_WaitBusy_GW() != SPI_SUCCESS )
//            {
//                return SPI_FAILED;
//            }
//            return SPI_WriteDisable_GW();
//        }
//    }
//    return SPI_FAILED;
//}
//
//unsigned char SPI_EraseBlock_GW(Uint32 iBlockNum,Uint16 State_BaoHu)//iBlockNum:0,1,2,3,4,5,6,7
//{//test ok
//    unsigned char iStatus=0x00;
//    unsigned char iAddress[3];
//    Uint32 iAddressTemp = 0x00;
//    unsigned char i=0;
//    Uint16 iDataBuff[4];
//    unsigned char iRCounts=0;
//    Uint16 iSendCmd=0;
//    if (  8 >iBlockNum  )
//    {//0x000000-0x01FFFF
//        /*iBlockNum -= 1;
//        iAddressTemp = 1;
//        iAddressTemp <<= 16;
//        iAddressTemp *= iBlockNum;*/
//        iAddressTemp=iBlockNum*0x10000;
//        iAddress[0] = (iAddressTemp>>16)&0xFF;
//        iAddress[1] = (iAddressTemp>>8)&0xFF;
//        iAddress[2] = iAddressTemp&0xFF;
//        if ( SPI_WriteEnable_GW() == SPI_SUCCESS )
//        {
//            if ( SPI_ReadStatusREG_GW(&iStatus) == SPI_SUCCESS )
//            {
//                if ( ( iStatus&0x3E ) != State_BaoHu )
//                {
//                    if ( SPI_WriteEnable_GW() == SPI_SUCCESS )
//                    {
//                        if ( SPI_WriteStatusREG_GW(State_BaoHu) == SPI_SUCCESS )
//                        {
//                            if ( SPI_ReadStatusREG_GW(&iStatus) == SPI_SUCCESS )
//                            {
//                                if ( State_BaoHu != (iStatus&0x3E) )
//                                {
//                                    return SPI_FAILED;
//                                }
//                            }else{
//                                return SPI_FAILED;
//                            }
//                        }else{
//                            return SPI_FAILED;
//                        }
//                    }else{
//                        return SPI_FAILED;
//                    }
//                }
//                if ( SPI_WaitBusy_GW() != SPI_SUCCESS )
//                {
//                    return SPI_FAILED;
//                }
//                iSendCmd = FLASH_ERASE_BLOCK_64K;
//                iSendCmd <<= 8;
//                iSendCmd += iAddress[0];
//                WriteDataSpi(iSendCmd);
//                iSendCmd = iAddress[1];
//                iSendCmd <<= 8;
//                iSendCmd += iAddress[2];
//                WriteDataSpi(iSendCmd);
//                gSpiTimeoutCounts = 0;
//                while ( SpiaRegs.SPIFFRX.bit.RXFFST <=1 )
//                {
//                    gSpiTimeoutCounts++;
//                    if ( SPI_COMM_TIMEOUT <= gSpiTimeoutCounts )
//                    {
//                        gSpiTimeoutCounts = 0;
//                        return SPI_FAILED;
//                    }
//                }
//                iRCounts = SpiaRegs.SPIFFRX.bit.RXFFST;
//                for ( i=0;i< iRCounts;i++ )
//                {
//                    iDataBuff[i] = SpiaRegs.SPIRXBUF;
//                }
//                if ( SPI_WaitBusy_GW() != SPI_SUCCESS )
//                {
//                    return SPI_FAILED;
//                }
//                return SPI_WriteDisable_GW();
//            }
//        }
//    }
//    return SPI_FAILED;
//}
//
//unsigned char SPI_EraseSector_GW(Uint32 iSectorNum)//0:successful,1:failed,iSectorNum:0-127
//{//test ok
//    unsigned char iStatus=0x00;
//    Uint16 iAddress[3];
//    Uint32 iAddressTemp = 0x00;
//    unsigned char i=0;
//    Uint16 iDataBuff[4];
//    unsigned char iRCounts=0;
//    Uint16 iSendCmd=0;
//    if (  127 >=iSectorNum  )
//    {//0x000000-0x01FFFF
//        //iSectorNum -= 1;
//        //iAddressTemp = 1;
//        //iAddressTemp <<= 12;
//        //iAddressTemp *= iSectorNum;
//        iAddressTemp =((iSectorNum)*0x1000);
//        iAddress[0] = (iAddressTemp>>16)&0xFF;
//        iAddress[1] = (iAddressTemp>>8)&0xFF;
//        iAddress[2] = iAddressTemp&0xFF;
//        if ( SPI_WriteEnable_GW() == SPI_SUCCESS )
//        {
//            if ( SPI_ReadStatusREG_GW(&iStatus) == SPI_SUCCESS )
//            {
//                //if ( ( iStatus&0x3E ) != FLASH_BaoHu_Boot )
//                if ( ( iStatus&0x3E ) != FLASH_BaoHu_No )
//                {
//                    if ( SPI_WriteEnable_GW() == SPI_SUCCESS )
//                    {
//                        //if ( SPI_WriteStatusREG_GW(FLASH_BaoHu_Boot) == SPI_SUCCESS )
//                        if ( SPI_WriteStatusREG_GW(FLASH_BaoHu_No) == SPI_SUCCESS )
//                        {
//                            if ( SPI_ReadStatusREG_GW(&iStatus) == SPI_SUCCESS )
//                            {
//                                //if ( FLASH_BaoHu_Boot != (iStatus&0x3E) )
//                                if ( FLASH_BaoHu_No != (iStatus&0x3E) )
//                                {
//                                    return SPI_FAILED;
//                                }
//                            }else{
//                                return SPI_FAILED;
//                            }
//                        }else{
//                            return SPI_FAILED;
//                        }
//                    }else{
//                        return SPI_FAILED;
//                    }
//                }
//                if ( SPI_WaitBusy_GW() != SPI_SUCCESS )
//                {
//                    return SPI_FAILED;
//                }
//                iSendCmd = FLASH_ERASE_SECTOR;
//                iSendCmd <<= 8;
//                iSendCmd += iAddress[0];
//                WriteDataSpi(iSendCmd);
//                iSendCmd = iAddress[1];
//                iSendCmd <<= 8;
//                iSendCmd += iAddress[2];
//                WriteDataSpi(iSendCmd);
//                gSpiTimeoutCounts = 0;
//                while ( SpiaRegs.SPIFFRX.bit.RXFFST <=1 )
//                {
//                    gSpiTimeoutCounts++;
//                    if ( SPI_COMM_TIMEOUT <= gSpiTimeoutCounts )
//                    {
//                        gSpiTimeoutCounts = 0;
//                        return SPI_FAILED;
//                    }
//                }
//                iRCounts = SpiaRegs.SPIFFRX.bit.RXFFST;
//                for ( i=0;i< iRCounts;i++ )
//                {
//                    iDataBuff[i] = SpiaRegs.SPIRXBUF;
//                }
//                if ( SPI_WaitBusy_GW() != SPI_SUCCESS )
//                {
//                    return SPI_FAILED;
//                }
//                return SPI_WriteDisable_GW();
//            }
//        }
//    }
//    return SPI_FAILED;
//}
//
//unsigned char SPI_WriteTwoByte_GW(Uint32 iAddress,Uint16 iData)
//{//test ok
//    unsigned char iStatus=0x00;
//    unsigned char iAddressTemp[3];
//    unsigned char i=0;
//    Uint16 iDataBuff[4];
//    unsigned char iRCounts=0;
//    Uint16 iSendCmd=0;
//    iAddressTemp[0] = (iAddress>>16)&0xFF;
//    iAddressTemp[1] = (iAddress>>8)&0xFF;
//    iAddressTemp[2] = iAddress&0xFF;
//    if ( SPI_WriteEnable_GW() == SPI_SUCCESS )
//    {
//        if ( SPI_ReadStatusREG_GW(&iStatus) == SPI_SUCCESS )
//        {
//            //if ( ( iStatus&0x3E ) != FLASH_BaoHu_Boot )
//            if ( ( iStatus&0x3E ) != FLASH_BaoHu_No )
//            {
//                if ( SPI_WriteEnable_GW() == SPI_SUCCESS )
//                {
//                    //if ( SPI_WriteStatusREG_GW(FLASH_BaoHu_Boot) == SPI_SUCCESS )
//                    if ( SPI_WriteStatusREG_GW(FLASH_BaoHu_No) == SPI_SUCCESS )
//                    {
//                        if ( SPI_ReadStatusREG_GW(&iStatus) == SPI_SUCCESS )
//                        {
//                            //if ( FLASH_BaoHu_Boot != (iStatus&0x3E) )
//                            if ( FLASH_BaoHu_No != (iStatus&0x3E) )
//                            {
//                                return SPI_FAILED;
//                            }
//                        }else{
//                            return SPI_FAILED;
//                        }
//                    }else{
//                        return SPI_FAILED;
//                    }
//                }else{
//                    return SPI_FAILED;
//                }
//            }
//            if ( SPI_WaitBusy_GW() != SPI_SUCCESS )
//            {
//                return SPI_FAILED;
//            }
//            iSendCmd = FLASH_WRITE_PAGE;
//            iSendCmd <<= 8;
//            iSendCmd += iAddressTemp[0];
//            WriteDataSpi(iSendCmd);
//            iSendCmd = iAddressTemp[1];
//            iSendCmd <<= 8;
//            iSendCmd += iAddressTemp[2];
//            WriteDataSpi(iSendCmd);
//            iSendCmd = iData;
//            WriteDataSpi(iSendCmd);
//            gSpiTimeoutCounts = 0;
//            while ( SpiaRegs.SPIFFRX.bit.RXFFST <=2 )
//            {
//                gSpiTimeoutCounts++;
//                if ( SPI_COMM_TIMEOUT <= gSpiTimeoutCounts )
//                {
//                    gSpiTimeoutCounts = 0;
//                    return SPI_FAILED;
//                }
//            }
//            iRCounts = SpiaRegs.SPIFFRX.bit.RXFFST;
//            for ( i=0;i< iRCounts;i++ )
//            {
//                iDataBuff[i] = SpiaRegs.SPIRXBUF;
//            }
//            return SPI_WriteDisable_GW();
//        }
//    }
//    return SPI_FAILED;
//}
//
//unsigned char SPI_ReadTwoByte_GW(Uint32 iAddress,Uint16 *iRData)
//{
//    //test ok
//    //unsigned char iStatus=0x00;
//    unsigned char iAddressTemp[3];
//    unsigned char i=0;
//    Uint16 iDataBuff[4];
//    unsigned char iRCounts=0;
//    Uint16 iSendCmd=0;
//    iAddressTemp[0] = (iAddress>>16)&0xFF;
//    iAddressTemp[1] = (iAddress>>8)&0xFF;
//    iAddressTemp[2] = iAddress&0xFF;
//
//    iSendCmd = FLASH_READ;
//    iSendCmd <<= 8;
//    iSendCmd += iAddressTemp[0];
//    WriteDataSpi(iSendCmd);
//    iSendCmd = iAddressTemp[1];
//    iSendCmd <<= 8;
//    iSendCmd += iAddressTemp[2];
//    WriteDataSpi(iSendCmd);
//    iSendCmd = 0x0000;
//    WriteDataSpi(iSendCmd);
//    gSpiTimeoutCounts = 0;
//    while ( SpiaRegs.SPIFFRX.bit.RXFFST <= 2 )
//    {
//        gSpiTimeoutCounts++;
//        if ( SPI_COMM_TIMEOUT <= gSpiTimeoutCounts )
//        {
//            gSpiTimeoutCounts = 0;
//            return SPI_FAILED;
//        }
//    }
//    iRCounts = SpiaRegs.SPIFFRX.bit.RXFFST;
//    for ( i=0;i< iRCounts;i++ )
//    {
//        iDataBuff[i] = SpiaRegs.SPIRXBUF;
//    }
//    *iRData = iDataBuff[2];
//    return SPI_SUCCESS;
//}
//
//unsigned char SPI_WriteMulByte_GW(Uint32 iAddress,Uint16 *iData,unsigned char iSendDataLen)
//{//test ok
//    unsigned char iStatus=0x00;
//    unsigned char iAddressTemp[3];
//    unsigned char i=0;
//    Uint32  j=0;
//    Uint16 iDataBuff[4];
//    unsigned char iRCounts=0;
//    Uint16 iSendCmd=0;
//    if ((iSendDataLen <= 2 )&&(iSendDataLen>=128) )
//    {
//        return SPI_FAILED;
//    }
//    if ( SPI_WriteEnable_GW() == SPI_SUCCESS )
//    {
//        //SPI_WriteStatusREG_GW(0x02);
//        if ( SPI_ReadStatusREG_GW(&iStatus) == SPI_SUCCESS )
//        {
//            //if ( ( iStatus&0x3E ) != FLASH_BaoHu_Boot )
//            if ( ( iStatus&0x3E ) != FLASH_BaoHu_No )
//            {
//                if ( SPI_WriteEnable_GW() == SPI_SUCCESS )
//                {
//                    //if ( SPI_WriteStatusREG_GW(FLASH_BaoHu_Boot) == SPI_SUCCESS )
//                    if ( SPI_WriteStatusREG_GW(FLASH_BaoHu_No) == SPI_SUCCESS )
//                    {
//                        if ( SPI_ReadStatusREG_GW(&iStatus) == SPI_SUCCESS )
//                        {
//                            //if ( FLASH_BaoHu_Boot != (iStatus&0x3E) )
//                            if ( FLASH_BaoHu_No != (iStatus&0x3E) )
//                            {
//                                return SPI_FAILED;
//                            }
//                        }else{
//                            return SPI_FAILED;
//                        }
//                    }else{
//                        return SPI_FAILED;
//                    }
//                }else{
//                    return SPI_FAILED;
//                }
//            }
//
//            for ( j=0;j<iSendDataLen;j+=2 )
//            {
//
//                iAddressTemp[0] = (iAddress>>16)&0xFF;
//                iAddressTemp[1] = (iAddress>>8)&0xFF;
//                iAddressTemp[2] = iAddress&0xFF;
//                iAddress += 4;
//                if ( SPI_WaitBusy_GW() != SPI_SUCCESS )
//                {
//                    return SPI_FAILED;
//                }
//                SPI_WriteEnable_GW();
//                iSendCmd = FLASH_WRITE_PAGE;
//                iSendCmd <<= 8;
//                iSendCmd += iAddressTemp[0];
//                WriteDataSpi(iSendCmd);//FLASH_WRITE_PAGE
//                iSendCmd = iAddressTemp[1];
//                iSendCmd <<= 8;
//                iSendCmd += iAddressTemp[2];
//                WriteDataSpi(iSendCmd);
//                WriteDataSpi(iData[j]);
//                WriteDataSpi(iData[j+1]);
//                gSpiTimeoutCounts = 0;
//                while ( SpiaRegs.SPIFFRX.bit.RXFFST <=3 )
//                {
//                    gSpiTimeoutCounts++;
//                    if ( SPI_COMM_TIMEOUT <= gSpiTimeoutCounts )
//                    {
//                        gSpiTimeoutCounts = 0;
//                        return SPI_FAILED;
//                    }
//                }
//                iRCounts = SpiaRegs.SPIFFRX.bit.RXFFST;
//                for ( i=0;i< iRCounts;i++ )
//                {
//                    iDataBuff[i] = SpiaRegs.SPIRXBUF;
//                }
//            }
//            if ( SPI_WaitBusy_GW() != SPI_SUCCESS )
//            {
//                return SPI_FAILED;
//            }
//            return SPI_WriteDisable_GW();
//        }
//    }
//    return SPI_FAILED;
//}
//
//unsigned char SPI_ReadMulByte33MHZ_GW(Uint32 iAddress,Uint16 *iRData,Uint32 iRecvDataLen)
//{//resv
//    //unsigned char iStatus=0x00;
//    unsigned char iAddressTemp[3];
//    Uint32 j=0;
//    unsigned char i = 0;
//    Uint16 iDataBuff[4];
//    unsigned char iRCounts=0;
//    Uint16 iSendCmd=0;
//
//    if ( ( iRecvDataLen <= 2 )&&(iRecvDataLen>=128) )
//    {
//        return SPI_FAILED;
//    }
//    for ( j=0;j<iRecvDataLen;j+=2 )
//    {
//        iAddressTemp[0] = (iAddress>>16)&0xFF;
//        iAddressTemp[1] = (iAddress>>8)&0xFF;
//        iAddressTemp[2] = iAddress&0xFF;
//        iAddress +=4;
//
//        iSendCmd = FLASH_READ_FAST;
//        iSendCmd <<= 8;
//        iSendCmd += iAddressTemp[0];
//        WriteDataSpi(iSendCmd);
//        iSendCmd = iAddressTemp[1];
//        iSendCmd <<= 8;
//        iSendCmd += iAddressTemp[2];
//        WriteDataSpi(iSendCmd);
//        WriteDataSpi(DUMMY_DATA);
//        WriteDataSpi(DUMMY_DATA);
//        gSpiTimeoutCounts = 0;
//        while ( SpiaRegs.SPIFFRX.bit.RXFFST <= 3 )
//        {
//            gSpiTimeoutCounts++;
//            if ( SPI_COMM_TIMEOUT <= gSpiTimeoutCounts )
//            {
//                gSpiTimeoutCounts = 0;
//                return SPI_FAILED;
//            }
//        }
//        iRCounts = SpiaRegs.SPIFFRX.bit.RXFFST;
//        for ( i=0;i< iRCounts;i++ )
//        {
//            iDataBuff[i] = SpiaRegs.SPIRXBUF;
//        }
//        iRData[j] = iDataBuff[2];
//        iRData[j+1] = iDataBuff[3];
//    }
//    return SPI_SUCCESS;
//}
//
//unsigned char SPI_ReadMulByte20MHZ_GW(Uint32 iAddress,Uint16 *iRData,Uint32 iRecvDataLen)
//{
//    //unsigned char iStatus=0x00;
//    unsigned char iAddressTemp[3];
//    Uint32 j=0;
//    unsigned char i = 0;
//    Uint16 iDataBuff[4];
//    unsigned char iRCounts=0;
//    Uint16 iSendCmd=0;
//    if ( ( iRecvDataLen <= 2 )&&(iRecvDataLen>=128) )
//    {
//        return SPI_FAILED;
//    }
//    for ( j=0;j<iRecvDataLen;j+=2 )
//    {
//        iAddressTemp[0] = (iAddress>>16)&0xFF;
//        iAddressTemp[1] = (iAddress>>8)&0xFF;
//        iAddressTemp[2] = iAddress&0xFF;
//        iAddress +=4;
//
//        iSendCmd = FLASH_READ;
//        iSendCmd <<= 8;
//        iSendCmd += iAddressTemp[0];
//        WriteDataSpi(iSendCmd);
//        iSendCmd = iAddressTemp[1];
//        iSendCmd <<= 8;
//        iSendCmd += iAddressTemp[2];
//        WriteDataSpi(iSendCmd);
//        WriteDataSpi(DUMMY_DATA);
//        WriteDataSpi(DUMMY_DATA);
//        gSpiTimeoutCounts = 0;
//        while ( SpiaRegs.SPIFFRX.bit.RXFFST <= 3 )
//        {
//            gSpiTimeoutCounts++;
//            if ( SPI_COMM_TIMEOUT <= gSpiTimeoutCounts )
//            {
//                gSpiTimeoutCounts = 0;
//                return SPI_FAILED;
//            }
//        }
//        iRCounts = SpiaRegs.SPIFFRX.bit.RXFFST;
//        for ( i=0;i< iRCounts;i++ )
//        {
//            iDataBuff[i] = SpiaRegs.SPIRXBUF;
//        }
//        iRData[j] = iDataBuff[2];
//        iRData[j+1] = iDataBuff[3];
//    }
//    return SPI_SUCCESS;
//}
//
//unsigned char SPI_WaitBusy_GW(void)
//{
//    unsigned char iStatus=0;
//    if ( SPI_ReadStatusREG_GW(&iStatus) == SPI_SUCCESS )
//    {
//        while (1 == (iStatus&0x01))
//        {
//            iStatus = 0x00;
//            SPI_ReadStatusREG_GW(&iStatus);
//        }
//        return SPI_SUCCESS;
//    }
//    return SPI_FAILED;
//}
//
//
//
