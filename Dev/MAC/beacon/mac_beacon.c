/*******************************************************************************/
//Module: MAC                                                                  */   
//File Name: mac_beacon                                                        */
//Description: This component will define the MAC layer behavior for beacon 
//             related function. For RFD, that includes:
//               + Process the received beacon and update information related
//                 to beacon
//               + For the beacon network, periodically handle the beacon and 
//                 inform the upper layer: beacon loss...
//               + Defines functions for beacon-request primitive
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date              Programmer      Details                                    */
//02/27/2007        pnthai          Created                                    */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*                  Include files                                       */
/************************************************************************/
#include "mac_beacon.h"

#include "types.h"
#include "mac_types.h"
#include "utils.h"

#include "mac_primitives.h"
#include "phy_primitives.h"
#include "phy_support.h"

#include "mac_tx.h"
#include "mac_mem.h"
#include "mac_packet.h"

#include "schedul.h"
#include "static_timer.h"
#include "ucos_ii.h"
/************************************************************************/
/*                  Constant declaration                                */
/************************************************************************/

/************************************************************************/
/*                  Type Definition                                     */
/************************************************************************/


/************************************************************************/
/*                  Variable declaration                                */
/************************************************************************/
BEACON_INFO     BeaconInfo;
SLOT_TIME_INFO  SlotInfo;

/* This is for temporarily used
    - Indication packet
*/
MLME_BEACON_NOTIFY_INDICATION  BeaconNof;
/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/

//-------------------------------------------------------------------------------------------------------
//  void mbcnGetBeaconMargin(void)
//
//  DESCRIPTION:
//      Because of the crystal inaccuracy, a margin must be added before the beacon that will
//      be received. The result is that:
//          - RX must be turned on a short while before the beacon is expected to be received.
//          - TX must stop before this margin is reached
//
//      The Excel worksheet called "Beacon reception margin.xls" can be used to calculate these 
//      constants.
//
//  RETURN VALUE:
//      UINT8
//          The margin as a number of backoff periods
//-------------------------------------------------------------------------------------------------------
/******************************************************************************/
//@FunctionName:  beacon_transmitConflictNotif 								  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void beacon_transmitConflictNotif()
{
	/*********** Variable declaration ***********/
    P_MPDU_TX       pTxMpdu;
    ADDRESS*        pCoorAddr;
	UINT8			coordAddrMode;
	/*********** Function body		 ***********/

    /* Prepare the frame request */
    pTxMpdu = mem_getFreeTxPkt();
    if (!pTxMpdu)
        return;
    mem_set(&pTxMpdu->Mpdu.FrmControl,2);

    //Prepare the payload of the POLL.Req
    pTxMpdu->Mpdu.PayLoad[0] = CMD_PAN_ID_CONFLICT_NOTIFICATION;

    //Set other information
    pTxMpdu->PktInfo.txMode      = TX_MODE_NO_CSMACA;
    pTxMpdu->PktInfo.nRetryLeft  = aMaxFrameRetries;
        
    if (mpib.macCoordShortAddress >= VALID_SHORT_ADDR)
    {
        pCoorAddr       = &mpib.macCoordExtendedAddress;
        coordAddrMode   = AM_EXTENDED_64;
    }
    else 
    {
        pCoorAddr       = &mpib.macCoordShortAddress;
        coordAddrMode   = AM_SHORT_16;
    }

    mac_formatMpdu(pTxMpdu,
                   MAC_FRM_CMD,
                   AM_EXTENDED_64,
                   mpib.macPANId,
                   (ADDRESS *) &aExtendedAddress,
                   coordAddrMode,
                   mpib.macPANId,
                   (ADDRESS *) pCoorAddr,
                   0,
                   pTxMpdu->Mpdu.PayLoad,
                   1);

    if(!tx_trans(pTxMpdu)) /* Transmission */     
    {
        mem_releaseTxPkt(pTxMpdu);
    }
} // End of functions

