/*******************************************************************************/
//Module: NWK                                                                  */   
//File Name: nwk_data.h                                                        */
//Description: This file will implement network data handler
//              - Transmission
//              - Relay data												   */
//              - Broadcast data                                               */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//07/16/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef NWK_DATA
#define NWK_DATA

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


/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/

/************************************************************************/
/*                      Static Function                                 */
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
void        nwk_dataInit();

/*-----------------------------------------------------------------------------

                    Data management function

------------------------------------------------------------------------------*/

/******************************************************************************/
//@FunctionName: This function will 
//                  - Format the NPDU data at the beginning of the Nsdu
//                    this memory already spared for NWK header
//                  - Get new memory block (32 bytes) for NPDU command, format
//                    it  
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
P_NPDU      nwk_makeNpdu(NWK_FRM_TYPE FrmType, BYTE NsduLength, BYTE *Nsdu);

/******************************************************************************/
//@FunctionName:  This function will handle broadcasting process              */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS    nwk_broadcast(P_NPDU pNpdu,UINT8 nLength, UINT8 AddrMode, 
                          ADDRESS*  pSrcAddr, UINT8 nHandle);

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS    nwk_send(P_NPDU pNpdu,UINT8 nLength, UINT8    NsduHandle);



#endif //!NWK_DATA
