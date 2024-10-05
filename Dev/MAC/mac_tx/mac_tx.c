/*******************************************************************************/
//Module: MAC\mac_tx                                                           */   
//File Name: mac_tx.c                                                          */
//Description: Handles functions for packet transmissions   				   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//01/10/2007        pnthai          create                                     */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "mac_tx.h"
#include "csma-ca.h"
#include "mac_mlme.h"
#include "mac_primitives.h"

#include "mac_packet.h"
#include "mac_mlme.h"
#include "phy_primitives.h"

#include "phy_support.h"
#include "static_timer.h"
#include "schedul.h"
#include "mac_beacon.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/

/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
MAC_TX              MacTx;
/* Scan information
*/
/************************************************************************/
/*                      Static Function                                  */
/************************************************************************/
/******************************************************************************/
//@FunctionName: tx_transTask							  	                  */
//@Description:	This is task to trans packet                                  */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void tx_transTask(void*,void* par_pParam);


/******************************************************************************/
//@FunctionName:  tx_send 										  	          */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL tx_send(P_MPDU_TX     par_pPacket);

/******************************************************************************/
//@FunctionName: tx_endTrans							  	                  */
//@Description:	 Procedure to end trans. of a packet: cmd, data...            */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    tx_endTrans(P_MPDU_TX     par_pMpdu, MAC_STATUS    par_macStatus);

/******************************************************************************/
//@FunctionName: tx_timeoutAck  										  	  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void tx_timeoutAck(void*, void* par_pParam);

/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/******************************************************************************/
//@FunctionName: tx_init  										  	          */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void tx_init()
{
    MacTx.eState    = MAC_TX_IDLE;
    MacTx.pPacket   = NULL;
}

/******************************************************************************/
//@FunctionName: Tx_Trans								  	                  */
//@Description:	 This function will create an task to handle packet trans     */
//               until successful or expired. Task created is queued in the   */
//               task queue                                                   */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL tx_trans(P_MPDU_TX     par_pMpdu)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/    
    par_pMpdu->bTransmitting = FALSE;                                   /* This packet is not current pkt*/

    return schedul_addTask(tx_transTask, TASK_PER_MAC_TX, (DWORD*)par_pMpdu);  /* Add task to trans*/
} // End of functions

/******************************************************************************/
//@FunctionName:Tx_notifyAck							  	                  */
//@Description:	This function will notify the Trans layer a ACK received      */
//	            Return will be TRUE if it is a valid ACK and current transTask
//              will be stopped, else return false and ACK will be discarded  */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL tx_notifyAck(P_MPDU_RX par_pRxMpdu)
{
	/*********** Variable declaration ***********/
    BOOL    bReturn;
	/*********** Function body		 ***********/
    bReturn = FALSE;

    if (MacTx.pPacket != NULL)
    {
#ifdef ZIGBEE_STUB
        par_pRxMpdu->Mpdu.SeqNum = MacTx.pPacket->Mpdu.SeqNum;
#endif
        if (MacTx.pPacket->Mpdu.SeqNum == par_pRxMpdu->Mpdu.SeqNum)
        {
            bReturn = TRUE;
            MacTx.eState = MAC_TX_ACKED;
            MacTx.bFrmPending = par_pRxMpdu->Mpdu.FrmControl.FramePending;
        }
    }
    return bReturn;
} // End of functions


/******************************************************************************/
//@FunctionName: Tx_send								  	                  */
//@Description:	 This function will handle sending a packet using CSMA-CA     */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL tx_send(P_MPDU_TX    par_pMpdu)
{
	/*********** Variable declaration ***********/
    BOOL                eTxState;
    BYTE                nTemp;
	/*********** Function body		 ***********/

    eTxState = FALSE;

    /* If using Csma-ca*/
    if (MacTx.bUsedCsmaca) 
    {    
        // Wait for the back-off slot boundary before transmitting
        if (MacTx.sCsmaca.bSlotted) 
            WAIT_FOR_BOUNDARY(); 
        else 
        {
#ifndef ZIGBEE_STUB
//             do {
//                 FASTSPI_UPD_STATUS(nTemp);
//             } while (!(nTemp & CC2420_RSSI_VALID_BM));
#endif
        }
        
    }    
    pddata_request(&par_pMpdu->Mpdu);
    return TRUE;
} // End of functions

