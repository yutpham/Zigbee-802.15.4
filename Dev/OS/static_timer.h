/*******************************************************************************/
//Module: OS                                                                   */   
//File Name: static_timer.h                                                    */
//Description: This file contains the type definitions for static_timer.c      */
//  -------------------------------------------------------------              */
//Revision History:                                                            */
//Date              Programmer      Details                                    */
//11/13/2006        pnthai          Create                                     */
//  -------------------------------------------------------------              */
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/ 
#ifndef MAC_TIMER_H
#define MAC_TIMER_H

/************************************************************************/
/*                  Include files                                       */
/************************************************************************/
#include "types.h"
/************************************************************************/
/*                  Type Definition                                     */
/************************************************************************/

/*-----------------------------------------------------------------------------
                        STATIC PERIODICAL TIMER

    To optimize the operation time, timer is pre-created at initialization
----------------------------------------------------------------------------- */
typedef enum  
{

    scanTimeout = 0x00,
    forceOffTrx,
    runAssoDataReq,
    timeoutAssoDataRes,
    createSyncBeaconTask,
    createTimeoutSyncBncTask,
    locateBeaconTimeout,
    timeoutAck,
    restartFromDelay,
    timeoutPoll,
    handleRxEnableTask
#if  ZIGBEE_DEVICE_FFD == TRUE
    ,createBeaconTxTask
#endif //!ZIGBEE_DEVICE_FFD == 1
}STATIC_TIMER_CALLBACK;

/******************************************************************************/
//@FunctionName: timer_init                                                   */
//@Description:  Specific initialization of abstraction of the OS timer       */
//                                                                            */    
//@Param                                                                      */
//                                                                            */   
/******************************************************************************/
BOOL    timer_init();

/******************************************************************************/
//@FunctionName: mactimer_startTimer                                          */
//@Description:                                                               */
//  Start a timer with specific duration and callback function                */    
//@Param                                                                      */
//                                                                            */   
/******************************************************************************/
BOOL timer_startTimer(STATIC_TIMER_CALLBACK         nIndex,
                      void*                         par_pParam,
                      UINT32                        par_Time);

/******************************************************************************/
//@FunctionName: mactimer_endTimer                                            */
//@Description:                                                               */
//  Start a timer with specified duration and callback function               */    
//@Param                                                                      */
//                                                                            */   
/******************************************************************************/
BOOL    timer_endTimer(STATIC_TIMER_CALLBACK         nIndex);


/*-----------------------------------------------------------------------------
                        A-PERIODICAL TIMER

This kind of timer is used for a-periodical task. This timer should 
created for a-periodical task: NWK, APS, APP to handle its own periodical action
----------------------------------------------------------------------------- */

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will reset the msg timer                        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mtimer_reset();

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mtimer_handler(void*   pTimer, void*   arg);
/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will create a msg timer                         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL mtimer_creatTmr(DWORD*     pMsg, UINT32    Period/*In 1ms*/,
                     BOOL bPeriodical, UINT8     Task);

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will create a msg timer                         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mtimer_endTmr(DWORD*     pMsg);

#endif  // !MAC_TIMER_H