/*****************************************************************************/
//@FunctionName: beacon_calBeaconMargin 									*/
//@Description:	Because of the crystal inaccuracy, a margin must be added    */
//              before the beacon that will be received. The result is that: */
//          - RX must be turned on a short while before the beacon is        */
//            expected to be received.
//          - TX must stop before this margin is reached                     */
//	                                                                         */    
//@Param                                                                     */
//Reused from C2420                                                          */   
/*****************************************************************************/
UINT8 beacon_calBeaconMargin(void) {
    switch (mpib.macBeaconOrder) {
        case 7:  return 2;
        case 8:  return 3;
        case 9:  return 5;
        case 10: return 10;
        case 11: return 20;
        case 12: return 40;
        case 13: return 79;
        case 14: return 158;
        default: return 1;
    }
} // beacon_calBeaconMargin


/******************************************************************************/
//@FunctionName: beacon_notifyBeacon                                          */
//@Description: This function will make a notification packet and notify the
//              upper layer the appearance of new beacon                      */
//                                                                            */    
//@Param                                                                      */
//                                                                            */   
/******************************************************************************/
void    beacon_notifyBeacon(P_MPDU_RX      par_pRxMpdu);

/******************************************************************************/
//@FunctionName: beacon_createSyncBeaconTask  								  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    beacon_synsBeaconTask(void* arg1,void* arg2);
void    beacon_createSyncBeaconTask(void* arg1,void* arg2)
{   
    schedul_addTask(beacon_synsBeaconTask,TASK_PER_MAC_PERIODICAL, NULL);/*Periodically track beacon*/
}

/******************************************************************************/
//@FunctionName: beacon_createTimeoutSyncBncTask  							  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    beacon_timeoutSyncBeacon(void* arg1,void* arg2);
void    beacon_createTimeoutSyncBncTask(void* arg1,void* arg2)
{   
    schedul_addTask(beacon_timeoutSyncBeacon,TASK_PER_MAC_PERIODICAL, NULL);/*Periodically track beacon*/
}

/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/******************************************************************************/
//@FunctionName: beacon_initBeacon  										  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
#if     ZIGBEE_DEVICE_FFD == TRUE  
void beacon_initBeacon()
{
	/*********** Variable declaration ***********/
    P_MPDU_TX   pBeacon;
	/*********** Function body		 ***********/
    /* Prepare the beacon */
    pBeacon             = BeaconInfo.pBeacon;
    if(!pBeacon)
    {
        BeaconInfo.pBeacon  = BeaconInfo.Beacon;   /*Allocate the beacon*/
        pBeacon             = BeaconInfo.pBeacon;
    }

    //set other information
    pBeacon->PktInfo.txMode      = TX_MODE_CSMACA|TX_MODE_FORCE_UNSLOTTED;
    pBeacon->PktInfo.nRetryLeft  = aMaxFrameRetries;
    pBeacon->Mpdu.nLength        = aMaxMACFrameSize;


    if (mpib.macShortAddress >= VALID_SHORT_ADDR)
        mac_formatMpdu(pBeacon,
                       MAC_FRM_BEACON,
                       AM_EXTENDED_64,
                       mpib.macPANId,
                       (ADDRESS *) &aExtendedAddress,
                       AM_NONE,
                       0,
                       NULL,
                       0,
                       NULL,
                       0);
    else
        mac_formatMpdu(pBeacon,
                       MAC_FRM_BEACON,
                       AM_SHORT_16,
                       mpib.macPANId,
                       (ADDRESS *) &mpib.macShortAddress,
                       AM_NONE,
                       0,
                       NULL,
                       0,
                       NULL,
                       0);
} // End of functions
#endif
/******************************************************************************/
//@FunctionName:  beacon_synSlotTime                                          */
//@Description:                                                               */
//                                                                            */    
//@Param                                                                      */
//                                                                            */   
/******************************************************************************/
void beacon_synSlotTime()
{
    DISABLE_GLOBAL_INT();        

//     // Delay the next timer tick
//     OCR1A = (MAC_TIMER_OVERFLOW_VALUE + SlotInfo.captureTcnt)
//           - (MAC_TIMER_BACKOFF_SLOT_OFFSET + 322 - 1);

    /*Sync bos counter*/
    SlotInfo.bosCounter -= SlotInfo.captureBosCounter;    
    // Clear the compare(A) interrupt flag
    TIFR = BM(OCF1A);
    // Set the beacon "time stamp"
    SlotInfo.bosCounterAdjustTime += (SlotInfo.captureBosCounter * (UINT32)
                                      MAC_SYMBOL_DURATION * (UINT32)
                                      aUnitBackoffPeriod)
                                   + ((UINT32) SlotInfo.captureTcnt
                                    / CLOCK_SPEED_MHZ);    
    ENABLE_GLOBAL_INT();
}