/******************************************************************************/
//@FunctionName: tx_transTask							  	                  */
//@Description:	This is task to trans packet                                  */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void tx_transTask(void* par_pParam, void* args)
{
	/*********** Variable declaration ***********/
    P_MPDU_TX       pMpdu;    
    CSMACA_RESULT   eResult;
    BOOL            bSlotted;
    PHY_STATUS      sStatus;
    UINT8           nTemp;
	/*********** Function body		 ***********/

    /* Set up for initialize transmission */
    pMpdu=(P_MPDU_TX)par_pParam;

    if (pMpdu->bTransmitting == FALSE)
    {
        MacTx.eState                = MAC_TX_INIT;
        MacTx.bUsedCsmaca           = ( pMpdu->PktInfo.txMode & TX_MODE_CSMACA);
        MacTx.pPacket               = pMpdu;
        pMpdu->bTransmitting        = TRUE;                           
    }

    switch(MacTx.eState)
    {
    case MAC_TX_INIT:

        //set internal value of the MPDU
        MacTx.eState      = MAC_TX_PROCESSING;

        //if used csma-ca, initialize it.
        if (MacTx.bUsedCsmaca)
        {
            //when using csma-ca transmission            
            bSlotted = !((mpib.macBeaconOrder == 15) || (pMpdu->PktInfo.txMode & TX_MODE_FORCE_UNSLOTTED)
                          ||  (BeaconInfo.bBeaconLost  == TRUE));
                csmaca_initCsmaca(&MacTx.sCsmaca, &pMpdu->PktInfo,bSlotted);

            //next state
            MacTx.eState = MAC_TX_PROCESSING;
        }    
        else
        {
            //set the initial state for Trans-layer
            MacTx.eState = MAC_TX_TRANS;        
        }        

        break;
    case MAC_TX_PROCESSING:
        //run CSMA-CA procedure
        csmaca_process(&MacTx.sCsmaca);
        eResult = MacTx.sCsmaca.eCurrentResult;
        if (eResult == CSMACA_RESULT_SUCCESS)
        {
            MacTx.eState = MAC_TX_TRANS;
        }
        else if ((eResult == CSMACA_RESULT_FAILURE)
            || (eResult == CSMACA_RESULT_SHORT_TIME))
        {
            //failed, end the transmission
            tx_endTrans(pMpdu, CHANNEL_ACCESS_FAILURE);            
            break;
        }
        else if (MacTx.sCsmaca.bWaitBeaconForTx == TRUE)
        {
            schedul_suspend(TASK_PER_MAC_TX);  /*Suspend the task*/ 
            break;
        }
        else 
            //no result now, try again
            break;
        
    case MAC_TX_TRANS:
        //start TX
        sStatus = mac_setTrxState(PHY_TX_ON);
        if (sStatus == PHY_SUCCESS)
        {
            if (! tx_send(pMpdu))
            {
                //try again, do it later
            }
            else
            {                
                //set timer to check ACK
                if(pMpdu->Mpdu.FrmControl.ACKRequest)
                {
                    /* Auto start RX to receive the ACK*/
                    phyrx_initOnRx();
                    MacTx.eState = MAC_TX_WAIT_ACK;
                    timer_startTimer(timeoutAck, (void*)pMpdu,
                                     (UINT32)(pMpdu->PktInfo.nDuration+2));
                }
                else 
                {
                    /*End of transmission*/
                    MacTx.eState = MAC_TX_TRANSMITTING;                                        
                }
            }
        }
        break;
    case MAC_TX_TRANSMITTING:        
        FASTSPI_UPD_STATUS(nTemp);
        if (!(nTemp & CC2420_TX_ACTIVE_BM))
        {
            tx_endTrans(pMpdu,SUCCESS);
        }
        break;
    case MAC_TX_WAIT_ACK:
        /*Waiting for ACK*/
        break;
    case MAC_TX_ACKED:

        //remove the timer
        if (MacTx.pPacket->Mpdu.FrmControl.ACKRequest)
            timer_endTimer(timeoutAck);
        
        //end of transmission successfully
        tx_endTrans(MacTx.pPacket, SUCCESS);


        break;
    case MAC_TX_NO_ACK:
        //no retry left, ACK not received yet
        tx_endTrans(pMpdu, NO_ACK);    
        
        break;
    default:
        break;
    }

} // End of functions

