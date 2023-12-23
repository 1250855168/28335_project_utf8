/*
 * distance_measurement.h
 *
 *  Created on: 2023年12月23日
 *      Author: ych
 */

#ifndef HARDWARE_INCLUDE_DISTANCE_MEASUREMENT_H_
#define HARDWARE_INCLUDE_DISTANCE_MEASUREMENT_H_


#include "DSP28x_Project.h"

/**
 * 初始化ECAP
 * 
*/
void Init_ECap1(void);

/**
 * 读取测量距离发送给串口
*/
void ReadDistance(void);


#endif /* HARDWARE_INCLUDE_DISTANCE_MEASUREMENT_H_ */