/******************************************************************************/
//@FunctionName:  beacon_updateNewBeacon                                      */
//@Description: This function will updates MAC with new received beacon       */
//              parameters in the beacon will be updated to PIB and Mac states*/
//                                                                            */    
//@Param                                                                      */
//                                                                            */   
/******************************************************************************/
void beacon_updateNewBeacon(P_MPDU_RX  par_pRxMpdu)
{
    /*********** Variable declaration ***********/
    SUPER_FRAME*        pSuperframe;
    PENDING_ADDR_SPEC*  pPending;
    UINT8           nOldBeaconOrder;
    UINT8           nPendAddrOffset;
    BOOL            bPendData;
    UINT8           nTemp;
    /*********** Function body       ***********/

    /*Check conflict PAN ID: for a PAN coordinator*/
    if ((macInfo.DeviceType == ZIGBEE_PAN_COOR))
    {
        if((par_pRxMpdu->Mpdu.PayLoad[1] & (0x4000 >> 8)) /*other coor. with the same PanID*/
            && (par_pRxMpdu->Mpdu.srcPanId == mpib.macPANId))
            mlmeSyncLossIndication(PAN_ID_CONFLICT);    
    }
    /*Check conflict PAN ID: for a router*/
    nTemp = par_pRxMpdu->Mpdu.FrmControl.SourceAddMode;
    if (((nTemp == AM_SHORT_16)&&(par_pRxMpdu->Mpdu.srcAddr.Short != mpib.macCoordShortAddress))
        || ((nTemp == AM_EXTENDED_64)&&(par_pRxMpdu->Mpdu.srcAddr.Extended != mpib.macCoordExtendedAddress)))
    {
        /*Transmit conflict command*/
        //beacon_transmitConflictNotif();
    }
    else if (((nTemp == AM_SHORT_16)&&(par_pRxMpdu->Mpdu.srcAddr.Short == mpib.macCoordShortAddress))
        || ((nTemp == AM_EXTENDED_64)&&(par_pRxMpdu->Mpdu.srcAddr.Extended == mpib.macCoordExtendedAddress)))
    {                    
//        TOGGLE_RLED();
        /*Not conflict*/
        pSuperframe = par_pRxMpdu->Mpdu.PayLoad;

        //update the PIB        
        nOldBeaconOrder             = mpib.macBeaconOrder;        
        mpib.macSuperframeOrder     = pSuperframe->SuperFrmOrder;
        mpib.macBeaconOrder         = pSuperframe->BeaconOrder;
        BeaconInfo.nFinalCapSlot    = pSuperframe->FinalCap;
        BeaconInfo.bBatteryLifeExt  = pSuperframe->BatLifeExt;

        // Calculate the beacon duration (BOS) including IFS
        BeaconInfo.nBeaconDuration = msupCalcPacketDuration(2
                                                          + 1 + 2
                                                          + ((par_pRxMpdu->Mpdu.FrmControl.SourceAddMode
                                                           == AM_EXTENDED_64)
                                                           ? 8
                                                           : 2)
                                                          + mac_getMpduLength(&par_pRxMpdu->Mpdu),
                                                            FALSE);    
        //align the timer with the beacon.
        if (mpib.macBeaconOrder != 15)
            beacon_synSlotTime();

        //If the type of network change from non-beacon to beacon
        if (nOldBeaconOrder == 15 && mpib.macBeaconOrder != 15)
        {
            //do nothing for now,
        }
        else if (nOldBeaconOrder != 15 && mpib.macBeaconOrder == 15)
            //update the delayed PIB attr. now
            mac_updateDelayedPib();       

        //transmit auto request data 
        if (mpib.macAutoRequest)            /* Search for the address to send the data */                        
        {
            bPendData = FALSE;
            // Find offset to Pending Address Specification in the payload
            //               < SFS + GTS >   <-------- GTS list ------->
            nPendAddrOffset =    2  +  1    + 3 * (par_pRxMpdu->Mpdu.PayLoad[2] & 0x07);        
            pPending    = &par_pRxMpdu->Mpdu.PayLoad[nPendAddrOffset++];   /* Get the Pending Address Specification*/             
            nTemp       = pPending->NumofShortAddr;
            while (nTemp--)                                   /* Short addresses are located first */
            {
                if (mpib.macShortAddress == *((WORD*) (par_pRxMpdu->Mpdu.PayLoad + nPendAddrOffset))) {
                    bPendData = TRUE;
                    break;
                }
                nPendAddrOffset += 2;
            }

            // Then check extended addresses, see if anyone has pending data!
            if (!bPendData)
                nTemp       = pPending->NumofExtAddr;
                while (nTemp--) {
                    if (aExtendedAddress == *(QWORD*) (par_pRxMpdu->Mpdu.PayLoad + nPendAddrOffset)) {
                        bPendData = TRUE;
                        break;
                    }
                    nPendAddrOffset += 8;
                }
    
            /* Send data request */
            if (bPendData)
                mlme_autoPoll();
        }                                   

        //make resume for the delayed transmission task, if exist
        if (MacTx.bUsedCsmaca && MacTx.sCsmaca.bWaitBeaconForTx)
            tx_resumeAfterBeacon(FALSE);    
        
        BeaconInfo.bBeaconLost      = FALSE;

        /* Mark that we received the beacon */
        if (BeaconInfo.bTrackBeacon || BeaconInfo.bTrackBeaconReq)
            beacon_beaconReceivedOnTracking();
    }   
} // End of functions