/******************************************************************************/
//@FunctionName:  tx_timeoutAck 										  	  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void tx_timeoutAck(void* args, void* par_pParam)
{
	/*********** Variable declaration ***********/
    P_MPDU_TX   pMpdu;
    
	/*********** Function body		 ***********/
    pMpdu = (P_MPDU_TX)par_pParam;
    
    //not received ACK yet, decrease nRetryLeft 
    pMpdu->PktInfo.nRetryLeft--;

    //and retry
    if (pMpdu->PktInfo.nRetryLeft > 0)
    {
        MacTx.eState = MAC_TX_INIT;
    }
    else
    {
        MacTx.eState = MAC_TX_NO_ACK;
    }

} // End of functions


/******************************************************************************/
//@FunctionName: tx_endTrans							  	                  */
//@Description:	 Procedure to end trans. of a packet: cmd, data...            */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    tx_endTrans(P_MPDU_TX     par_pMpdu, MAC_STATUS    par_macStatus)
{
	/*********** Variable declaration ***********/
        
	/*********** Function body		 ***********/
    mac_manageTrxAfterTx(par_pMpdu,par_macStatus);

    /*Indirect packet*/
#if     ZIGBEE_DEVICE_FFD == TRUE  
    if (par_pMpdu->PktInfo.txOptions & TX_OPT_INDIRECT)
    {
        if(par_macStatus == SUCCESS)
        {
            indirectTx_removePkt(par_pMpdu);        
        }
    }
#endif
    if (!(par_pMpdu->PktInfo.txOptions & TX_OPT_INDIRECT))
        /* Call to appropriate transaction */
        switch(par_pMpdu->Mpdu.FrmControl.FrameType)
        {
        case MAC_FRM_CMD:
            switch(macInfo.sState.nTransacType)
            {
            case MAC_TRANSAC_ASSOSCIATION:
                mlme_notifyAssociation(par_macStatus, (void*)par_pMpdu);
        	    break;
            case MAC_TRANSAC_DISASSOSCIATION:
                mlme_handleDisassoTransition(par_macStatus, (void*)par_pMpdu);
                break;
            case MAC_TRANSAC_POLL:
                mlme_handlePollTransition(par_macStatus, par_pMpdu);
                break;            
            case MAC_TRANSAC_SCAN:
                if (par_macStatus != SUCCESS) /* Failure to transmit on the channel, finished the channel */
                {
                    macInfo.sState.nTransacState = SCAN_FINISHED_CHANNEL; /*Finish scan current channel*/                   
                    break;
                }
                switch(pScanInfo->scanType)     /* If success in transmitting */
                {
                case ED_SCAN:  
                    break;
                case ACTIVE_SCAN:  
                    /* Set timer to check the timeout */
                    timer_startTimer(scanTimeout,NULL, 
                     ((UINT32) aBaseSuperframeDuration / (UINT32) aUnitBackoffPeriod) * (((UINT32) 1 << pScanInfo->scanDuration) + 2));               
                    mem_releaseTxPkt(MacTx.pPacket);
                    break;
                case PASSIVE_SCAN:
                    break;
                case ORPHAN_SCAN:  
                    timer_startTimer(scanTimeout,NULL,                      /* Set timer to check the timeout */
                     (UINT16) aResponseWaitTime / aUnitBackoffPeriod);                               
                    break;
                }   
                break;  
            case MAC_TRANSAC_NO:            
                mem_releaseTxPkt(MacTx.pPacket);
                break;
            default:
                mem_releaseTxPkt(MacTx.pPacket);
                break;
            }
    	    break;
        case MAC_FRM_DATA:
            if (!(MacTx.pPacket->PktInfo.txMode&TX_MODE_MAC_INTERNAL))
                mcpsDataConfirm(MacTx.pPacket->PktInfo.msduHandle,par_macStatus);
            mem_releaseTxPkt(MacTx.pPacket);
            break;
        case MAC_FRM_BEACON:
        case MAC_FRM_ACK:
            break;
        default:
            break;
        }

    /*Finished trans*/
    MacTx.eState = MAC_TX_IDLE;                                        
    
    //remove this task,
    schedul_removeTask(TASK_PER_MAC_TX);  
} // End of functions

