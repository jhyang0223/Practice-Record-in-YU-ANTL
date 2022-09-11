/*
 *  ======== main ========
 */

#include "main.h"

int main(void)
{
    /* Call driver init functions */
    General_init();

    Semaphore_Params sema_params;
    Semaphore_Params_init(&sema_params);
    sema_params.mode = Semaphore_Mode_BINARY;

    Thread_Params thread_params;

    /**********************************************************************************
                                    Start Test Params
    **********************************************************************************/
    thread_params.shared_mac_params = (Shared_MAC_Params*)malloc(sizeof(Shared_MAC_Params));
    thread_params.shared_mac_params->mac_rf.freq_tbl_idx = RF_PARAM_PHY_FREQ_917MHZ;
    thread_params.shared_mac_params->mac_rf.dr_tbl_idx = RF_PARAM_PHY_DR_500KBPS;
    thread_params.shared_mac_params->mac_rf.tx_pwr_tbl_idx = RF_PARAM_PHY_RE_TXPWR_26DBM;
    thread_params.shared_mac_params->mac_mode = MAC_MODE_AP;
    thread_params.shared_mac_params->mac_exe = MAC_EXE_IDLE;


    /**********************************************************************************
                                    End Test Params
    **********************************************************************************/
    thread_params.cmd_req_queue.handle = Queue_create(NULL, NULL);
    thread_params.cmd_res_queue.handle = Queue_create(NULL, NULL);
    thread_params.mac_req_queue.handle = Queue_create(NULL, NULL);
    thread_params.mac_res_queue.handle = Queue_create(NULL, NULL);

    thread_params.cmd_req_queue.sema = Semaphore_create(1, &sema_params, NULL);
    thread_params.cmd_res_queue.sema = Semaphore_create(1, &sema_params, NULL);
    thread_params.mac_req_queue.sema = Semaphore_create(1, &sema_params, NULL);
    thread_params.mac_res_queue.sema = Semaphore_create(1, &sema_params, NULL);

    thread_params.queue_moni_th_wake_evt = Event_create(NULL, NULL);
    thread_params.cmd_proc_th_wake_evt = Event_create(NULL, NULL);
    thread_params.mac_th_wake_evt = Event_create(NULL, NULL);

    thread_params.cmd_req_queue.size = (int*)malloc(sizeof(int));
    *(thread_params.cmd_req_queue.size) = 0;
    thread_params.mac_req_queue.size = (int*)malloc(sizeof(int));
    *(thread_params.mac_req_queue.size) = 0;
    thread_params.cmd_res_queue.size = (int*)malloc(sizeof(int));
    *(thread_params.cmd_res_queue.size) = 0;
    thread_params.mac_res_queue.size = (int*)malloc(sizeof(int));
    *(thread_params.mac_res_queue.size) = 0;

    // GPIO
    GPIO_init();
    initLED();

    // UART
    UART_init();
    intDebug();

    // SPI
    SPI_init();
    initHCI();

    // RF
    initRFCtlIF();
    initRangeExtender();
    enableHGMRangeExtender();

    //initSPITRXThread(&thread_params);
    initMACThread(&thread_params);
    //initCmdProcess(&thread_params);
    //initQueueMonitor(&thread_params);

    BIOS_start();

    return (0);
}
