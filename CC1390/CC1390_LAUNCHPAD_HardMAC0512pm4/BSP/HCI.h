/*
 * CC1310_Control_Interface.h
 *
 *  Created on: 2018. 2. 19.
 *      Author: Hello_DELL
 */

#ifndef BSP_HCI_H_
#define BSP_HCI_H_


/* Standard C Libraries */
/* TI Drivers */
/* Board Header files */
#include "Board_CC1390_Launchpad.h"
#include "RF_Control_Interface.h"
/* Thread Header */
/* RTOS header files */
/* TI RF Utils */
/* Driverlib Header files */


/**********************************************************************************
                        HCI Frame - Start byte(2Bytes)
**********************************************************************************/
#define HCI_FRAME_START_BYTE1           0xAA
#define HCI_FRAME_START_BYTE2           0x7E

/**********************************************************************************
                        HCI Frame Control Field(1Byte)
    1bytes = Frame Type(3Bits) + Command Type(2bits) + Command Sub-Type(3bits)
**********************************************************************************/
// Frame Type(3bits)
#define HCI_FRAME_FC_FT_REQ_GET         0b000
#define HCI_FRAME_FC_FT_RES_GET         0b001
#define HCI_FRAME_FC_FT_REQ_SET         0b010
#define HCI_FRAME_FC_FT_RES_SET         0b011
#define HCI_FRAME_FC_FT_NACK            0b100
#define HCI_FRAME_FC_FT_DUMMY           0b111

// Command Type(2bits)
#define HCI_FRAME_FC_CT_SYS             0b00
#define HCI_FRAME_FC_CT_MAC_PARAM       0b01
#define HCI_FRAME_FC_CT_MAC_TRX         0b10

// Command Sub Type(3bits)
// SYS
#define HCI_FRAME_FC_CST_SYS_RESET      0b000

// MAC
#define HCI_FRAME_FC_CST_MAC_EXE        0b000
#define HCI_FRAME_FC_CST_MAC_RF         0b001
#define HCI_FRAME_FC_CST_MAC_MODE       0b010

// MAC TRX
#define HCI_FRAME_FC_CST_MAC_TX         0b000
#define HCI_FRAME_FC_CST_MAC_RX         0b001

typedef enum {
    MAC_EXE_IDLE = 0,
    MAC_EXE_BUSY
} MAC_EXE;

typedef enum {
    MAC_MODE_AP = 0,
    MAC_MODE_NODE
} MAC_MODE;

/**********************************************************************************
                        ACK Set Response Type
**********************************************************************************/
#define HCI_FRAME_RES_OK                0
#define HCI_FRAME_NACK_BUF_FULL         1


/**********************************************************************************
                        HCI Frame
**********************************************************************************/

#define HCI_FRAME_SOF_SIZE              2
#define HCI_FRAME_FC_SIZE               1
#define HCI_FRAME_LENGTH_SIZE           1
#define HCI_FRAME_PAYLOAD_SIZE          255
#define HCI_FRAME_SIZE                  HCI_FRAME_SOF_SIZE + HCI_FRAME_FC_SIZE + HCI_FRAME_LENGTH_SIZE + HCI_FRAME_PAYLOAD_SIZE

#define HCI_FRAME_DUMMY_BYTE            0xFF


typedef struct {
    unsigned char frame_type : 3;
    unsigned char cmd_type : 2;
    unsigned char cmd_sub_type : 3;
} HCI_Frame_FC;

typedef struct {
    char sof[HCI_FRAME_SOF_SIZE];
    HCI_Frame_FC fc;
    unsigned char length;
    char payload[HCI_FRAME_PAYLOAD_SIZE];
} HCI_Frame;

void initHCI();
void resetHCI();
void rwHCI(char *tx_buff, char *rx_buff, int size);
void pudnHCI();


#endif /* BSP_HCI_H_ */
