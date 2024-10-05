/*******************************************************************************/
//Module: MAC/mac_primitive                                                    */   
//File Name: mac_state.c                                                       */
//Description: This files will provide function to manage MAC state			   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//01/30/2007        pnthai          Create                                     */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/


/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "mac_state.h"
#include "mac_types.h"
#include "mac_primitives.h"
#include "phy_support.h"
#include "mac_beacon.h"
#include "mac_tx.h"
#include "phy_primitives.h"
#include "mac_mlme.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/


/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/

//variable holds MAC PIB
MAC_PIB         mpib;

//Variable holds MAC info.
MAC_INFO        macInfo;

/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/


/************************************************************************/
/*                      Function                                        */
/************************************************************************/


/******************************************************************************/
//@FunctionName: mac_init   										  	      */
//@Description:	Call this function to first time initialization of MAC        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mac_init()
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    macInfo.sState.nTransacType = MAC_INITALIZATION;  /* initialization */
    phytrx_setPwrMode(PHY_PWR_ON);                    /* Initialize at first time power-on */
   
} // End of functions



/******************************************************************************/
//@FunctionName: mac_requestState  										  	  */
//@Description:	This function will control state transition of MAC            */
//              Currently, only one transaction is allowed at tim TBD: check this
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL    mac_requestState(UINT8 par_nTransacType, UINT8 par_nTransacState)
{
	/*********** Variable declaration ***********/
    BOOL        bResult;
	/*********** Function body		 ***********/
    bResult = TRUE;

    switch(par_nTransacType)
    {
    case MAC_TRANSAC_NO:
        bResult = TRUE;
    	break;
    case MAC_TRANSAC_ASSOSCIATION:
        switch(par_nTransacState)
        {
        case MAC_ASSOC_REQUEST:
            bResult = ((macInfo.sState.nTransacType == MAC_TRANSAC_NO)
                        || (macInfo.sState.nTransacType == MAC_ASSOC_REQUEST));
        	break;
        case MAC_ASSOC_REQUEST_SENT:
            bResult = (macInfo.sState.nTransacType == MAC_TRANSAC_ASSOSCIATION
                       && macInfo.sState.nTransacState == MAC_ASSOC_REQUEST);
        	break;
        case MAC_ASSOC_DATA_REQUEST:
            bResult = (macInfo.sState.nTransacType == MAC_TRANSAC_ASSOSCIATION
                       && macInfo.sState.nTransacState == MAC_ASSOC_REQUEST_SENT);
            break;
        case MAC_ASSOC_DATA_REQUEST_SENT:
            bResult = (macInfo.sState.nTransacType == MAC_TRANSAC_ASSOSCIATION
                       && macInfo.sState.nTransacState == MAC_ASSOC_DATA_REQUEST);
            break;
        default:
            break;
        }
            
    	break;
    case MAC_TRANSAC_SCAN:
        switch(par_nTransacState)
        {
        case SCAN_INITIALIZATION:
            bResult = (macInfo.sState.nTransacType == MAC_TRANSAC_NO);
            break;
        case SCAN_INIT_NEW_CHANNEL:
        case SCAN_SCANNING:
        case SCAN_FINISHED_CHANNEL:
            break;
        case SCAN_ORPHAN_REALIGNED:
            bResult = (macInfo.sState.nTransacType == SCAN_SCANNING);
            break;
        case SCAN_FINISHED:
            break;
        default:
            break;
        }
    	break;
    case MAC_TRANSAC_RXENABLE:
        bResult = (macInfo.sState.nTransacType == MAC_TRANSAC_NO
                    && macInfo.sState.nTransacState == MAC_TRANSAC_NO);
        break;
    default:
        break;
    }

    if (bResult)
    {
        macInfo.sState.nTransacType = par_nTransacType;
        macInfo.sState.nTransacState = par_nTransacState;
    }
    
    return bResult;
} // End of functions


