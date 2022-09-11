/*
 * CC1310_TRX.h
 *
 *  Created on: 2018. 2. 28.
 *      Author: Hello_DELL
 */

#ifndef CC1310_TRX_CC1310_TRX_H_
#define CC1310_TRX_CC1310_TRX_H_


/* Standard C Libraries */
#include <pthread.h>    /* POSIX Header files */
#include <stdlib.h>
#include <string.h>
/* TI Drivers */
/* Board Header files */
#include "main.h"
#include "BSP/GPIO_LED.h"
/* Thread Header */
/* RTOS header files */
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/BIOS.h>
/* TI RF Utils */
/* Driverlib Header files */
#include <ti/devices/cc13x0/driverlib/sys_ctrl.h>


void initSPITRXThread();



#endif /* CC1310_TRX_CC1310_TRX_H_ */