/******************************************************************************/
//@FunctionName:  beacon_synsBeaconTask                                       */
//@Description:	 This task will create a timer to enable Rx to track beacon   */
//               If necessary, it will create an other timer to track beacon  */
//               periodically                                                 */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void beacon_synsBeaconTask(void* Param,void* arg1)
{
   /*********** Variable declaration ***********/
    BYTE*               pTem;
    /*********** Function body       ***********/
    pTem = &Param;
               
    if (pTem[0] == 0x00)  /* Periodically track beacon */
    {
        
        //setup a timer to process beacon timeout
        timer_startTimer(createTimeoutSyncBncTask,NULL, ((UINT32)(aBaseSuperframeDuration / aUnitBackoffPeriod) << mpib.macBeaconOrder));    

        //if the maximum of beacon loss is reached
        if(BeaconInfo.nCurrNumOfLoss == 0)
        {
            //Beacon is loss, remove the current mode
            BeaconInfo.bTrackBeacon     = FALSE;
            BeaconInfo.bBeaconLost      = TRUE;
            //Beacon loss indication
            mlmeSyncLossIndication(BEACON_LOSS);
        }
    }
    else /* Sync beacon request from upper */
    {
        //Check the current state of syn.-beacon 
        if (BeaconInfo.bTrackBeacon)
        {
            //if currently, beacon is tracked and already syn., cancel the current tracking task
            timer_endTimer(createSyncBeaconTask); 
            timer_endTimer(createTimeoutSyncBncTask);
        }

        //reset the state
        BeaconInfo.bTrackBeaconReq  = TRUE;
        BeaconInfo.bTrackBeacon     = pTem[1];    
        BeaconInfo.nCurrNumOfLoss   = aMaxLostBeacons;
        BeaconInfo.bBeaconLost      = FALSE;

        //Set the current channel and other necessary parameters
        plmeSET_request(PHY_PIB_CURRENT_CHANNEL, pTem[0]);

        //Add a timer to track beacon searching process in a beacon period
        timer_startTimer(createTimeoutSyncBncTask,NULL, (UINT32) (aBaseSuperframeDuration / aUnitBackoffPeriod) << (UINT32) mpib.macBeaconOrder);
    }
    
    //start RX to receive the beacon
    mac_setTrxState(PHY_RX_ON);    

    //remove the current task
    schedul_removeTask(TASK_PER_MAC_PERIODICAL);           
} // End of functions

