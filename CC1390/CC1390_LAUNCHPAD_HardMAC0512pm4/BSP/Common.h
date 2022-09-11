/*
 * Common.h
 *
 *  Created on: 2018. 3. 4.
 *      Author: Hello_DELL
 */


#ifndef BSP_COMMON_H_
#define BSP_COMMON_H_


/**********************************************************************************
                        Common Station State Table
**********************************************************************************/

typedef enum {
    COM_STA_STATE_RF_IDLE = 0,
    COM_STA_STATE_RF_BUSY
} Common_Station_State_Table;


/**********************************************************************************
                        Common Station Info Type Table
                        Common Station Info RF Mode Table
**********************************************************************************/
typedef enum {
    COM_STA_INFO_TYPE_AP = 0,
    COM_STA_INFO_TYPE_NODE,
} Common_Station_Info_Type_Table;

typedef enum {
    COM_STA_INFO_TYPE_RF_MODE_TRANS = 0,
    COM_STA_INFO_TYPE_RF_MODE_MAC,
} Common_Station_Info_RF_Mode_Table;


typedef struct {
    unsigned char operating_state;
} Common_Station_State;

typedef struct {
    unsigned char type_tbl_index;       // AP / Node
    unsigned char rf_mode_tbl_index;    // Transceiver / HSCT MAC
} Common_Station_Info;


#endif /* BSP_COMMON_H_ */
