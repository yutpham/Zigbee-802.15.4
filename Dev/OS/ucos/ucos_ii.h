/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                          (c) Copyright 1992-2006, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File    : uCOS_II.H
* By      : Jean J. Labrosse
* Version : V2.83
*********************************************************************************************************
*/

#ifndef   OS_uCOS_II_H
#define   OS_uCOS_II_H

#ifdef __cplusplus
extern "C" {
#endif

/*
*********************************************************************************************************
*                                          uC/OS-II VERSION NUMBER
*********************************************************************************************************
*/

#define  OS_VERSION                 283u                /* Version of uC/OS-II (Vx.yy mult. by 100)    */

/*
*********************************************************************************************************
*                                           INCLUDE HEADER FILES
*********************************************************************************************************
*/

#include <os_cpu.h>
#include <os_cfg_r.h>
/*
*********************************************************************************************************
*                                             MISCELLANEOUS
*********************************************************************************************************
*/

#ifdef   OS_GLOBALS
#define  OS_EXT
#else
#define  OS_EXT  extern
#endif

#ifndef  OS_FALSE
#define  OS_FALSE                     0u
#endif

#ifndef  OS_TRUE
#define  OS_TRUE                         1u
#endif

#define  OS_ASCII_NUL          (INT8U)0

#define  OS_PRIO_SELF              0xFFu                /* Indicate SELF priority                      */

#if OS_TASK_STAT_EN > 0
#define  OS_N_SYS_TASKS               2u                /* Number of system tasks                      */
#else
#define  OS_N_SYS_TASKS               1u
#endif

#define  OS_TASK_STAT_PRIO  (OS_LOWEST_PRIO - 1)        /* Statistic task priority                     */
#define  OS_TASK_IDLE_PRIO  (OS_LOWEST_PRIO)            /* IDLE      task priority                     */

#if OS_LOWEST_PRIO <= 63
#define  OS_EVENT_TBL_SIZE ((OS_LOWEST_PRIO) / 8 + 1)   /* Size of event table                         */
#define  OS_RDY_TBL_SIZE   ((OS_LOWEST_PRIO) / 8 + 1)   /* Size of ready table                         */
#else
#define  OS_EVENT_TBL_SIZE ((OS_LOWEST_PRIO) / 16 + 1)  /* Size of event table                         */
#define  OS_RDY_TBL_SIZE   ((OS_LOWEST_PRIO) / 16 + 1)  /* Size of ready table                         */
#endif

#define  OS_TASK_IDLE_ID          65535u                /* ID numbers for Idle, Stat and Timer tasks   */
#define  OS_TASK_STAT_ID          65534u
#define  OS_TASK_TMR_ID           65533u

#define  OS_EVENT_EN       (((OS_Q_EN > 0) && (OS_MAX_QS > 0)) || (OS_MBOX_EN > 0) || (OS_SEM_EN > 0) || (OS_MUTEX_EN > 0))

/*$PAGE*/
/*
*********************************************************************************************************
*                              TASK STATUS (Bit definition for OSTCBStat)
*********************************************************************************************************
*/
#define  OS_STAT_RDY               0x00u    /* Ready to run                                            */
#define  OS_STAT_SEM               0x01u    /* Pending on semaphore                                    */
#define  OS_STAT_MBOX              0x02u    /* Pending on mailbox                                      */
#define  OS_STAT_Q                 0x04u    /* Pending on queue                                        */
#define  OS_STAT_SUSPEND           0x08u    /* Task is suspended                                       */
#define  OS_STAT_MUTEX             0x10u    /* Pending on mutual exclusion semaphore                   */
#define  OS_STAT_FLAG              0x20u    /* Pending on event flag group                             */

#define  OS_STAT_PEND_ANY         (OS_STAT_SEM | OS_STAT_MBOX | OS_STAT_Q | OS_STAT_MUTEX | OS_STAT_FLAG)

/*
*********************************************************************************************************
*                                        OS_EVENT types
*********************************************************************************************************
*/
#define  OS_EVENT_TYPE_UNUSED         0u
#define  OS_EVENT_TYPE_MBOX           1u
#define  OS_EVENT_TYPE_Q              2u
#define  OS_EVENT_TYPE_SEM            3u
#define  OS_EVENT_TYPE_MUTEX          4u
#define  OS_EVENT_TYPE_FLAG           5u

#if OS_ZIGBEE_FRW  == 0
#define  OS_TMR_TYPE                  100u    /* Used to identify Timers ...                             */
                                            /* ... (Must be different value than OS_EVENT_TYPE_???)    */
#else
#define  OS_TMR_TYPE                  1
#endif

/*
*********************************************************************************************************
*                                         EVENT FLAGS
*********************************************************************************************************
*/
#define  OS_FLAG_WAIT_CLR_ALL         0u    /* Wait for ALL    the bits specified to be CLR (i.e. 0)   */
#define  OS_FLAG_WAIT_CLR_AND         0u

#define  OS_FLAG_WAIT_CLR_ANY         1u    /* Wait for ANY of the bits specified to be CLR (i.e. 0)   */
#define  OS_FLAG_WAIT_CLR_OR          1u

#define  OS_FLAG_WAIT_SET_ALL         2u    /* Wait for ALL    the bits specified to be SET (i.e. 1)   */
#define  OS_FLAG_WAIT_SET_AND         2u

#define  OS_FLAG_WAIT_SET_ANY         3u    /* Wait for ANY of the bits specified to be SET (i.e. 1)   */
#define  OS_FLAG_WAIT_SET_OR          3u


#define  OS_FLAG_CONSUME           0x80u    /* Consume the flags if condition(s) satisfied             */


#define  OS_FLAG_CLR                  0u
#define  OS_FLAG_SET                  1u

/*
*********************************************************************************************************
*                                   Values for OSTickStepState
*
* Note(s): This feature is used by uC/OS-View.
*********************************************************************************************************
*/

#if OS_TICK_STEP_EN > 0
#define  OS_TICK_STEP_DIS             0u    /* Stepping is disabled, tick runs as mormal               */
#define  OS_TICK_STEP_WAIT            1u    /* Waiting for uC/OS-View to set OSTickStepState to _ONCE  */
#define  OS_TICK_STEP_ONCE            2u    /* Process tick once and wait for next cmd from uC/OS-View */
#endif

/*
*********************************************************************************************************
*       Possible values for 'opt' argument of OSSemDel(), OSMboxDel(), OSQDel() and OSMutexDel()
*********************************************************************************************************
*/
#define  OS_DEL_NO_PEND               0u
#define  OS_DEL_ALWAYS                1u

/*
*********************************************************************************************************
*                                     OS???PostOpt() OPTIONS
*
* These #defines are used to establish the options for OSMboxPostOpt() and OSQPostOpt().
*********************************************************************************************************
*/
#define  OS_POST_OPT_NONE          0x00u    /* NO option selected                                      */
#define  OS_POST_OPT_BROADCAST     0x01u    /* Broadcast message to ALL tasks waiting                  */
#define  OS_POST_OPT_FRONT         0x02u    /* Post to highest priority task waiting                   */
#define  OS_POST_OPT_NO_SCHED      0x04u    /* Do not call the scheduler if this option is selected    */

/*
*********************************************************************************************************
*                                 TASK OPTIONS (see OSTaskCreateExt())
*********************************************************************************************************
*/
#define  OS_TASK_OPT_NONE        0x0000u    /* NO option selected                                      */
#define  OS_TASK_OPT_STK_CHK     0x0001u    /* Enable stack checking for the task                      */
#define  OS_TASK_OPT_STK_CLR     0x0002u    /* Clear the stack when the task is create                 */
#define  OS_TASK_OPT_SAVE_FP     0x0004u    /* Save the contents of any floating-point registers       */

/*
*********************************************************************************************************
*                            TIMER OPTIONS (see OSTmrStart() and OSTmrStop())
*********************************************************************************************************
*/
#define  OS_TMR_OPT_NONE              0u    /* No option selected                                      */

#define  OS_TMR_OPT_ONE_SHOT          1u    /* Timer will not automatically restart when it expires    */
#define  OS_TMR_OPT_PERIODIC          2u    /* Timer will     automatically restart when it expires    */

#define  OS_TMR_OPT_CALLBACK          3u    /* OSTmrStop() option to call 'callback' w/ timer arg.     */
#define  OS_TMR_OPT_CALLBACK_ARG      4u    /* OSTmrStop() option to call 'callback' w/ new   arg.     */

/*
*********************************************************************************************************
*                                            TIMER STATES
*********************************************************************************************************
*/
#define  OS_TMR_STATE_UNUSED          0u
#define  OS_TMR_STATE_STOPPED         1u
#define  OS_TMR_STATE_COMPLETED       2u
#define  OS_TMR_STATE_RUNNING         3u

/*
*********************************************************************************************************
*                                             ERROR CODES
*********************************************************************************************************
*/
#define OS_NO_ERR                     0u

#define OS_ERR_EVENT_TYPE             1u
#define OS_ERR_PEND_ISR               2u
#define OS_ERR_POST_NULL_PTR          3u
#define OS_ERR_PEVENT_NULL            4u
#define OS_ERR_POST_ISR               5u
#define OS_ERR_QUERY_ISR              6u
#define OS_ERR_INVALID_OPT            7u
#define OS_ERR_TASK_WAITING           8u
#define OS_ERR_PDATA_NULL             9u

#define OS_TIMEOUT                   10u
#define OS_TASK_NOT_EXIST            11u
#define OS_ERR_EVENT_NAME_TOO_LONG   12u
#define OS_ERR_FLAG_NAME_TOO_LONG    13u
#define OS_ERR_TASK_NAME_TOO_LONG    14u
#define OS_ERR_PNAME_NULL            15u
#define OS_ERR_TASK_CREATE_ISR       16u
#define OS_ERR_PEND_LOCKED           17u

#define OS_MBOX_FULL                 20u

#define OS_Q_FULL                    30u
#define OS_Q_EMPTY                   31u

#define OS_PRIO_EXIST                40u
#define OS_PRIO_ERR                  41u
#define OS_PRIO_INVALID              42u

#define OS_SEM_OVF                   50u

#define OS_TASK_DEL_ERR              60u
#define OS_TASK_DEL_IDLE             61u
#define OS_TASK_DEL_REQ              62u
#define OS_TASK_DEL_ISR              63u

#define OS_NO_MORE_TCB               70u

#define OS_TIME_NOT_DLY              80u
#define OS_TIME_INVALID_MINUTES      81u
#define OS_TIME_INVALID_SECONDS      82u
#define OS_TIME_INVALID_MILLI        83u
#define OS_TIME_ZERO_DLY             84u

#define OS_TASK_SUSPEND_PRIO         90u
#define OS_TASK_SUSPEND_IDLE         91u

#define OS_TASK_RESUME_PRIO         100u
#define OS_TASK_NOT_SUSPENDED       101u

#define OS_MEM_INVALID_PART         110u
#define OS_MEM_INVALID_BLKS         111u
#define OS_MEM_INVALID_SIZE         112u
#define OS_MEM_NO_FREE_BLKS         113u
#define OS_MEM_FULL                 114u
#define OS_MEM_INVALID_PBLK         115u
#define OS_MEM_INVALID_PMEM         116u
#define OS_MEM_INVALID_PDATA        117u
#define OS_MEM_INVALID_ADDR         118u
#define OS_MEM_NAME_TOO_LONG        119u

#define OS_ERR_NOT_MUTEX_OWNER      120u

#define OS_TASK_OPT_ERR             130u

#define OS_ERR_DEL_ISR              140u
#define OS_ERR_CREATE_ISR           141u

#define OS_FLAG_INVALID_PGRP        150u
#define OS_FLAG_ERR_WAIT_TYPE       151u
#define OS_FLAG_ERR_NOT_RDY         152u
#define OS_FLAG_INVALID_OPT         153u
#define OS_FLAG_GRP_DEPLETED        154u

#define OS_ERR_PIP_LOWER            160u

#define OS_ERR_TMR_INVALID_DLY      170u
#define OS_ERR_TMR_INVALID_PERIOD   171u
#define OS_ERR_TMR_INVALID_OPT      172u
#define OS_ERR_TMR_INVALID_NAME     173u
#define OS_ERR_TMR_NON_AVAIL        174u
#define OS_ERR_TMR_INACTIVE         175u
#define OS_ERR_TMR_INVALID_DEST     176u
#define OS_ERR_TMR_INVALID_TYPE     177u
#define OS_ERR_TMR_INVALID          178u
#define OS_ERR_TMR_ISR              179u
#define OS_ERR_TMR_NAME_TOO_LONG    180u
#define OS_ERR_TMR_INVALID_STATE    181u
#define OS_ERR_TMR_STOPPED          182u
#define OS_ERR_TMR_NO_CALLBACK      183u


/*
*********************************************************************************************************
*                                     MEMORY PARTITION DATA STRUCTURES
*********************************************************************************************************
*/

#if (OS_MEM_EN > 0) && (OS_MAX_MEM_PART > 0)
typedef struct os_mem {                   /* MEMORY CONTROL BLOCK                                      */
    void   *OSMemAddr;                    /* Pointer to beginning of memory partition                  */
    void   *OSMemFreeList;                /* Pointer to list of free memory blocks                     */
    INT32U  OSMemBlkSize;                 /* Size (in bytes) of each block of memory                   */
    INT32U  OSMemNBlks;                   /* Total number of blocks in this partition                  */
    INT32U  OSMemNFree;                   /* Number of memory blocks remaining in this partition       */
#if OS_MEM_NAME_SIZE > 1
    INT8U   OSMemName[OS_MEM_NAME_SIZE];  /* Memory partition name                                     */
#endif
} OS_MEM;


typedef struct os_mem_data {
    void   *OSAddr;                    /* Pointer to the beginning address of the memory partition     */
    void   *OSFreeList;                /* Pointer to the beginning of the free list of memory blocks   */
    INT32U  OSBlkSize;                 /* Size (in bytes) of each memory block                         */
    INT32U  OSNBlks;                   /* Total number of blocks in the partition                      */
    INT32U  OSNFree;                   /* Number of memory blocks free                                 */
    INT32U  OSNUsed;                   /* Number of memory blocks used                                 */
} OS_MEM_DATA;
#endif


/*$PAGE*/
/*
************************************************************************************************************************
*                                                   TIMER DATA TYPES
************************************************************************************************************************
*/

#if OS_TMR_EN > 0
typedef  void (*OS_TMR_CALLBACK)(void *ptmr, void *parg);


typedef  struct  os_tmr {
    OS_TMR_CALLBACK  OSTmrCallback;                   /* Function to call when timer expires                           */
    void            *OSTmrCallbackArg;                /* Argument to pass to function when timer expires               */
    void            *OSTmrNext;                       /* Double link list pointers                                     */
    void            *OSTmrPrev;
    INT32U           OSTmrMatch;                      /* Timer expires when OSTmrTime == OSTmrMatch                    */
#if OS_ZIGBEE_FRW  == 0
    INT32U           OSTmrDly;                        /* Delay time before periodic update starts                      */
    INT32U           OSTmrPeriod;                     /* Period to repeat timer                                        */
#else
    INT16U           OSTmrPeriod;                     /* Period to repeat timer                                        */
#endif //!OS_ZIGBEE_FRW  == 0
#if OS_TMR_CFG_NAME_SIZE > 0
    INT8U            OSTmrName[OS_TMR_CFG_NAME_SIZE]; /* Name to give the timer                                        */
#endif
    INT8U            OSTmrType  :2;                       /* Should be set to OS_TMR_TYPE                                  */
    INT8U            OSTmrOpt   :3;                        /* Options (see OS_TMR_OPT_???)                                  */
    INT8U            OSTmrState :3;                      /* Indicates the state of the timer:                             */
                                                      /*     OS_TMR_STATE_UNUSED                                       */
                                                      /*     OS_TMR_STATE_RUNNING                                      */
                                                      /*     OS_TMR_STATE_STOPPED                                      */
} OS_TMR;



typedef  struct  os_tmr_wheel {
    OS_TMR          *OSTmrFirst;                      /* Pointer to first timer in linked list                         */
    INT16U           OSTmrEntries;
} OS_TMR_WHEEL;
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/
OS_EXT  INT8U             OSIntNesting;             /* Interrupt nesting level                         */

#if (OS_MEM_EN > 0) && (OS_MAX_MEM_PART > 0)
OS_EXT  OS_MEM           *OSMemFreeList;            /* Pointer to free list of memory partitions       */
OS_EXT  OS_MEM            OSMemTbl[OS_MAX_MEM_PART];/* Storage for memory partition manager            */
#endif


#if OS_TMR_EN > 0
OS_EXT  INT16U            OSTmrFree;                /* Number of free entries in the timer pool        */
OS_EXT  INT16U            OSTmrUsed;                /* Number of timers used                           */
OS_EXT  INT32U            OSTmrTime;                /* Current timer time                              */

OS_EXT  OS_TMR            OSTmrTbl[OS_TMR_CFG_MAX]; /* Table containing pool of timers                 */
OS_EXT  OS_TMR           *OSTmrFreeList;            /* Pointer to free list of timers                  */
OS_EXT  OS_TMR_WHEEL      OSTmrWheelTbl[OS_TMR_CFG_WHEEL_SIZE];
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                                     (Target Independent Functions)
*********************************************************************************************************
*/


void          OS_MemClr             (INT8U           *pdest, 
                                     INT16U           size);

void          OS_MemCopy            (INT8U           *pdest, 
                                     INT8U           *psrc, 
                                     INT16U           size);

#if OS_EVENT_EN && (OS_EVENT_NAME_SIZE > 1)
INT8U         OSEventNameGet        (OS_EVENT        *pevent, 
                                     INT8U           *pname, 
                                     INT8U           *err);

void          OSEventNameSet        (OS_EVENT        *pevent, 
                                     INT8U           *pname, 
                                     INT8U           *err);
#endif

/*
*********************************************************************************************************
*                                           MEMORY MANAGEMENT
*********************************************************************************************************
*/

#if (OS_MEM_EN > 0) && (OS_MAX_MEM_PART > 0)

OS_MEM       *OSMemCreate           (void            *addr, 
                                     INT32U           nblks, 
                                     INT32U           blksize, 
                                     INT8U           *err);

void         *OSMemGet              (OS_MEM          *pmem, 
                                     INT8U           *err);
#if OS_MEM_NAME_SIZE > 1
INT8U         OSMemNameGet          (OS_MEM          *pmem, 
                                     INT8U           *pname, 
                                     INT8U           *err);

void          OSMemNameSet          (OS_MEM          *pmem, 
                                     INT8U           *pname, 
                                     INT8U           *err);
#endif
INT8U         OSMemPut              (OS_MEM          *pmem, 
                                     void            *pblk);

#if OS_MEM_QUERY_EN > 0
INT8U         OSMemQuery            (OS_MEM          *pmem, 
                                     OS_MEM_DATA     *p_mem_data);
#endif

#endif


/*
*********************************************************************************************************
*                                            TIMER MANAGEMENT
*********************************************************************************************************
*/

#if OS_TMR_EN > 0
OS_TMR      *OSTmrCreate            (INT32U           dly, 
                                     INT32U           period, 
                                     INT8U            opt, 
                                     OS_TMR_CALLBACK  callback, 
                                     void            *callback_arg, 
                                     INT8U           *name, 
                                     INT8U           *perr);

BOOLEAN      OSTmrDel               (OS_TMR          *ptmr,
                                     INT8U           *perr);

#if OS_TMR_CFG_NAME_SIZE > 0
INT8U        OSTmrNameGet           (OS_TMR          *ptmr, 
                                     INT8U           *pdest, 
                                     INT8U           *perr);
#endif
INT32U       OSTmrRemainGet         (OS_TMR          *ptmr, 
                                     INT8U           *perr);

INT8U        OSTmrStateGet          (OS_TMR          *ptmr, 
                                     INT8U           *perr);

BOOLEAN      OSTmrStart             (OS_TMR          *ptmr,
                                     INT8U           *perr);

BOOLEAN      OSTmrStop              (OS_TMR          *ptmr, 
                                     INT8U            opt, 
                                     void            *callback_arg, 
                                     INT8U           *perr);

INT8U        OSTmrSignal            (void);
void         OSTmr_Task             (void *p_arg);
#endif


/*
*********************************************************************************************************
*                                             TIMER MANAGEMENT
*********************************************************************************************************
*/

#ifndef OS_TMR_EN
#error  "OS_CFG.H, Missing OS_TMR_EN: When (1) enables code generation for Timer Management"
#elif   OS_TMR_EN > 0
    #if     (OS_SEM_EN == 0) && (OS_ZIGBEE_FRW == 0)
    #error  "OS_CFG.H, Semaphore management is required (set OS_SEM_EN to 1) when enabling Timer Management."
    #error  "          Timer management require TWO semaphores."
    #endif

    #ifndef OS_TMR_CFG_MAX
    #error  "OS_CFG.H, Missing OS_TMR_CFG_MAX: Determines the total number of timers in an application (2 .. 65500)"
    #else
        #if OS_TMR_CFG_MAX < 2
        #error  "OS_CFG.H, OS_TMR_CFG_MAX should be between 2 and 65500"
        #endif

        #if OS_TMR_CFG_MAX > 65500
        #error  "OS_CFG.H, OS_TMR_CFG_MAX should be between 2 and 65500"
        #endif
    #endif

    #ifndef OS_TMR_CFG_WHEEL_SIZE
    #error  "OS_CFG.H, Missing OS_TMR_CFG_WHEEL_SIZE: Sets the size of the timer wheel (1 .. 1023)"
    #else
        #if OS_TMR_CFG_WHEEL_SIZE < 2
        #error  "OS_CFG.H, OS_TMR_CFG_WHEEL_SIZE should be between 2 and 1024"
        #endif

        #if OS_TMR_CFG_WHEEL_SIZE > 1024
        #error  "OS_CFG.H, OS_TMR_CFG_WHEEL_SIZE should be between 2 and 1024"
        #endif
    #endif

    #ifndef OS_TMR_CFG_NAME_SIZE
    #error  "OS_CFG.H, Missing OS_TMR_CFG_NAME_SIZE: Determines the number of characters used for Timer names"
    #endif

    #ifndef OS_TMR_CFG_TICKS_PER_SEC
    #error  "OS_CFG.H, Missing OS_TMR_CFG_TICKS_PER_SEC: Determines the rate at which tiem timer management task will run (Hz)"
    #endif

#endif


#ifdef __cplusplus
}
#endif
#endif