/******************************************************************************/
//@FunctionName: beacon_beaconReceivedOnTracking  							  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void beacon_beaconReceivedOnTracking()
{
	/*********** Variable declaration ***********/
    
	/*********** Function body		 ***********/
    if (BeaconInfo.bTrackBeaconReq)
        BeaconInfo.bTrackBeaconReq = FALSE;

    /*End timer*/
    timer_endTimer(createTimeoutSyncBncTask);    
    timer_endTimer(createSyncBeaconTask);

    BeaconInfo.nCurrNumOfLoss = aMaxLostBeacons;
    /* Continue to track the beacon */
    if (BeaconInfo.bTrackBeacon)
        timer_startTimer(createSyncBeaconTask,NULL,
                        ((UINT32) (aBaseSuperframeDuration / aUnitBackoffPeriod) << (UINT32) mpib.macBeaconOrder)
                           - 4*beacon_calBeaconMargin());
} // End of functions

/******************************************************************************/
//@FunctionName:  beacon_timeoutSyncBeacon 									  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void beacon_timeoutSyncBeacon(void* args,void* par_pParam)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    /* End of initial search, lost beacon*/
    if (BeaconInfo.bTrackBeaconReq)         /* Beacon are being searching*/
    {
        BeaconInfo.bTrackBeaconReq = FALSE;
        
        //turn off the Rx    
        mac_setTrxState(PHY_TRX_OFF);

        //Beacon loss indication
        mlmeSyncLossIndication(BEACON_LOSS); 
        
        return;
    }

    /* Time out tracking periodically beacon*/
    if (BeaconInfo.bTrackBeacon)    
    {
        BeaconInfo.nCurrNumOfLoss--;    /* reduce beacon loss counter */    

        timer_endTimer(createSyncBeaconTask);
        /* Continue to track the beacon */
        timer_startTimer(createSyncBeaconTask,NULL,
                        ((UINT32) (aBaseSuperframeDuration / aUnitBackoffPeriod) << (UINT32) mpib.macBeaconOrder)
                          - 2*beacon_calBeaconMargin());
    }
    schedul_removeTask(TASK_PER_MAC_PERIODICAL);
} // End of functions

/******************************************************************************/
//@FunctionName: beacon_notifyBeacon                                          */
//@Description: This function will make a notification packet and notify the
//              upper layer the appearance of new beacon                      */
//                                                                            */    
//@Param                                                                      */
//                                                                            */   
/******************************************************************************/
void beacon_notifyBeacon(P_MPDU_RX      par_pRxMpdu)
{
    /*********** Variable declaration ***********/
    MLME_BEACON_NOTIFY_INDICATION*  pBeaconNof;
    PENDING_ADDR_SPEC*              pPending;
    UINT8                           nOffset;
    UINT8                           nTemp;
    UINT8                           NumPendingAddr;
    /*********** Function body       ***********/
    pBeaconNof = &BeaconNof;

    //Make up a beacon notification
    pBeaconNof->PanDescriptor.LogicalChannel = PhyInfo.PhyPIB.phyCurrentChannel;
    pBeaconNof->PanDescriptor.LinkQuality = par_pRxMpdu->Mpdu.Fcs.LinkQuality;
    pBeaconNof->PanDescriptor.SuperframeSpec = *((WORD *)
                                                 par_pRxMpdu->Mpdu.PayLoad);
    pBeaconNof->PanDescriptor.GTSPermit = !!(par_pRxMpdu->Mpdu.PayLoad[2] & 0x80);
    pBeaconNof->PanDescriptor.SecurityUse = par_pRxMpdu->Mpdu.FrmControl.SecurityEnable;
    pBeaconNof->PanDescriptor.SecurityFailure = par_pRxMpdu->Mpdu.FrmControl.SecurityEnable; //TBD
    pBeaconNof->PanDescriptor.CoordAddress = par_pRxMpdu->Mpdu.srcAddr;    
    pBeaconNof->PanDescriptor.CoordAddrMode = par_pRxMpdu->Mpdu.FrmControl.SourceAddMode;
    pBeaconNof->PanDescriptor.CoordPANId = par_pRxMpdu->Mpdu.srcPanId;
    //pBeaconNof->PanDescriptor.ACLEntry, Timestamp, ACLEntry       = TBD
    pBeaconNof->PanDescriptor.TimeStamp  = OSTmrTime;   

    //Gts information field
    nOffset = (par_pRxMpdu->Mpdu.PayLoad[2] & 0x07);
    if (nOffset)
        nOffset = 3 + 1 + 3 * nOffset;
    else
        nOffset = 3;        //GTS not present;

    //Pending address
    pBeaconNof->PendAddrSpec = par_pRxMpdu->Mpdu.PayLoad[nOffset];
    pPending                 = &pBeaconNof->PendAddrSpec;

    nOffset++; /*Pending spec*/

	nTemp   = 0;
    //copy the address list: Short-Addr
    NumPendingAddr           = pPending->NumofShortAddr;
    while (NumPendingAddr--) {
        nTemp++;                                                                    
        pBeaconNof->AddrList[nTemp].Short = *((WORD*) (par_pRxMpdu->Mpdu.PayLoad + nOffset /* Super frame and GTS */
                                                  + nTemp*sizeof(WORD)));
    }
    nOffset += pPending->NumofShortAddr*sizeof(WORD);

    //copy the address list: Ext-Addr
    NumPendingAddr           = pPending->NumofExtAddr;
    while (NumPendingAddr--) {
        nTemp++;                                                                    
        pBeaconNof->AddrList[nTemp].Extended = *((QWORD*) (par_pRxMpdu->Mpdu.PayLoad + nOffset /* Super frame and GTS */
                                                  + nTemp*sizeof(QWORD)));
    }   
    nOffset +=  pPending->NumofExtAddr*sizeof(QWORD);

    //copy the payload
    pBeaconNof->sduLength = par_pRxMpdu->Mpdu.nLength-nOffset;    /* Super frame and GTS and pending addrs.*/
    mem_cpy(&pBeaconNof->Payload, par_pRxMpdu->Mpdu.PayLoad+nOffset, pBeaconNof->sduLength);

    //notify the upper layer
    mlmeBeaconNotifyIndication(pBeaconNof);

} // End of functions

