/*******************************************************************************/
//Module: MAC\mac_tx                                                           */   
//File Name: mac_tx.h                                                          */
//Description: Handles type definition for packet transmissions  			   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//01/10/2007        pnthai          create                                     */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef MAC_TX_H
#define MAC_TX_H

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "mac_types.h"
#include "mac_packet.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/

/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
extern MAC_TX              MacTx;

/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/******************************************************************************/
//@FunctionName:   tx_init										  	          */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void tx_init();


/******************************************************************************/
//@FunctionName: tx_trans								  	                  */
//@Description:	 This function will create an task to handle packet trans     */
//               until successful or expired                                  */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL tx_trans(P_MPDU_TX     par_pMpdu);

/******************************************************************************/
//@FunctionName: tx_transTask							  	                  */
//@Description:	This is task to trans packet                                  */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void tx_transTask(void*,void* par_pParam);

/******************************************************************************/
//@FunctionName:   tx_resumeAfterBeacon								          */
//@Description:	 This function will resume the delayed trans. task after beacon*/
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void tx_resumeAfterBeacon(BOOL  bTransBeacon);


/******************************************************************************/
//@FunctionName:Tx_notifyAck							  	                  */
//@Description:	This function will notify the Trans layer a ACK received      */
//	            Return will be TRUE if it is a valid ACK and current transTask
//              will be stopped, else return false and ACK will be discarded  */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL tx_notifyAck(P_MPDU_RX par_pRxMpdu);


#endif //!MAC_TX_H

