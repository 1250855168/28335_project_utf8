/*
 * w25q64.h
 *
 *  Created on: 2023年12月18日
 *      Author: ych
 */

#ifndef HARDWARE_INCLUDE_W25Q64_H_
#define HARDWARE_INCLUDE_W25Q64_H_

void spi_init();

void WriteDataSpi(Uint16 iData);

unsigned char ReadMfrDeviceID_SPI(void);

unsigned char SPI_ReadStatusREG_GW(unsigned char *iStatus);

unsigned char SPI_WriteStatusREG_GW(unsigned char iValue);

unsigned char SPI_WriteEnable_GW(void);

unsigned char SPI_WriteDisable_GW(void);

unsigned char SPI_EraseChip_GW(void);

unsigned char SPI_EraseBlock_GW(Uint32 iBlockNum,Uint16 State_BaoHu);

unsigned char SPI_EraseSector_GW(Uint32 iSectorNum);

unsigned char SPI_WriteTwoByte_GW(Uint32 iAddress,Uint16 iData);

unsigned char SPI_ReadTwoByte_GW(Uint32 iAddress,Uint16 *iRData);

unsigned char SPI_WriteMulByte_GW(Uint32 iAddress,Uint16 *iData,unsigned char iSendDataLen);

unsigned char SPI_ReadMulByte33MHZ_GW(Uint32 iAddress,Uint16 *iRData,Uint32 iRecvDataLen);

unsigned char SPI_ReadMulByte20MHZ_GW(Uint32 iAddress,Uint16 *iRData,Uint32 iRecvDataLen);

unsigned char SPI_WaitBusy_GW(void);



#endif /* HARDWARE_INCLUDE_W25Q64_H_ */
