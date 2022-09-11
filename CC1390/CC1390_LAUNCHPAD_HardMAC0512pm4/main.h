/*
 * main.h
 *
 *  Created on: 2018. 2. 28.
 *      Author: Hello_DELL
 */

#ifndef MAIN_H_
#define MAIN_H_


/* Standard C Libraries */
/* TI Drivers */
/* Board Header files */
#include "BSP/Common.h"
#include "BSP/RF_Control_Interface.h"
#include "BSP/UART_Debug.h"
#include "BSP/Range_Extender.h"
#include "BSP/HCI.h"
/* Thread Header */
#include "Threads/MAC.h"
#include "Threads/CC1310_TRX.h"
#include "Threads/Cmd_Process.h"
#include "Threads/Queue_Monitor.h"
/* RTOS header files */
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>
#include "BSP/Board_CC1390_Launchpad.h"

/* TI RF Utils */
/* Driverlib Header files */


#define CONTOL_REQ_BUF_SIZE                 5 //CONTOL_BUF_SIZE

#define QUEUE_MONITOR_WAKE_EVT              Event_Id_00
#define CMD_PROCESS_WAKE_EVT                Event_Id_00
#define MAC_THREAD_IDLE_EVT                 Event_Id_00
#define MAC_THREAD_BUSY_TRANS_WAKE_EVT      Event_Id_01
#define MAC_THREAD_BUSY_MAC_WAKE_EVT        Event_Id_02


typedef struct {
    unsigned char mac_exe;
    RF_Params_Phy mac_rf;
    unsigned char mac_mode;
} Shared_MAC_Params;


typedef struct {
    Queue_Elem elem;
    HCI_Frame hci_frame;
} HCI_Frame_Queue_Entry;

typedef struct {
    Queue_Handle handle;
    Semaphore_Handle sema;
    int *size;
} HCI_Frame_Queue;

typedef struct {
    HCI_Frame_Queue cmd_req_queue;
    HCI_Frame_Queue cmd_res_queue;
    HCI_Frame_Queue mac_req_queue;
    HCI_Frame_Queue mac_res_queue;
    Event_Handle cmd_proc_th_wake_evt;
    Event_Handle queue_moni_th_wake_evt;
    Event_Handle mac_th_wake_evt;
    Shared_MAC_Params *shared_mac_params;
} Thread_Params;


#endif /* MAIN_H_ */
