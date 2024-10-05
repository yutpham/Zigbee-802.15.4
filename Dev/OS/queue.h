/*******************************************************************************/
//Module: OS                                                                   */   
//File Name: queue.h                                                           */
//Description: This component implement a static message queue                 */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//05/11/2007        pnthai          Created                                    */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef QUEUE_H
#define QUEUE_H
/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "types.h"

/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/
/* Type of message using in the Message Queue*/

typedef struct
{                                   
    void        *QStart;            /* Pointer to start of queue data                              */
    void        *QEnd;              /* Pointer to end   of queue data                              */
    void        *QIn;               /* Pointer to where next message will be inserted  in   the Q  */
    void        *QOut;              /* Pointer to where next message will be extracted from the Q  */
    UINT8       QSize;              /* Size of queue (maximum number of entries)                   */
    UINT8       QEntries;             /* Current number of entries in the queue                      */
} Q_QUEUE;

typedef Q_QUEUE*          P_Q_QUEUE;           


/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/



/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/******************************************************************************/
//@FunctionName: This function will initialize message queue respecting size  */
//               of each element and queue
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    queue_init(P_Q_QUEUE par_pQueue, BYTE*   par_pQueueSpace, UINT8    par_nQSize);

/******************************************************************************/
//@FunctionName:  queue_push    						  	                  */
//@Description:	  This function will push the message to the head of queue    */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL    queue_push(P_Q_QUEUE par_pQueue, void*   par_pMsg);

/******************************************************************************/
//@FunctionName:  queue_pop    			    			  	                  */
//@Description:	  This function will pop the message from the tail of queue   */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL    queue_pop(P_Q_QUEUE par_pQueue, BYTE*   par_ppMsg);

#endif //!QUEUE_H
