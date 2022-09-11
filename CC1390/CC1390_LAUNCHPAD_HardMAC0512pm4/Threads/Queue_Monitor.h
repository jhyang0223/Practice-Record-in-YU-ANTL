/*
 * Queue_Monitor.h
 *
 *  Created on: 2018. 3. 1.
 *      Author: Hello_DELL
 */

#ifndef THREADS_QUEUE_MONITOR_H_
#define THREADS_QUEUE_MONITOR_H_


/* Standard C Libraries */
#include <pthread.h>    /* POSIX Header files */
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


void initQueueMonitor();



#endif /* THREADS_QUEUE_MONITOR_H_ */
