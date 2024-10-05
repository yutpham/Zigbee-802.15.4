/*******************************************************************************/
//Module: MAC/mac_rx                                                           */   
//File Name: mac_rx.c                                                          */
//Description: This file implements packet reception mechanisms  			   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//01/15/2007        pnthai          Create                                     */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "mac_rx.h"
#include "mac_packet.h"
#include "mac_primitives.h"
#include "mac_beacon.h"
#include "phy_primitives.h"
#include "mac_tx.h"
#include "mac_mem.h"
#include "mac_mlme.h"
#include "schedul.h"
#include "utils.h"
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
//@FunctionName:  macrx_ProcessMpdu 					  	                  */
//@Description:	this function will process the received packet                */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
MCPS_DATA_INDICATION* pIndiData;
void macrx_ProcessMpdu(P_MPDU_RX par_pRxMpdu)
{
	/*********** Variable declaration ***********/
    BYTE*           pPayload;
    BOOL            bEnded;
    FRAME_CONTROL*  pFrameCtrl;
	/*********** Function body		 ***********/
    bEnded = FALSE;


    /* If ACK */
    if(par_pRxMpdu->Mpdu.FrmControl.FrameType == MAC_FRM_ACK)
    {
        /* For ACK no Rx packet is created  */
        tx_notifyAck(par_pRxMpdu);         
        return;
    }

    pFrameCtrl = &par_pRxMpdu->Mpdu.FrmControl;
    /* Transaction oriented command processing */
    switch(macInfo.sState.nTransacType)
    {
    case MAC_TRANSAC_NO:
        break;
    case MAC_TRANSAC_ASSOSCIATION:
        if (par_pRxMpdu->Mpdu.FrmControl.FrameType == MAC_FRM_CMD
            && par_pRxMpdu->Mpdu.PayLoad[0] == CMD_ASSOCIATION_RESPONSE)
        {
            mlme_notifyAssociation(SUCCESS, (void*)par_pRxMpdu);            
        }
        if (par_pRxMpdu->Mpdu.FrmControl.FrameType == MAC_FRM_DATA
            && par_pRxMpdu->Mpdu.nLength == 0)
        {
            mlme_notifyAssociation(NO_DATA, (void*)par_pRxMpdu);            
        }
        break;
    case MAC_TRANSAC_DISASSOSCIATION:
        break;
    case MAC_TRANSAC_POLL:
        mac_requestState(MAC_TRANSAC_POLL, POLL_FINISHED);      /* In the poll transition */
        mlme_handlePollTransition(SUCCESS, par_pRxMpdu);
        break;
    case MAC_TRANSAC_SCAN:
        switch(pScanInfo->scanType)
        {
        case ED_SCAN:  
            break;
        case ACTIVE_SCAN:  
        case PASSIVE_SCAN:
            /* If it is in scanning state */
            if (macInfo.sState.nTransacType == MAC_TRANSAC_SCAN)
                mlme_handlePktInScan(par_pRxMpdu);
            break;
        case ORPHAN_SCAN:  
            if (par_pRxMpdu->Mpdu.PayLoad[0] == CMD_COORDINATOR_REALIGNMENT)
                mlme_handlePktInScan(par_pRxMpdu);
            break;            
        }   
    default:
        break;
    }       
    
    /* Command oriented processing */
    switch(par_pRxMpdu->Mpdu.FrmControl.FrameType)
    {
   case MAC_FRM_CMD:
        switch(par_pRxMpdu->Mpdu.PayLoad[0])
        {
        case CMD_COORDINATOR_REALIGNMENT:

            /* Process the command: Update data */
            pPayload = par_pRxMpdu->Mpdu.PayLoad;
            mlmeSetRequest(MAC_COORD_EXTENDED_ADDRESS, (WORD*) &par_pRxMpdu->Mpdu.srcAddr);
            mlmeSetRequest(MAC_PAN_ID, (WORD*) &pPayload[1]);
            mlmeSetRequest(MAC_COORD_SHORT_ADDRESS, (WORD*) &pPayload[3]);
            plmeSET_request(PHY_PIB_CURRENT_CHANNEL, pPayload[5]);

            /* For orphan responses, with extended destination address, 
            update the short address from the realignment frame */
            if (par_pRxMpdu->Mpdu.FrmControl.DestAddMode== AM_EXTENDED_64) {
                mlmeSetRequest(MAC_SHORT_ADDRESS, (WORD*) &pPayload[6]);
            }            
            mlmeSyncLossIndication(REALIGNMENT);   
            break;
#if     ZIGBEE_DEVICE_FFD == TRUE
        case CMD_ASSOCIATION_REQUEST:
            mlmeAssociateIndication(&par_pRxMpdu->Mpdu.srcAddr,par_pRxMpdu->Mpdu.PayLoad[1],FALSE,0);
            break;
        case CMD_DATA_REQUEST:                    
            indirectTx_RespondDataReq(par_pRxMpdu);
            break;
        case CMD_PAN_ID_CONFLICT_NOTIFICATION:
        /* The PAN Coordinator has received a PAN ID conflict notification 
            command from a device on its own PAN */
            if (par_pRxMpdu->Mpdu.srcPanId == mpib.macPANId)
                mlmeSyncLossIndication(PAN_ID_CONFLICT);
            break;            
        case CMD_DISASSOCIATION_NOTIFICATION:            
            if ((pFrameCtrl->SourceAddMode == AM_EXTENDED_64) && 
                (pFrameCtrl->DestAddMode == AM_EXTENDED_64)) {                        
                mlmeDisassociateIndication(&par_pRxMpdu->Mpdu.srcAddr.Extended, par_pRxMpdu->Mpdu.PayLoad[1], 
                                           FALSE,FALSE);
            }
            break;
        case CMD_ORPHAN_NOTIFICATION:
            if ((pFrameCtrl->SourceAddMode == AM_EXTENDED_64) && 
                (pFrameCtrl->DestAddMode   == AM_SHORT_16)) {
   
                // Must have broadcast source and destination PAN
                if ((par_pRxMpdu->Mpdu.srcPanId == 0xFFFF) && 
                    (par_pRxMpdu->Mpdu.dstPanId == 0xFFFF))
                    mlmeOrphanIndication(&par_pRxMpdu->Mpdu.srcAddr.Extended, 
                    pFrameCtrl->SecurityEnable,0x00);
            }            
            break;
        case CMD_BEACON_REQUEST:	/* Only FFD, device will omit */
            beacon_transBeaconOnRequest();
            break;
#endif  //!ZIGBEE_DEVICE_FFD == TRUE  						
        case CMD_GTS_REQUEST:
        default:
            break;
        }       
    	break;
    case MAC_FRM_DATA:        
        /* Format indication data*/
        pIndiData = (MCPS_DATA_INDICATION*)mem_getMem(sizeof(MCPS_DATA_INDICATION));    
        if (pIndiData)
        {
            pIndiData->srcAddrMode      = pFrameCtrl->SourceAddMode;
            pIndiData->srcPanId         = par_pRxMpdu->Mpdu.srcPanId;
            pIndiData->srcAddr          = par_pRxMpdu->Mpdu.srcAddr;
            pIndiData->dstAddrMode      = pFrameCtrl->DestAddMode;
            pIndiData->dstPanId         = par_pRxMpdu->Mpdu.dstPanId;
            pIndiData->dstAddr          = par_pRxMpdu->Mpdu.dstAddr;
            pIndiData->mpduLinkQuality  = par_pRxMpdu->Mpdu.Fcs.LinkQuality;
            pIndiData->securityUse      = FALSE;
            pIndiData->aclEntry         = 0;
            pIndiData->msduLength       = par_pRxMpdu->Mpdu.nLength;
            pIndiData->pMsdu = mem_getMem(par_pRxMpdu->Mpdu.nLength);
            if (pIndiData->pMsdu)
            {
                mem_cpy(pIndiData->pMsdu, par_pRxMpdu->Mpdu.PayLoad , pIndiData->msduLength);
                //call to DATA.indication
                mcpsDataIndication(pIndiData);
            }
            else
                mem_freeMem(pIndiData);
        }
        /*Omit packet if not enough memory*/
        break;
    case MAC_FRM_BEACON:
        if(macInfo.sState.nTransacType == MAC_TRANSAC_SCAN) 
        {
            //Make notification to upper layer
            beacon_notifyBeacon(par_pRxMpdu);
        }
        else if (par_pRxMpdu->Mpdu.srcPanId == mpib.macPANId)
        {            
            beacon_updateNewBeacon(par_pRxMpdu);                /* Notify the beacon */
        }

        break;
    case MAC_FRM_ACK:        
        break;
    default:
        break;
    }

    //remove the packet
    mem_releaseRxPkt(par_pRxMpdu); 
} // End of functions
