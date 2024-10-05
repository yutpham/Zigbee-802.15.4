/*******************************************************************************/
//Module: OS                                                                   */   
//File Name: queue.c                                                           */
//Description: This component implement a static message queue with message 
//             size of 4 bytes (DWORD)                                         */
/*
Specific functions are implemented on this queue
    - Push
    - Pop
*/
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//05/11/2007        pnthai          Created                                    */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "queue.h"

/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/*******************************_*****************************************/
/*					Type Definition										*/
/************************************************************************/


/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/



/************************************************************************/
/*                      Function                                        */
/************************************************************************/


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    queue_init(P_Q_QUEUE par_pQueue, BYTE*   par_pQueueSpace, UINT8    par_nQSize)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    par_pQueue->QStart      = par_pQueueSpace;                  /*      Initialize the queue                 */
    par_pQueue->QEnd        = &par_pQueueSpace[par_nQSize*sizeof(DWORD)];
    par_pQueue->QIn         = par_pQueueSpace;
    par_pQueue->QOut        = par_pQueueSpace;
    par_pQueue->QSize       = par_nQSize;
    par_pQueue->QEntries    = 0;
} // End of functions                   


/******************************************************************************/
//@FunctionName:  queue_push    						  	                  */
//@Description:	  This function will push the message to the head of queue    */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL    queue_push(P_Q_QUEUE par_pQueue, void*   par_pMsg)
{
	/*********** Variable declaration ***********/
    
	/*********** Function body		 ***********/
    if (par_pQueue->QEntries >= par_pQueue->QSize)          /* Make sure queue is not full                   */
        return FALSE;

    *(DWORD*)par_pQueue->QIn = *(DWORD*)par_pMsg;           /* Insert message into queue                     */
    par_pQueue->QIn += sizeof(DWORD);
    par_pQueue->QEntries++;                                 /* Update the nbr of entries in the queue        */
    if (par_pQueue->QIn == par_pQueue->QEnd) {              /* Wrap IN ptr if we are at end of queue         */
        par_pQueue->QIn = par_pQueue->QStart;
    }
    return TRUE;	
} // End of functions

/******************************************************************************/
//@FunctionName:  queue_pop    			    			  	                  */
//@Description:	  This function will pop the message from the tail of queue   */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL    queue_pop(P_Q_QUEUE par_pQueue, BYTE*   par_pMsg)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    if (par_pQueue->QEntries > 0) {                    /* See if any messages in the queue                   */
        *(DWORD*)par_pMsg = *(DWORD*)par_pQueue->QOut;                     /* Yes, extract oldest message from the queue         */
        par_pQueue->QOut += sizeof(DWORD);

        par_pQueue->QEntries--;                        /* Update the number of entries in the queue          */
        if (par_pQueue->QOut == par_pQueue->QEnd) {          /* Wrap OUT pointer if we are at the end of the queue */
            par_pQueue->QOut = par_pQueue->QStart;
        }
        return TRUE;
    } 
    return FALSE;
} // End of functions