/******************************************************************************/
//@FunctionName: mac_validateMacState   									  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL mac_validateMacState(MAC_COMMAND_TYPE parCmd, MAC_FRM_TYPE par_Frm)
{
	/*********** Variable declaration ***********/
    BOOL        bResult;

	/*********** Function body		 ***********/
    bResult = TRUE;
    switch(par_Frm)
    {
   case MAC_FRM_CMD:
        switch(parCmd)
        {
        case CMD_ASSOCIATION_RESPONSE:
            
            bResult = (macInfo.sState.nTransacType==MAC_TRANSAC_ASSOSCIATION
                      && macInfo.sState.nTransacState==MAC_ASSOC_DATA_REQUEST_SENT);

        	break;
        case CMD_DISASSOCIATION_NOTIFICATION:
            bResult = (macInfo.sState.nTransacType==MAC_TRANSAC_DISASSOSCIATION);
            break;
        case CMD_COORDINATOR_REALIGNMENT:
            bResult = ((macInfo.DeviceType == ZIGBEE_ROUTER) || (macInfo.DeviceType == ZIGBEE_ENDPOINT));
            break;
        case CMD_DATA_REQUEST:
        case CMD_PAN_ID_CONFLICT_NOTIFICATION:
        case CMD_ORPHAN_NOTIFICATION:
        case CMD_BEACON_REQUEST:
        case CMD_GTS_REQUEST:
            bResult = ((macInfo.DeviceType == ZIGBEE_ROUTER) || (macInfo.DeviceType == ZIGBEE_PAN_COOR));
            break;
        default:
            break;
        }
        
    	break;
    case MAC_FRM_DATA:

        bResult = TRUE;    

        break;
    case MAC_FRM_BEACON:               
        bResult = TRUE;        
        break;
    case MAC_FRM_ACK:
                
        bResult =  (MacTx.eState==MAC_TX_WAIT_ACK);

        break;
    default:
        break;
    }    

    return bResult;

} // End of functions


/******************************************************************************/
//@FunctionName: mac_validateFrmData  									      */
//@Description:	 This function will validate the coherence of data with the
//               current data of MAC                                          */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL mac_validateFrmData(P_MPDU_RX par_pRxMpdu) 
{
	/*********** Variable declaration ***********/
    BOOL    bReturn;
	/*********** Function body		 ***********/
    bReturn = TRUE;
    switch(par_pRxMpdu->Mpdu.FrmControl.FrameType)
    {
    case MAC_FRM_CMD:
        switch(par_pRxMpdu->Mpdu.PayLoad[0])
        {
        case CMD_ASSOCIATION_REQUEST:
            bReturn = mpib.macAssociationPermit;
            break;
        case CMD_ASSOCIATION_RESPONSE:
            break;
        case CMD_DISASSOCIATION_NOTIFICATION:
            break;
        case CMD_COORDINATOR_REALIGNMENT:
            bReturn = FALSE;
            if (par_pRxMpdu->Mpdu.FrmControl.SourceAddMode == AM_EXTENDED_64)
                if(((macInfo.sState.nTransacType == MAC_TRANSAC_SCAN)
                    && (par_pRxMpdu->Mpdu.FrmControl.DestAddMode == AM_EXTENDED_64))
                    || (par_pRxMpdu->Mpdu.srcAddr.Extended == mpib.macCoordExtendedAddress))             
                    bReturn =  TRUE;    
            break;
        case CMD_DATA_REQUEST:
            break;
        case CMD_PAN_ID_CONFLICT_NOTIFICATION:
            break;
        case CMD_ORPHAN_NOTIFICATION:                        
            break;
        case CMD_BEACON_REQUEST:
            break;
        case CMD_GTS_REQUEST:
            break;
        default:
            break;
        }       
    case MAC_FRM_DATA:
        break;
    case MAC_FRM_BEACON:
        break;
    case MAC_FRM_ACK:
        break;
    default:
        break;
    }

    return bReturn;
} // End of functions

