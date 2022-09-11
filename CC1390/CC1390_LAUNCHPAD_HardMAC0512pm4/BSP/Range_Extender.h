/*
 * Range_Extender.h
 *
 *  Created on: 2018. 3. 14.
 *      Author: Hello_DELL
 */

#ifndef BSP_RANGE_EXTENDER_H_
#define BSP_RANGE_EXTENDER_H_


/* Standard C Libraries */
/* TI Drivers */
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
/* Board Header files */
#include "Board_CC1390_Launchpad.h"
/* Thread Header */
/* RTOS header files */
/* TI RF Utils */
/* Driverlib Header files */


void initRangeExtender();
void enableHGMRangeExtender();
void disableHGMRangeExtender();


#endif /* BSP_RANGE_EXTENDER_H_ */
