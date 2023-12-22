/*
 * epwm_led.h
 *
 *  Created on: 2023年12月2日
 *      Author: ych
 */

#ifndef INCLUDE_EPWM_LED_H_
#define INCLUDE_EPWM_LED_H_

#include"DSP2833x_Project.h"

#if (CPU_FRQ_150MHZ)
  #define CPU_CLK   150e6
#endif
#if (CPU_FRQ_100MHZ)
  #define CPU_CLK   100e6
#endif
#define PWM_CLK   10e1                // If diff freq. desired, change freq here.
#define SP        CPU_CLK/(2*PWM_CLK)
#define TBCTLVAL  0x200E              // Up-down cnt, timebase = SYSCLKOUT


void epwm1_led_init(void);


#endif /* INCLUDE_EPWM_LED_H_ */