/******************************************************************************/
//@FunctionName:tx_transBeacon   										  	  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void tx_transBeacon(P_MPDU_TX par_pBeacon)
{
    phytx_turnOnTx();
    pddata_request(&par_pBeacon->Mpdu);
    mac_setTrxState(PHY_RX_ON);
} // End of functions

/******************************************************************************/
//@FunctionName:  tx_resumeAfterBeaconTxTask 								  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void tx_resumeAfterBeaconTxTask(void* arg, void* arg1)
{
    tx_resumeAfterBeacon(TRUE);
    schedul_removeTask(TASK_PER_MAC_PERIODICAL);
} // End of functions
void tx_resumeAfterTxBeacon()
{
    if (macInfo.DeviceType == ZIGBEE_PAN_COOR)
        SlotInfo.bosCounter = BeaconInfo.nBeaconDuration + 2;
    schedul_addTask(tx_resumeAfterBeaconTxTask,TASK_PER_MAC_PERIODICAL,NULL);
} // End of functions

/******************************************************************************/
//@FunctionName:  tx_resumeAfterBeacon 										  */
//@Description:	 This function will resume the delayed trans. task after beacon*/
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT32 capRequired;
UINT32 capLeft;
UINT32 nCap;
void tx_resumeAfterBeacon(BOOL  bTransBeacon)
{
    if (MacTx.sCsmaca.bWaitBeaconForTx == TRUE) 
    {            
        if (bTransBeacon)/*Beacon transmitted*/
        {
            if (!MacTx.pPacket->PktInfo.bToCoord) /*To child or others */
			{
                nCap    = (((aBaseSlotDuration / aUnitBackoffPeriod) * (UINT32) (BeaconInfo.nFinalCapSlot + (UINT32) 1)) << (UINT32) mpib.macSuperframeOrder);
                /*            <- The CAP duration >   <------ Used CAP ------->         <-- Beacon margin --> */
                capLeft     = nCap                   - (BeaconInfo.nBeaconDuration+ 1) - beacon_calBeaconMargin(); 
			}
            else
                capLeft = 0;

        }
        else /*Beacon received*/
        {
            /* Cancel location timeout*/
            timer_endTimer(locateBeaconTimeout);

            if (MacTx.pPacket->PktInfo.bToCoord) /*To parent*/
			{
                nCap    = (((aBaseSlotDuration / aUnitBackoffPeriod) * (UINT32) (BeaconInfo.nFinalCapSlot + (UINT32) 1)) << (UINT32) mpib.macSuperframeOrder);
                //            <- The CAP duration >   <------- Used CAP ------>    <-- Beacon margin -->                                                                  
                capLeft     = nCap                 - (SlotInfo.bosCounter + 1) -  beacon_calBeaconMargin();         
			}
            else
                capLeft = 0;
        }

         /* Calculate the CAP required (number of backoff slots) */ 
        if (BeaconInfo.bBatteryLifeExt) {
            /*Need to be corrected*/
            //        <----------------- The CAP duration ----------------->   <------ Used CAP ------->
            capLeft     = (BeaconInfo.nBeaconDuration + mpib.macBattLifeExtPeriods) - (SlotInfo.bosCounter + 1);
            capRequired = MacTx.sCsmaca.nRandomBackoff + 2 + 1 + MacTx.sCsmaca.bSlotted;
        } 
        else 
            capRequired = MacTx.sCsmaca.nRandomBackoff + 2 + 
                            MacTx.pPacket->PktInfo.nDuration;

        /* Enough time left in the CAP */
        if (capLeft >= capRequired) 
        {
            schedul_activate(TASK_PER_MAC_TX);            
            MacTx.sCsmaca.bWaitBeaconForTx = FALSE;           
        } 
        else 
        {
            /*Adjust the time remained*/
            if (capLeft > 0) 
            {
                MacTx.sCsmaca.nRandomBackoff = MAX(0, MacTx.sCsmaca.nRandomBackoff - capLeft);
            }
        }
    }
} // End of functions

 
