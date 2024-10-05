/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-2006, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                  uC/OS-II Configuration File for V2.8x
*
* File    : OS_CFG.H
* By      : Jean J. Labrosse
* Version : V2.83
*********************************************************************************************************
*/

#ifndef OS_CFG_H
#define OS_CFG_H

                                       /* ---------------------- MISCELLANEOUS -----------------------   */
#define OS_ARG_CHK_EN             0    /* Enable (1) or Disable (0) argument checking                    */

                                       /* --------------------- MEMORY MANAGEMENT --------------------   */
#define OS_MEM_EN                 1    /* Enable (1) or Disable (0) code generation for MEMORY MANAGER   */
#define OS_MEM_QUERY_EN           0    /*     Include code for OSMemQuery()                              */
#define OS_MEM_NAME_SIZE          0    /*     Determine the size of a memory partition name              */
#define OS_MAX_MEM_PART           6    /* Max. number of memory partitions                               */

                                       /* --------------------- TIMER MANAGEMENT ---------------------   */
#define OS_TMR_EN                 1    /* Enable (1) or Disable (0) code generation for TIMERS           */
#define OS_TMR_CFG_MAX            16    /*    Maximum number of timers                                   */
#define OS_TMR_CFG_NAME_SIZE      0    /*     Determine the size of a timer name                         */
#define OS_TMR_CFG_WHEEL_SIZE     8    /*     Size of timer wheel (#Spokes)                              */
#define OS_TMR_CFG_TICKS_PER_SEC 10    /*     Rate at which timer management task runs (Hz)              */


// Start; 12/4/2006 
#define OS_TMR_EN				 1
#define OS_ZIGBEE_FRW            1   
#define OS_TASK_TMR_PRIO 		 1
#define OS_GLOBALS
// End: 12/4/2006 

#endif
