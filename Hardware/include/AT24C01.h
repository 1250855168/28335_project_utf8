/*
 * AT24C01.h
 *
 *  Created on: 2023年12月13日
 *      Author: ych
 */

#ifndef HARDWARE_INCLUDE_AT24C01_H_
#define HARDWARE_INCLUDE_AT24C01_H_

#include"DSP2833x_Project.h"
#include<iic.h>

/**
 *  @brief              AT24C01的初始化
 *  @parameter              无
 *  @return_value           无
 */
void At24c01_Init();

/**
 *  @brief              IIC模块发送准备OK
 *  @parameter              无
 *  @return_value           无
 */
Uint16  I2C_xrdy();

/**
 *  @brief              IIC模块接收准备OK
 *  @parameter              无
 *  @return_value           无
 */
Uint16  I2C_rrdy();

/**
 *  @brief                  IIC写数据
 *  @parameter              SlaveAddress：从机地址；  RomAddress：寄存器地址； number：写入数据的字节数；  *Wdata：写入数据的地址
 *  @return_value           状态标志
 */
Uint16 WriteData( Uint16 SlaveAddress, Uint16 RomAddress,Uint16 number, Uint8 *Wdata);

/**
 *  @brief                  IIC写数据
 *  @parameter              SlaveAddress：从机地址；  RomAddress：寄存器地址； number：写入数据的字节数；  *RamAddr：读出数据存放的地址
 *  @return_value           状态标志
 */
Uint16 ReadData( Uint16 SlaveAddress, Uint16 RomAddress,Uint16 number, Uint16  *RamAddr);


/**
 *  @brief                 IIC向寄存器的某一位写数据
 *  @parameter              SlaveAddress：从机地址；  regaddress：寄存器地址； bitNum：某一位；  data：写入数据
 *  @return_value           无
 */
void IICwriteBit(Uint16 slaveaddress, Uint16 regaddress, Uint8 bitNum, Uint8 data);

/**
 *  @brief                 IIC向寄存器的某几位写数据
 *  @parameter              SlaveAddress：从机地址；  regaddress：寄存器地址； bitStart：开始位；  length：长度；data：写入数据
 *  @return_value           无
 */
void IICwriteBits(Uint16 slaveaddress,Uint16 regaddress,Uint8 bitStart,Uint8 length,Uint8 data);


#endif /* HARDWARE_INCLUDE_AT24C01_H_ */