#if     ZIGBEE_DEVICE_FFD == TRUE  
/******************************************************************************/
//@FunctionName: beacon_formatBeacon  										  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void beacon_formatBeacon()
{
	/*********** Variable declaration ***********/
    SUPER_FRAME*        sSuperFrm; 
    BYTE*               pPayload;
    P_MPDU_TX           pBeacon;
	/*********** Function body		 ***********/
    pBeacon             = BeaconInfo.pBeacon;
    /* Update the beacon */
    pBeacon->Mpdu.SeqNum = mpib.macBSN++;

    /* Format the beacon data */
    pPayload = BeaconInfo.pBeacon->Mpdu.PayLoad;
    /* Superframe */
    sSuperFrm = (SUPER_FRAME*)pPayload;
    sSuperFrm->AssoPermit       = mpib.macAssociationPermit;
    sSuperFrm->BatLifeExt       = mpib.macBattLifeExt;
    sSuperFrm->BeaconOrder      = mpib.macBeaconOrder;
    sSuperFrm->SuperFrmOrder    = mpib.macSuperframeOrder;
    sSuperFrm->PanCoord         = (macInfo.DeviceType == ZIGBEE_PAN_COOR);
    sSuperFrm->FinalCap         = 0; /*TBD*/

    /*GTS*/
    pPayload[sizeof(SUPER_FRAME)] = 0;

    /*Beacon payload*/
    mem_cpy(pPayload+sizeof(SUPER_FRAME) + 1/*GTS*/ + BeaconInfo.nPendingAddrLen,
			mpib.macBeaconPayload,mpib.macBeaconPayloadLength);
    BeaconInfo.pBeacon->Mpdu.nLength = sizeof(SUPER_FRAME) + 1/*GTS*/+
			BeaconInfo.nPendingAddrLen + mpib.macBeaconPayloadLength;

    /*Beacon duration*/
    BeaconInfo.nBeaconDuration = msupCalcPacketDuration(BeaconInfo.pBeacon->Mpdu.nLength,FALSE);
} // End of functions

