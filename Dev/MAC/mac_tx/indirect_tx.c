/*******************************************************************************/
//Module: MAC\mac_tx                                                           */   
//File Name: indirect_tx.c                                                     */
//Description: This	component implement the indirect transmission from coor.   */
//              															   */
//          
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//06/24/2007                                                                   */
//------------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "indirect_tx.h"
#include "linklist.h"
#include "mac_beacon.h"

#if     ZIGBEE_DEVICE_FFD == TRUE  
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/
typedef struct  
{
    UINT8       TTL;
    P_MPDU_TX   pPacket;
    UINT8       NextIndex;  /*Index of next packet in the list*/
}INDIRECT_PKT;

/* Pool of indirect packet */
INDIRECT_PKT        IndirectPkts[MAC_TX_POOL_SIZE];
UINT8               FirstPkt;
UINT8               LastPkt;
/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/

/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/
/******************************************************************************/
//@FunctionName: indirectTx_releasePkt                                        */
//@Description:	 This function will be called to release a pkt in the indirect
//               packet list:
//                  - Data packet: inform the nwk layer
//                  - CMD  packet: release without informing                  */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void indirectTx_releasePkt(P_MPDU_TX par_pPacket, MAC_STATUS Status)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    if (par_pPacket == NULL)
        return;

    if (par_pPacket->Mpdu.FrmControl.FrameType == MAC_FRM_DATA)
    {
        /*Call to upper*/
        mcpsDataConfirm(par_pPacket->PktInfo.msduHandle, Status);   
    }
    
    /*remove packet*/
    mem_releaseTxPkt(par_pPacket);                                          
} // End of functions
/************************************************************************/
/*                      Function                                        */
/************************************************************************/
/******************************************************************************/
//@FunctionName: indirectTx_init   										  	  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void indirectTx_init()
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    for (UINT8 index = 0; index <MAC_TX_POOL_SIZE; index++ )
    {
        IndirectPkts[index].pPacket     = NULL;
        IndirectPkts[index].NextIndex   = MAC_TX_POOL_SIZE;
        FirstPkt                        = MAC_TX_POOL_SIZE;
        LastPkt                         = MAC_TX_POOL_SIZE;
    }

} // End of functions

/******************************************************************************/
//@FunctionName: This function will add packet into the indirect packet queue */
//@Description:	                                                              */
//	                                                                          */    
//@Param         TRUE: If packet is added into the queue
//               FALSE:If packet is not added into the queue                  */
//	                                                                          */   
/******************************************************************************/
BOOL indirectTx_addPkt(P_MPDU_TX par_pPacket)
{
 	/*********** Variable declaration ***********/
 	/*********** Function body		 ***********/
    /*Search for free slot */
    DISABLE_GLOBAL_INT();
    for (UINT8 index = 0; index <MAC_TX_POOL_SIZE; index++ )
    {
        if (IndirectPkts[index].pPacket == NULL)
        {
            IndirectPkts[index].pPacket             = par_pPacket;
            IndirectPkts[index].TTL                 = mpib.macTransactionPersistenceTime;           
            par_pPacket->PktInfo.IndirectPktIndex   = index;

            /*Update next index*/
            /*First packet*/
            if (FirstPkt == MAC_TX_POOL_SIZE)             
                FirstPkt = index;
            else
                IndirectPkts[LastPkt].NextIndex = index;
            LastPkt = index;

            ENABLE_GLOBAL_INT();
            return TRUE;
        }
    }
    ENABLE_GLOBAL_INT();
    return FALSE;
} // End of functions

/******************************************************************************/
//@FunctionName: indirectTx_controlIndirectPkt
//@Description:	This function will control the indirect packet   			  */
/*                  - Update the TTL
                    - Remove the expired pkt
                    - Update the pending addr to the beacon payload                 
*/                                                             
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT8 indirectTx_controlIndirectPkt(BYTE* pPayload)
{

	/*********** Variable declaration ***********/
    PENDING_ADDR_SPEC*          pPendingAddrSpec;
    UINT8                       nNumExtAddr;
    UINT8                       Index;
    UINT8                       PrevIndex;
    UINT8                       nPendingAddrLen;
	/*********** Function body		 ***********/
    nNumExtAddr = 0;
    nPendingAddrLen = sizeof(PENDING_ADDR_SPEC);
    DISABLE_GLOBAL_INT();
    Index       = FirstPkt;
    while (Index != MAC_TX_POOL_SIZE && IndirectPkts[Index].pPacket) /* is the valid packet*/
    {
        if (!IndirectPkts[Index].TTL--)
        {
            indirectTx_releasePkt(IndirectPkts[Index].pPacket, TRANSACTION_EXPIRED);
            IndirectPkts[Index].pPacket = NULL;

            /*If we removed the fist packet*/
            if (Index == FirstPkt)
                FirstPkt    = IndirectPkts[Index].NextIndex;

            /*If not the first packet*/
            else
                IndirectPkts[PrevIndex].NextIndex = IndirectPkts[Index].NextIndex;           

            /*Last packet is removed */
            if (Index == LastPkt)
                LastPkt = PrevIndex;
        }       
        else
        {
            /* Omit nwk addr */
            if (IndirectPkts[Index].pPacket->Mpdu.FrmControl.DestAddMode == AM_EXTENDED_64)
            {
                *(QWORD*)(pPayload+sizeof(PENDING_ADDR_SPEC)+
                    nNumExtAddr*sizeof(QWORD)) = IndirectPkts[Index].pPacket->Mpdu.dstAddr.Extended;
                nNumExtAddr++;
            }
        }
        Index       = IndirectPkts[Index].NextIndex;
        if (FirstPkt == Index)
            PrevIndex   = MAC_TX_POOL_SIZE;
        else
            PrevIndex   = Index;
    }
    /*implemented ext add only, please do if need to complete with ex-add*/
    pPendingAddrSpec                    = (PENDING_ADDR_SPEC*)(pPayload);
    pPendingAddrSpec->NumofExtAddr      = nNumExtAddr;
    pPendingAddrSpec->NumofShortAddr    = 0;
    nPendingAddrLen                     += nNumExtAddr*sizeof(QWORD);
    ENABLE_GLOBAL_INT();
    return  nPendingAddrLen;
} // End of functions

