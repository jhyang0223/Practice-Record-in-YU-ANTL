/*
 * RF_Control_Interface.c
 *
 *  Created on: 2018. 3. 2.
 *      Author: Hello_DELL
 */


#include "RF_Control_Interface.h"

RF_Packet_Recv_Queue pakcet_recv_queue;

static RF_Params rf_params;
static RF_Handle rf_handle;
static RF_Object rf_object;
static unsigned char cur_rf_dr;

static rfc_dataEntryGeneral_t *tx_data_entry, *cur_rx_data_entry;
static rfc_propRxOutput_t rx_statistics_prop; // Output structure for CMD_PROP_RX
static dataQueue_t rf_tx_data_queue, rf_rx_data_queue;

#pragma DATA_ALIGN (tx_data_entry_buffer, 4);
static uint8_t tx_data_entry_buffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_TX_DATA_ENTRIES, RF_PACKET_MAX_SIZE, NUM_TX_APPENDED_BYTES)];
#pragma DATA_ALIGN (rx_data_entry_buffer, 4);
static uint8_t rx_data_entry_buffer[RF_QUEUE_DATA_ENTRY_BUFFER_SIZE(NUM_RX_DATA_ENTRIES, RF_PACKET_MAX_SIZE, NUM_RX_APPENDED_BYTES)];

static Semaphore_Params rx_queue_sema_params;

void initRFCtlIF()
{
    RF_Params_init(&rf_params);

    if( RFQueue_defineQueue(&rf_tx_data_queue,
                            tx_data_entry_buffer,
                            sizeof(tx_data_entry_buffer),
                            NUM_TX_DATA_ENTRIES,
                            RF_PACKET_MAX_SIZE + NUM_TX_APPENDED_BYTES))
    {
        /* Failed to allocate space for all data entries */
        while(true);
    }
    RF_cmdTxHS.pQueue = &rf_tx_data_queue;
    RF_cmdTxHS.startTrigger.triggerType = TRIG_NOW;
    RF_cmdTxHS.startTrigger.pastTrig = 0;
    RF_cmdTxHS.startTime = 0;

    tx_data_entry = (rfc_dataEntryGeneral_t*)&tx_data_entry_buffer;

    if( RFQueue_defineQueue(&rf_rx_data_queue,
                            rx_data_entry_buffer,
                            sizeof(rx_data_entry_buffer),
                            NUM_RX_DATA_ENTRIES,
                            RF_PACKET_MAX_SIZE + NUM_RX_APPENDED_BYTES))
    {
        // Failed to allocate space for all data entries
        while(true);
    }

    RF_cmdPropRx.pQueue = &rf_rx_data_queue;
    RF_cmdPropRx.pOutput = (uint8_t*)&rx_statistics_prop;
    RF_cmdPropRx.maxPktLen = RF_PACKET_MAX_SIZE;
    RF_cmdPropRx.rxConf.bAutoFlushCrcErr = 1;
    RF_cmdPropRx.rxConf.bAutoFlushIgnored = 1;

    RF_cmdRxHS.pQueue = &rf_rx_data_queue;
    RF_cmdRxHS.maxPktLen = RF_PACKET_MAX_SIZE;
    RF_cmdRxHS.rxConf.bAutoFlushCrcErr = 1;

    RF_cmdNop.pNextOp = (rfc_radioOp_t*)&RF_cmdPropCs;
    RF_cmdPropCs.pNextOp = (rfc_radioOp_t*)&RF_cmdCountBranch;
    RF_cmdCountBranch.pNextOp = (rfc_radioOp_t*)&RF_cmdPropTx;
    RF_cmdCountBranch.pNextOpIfOk = (rfc_radioOp_t*)&RF_cmdPropCs;

    RF_cmdPropCs.rssiThr = RSSI_THRESHOLD_DBM;
    RF_cmdPropCs.csEndTime = IDLE_TIME_US * 4;
    RF_cmdCountBranch.counter = CS_RETRIES_WHEN_BUSY;

    rf_handle = RF_open(&rf_object, &RF_prop_FSK, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup_FSK, &rf_params);
    RF_postCmd(rf_handle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);
    cur_rf_dr = RF_PARAM_PHY_DR_50KBPS;

    Semaphore_Params_init(&rx_queue_sema_params);
    rx_queue_sema_params.mode = Semaphore_Mode_BINARY;

    pakcet_recv_queue.handle = Queue_create(NULL, NULL);
    pakcet_recv_queue.sema = Semaphore_create(1, &rx_queue_sema_params, NULL);
    pakcet_recv_queue.size = 0;
}