/******************************************************************************/
//@FunctionName: mac_updateDelayedPib  										  */
//@Description:	This function will update the delayed Pib. It will be called 
//              at starting of a new super-frame                               */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mac_updateDelayedPib()
{
    DISABLE_GLOBAL_INT();
    if (macInfo.pibTempBuffer.updateMask & MPIB_UPD_BATT_LIFE_EXT_BM)    mpib.macBattLifeExt = macInfo.pibTempBuffer.macBattLifeExt;
    if (macInfo.pibTempBuffer.updateMask & MPIB_UPD_BEACON_ORDER_BM)     mpib.macBeaconOrder = macInfo.pibTempBuffer.macBeaconOrder;
    if (macInfo.pibTempBuffer.updateMask & MPIB_UPD_RX_ON_WHEN_IDLE_BM)  mpib.macRxOnWhenIdle = macInfo.pibTempBuffer.macRxOnWhenIdle;
    if (macInfo.pibTempBuffer.updateMask & MPIB_UPD_SUPERFRAME_ORDER_BM) mpib.macSuperframeOrder = macInfo.pibTempBuffer.macSuperframeOrder;
    macInfo.pibTempBuffer.updateMask = 0x00;
    ENABLE_GLOBAL_INT();
}


/******************************************************************************/
//@FunctionName: mac_manageTrxAfterTx  										  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mac_manageTrxAfterTx(P_MPDU_TX     par_pMpdu, MAC_STATUS    par_macStatus)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    /* Depend on transaction */
    switch(macInfo.sState.nTransacType)
    {
    case MAC_TRANSAC_ASSOSCIATION:
        if (macInfo.sState.nTransacState != MAC_ASSOC_DATA_REQUEST)
            phyrx_turnOffTrx(NULL);
        else
            phyrx_initOnRx();
        break;
    case MAC_TRANSAC_SCAN:
            phyrx_initOnRx();                       /* Auto start RX to receive the ACK*/
    default:
        if (mpib.macRxOnWhenIdle || BeaconInfo.bTrackBeaconReq)
        {
            phyrx_initOnRx();                       /* Auto start RX */
        }
        else
        {
            phyrx_turnOffTrx(NULL);
        }
        break;
    }
} // End of functions


/******************************************************************************/
//@FunctionName:  mac_setTrxState 										  	  */
//@Description:	This function will control the TRX state                      */
//                  - Turn on Rx while Tx is on, do nothing
//                  - Turn off Rx while Tx is on, do nothing
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
PHY_STATUS mac_setTrxState(PHY_STATUS par_State)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    //cause PHY change state
    if (((PhyInfo.PhyState == PHY_RX_ON) || (PhyInfo.PhyState == PHY_TRX_OFF)
        || (PhyInfo.PhyState == PHY_TX_ON))&&(PhyInfo.PhyState == par_State))
    {
    }
    else if (par_State == PHY_FORCE_TRX_OFF && PhyInfo.PhyState != PHY_TRX_OFF)
        phyrx_turnOffTrx(NULL);                     /* change state now !*/
    else if (par_State == PHY_TX_ON)
        phytx_turnOnTx();
    else if (par_State == PHY_TRX_OFF)
    {
        if (!mpib.macRxOnWhenIdle && !BeaconInfo.bTrackBeaconReq)
        {
            phyrx_turnOffTrx(NULL);
        }
    }
    else if ((PhyInfo.PhyState == PHY_TX_ON)&& (par_State == PHY_RX_ON))
        phyrx_initOnRx();
    else if (par_State == PHY_TX_ON)
        phytx_turnOnTx(NULL);
    else if ((par_State == PHY_RX_ON))    
        phyrx_turnOnRx(NULL);

    return PHY_SUCCESS;
} // End of functions

/******************************************************************************/
//@FunctionName: mac_forceOffTrx  										  	  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void mac_forceOffTrx(void*  arg1, void* arg2)
{
    phyrx_turnOffTrx(NULL);
}