/******************************************************************************/
//@FunctionName: beacon_transBeacon  										  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void beacon_transBeacon()
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    /* Update the indirect queue: Update TTL and remove expired packet */
    BeaconInfo.nPendingAddrLen = indirectTx_controlIndirectPkt(BeaconInfo.pBeacon->Mpdu.PayLoad+2+1/*S-frm+GTS*/);

    /* Make beacon */
    beacon_formatBeacon();      
    
    /* Transmit the beacon */
    tx_transBeacon(BeaconInfo.pBeacon);                      

    /**/
    DISABLE_GLOBAL_INT();
    mpib.macBeaconTxTime = OSTmrTime;        
    ENABLE_GLOBAL_INT();

    tx_resumeAfterTxBeacon();
} // End of functions
/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void beacon_transBeaconOnRequest()
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    /* Make beacon */
    BeaconInfo.nPendingAddrLen = 1;         /*No pending*/
    BeaconInfo.pBeacon->Mpdu.PayLoad[3] = 0;
    beacon_formatBeacon();      

    /* Transmit the beacon */
    tx_trans(BeaconInfo.pBeacon);                      
} // End of functions
/******************************************************************************/
//@FunctionName:  beacon_transBeaconTask 									  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void beacon_endBeaconTx(){timer_endTimer(createBeaconTxTask);} 
void beacon_createBeaconTxTask(void* arg1,void* arg2);
void beacon_transBeaconTask(void* par_pParam, void* args)
{
	/*********** Variable declaration ***********/
	UINT32		nBeaconDuration;
	/*********** Function body		 ***********/    
    /*Update the buffered PIB */
    mac_updateDelayedPib();
   
    /*Schedule for the next trans*/
    schedul_removeTask(TASK_PER_MAC_PERIODICAL);       /* Remove */

    /*Schedule beacon trans with adjusted time*/
    nBeaconDuration = ((UINT32)(aBaseSuperframeDuration / aUnitBackoffPeriod) << mpib.macBeaconOrder);
    if ((BeaconInfo.nBeaconOffset != 0)) /*This will be sync by parent*/
    {
        /*Sync the beacon with the parent*/
            if (BeaconInfo.nBeaconOffset <= SlotInfo.bosCounter%nBeaconDuration) /*Beacon is delayed*/
            {
                nBeaconDuration = nBeaconDuration - (SlotInfo.bosCounter%nBeaconDuration - BeaconInfo.nBeaconOffset);
            }
            else  /*Beacon is faster than the parent*/
            {
                nBeaconDuration = nBeaconDuration + (BeaconInfo.nBeaconOffset - SlotInfo.bosCounter%nBeaconDuration);
            }
    }   

    /*Sync the beacon*/
    timer_startTimer(createBeaconTxTask,NULL,          /* And call-back later */
                    nBeaconDuration);
   
    /*Transmit beacon*/
    beacon_transBeacon();    

    /* set up the end of CAP*/
    if (!mpib.macAssociationPermit) 
    {
        if (mpib.macRxOnWhenIdle)
		{
			
            timer_startTimer(forceOffTrx,NULL, (UINT32)(beacon_calBeaconMargin()+
                (UINT32)(((aBaseSlotDuration / aUnitBackoffPeriod) * (UINT32) (BeaconInfo.nFinalCapSlot + (UINT32) 1)) << (UINT32) mpib.macSuperframeOrder)));
		}
        else
            mac_setTrxState(PHY_TRX_OFF);
    }
    
	//TOGGLE_RLED(); /*Beacon*/
} // End of functions

/******************************************************************************/
//@FunctionName: beacon_createBeaconTxTask  								  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void beacon_createBeaconTxTask(void* arg1,void* arg2)
{
    schedul_addTask(beacon_transBeaconTask, TASK_PER_MAC_PERIODICAL,NULL);
}

/******************************************************************************/
//@FunctionName:  beacon_startBeacon 										  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void beacon_startBeacon(void* par_pTimer, void* par_pParam)
{
	/*********** Variable declaration ***********/
    UINT8   nErr;
	/*********** Function body		 ***********/
    if (par_pTimer!=NULL)
        OSTmrDel(par_pTimer,&nErr);

    beacon_createBeaconTxTask(NULL, NULL);
    /*Mark the beacon transmission point*/
    if (macInfo.DeviceType == ZIGBEE_ROUTER)
        BeaconInfo.nBeaconOffset = SlotInfo.bosCounter%((UINT32)(aBaseSuperframeDuration / aUnitBackoffPeriod) << mpib.macBeaconOrder);
} // End of functions

#endif  // ! ZIGBEE_DEVICE_FFD == TRUE  