signed char isChannelIdle(unsigned int time)
{

    RF_cmdPropCs.startTime = time;
    RF_runCmd(rf_handle, (RF_Op*)&RF_cmdPropCs, RF_PriorityNormal, NULL, 0);

    if(RF_cmdPropCs.status == PROP_DONE_IDLE)
        return 1;
    else
        return 0;
}

void setPhyRFCtlIF(RF_Params_Phy phy_param)
{
    RF_close(rf_handle);

    switch(phy_param.freq_tbl_idx)
    {
    case RF_PARAM_PHY_FREQ_917MHZ:
        RF_cmdPropRadioDivSetup_FSK.centerFreq = 0x364;//0x0395;
        RF_cmdPropRadioDivSetup_LRM.centerFreq = 0x364;//0x0395;
        RF_cmdPropRadioDivSetup_SL_LRM.centerFreq = 0x364;//0x0395;
        RF_cmdFs.frequency = 0x364;//0x0395;
        RF_cmdFs.fractFreq = 0x0000;
        break;
    case RF_PARAM_PHY_FREQ_918MHZ:
        RF_cmdPropRadioDivSetup_FSK.centerFreq = 0x0396;
        RF_cmdPropRadioDivSetup_LRM.centerFreq = 0x0396;
        RF_cmdPropRadioDivSetup_SL_LRM.centerFreq = 0x0396;
        RF_cmdFs.frequency = 0x0396;
        RF_cmdFs.fractFreq = 0x0000;
        break;
    case RF_PARAM_PHY_FREQ_919MHZ:
        RF_cmdPropRadioDivSetup_FSK.centerFreq = 0x0397;
        RF_cmdPropRadioDivSetup_LRM.centerFreq = 0x0397;
        RF_cmdPropRadioDivSetup_SL_LRM.centerFreq = 0x0397;
        RF_cmdFs.frequency = 0x0397;
        RF_cmdFs.fractFreq = 0x0000;
        break;
    case RF_PARAM_PHY_FREQ_920MHZ:
        RF_cmdPropRadioDivSetup_FSK.centerFreq = 0x0398;
        RF_cmdPropRadioDivSetup_LRM.centerFreq = 0x0398;
        RF_cmdPropRadioDivSetup_SL_LRM.centerFreq = 0x0398;
        RF_cmdFs.frequency = 0x0398;
        RF_cmdFs.fractFreq = 0x0000;
        break;
    case RF_PARAM_PHY_FREQ_921MHZ:
        RF_cmdPropRadioDivSetup_FSK.centerFreq = 0x0399;
        RF_cmdPropRadioDivSetup_LRM.centerFreq = 0x0399;
        RF_cmdPropRadioDivSetup_SL_LRM.centerFreq = 0x0399;
        RF_cmdFs.frequency = 0x0399;
        RF_cmdFs.fractFreq = 0x0000;
        break;
    case RF_PARAM_PHY_FREQ_922MHZ:
        RF_cmdPropRadioDivSetup_FSK.centerFreq = 0x039A;
        RF_cmdPropRadioDivSetup_LRM.centerFreq = 0x039A;
        RF_cmdPropRadioDivSetup_SL_LRM.centerFreq = 0x039A;
        RF_cmdFs.frequency = 0x039A;
        RF_cmdFs.fractFreq = 0x0000;
        break;
    case RF_PARAM_PHY_FREQ_923MHZ:
        RF_cmdPropRadioDivSetup_FSK.centerFreq = 0x039B;
        RF_cmdPropRadioDivSetup_LRM.centerFreq = 0x039B;
        RF_cmdPropRadioDivSetup_SL_LRM.centerFreq = 0x039B;
        RF_cmdFs.frequency = 0x039B;
        RF_cmdFs.fractFreq = 0x0000;
        break;
    }

    switch(phy_param.tx_pwr_tbl_idx)
    {
    case RF_PARAM_PHY_RE_TXPWR_7DBM:
        enableHGMRangeExtender();
        RF_cmdPropRadioDivSetup_FSK.txPower = 0x00C0;
        RF_cmdPropRadioDivSetup_LRM.txPower = 0x00C0;
        RF_cmdPropRadioDivSetup_SL_LRM.txPower = 0x00C0;
        break;
    case RF_PARAM_PHY_RE_TXPWR_14DBM:
        enableHGMRangeExtender();
        RF_cmdPropRadioDivSetup_FSK.txPower = 0x00C1;
        RF_cmdPropRadioDivSetup_LRM.txPower = 0x00C1;
        RF_cmdPropRadioDivSetup_SL_LRM.txPower = 0x00C1;
        break;
    case RF_PARAM_PHY_RE_TXPWR_18DBM:
        enableHGMRangeExtender();
        RF_cmdPropRadioDivSetup_FSK.txPower = 0x00C2;
        RF_cmdPropRadioDivSetup_LRM.txPower = 0x00C2;
        RF_cmdPropRadioDivSetup_SL_LRM.txPower = 0x00C2;
        break;
    case RF_PARAM_PHY_RE_TXPWR_20DBM:
        enableHGMRangeExtender();
        RF_cmdPropRadioDivSetup_FSK.txPower = 0x00C3;
        RF_cmdPropRadioDivSetup_LRM.txPower = 0x00C3;
        RF_cmdPropRadioDivSetup_SL_LRM.txPower = 0x00C3;
        break;
    case RF_PARAM_PHY_RE_TXPWR_22DBM:
        enableHGMRangeExtender();
        RF_cmdPropRadioDivSetup_FSK.txPower = 0x00C4;
        RF_cmdPropRadioDivSetup_LRM.txPower = 0x00C4;
        RF_cmdPropRadioDivSetup_SL_LRM.txPower = 0x00C4;
        break;
    case RF_PARAM_PHY_RE_TXPWR_23DBM:
        enableHGMRangeExtender();
        RF_cmdPropRadioDivSetup_FSK.txPower = 0x00C5;
        RF_cmdPropRadioDivSetup_LRM.txPower = 0x00C5;
        RF_cmdPropRadioDivSetup_SL_LRM.txPower = 0x00C5;
        break;
    case RF_PARAM_PHY_RE_TXPWR_24DBM:
        enableHGMRangeExtender();
        RF_cmdPropRadioDivSetup_FSK.txPower = 0x00C6;
        RF_cmdPropRadioDivSetup_LRM.txPower = 0x00C6;
        RF_cmdPropRadioDivSetup_SL_LRM.txPower = 0x00C6;
        break;
    case RF_PARAM_PHY_RE_TXPWR_25DBM:
        enableHGMRangeExtender();
        RF_cmdPropRadioDivSetup_FSK.txPower = 0x00C9;
        RF_cmdPropRadioDivSetup_LRM.txPower = 0x00C9;
        RF_cmdPropRadioDivSetup_SL_LRM.txPower = 0x00C9;
        break;
    case RF_PARAM_PHY_RE_TXPWR_26DBM:
        enableHGMRangeExtender();
        RF_cmdPropRadioDivSetup_FSK.txPower = 0x00CE;
        RF_cmdPropRadioDivSetup_LRM.txPower = 0x00CE;
        RF_cmdPropRadioDivSetup_SL_LRM.txPower = 0x00CE;
        break;
    }

    switch(phy_param.dr_tbl_idx)
    {
        /*
    case RF_PARAM_PHY_DR_625BPS:
        RF_cmdPropTx.syncWord = 0x00000000;
        RF_cmdPropRx.syncWord = 0x00000000;

        rf_handle = RF_open(&rf_object, &RF_prop_LRM, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup_LRM, &rf_params);
        RF_postCmd(rf_handle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

        cur_rf_dr = RF_PARAM_PHY_DR_625BPS;
        break;
    case RF_PARAM_PHY_DR_2_5KBPS:
        RF_cmdPropTx.syncWord = 0x00000000;
        RF_cmdPropRx.syncWord = 0x00000000;

        RF_cmdPropRadioDivSetup_SL_LRM.pRegOverride = pOverrides_2_5Kbps;

        rf_handle = RF_open(&rf_object, &RF_prop_SL_LRM, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup_SL_LRM, &rf_params);
        RF_postCmd(rf_handle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

        cur_rf_dr = RF_PARAM_PHY_DR_2_5KBPS;
        break;
        */
    case RF_PARAM_PHY_DR_50KBPS:
        RF_cmdPropTx.syncWord = 0x930B51DE;
        RF_cmdPropRx.syncWord = 0x930B51DE;

        RF_cmdPropRadioDivSetup_FSK.modulation.deviation = 0x64;
        RF_cmdPropRadioDivSetup_FSK.symbolRate.preScale = 0xF;
        RF_cmdPropRadioDivSetup_FSK.symbolRate.rateWord = 0x8000;
        RF_cmdPropRadioDivSetup_FSK.rxBw = 0x24;
        RF_cmdPropRadioDivSetup_FSK.pRegOverride = pOverrides_50Kbps;

        rf_handle = RF_open(&rf_object, &RF_prop_FSK, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup_FSK, &rf_params);
        RF_postCmd(rf_handle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

        cur_rf_dr = RF_PARAM_PHY_DR_50KBPS;
        break;
    case RF_PARAM_PHY_DR_100KBPS:
        RF_cmdPropTx.syncWord = 0x930B51DE;
        RF_cmdPropRx.syncWord = 0x930B51DE;

        RF_cmdPropRadioDivSetup_FSK.modulation.deviation = 0xC8;
        RF_cmdPropRadioDivSetup_FSK.symbolRate.preScale = 0xF;
        RF_cmdPropRadioDivSetup_FSK.symbolRate.rateWord = 0x10000;
        RF_cmdPropRadioDivSetup_FSK.rxBw = 0x27;
        RF_cmdPropRadioDivSetup_FSK.pRegOverride = pOverrides_100Kbps;

        rf_handle = RF_open(&rf_object, &RF_prop_FSK, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup_FSK, &rf_params);
        RF_postCmd(rf_handle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

        cur_rf_dr = RF_PARAM_PHY_DR_100KBPS;
        break;
    case RF_PARAM_PHY_DR_200KBPS:
        RF_cmdPropTx.syncWord = 0x930B51DE;
        RF_cmdPropRx.syncWord = 0x930B51DE;

        RF_cmdPropRadioDivSetup_FSK.modulation.deviation = 0x118;
        RF_cmdPropRadioDivSetup_FSK.symbolRate.preScale = 0xF;
        RF_cmdPropRadioDivSetup_FSK.symbolRate.rateWord = 0x20000;
        RF_cmdPropRadioDivSetup_FSK.rxBw = 0x29;
        RF_cmdPropRadioDivSetup_FSK.pRegOverride = pOverrides_200Kbps;

        rf_handle = RF_open(&rf_object, &RF_prop_FSK, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup_FSK, &rf_params);
        RF_postCmd(rf_handle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

        cur_rf_dr = RF_PARAM_PHY_DR_200KBPS;
        break;
    case RF_PARAM_PHY_DR_300KBPS:
        RF_cmdPropTx.syncWord = 0x930B51DE;
        RF_cmdPropRx.syncWord = 0x930B51DE;

        RF_cmdPropRadioDivSetup_FSK.modulation.deviation = 0x1A4;
        RF_cmdPropRadioDivSetup_FSK.symbolRate.preScale = 0x5;
        RF_cmdPropRadioDivSetup_FSK.symbolRate.rateWord = 0x10000;
        RF_cmdPropRadioDivSetup_FSK.rxBw = 0x2C;
        RF_cmdPropRadioDivSetup_FSK.pRegOverride = pOverrides_300Kbps;

        rf_handle = RF_open(&rf_object, &RF_prop_FSK, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup_FSK, &rf_params);
        RF_postCmd(rf_handle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

        cur_rf_dr = RF_PARAM_PHY_DR_300KBPS;
        break;
    case RF_PARAM_PHY_DR_400KBPS:
        RF_cmdPropTx.syncWord = 0x930B51DE;
        RF_cmdPropRx.syncWord = 0x930B51DE;

        RF_cmdPropRadioDivSetup_FSK.modulation.deviation = 0x230;
        RF_cmdPropRadioDivSetup_FSK.symbolRate.preScale = 0xF;
        RF_cmdPropRadioDivSetup_FSK.symbolRate.rateWord = 0x40000;
        RF_cmdPropRadioDivSetup_FSK.rxBw = 0x2C;
        RF_cmdPropRadioDivSetup_FSK.pRegOverride = pOverrides_400Kbps;

        rf_handle = RF_open(&rf_object, &RF_prop_FSK, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup_FSK, &rf_params);
        RF_postCmd(rf_handle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

        cur_rf_dr = RF_PARAM_PHY_DR_400KBPS;
        break;
    case RF_PARAM_PHY_DR_500KBPS:
        RF_cmdPropTx.syncWord = 0x930B51DE;
        RF_cmdPropRx.syncWord = 0x930B51DE;

        RF_cmdPropRadioDivSetup_FSK.modulation.deviation = 0x2BC;
        RF_cmdPropRadioDivSetup_FSK.symbolRate.preScale = 0x6;
        RF_cmdPropRadioDivSetup_FSK.symbolRate.rateWord = 0x20000;
        RF_cmdPropRadioDivSetup_FSK.rxBw = 0x2F;
        RF_cmdPropRadioDivSetup_FSK.pRegOverride = pOverrides_500Kbps;

        rf_handle = RF_open(&rf_object, &RF_prop_FSK, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup_FSK, &rf_params);
        RF_postCmd(rf_handle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

        cur_rf_dr = RF_PARAM_PHY_DR_500KBPS;
        break;
    case RF_PARAM_PHY_DR_4MBPS:
        rf_handle = RF_open(&rf_object, &RF_prop_HSM, (RF_RadioSetup*)&RF_cmdRadioSetup_HSM, &rf_params);
        RF_postCmd(rf_handle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

        cur_rf_dr = RF_PARAM_PHY_DR_4MBPS;
        break;
    }
}

void txRFCtlIF(char *buf, int buf_size, unsigned int start_delay)
{
    switch(cur_rf_dr)
    {
    //case RF_PARAM_PHY_DR_625BPS:
    //case RF_PARAM_PHY_DR_2_5KBPS:
    case RF_PARAM_PHY_DR_50KBPS:
    case RF_PARAM_PHY_DR_100KBPS:
    case RF_PARAM_PHY_DR_200KBPS:
    case RF_PARAM_PHY_DR_300KBPS:
    case RF_PARAM_PHY_DR_400KBPS:
    case RF_PARAM_PHY_DR_500KBPS:
    {
        RF_cmdPropTx.pktLen = buf_size;
        RF_cmdPropTx.pPkt = (uint8_t*)buf;

        if(start_delay == NULL)
        {
            RF_cmdPropTx.startTrigger.triggerType = TRIG_NOW;
            RF_cmdPropTx.startTrigger.pastTrig = 0;
            RF_cmdPropTx.startTime = 0;
        }
        else
        {
            RF_cmdPropTx.startTrigger.triggerType = TRIG_ABSTIME;
            RF_cmdPropTx.startTrigger.pastTrig = 1;
            RF_cmdPropTx.startTime = start_delay;
        }
        RF_runCmd(rf_handle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal, NULL, 0);
        break;
    }
    case RF_PARAM_PHY_DR_4MBPS:
    {
        tx_data_entry->length = buf_size;
        memcpy(&tx_data_entry->data, buf, tx_data_entry->length);

        if(start_delay == NULL)
        {
            RF_cmdTxHS.startTrigger.triggerType = TRIG_NOW;
            RF_cmdTxHS.startTrigger.pastTrig = 0;
            RF_cmdTxHS.startTime = 0;
        }
        else
        {
            RF_cmdTxHS.startTrigger.triggerType = TRIG_ABSTIME;
            RF_cmdTxHS.startTrigger.pastTrig = 1;
            RF_cmdTxHS.startTime = start_delay;
        }
        RF_runCmd(rf_handle, (RF_Op*)&RF_cmdTxHS, RF_PriorityNormal, NULL, 0);
        break;
    }
    }
}

void txCSMACallbackRFCtlIF(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
    if ((e & RF_EventLastCmdDone) && (RF_cmdPropTx.status == PROP_DONE_OK))
    {
        csma_tx_ok = 1;
    }
}

void txCSMARFCtlIF(char *buf, int buf_size, unsigned int start_delay)
{
    if(start_delay == NULL)
    {
        RF_cmdNop.startTrigger.triggerType = TRIG_NOW;
        RF_cmdNop.startTrigger.pastTrig = 0;
        RF_cmdNop.startTime = 0;
    }
    else
    {
        RF_cmdNop.startTrigger.triggerType = TRIG_ABSTIME;
        RF_cmdNop.startTrigger.pastTrig = 1;
        RF_cmdNop.startTime = start_delay;
    }

    RF_cmdPropTx.startTrigger.triggerType = TRIG_NOW;
    RF_cmdPropTx.startTrigger.pastTrig = 0;
    RF_cmdPropTx.startTime = 0;
    RF_cmdPropTx.pktLen = buf_size;
    RF_cmdPropTx.pPkt = (uint8_t*)buf;

    RF_runCmd(rf_handle, (RF_Op*)&RF_cmdNop, RF_PriorityNormal, &txCSMACallbackRFCtlIF, 0);
}

void rxCallbackRFCtlIF(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
    if (e & RF_EventRxEntryDone)
    {
        cur_rx_data_entry = RFQueue_getDataEntry();

        RF_Packet_Recv_Queue_Entry *packet_entry = (RF_Packet_Recv_Queue_Entry*)malloc(sizeof(RF_Packet_Recv_Queue_Entry));

        switch(cur_rf_dr)
        {
        //case RF_PARAM_PHY_DR_625BPS:
        //case RF_PARAM_PHY_DR_2_5KBPS:
        case RF_PARAM_PHY_DR_50KBPS:
        case RF_PARAM_PHY_DR_100KBPS:
        case RF_PARAM_PHY_DR_200KBPS:
        case RF_PARAM_PHY_DR_300KBPS:
        case RF_PARAM_PHY_DR_400KBPS:
        case RF_PARAM_PHY_DR_500KBPS:
        {
            packet_entry->packet_len = *(uint8_t*)(&cur_rx_data_entry->data);
            uint8_t* packet_data_pointer = (uint8_t*)(&cur_rx_data_entry->data + 1);
            packet_entry->packet = (char*)malloc(packet_entry->packet_len);
            memcpy(packet_entry->packet, packet_data_pointer, packet_entry->packet_len);
            memcpy(&(packet_entry->rssi), packet_data_pointer + packet_entry->packet_len, sizeof(char));
            break;
        }
        case RF_PARAM_PHY_DR_4MBPS:
        {
            packet_entry->packet_len = ((*(uint8_t*)(&cur_rx_data_entry->data + 1)) << 8) | (*(uint8_t*)(&cur_rx_data_entry->data));
            uint8_t* packet_data_pointer = (uint8_t*)(&cur_rx_data_entry->data + 2);
            packet_entry->packet = (char*)malloc(packet_entry->packet_len);
            memcpy(packet_entry->packet, packet_data_pointer, packet_entry->packet_len);
            memcpy(&(packet_entry->rssi), packet_data_pointer + packet_entry->packet_len, sizeof(char));
            break;
        }
        }

        Semaphore_pend(pakcet_recv_queue.sema, BIOS_WAIT_FOREVER);
        Queue_enqueue(pakcet_recv_queue.handle, &(packet_entry->elem));
        pakcet_recv_queue.size++;
        Semaphore_post(pakcet_recv_queue.sema);

        RFQueue_nextEntry();
    }
}

void rxRFCtlIF(RF_CmdHandle *cmd_handle, unsigned int end_time, RX_RF_Mode rx_mode)
{
    switch(cur_rf_dr)
    {
    //case RF_PARAM_PHY_DR_625BPS:
    //case RF_PARAM_PHY_DR_2_5KBPS:
    case RF_PARAM_PHY_DR_50KBPS:
    case RF_PARAM_PHY_DR_100KBPS:
    case RF_PARAM_PHY_DR_200KBPS:
    case RF_PARAM_PHY_DR_300KBPS:
    case RF_PARAM_PHY_DR_400KBPS:
    case RF_PARAM_PHY_DR_500KBPS:
    {
        if((end_time == NULL) && (rx_mode == RUN))
        {
            RF_cmdPropRx.pktConf.bRepeatOk = 0;
            RF_cmdPropRx.pktConf.bRepeatNok = 0;
            RF_cmdPropRx.endTrigger.triggerType = TRIG_NEVER;
            RF_cmdPropRx.endTime = 0;
        }
        else if((end_time == NULL) && (rx_mode == POST))
        {
            RF_cmdPropRx.pktConf.bRepeatOk = 1;
            RF_cmdPropRx.pktConf.bRepeatNok = 1;
            RF_cmdPropRx.endTrigger.triggerType = TRIG_NEVER;
            RF_cmdPropRx.endTime = 0;
        }
        if((end_time != NULL) && (rx_mode == RUN))
        {
            RF_cmdPropRx.pktConf.bRepeatOk = 1;
            RF_cmdPropRx.pktConf.bRepeatNok = 1;
            RF_cmdPropRx.endTrigger.triggerType = TRIG_ABSTIME;
            RF_cmdPropRx.endTime = end_time;
        }

        if(rx_mode == RUN)
            RF_runCmd(rf_handle, (RF_Op*)&RF_cmdPropRx, RF_PriorityNormal, &rxCallbackRFCtlIF, RF_EventRxEntryDone);
        else if(rx_mode == POST)
            *cmd_handle = RF_postCmd(rf_handle, (RF_Op*)&RF_cmdPropRx, RF_PriorityNormal, &rxCallbackRFCtlIF, RF_EventRxEntryDone);
        break;
    }
    case RF_PARAM_PHY_DR_4MBPS:
    {
        if((end_time == NULL) && (rx_mode == RUN))
        {
            RF_cmdRxHS.pktConf.bRepeatOk = 0;
            RF_cmdRxHS.pktConf.bRepeatNok = 0;
            RF_cmdRxHS.endTrigger.triggerType = TRIG_NEVER;
            RF_cmdRxHS.endTime = 0;
        }
        else if((end_time == NULL) && (rx_mode == POST))
        {
            RF_cmdRxHS.pktConf.bRepeatOk = 1;
            RF_cmdRxHS.pktConf.bRepeatNok = 1;
            RF_cmdRxHS.endTrigger.triggerType = TRIG_NEVER;
            RF_cmdRxHS.endTime = 0;
        }
        if((end_time != NULL) && (rx_mode == RUN))
        {
            RF_cmdRxHS.pktConf.bRepeatOk = 1;
            RF_cmdRxHS.pktConf.bRepeatNok = 1;
            RF_cmdRxHS.endTrigger.triggerType = TRIG_ABSTIME;
            RF_cmdRxHS.endTime = end_time;
        }

        if(rx_mode == RUN)
            RF_runCmd(rf_handle, (RF_Op*)&RF_cmdRxHS, RF_PriorityNormal, &rxCallbackRFCtlIF, RF_EventRxEntryDone);
        else if(rx_mode == POST)
            *cmd_handle = RF_postCmd(rf_handle, (RF_Op*)&RF_cmdRxHS, RF_PriorityNormal, &rxCallbackRFCtlIF, RF_EventRxEntryDone);
        break;
    }
    }
}

void rxRFCancelCtlIF(RF_CmdHandle *cmd_handle)
{
    RF_cancelCmd(rf_handle, *cmd_handle, 0);
    RF_pendCmd(rf_handle, *cmd_handle, RF_EventRxEntryDone);
}

void dealyRFCtlIF(unsigned int delay)
{
    RF_cmdNop_delay.startTrigger.triggerType = TRIG_ABSTIME;
    RF_cmdNop_delay.startTrigger.pastTrig = 1;
    RF_cmdNop_delay.startTime = delay;
    RF_runCmd(rf_handle, (RF_Op*)&RF_cmdNop_delay, RF_PriorityNormal, NULL, 0);
}

char getRSSIRfCtlIF()
{
    return RF_getRssi(rf_handle);
}

void initRXResultRFCtlIF()
{
    rx_statistics_prop.nRxOk = 0;
    rx_statistics_prop.nRxNok = 0;
}

RF_Packet_Recv_Info getRXResultRFCtlIF()
{
    RF_Packet_Recv_Info rx_info;

    rx_info.crc_ok = rx_statistics_prop.nRxOk;
    rx_info.crc_nok = rx_statistics_prop.nRxNok;
    return rx_info;
}

unsigned char getLQI()
{
    RF_runImmediateCmd(rf_handle, (uint32_t*)&RF_cmdReadLQI);
    return RF_cmdReadLQI.value;
}