/******************************************************************************/
//@FunctionName: indirectTx_getPkt						  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
P_MPDU_TX indirectTx_getPkt(ADDRESS *par_pDestAddr, UINT8     par_Addrmode)
{
	/*********** Variable declaration ***********/
    UINT8            Index;
	/*********** Function body		 ***********/
    DISABLE_GLOBAL_INT();
    Index       = FirstPkt;
    while (Index != MAC_TX_POOL_SIZE && IndirectPkts[Index].pPacket) /* is the valid packet*/
    {
        if ((par_pDestAddr->Extended == IndirectPkts[Index].pPacket->Mpdu.dstAddr.Extended && par_Addrmode == AM_EXTENDED_64) || 
            (par_pDestAddr->Short == IndirectPkts[Index].pPacket->Mpdu.dstAddr.Short && par_Addrmode == AM_SHORT_16))
        {
            ENABLE_GLOBAL_INT();
            return IndirectPkts[Index].pPacket;                       
        }
        Index       = IndirectPkts[Index].NextIndex;
    }
    ENABLE_GLOBAL_INT();
    return NULL;
} // End of functions

/******************************************************************************/
//@FunctionName:   indirectTx_removePkt										  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void indirectTx_removePkt(P_MPDU_TX par_pMpduTx)
{
	/*********** Variable declaration ***********/
    UINT8            Index;   
    UINT8            removedIndex;
	/*********** Function body		 ***********/
    DISABLE_GLOBAL_INT();
    Index           = FirstPkt;
    removedIndex    = par_pMpduTx->PktInfo.IndirectPktIndex;
    /*First packet is removed */
    if (removedIndex == FirstPkt)
    {
        FirstPkt = IndirectPkts[FirstPkt].NextIndex;
        indirectTx_releasePkt(par_pMpduTx, SUCCESS);
        IndirectPkts[removedIndex].pPacket     = NULL;
        IndirectPkts[removedIndex].NextIndex   = MAC_TX_POOL_SIZE;

        /*Last packet is removed */
        if (removedIndex == LastPkt)
            LastPkt = MAC_TX_POOL_SIZE;
    }
    else
    {
        while (Index != MAC_TX_POOL_SIZE && IndirectPkts[Index].pPacket) /* is the valid packet*/
        {
            if (IndirectPkts[Index].NextIndex == removedIndex)      
                break;         
            Index       = IndirectPkts[Index].NextIndex;
        }

        if ((removedIndex == IndirectPkts[Index].NextIndex) 
            && (Index != MAC_TX_POOL_SIZE))
        {
            IndirectPkts[Index].NextIndex = IndirectPkts[removedIndex].NextIndex;
        
            indirectTx_releasePkt(par_pMpduTx,SUCCESS);
            IndirectPkts[removedIndex].pPacket     = NULL;
            IndirectPkts[removedIndex].NextIndex   = MAC_TX_POOL_SIZE;

            /*Last packet is removed */
            if (removedIndex == LastPkt)
                LastPkt = Index;
        }
    }
    ENABLE_GLOBAL_INT();
    return;
} // End of functions


/******************************************************************************/
//@FunctionName: indirectTx_RespondDataReq  								  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void indirectTx_RespondDataReq(P_MPDU_RX par_pMpduTx)
{
	/*********** Variable declaration ***********/
    P_MPDU_TX       pTxMpdu; 
    ADDRESS*        pCoorAddr;
	UINT8			coordAddrMode;
	/*********** Function body		 ***********/
    pTxMpdu = indirectTx_getPkt(&par_pMpduTx->Mpdu.srcAddr,par_pMpduTx->Mpdu.FrmControl.SourceAddMode);
    if (pTxMpdu)
    {
        tx_trans((P_MPDU_TX)pTxMpdu);
        return;
    }
    else
    {            
        /* Prepare the frame request */
        pTxMpdu = mem_getFreeTxPkt();
        if (!pTxMpdu)
        {
            return;
        }
        mem_set(&pTxMpdu->Mpdu.FrmControl,2);

        //Set other information
        pTxMpdu->PktInfo.txMode      = TX_MODE_CSMACA|TX_MODE_MAC_INTERNAL;
        pTxMpdu->PktInfo.nRetryLeft  = 1;
    
        mac_formatMpdu(pTxMpdu,
                       MAC_FRM_DATA,
                       AM_SHORT_16,
                       mpib.macPANId,
                       (ADDRESS *) &mpib.macShortAddress,
                       par_pMpduTx->Mpdu.FrmControl.SourceAddMode,
                       par_pMpduTx->Mpdu.srcPanId,
                       (ADDRESS *) &par_pMpduTx->Mpdu.srcAddr,
                       TX_OPT_ACK_REQ,
                       NULL,
                       0);

        if(!tx_trans(pTxMpdu)) /* Transmission */     
            mem_releaseTxPkt(pTxMpdu);  
    }
} // End of functions

#endif  //!   ZIGBEE_DEVICE_FFD == anTRUE  
