/*
 * sci.h
 *
 *  Created on: 2023年12月1日
 *      Author: ych
 */

#ifndef SRC_SCI_H_
#define SRC_SCI_H_

#include"DSP2833x_Project.h"


void Sci_Init(void);
void scia_xmit(int a);//发送字节的函数
void scia_msg(char * msg);//发送数组的函数


#endif /* SRC_SCI_H_ */
