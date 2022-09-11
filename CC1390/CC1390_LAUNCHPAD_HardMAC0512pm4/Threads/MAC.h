/*
 * MAC.h
 *
 *  Created on: 2018. 2. 28.
 *      Author: Hello_DELL
 */

#ifndef MAC_MAC_H_
#define MAC_MAC_H_


/* Standard C Libraries */
#include <stdio.h>
#include <pthread.h>    /* POSIX Header files */
/* TI Drivers */
/* Board Header files */
#include "main.h"
#include "BSP/GPIO_LED.h"
#include "BSP/RF_Control_Interface.h"
#include "BSP/UART_Debug.h"
/* Thread Header */
/* RTOS header files */
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <math.h>
#include <xdc/runtime/System.h> //for test
//#include <ti/sysbios/knl/Queue.h>
/* TI RF Utils */
/* Driverlib Header files */

/***************************************
                Defines
 ***************************************/
//#define TEST_NODE                 //Node
#define BASIC_BITRATE               RF_PARAM_PHY_DR_500KBPS

#define ST_ADDR                     11
#define AP_ADDR                     0xFFFE
#define BROAD_CAST_ADDR             0xFFFF

#define SUPER_FRAME_SIZE            100000  //us
#define GUARD_INTERVAL              1000
#define MINI_SLOT_SIZE              500     //us

/***************************************
          Type Define Structure
 ***************************************/
typedef struct {
    uint16_t frame_control;
    uint16_t src_addr;
    uint16_t dst_addr; //6byte
}MACHeader;

typedef struct {
    uint8_t mCSMA_position;
    uint8_t mCSMA_len;
    uint8_t mCSMA_slot_num;
}CSMASlotInformation;

typedef struct {
    uint16_t mTslotPosition;
    uint16_t startTDMAslot;
    uint16_t mTslotSize;
    uint8_t numTslot;
}TDMASlotInformation;

typedef struct{
    MACHeader MH;               //6byte
    uint8_t mBeacon_interval;   //1byte
    //CSMASlotInformation csi;  //3byte
    TDMASlotInformation tsi;
}BeaconFrame;

typedef struct {
    uint16_t info; //rx_seq_num
    uint8_t more_data; //no use
}Ack;

typedef struct {
    MACHeader MH;
    uint16_t seq_num; //tx_seq_num
    uint8_t more_flag;
    uint8_t dummy[241];
    Ack ack;
}DataFrame;

/***************************************
            Type Define enum
 ***************************************/
typedef enum{
    BeaconPhase = 0,
    TransmissionPhase,
}Control_Phase;

typedef enum {
    Beacon = 0,
    Data,       //1
}FrameType;

void initMACThread();

#endif /* MAC_MAC_H_ */
