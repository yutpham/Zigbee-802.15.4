/*******************************************************************************/
//Module:  MAC/common                                                          */   
//File Name: mac_task                                                          */
//Description: Mac memory management funtions                                  */
//  -------------------------------------------------------------              */
//Revision History:                                                            */
//Date						 Programmer		Details                            */
//11/15/2006                 pnthai                                            */
//  -------------------------------------------------------------              */
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "mac_mem.h"
#include "ucos_ii.h"
/************************************************************************/
/*					Type Definition										*/
/************************************************************************/

/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
BYTE                         MacRxPktPool[MAC_RX_POOL_SIZE][sizeof(MPDU_RX)];
BYTE                         MacTxPktPool[MAC_TX_POOL_SIZE][sizeof(MPDU_TX)];


OS_MEM*                      MacRxMem;
OS_MEM*                      MacTxMem;


/******************************************************************************/
//@FunctionName:mem_initialize   										  	  */
//@Description:	Initialize memory management component of the MAC             */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mem_initialize()
{
	/*********** Variable declaration ***********/
    UINT8           nErr;
	/*********** Function body		 ***********/
    
    //create memory partition
    MacRxMem = OSMemCreate(MacRxPktPool, 
                           MAC_RX_POOL_SIZE,
                           sizeof(MPDU_RX),
                           &nErr);               
    MacTxMem = OSMemCreate(MacTxPktPool, 
                           MAC_TX_POOL_SIZE,
                           sizeof(MPDU_TX),
                           &nErr);
} // End of functions


/******************************************************************************/
//@FunctionName: mem_release  										  	      */
//@Description:	Release all the memory  managed by the MAC                    */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mem_release()
{
	/*********** Variable declaration ***********/
    UINT8           nErr;
	/*********** Function body		 ***********/    
    //release memory partition
    OSMemDelete(MacRxMem,&nErr);               
    OSMemDelete(MacTxMem,&nErr);
} // End of functions


/******************************************************************************/
//@FunctionName:mem_getFreeTxPkt   										  	  */
//@Description:	Get a free tx packet                                          */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
P_MPDU_TX mem_getFreeTxPkt()
{
	/*********** Variable declaration ***********/
    UINT8           nErr;
	/*********** Function body		 ***********/
    return (P_MPDU_TX)(OSMemGet(MacTxMem, &nErr));
} // End of functions

/******************************************************************************/
//@FunctionName:  mem_releaseTxPkt 										  	  */
//@Description:	Release a tx packet                                           */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mem_releaseTxPkt(P_MPDU_TX par_pPacket)
{
	/*********** Variable declaration ***********/
    UINT8           nErr;
	/*********** Function body		 ***********/
    nErr = OSMemPut(MacTxMem, (void*)(par_pPacket));
} // End of functions

/******************************************************************************/
//@FunctionName:  mem_getFreeRxPkt 										  	  */
//@Description:	Get a free Rx packet                                          */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
P_MPDU_RX mem_getFreeRxPkt()
{
	/*********** Variable declaration ***********/
    UINT8           nErr;
	/*********** Function body		 ***********/
    return (P_MPDU_RX)(OSMemGet(MacRxMem, &nErr));
} // End of functions

/******************************************************************************/
//@FunctionName: mem_releaseRxPkt  										  	  */
//@Description:	Release a tx packet                                           */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mem_releaseRxPkt(P_MPDU_RX par_pPacket)
{
	/*********** Variable declaration ***********/
    UINT8           nErr;
	/*********** Function body		 ***********/
    nErr = OSMemPut(MacRxMem, (void*)((void*)par_pPacket));
} // End of functions


