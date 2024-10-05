/*******************************************************************************/
//Module: MAC/mac_primitive                                                    */   
//File Name: mac_mcps.c                                                        */
//Description: This file implement the mcps-sap of the MAC  				   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//02/10/2007        pnthai          Create                                     */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "mac_primitives.h"
#include "mac_mem.h"
#include "mac_packet.h"
#include "mac_tx.h"
#include "phy_primitives.h"
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


//*********************************************************************************************
//@FunctionName: mcpsDataRequest
//@Description:
//	Issued by local SSCS to MLME
//@Param
//	*pMCPS_Data_Request -	MCPS_DATA_REQUEST structure
//*********************************************************************************************
MAC_STATUS mcpsDataRequest(   UINT8   SrcAddrMode,      // The source addressing mode for this primitive and subsequesnt MPDU. (00-03)
                        UINT16  SrcPANId,         // The 16bit PAN identifier of the entity from which the MSDU is being tranferred
                        ADDRESS* pSrcAddr,          // The individual device address of the entity from which the MSDU is being transferred
                        UINT8   DstAddrMode,      // The destination addressing mode for this primitive
                        UINT16  DstPANId,         // The 16bit PAN identifier of the entity to which the MSDU is being tranferred
                        ADDRESS* pDstAddr,          // The individual device address to which the MSDU is being tranferred
                        UINT8   msduLength,       // The number of octets forming the MSDU
                        BYTE    *msdu,            // The set of octets forming the MSDU
                        UINT8   msduHandle,       // The handle associated with the MSDU to transmitted by the MAC sublayer entity
                        BYTE    TxOptions)        // The transmission options for this MSDU
{
   
    P_MPDU_TX            pTxMPDU;
    MAC_STATUS           macStatus;


#if     ZIGBEE_DEVICE_FFD == FALSE
        return UNSUPPORTED_ATTRIBUTE;
#endif

    /* Sercurity option */
    if (TxOptions & TX_OPT_SECURITY_ENABLE)
        return UNSUPPORTED_ATTRIBUTE;

    /* GTS option */
    //The indirect transmission bit is
    //ignored if the GTS transmission bit is set to 1, the destination address is not present, or the MAC sublayer
    //receiving this primitive is not the MAC sublayer of a coordinator.
    //GTS transmission is required or not required
    //TBD
    if (TxOptions & TX_OPT_GTS)
        return UNSUPPORTED_ATTRIBUTE;

    //Get a free MPDU
    pTxMPDU = mem_getFreeTxPkt();
    if(pTxMPDU == NULL)              /*  If no available packet, try later*/
       return TRANSACTION_OVERFLOW;
    mem_set(&pTxMPDU->Mpdu.FrmControl,2);   

    //Building the MPDU frame
    macStatus = mac_formatMpdu(pTxMPDU, MAC_FRM_DATA, SrcAddrMode,SrcPANId,
                               pSrcAddr, DstAddrMode, DstPANId, pDstAddr,
                               TxOptions, msdu, msduLength);

    if (macStatus != SUCCESS)        
        return FRAME_TOO_LONG;  /* Can not build the frame */

    //set other information
    pTxMPDU->PktInfo.nRetryLeft  = aMaxFrameRetries;
    pTxMPDU->PktInfo.msduHandle  = msduHandle;   
    pTxMPDU->PktInfo.txMode      = TX_MODE_CSMACA;

    /* Indirect transmission option */
    //If the TxOptions parameter specifies that an indirect transmission is required and this primitive is received
    //by the MAC sublayer of a coordinator
#if     ZIGBEE_DEVICE_FFD == TRUE  
    if (TxOptions & TX_OPT_INDIRECT)
        macStatus = indirectTx_addPkt(pTxMPDU);         /*Indirect transmission */
    else
#endif
        //transmit the packet
    macStatus = tx_trans(pTxMPDU);
    if(!macStatus) 
        mem_releaseTxPkt(pTxMPDU);

#ifdef ZIGBEE_STUB
        MacTx.pPacket = pTxMPDU;
        tx_endTrans(pTxMPDU,SUCCESS);
#endif

    return (macStatus == TRUE)?SUCCESS:TRANSACTION_OVERFLOW;
} //mcpsDataRequest


//*********************************************************************************************
//@FunctionName: mlmePurgeRequest
//@Description:
//	Optional for FFD & RFD
//@Param
//	UINT8   msduHandle: handle to the transaction that will be purge
//*********************************************************************************************
void mcpsPurgeRequest(UINT8   msduHandle)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    MAC_TX_PACKET *pIndirectPacket;
    
//     // For each packet in the queue ...
// restart:
//     packetIndex = miqInfo.firstIndirectPacket;
//     while (packetIndex != NO_PACKET) {
//         pIndirectPacket = &pMtxPacketPool[packetIndex];
//         
//         // If there's a match on the MSDU handle
//         if ((pIndirectPacket->msduHandle == msduHandle) && (pIndirectPacket->type = FT_DATA)) {
//             
//             // Just wait if the packet has been requested (we shouldn't interfere with the transmission)
//             if (pIndirectPacket->requested) {
//                 goto restart;
//             } else {
//                 pIndirectPacket->timeToLive = MIQ_PACKET_PURGED;
//                 mschAddTask(mschReserveTask(), MAC_TASK_PRI_MEDIUM, miqExpireIndirectPacketsTask, (WORD) pIndirectPacket);
//                 return SUCCESS;
//             }
//         }
//                 
//         // Move to the next packet in the queue
//         packetIndex = pIndirectPacket->nextPacket;
//     }
//     
//     return INVALID_HANDLE;

} // End of functions
