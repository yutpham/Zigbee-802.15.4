/*******************************************************************************/
//Module: MAC/mac_primitive                                                    */   
//File Name: mac_mlme.h				                                           */
//Description: This file will hold internal interfaces to MAC PRIMITIVES 
//             components to handle the internal interaction between components*/
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//02/21/2007                                                   */
//------------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef MAC_MLME_H
#define MAC_MLME_H


/************************************************************************/
/*					Include files										*/
/************************************************************************/

/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/
/* Scan information */
// Internal module data
typedef struct {    
    // Scan parameters
    BYTE    scanType;
    DWORD   scanChannels;
    UINT8   scanDuration;    
    // Scan status
    UINT8   currentChannel;    
    // Variable to store the current channel
    UINT8   oldPhyCurrentChannel;
    /* CMD for orphan scan */
    P_MPDU_TX   pScanCMD;
    BOOL        bRealigned;
} MAC_SCAN_INFO;

/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
extern MAC_SCAN_INFO*       pScanInfo;

/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/


/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/******************************************************************************/
//@FunctionName:   mlme_handleDisassoTransition								  */
//@Description:	This function will handle the transition of disassociation req*/
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mlme_handleDisassoTransition(MAC_STATUS par_macStatus, void* par_Param);


/******************************************************************************/
//@FunctionName:  mlme_handleDisassoReq 									  */
//@Description:	This function will handle the disassociation request          */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mlme_handleDisassoReq(MAC_STATUS par_macStatus, void* par_Param);


/******************************************************************************/
//@FunctionName: mlme_handlePktInScan   									  */
//@Description:	 This function will attract the data from the beacon before
//               it is past to upper layer. Information will be store in the
//               SCAN_RESULT structure                                        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mlme_handlePktInScan(P_MPDU_RX par_pRxPkt);

#endif //!MAC_MLME_H
