/*******************************************************************************/
//Module: MAC/phy_primitives                                                   */   
//File Name: phy_primitives.c                                                  */
//Description: Implements PHY function primitives                   		   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//12/27/2006        pnthai          Created                                    */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "phy_primitives.h"
#include "phy_support.h"

#include "mac_rx.h"
#include "mac_packet.h"
#include "mac_mem.h"

#include "schedul.h"

/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/


/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
PHY_INFO            PhyInfo;

/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/


/************************************************************************/
/*                      Function                                        */
/************************************************************************/



/******************************************************************************/
//@FunctionName: pddata_request  										  	  */
//@Description:	 The PD-DATA.request primitive requests the transfer of an 
//               MPDU (i.e., PSDU) from the MAC sublayer to                   */
//               the local PHY entity.                                        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
PHY_STATUS pddata_request(MPDU* par_pMpdu)
{
	/*********** Variable declaration ***********/
    PHY_STATUS      eStatus;    
    UINT8           nLength;
	/*********** Function body		 ***********/
    eStatus = PHY_INVALID_PARAMETER;

    if ((PhyInfo.PhyState == PHY_RX_ON) || (PhyInfo.PhyState == PHY_TRX_OFF))
    {
        eStatus = PhyInfo.PhyState;
    }

    if (PhyInfo.PhyState == PHY_TX_ON)
    {
        // Write length and the payload to the TX FIFO
        nLength= mac_getMpduLength(par_pMpdu)+MAC_FCS_LEN;
        phy_WriteFifo(&nLength, 1);

        /*Write the header and SEQ*/
        phy_WriteFifo(&par_pMpdu->FrmControl, sizeof(FRAME_CONTROL)+sizeof(UINT8));

        /*Write the des. address*/
        if (par_pMpdu->FrmControl.DestAddMode >= AM_SHORT_16)
        {
            nLength = sizeof(WORD) + (par_pMpdu->FrmControl.DestAddMode)*6-10;
            phy_WriteFifo(&par_pMpdu->dstPanId, nLength);
        }        

        /*Write the source address*/
        if (par_pMpdu->FrmControl.SourceAddMode >= AM_SHORT_16)
        {
            if (!par_pMpdu->FrmControl.IntraPAN)
            {
                nLength = sizeof(WORD)+(par_pMpdu->FrmControl.SourceAddMode)*6-10;
                phy_WriteFifo(&par_pMpdu->srcPanId, nLength);
            }
            else
            {
                nLength = (par_pMpdu->FrmControl.SourceAddMode)*6-10;
                phy_WriteFifo(&par_pMpdu->srcAddr, nLength);
            }            
        }        
        /*Send the payload*/
        phy_WriteFifo(&par_pMpdu->PayLoad, par_pMpdu->nLength);
        
        eStatus = PHY_SUCCESS;
    }

    return eStatus;
    
} // End of functions

/******************************************************************************/
//@FunctionName: pddata_indicationTask  	
//@Description:	The PD-DATA.indication primitive indicates the transfer of an 
//              MPDU (i.e., PSDU) from the PHY to the                         */
//              local MAC sublayer entity.                                    */
//	            Don't care Link quality                                       */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void pddata_indicationTask(void* par_pParam, void* args)
{
	/*********** Variable declaration ***********/
    P_MPDU_RX       pMpduRx;    
	/*********** Function body		 ***********/
    pMpduRx = (P_MPDU_RX)par_pParam;

   //call to appropriate function to process the incoming packet
    macrx_ProcessMpdu(pMpduRx);

    /* Decrease number of queuing pkt*/
    PhyInfo.nQueueingPkt--;

    schedul_removeTask(TASK_PER_MAC_RX);    /* Remove this task */

} // End of functions

/******************************************************************************/
//@FunctionName: pddata_indication  										  */
//@Description:	This indication will create a task to process the packet      */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void pddata_indication(P_MPDU_RX pRxMpdu,  UINT8 pPduLinkQuality)
{
	/*********** Variable declaration ***********/
    FRAME_CONTROL*  pFrmCtrl;
    BOOL            bResult;    
    /*********** Function body		 ***********/
    
    pFrmCtrl = (FRAME_CONTROL*)&pRxMpdu->Mpdu;   

    /*Validate state*/
    bResult = mac_validateMacState(pRxMpdu->Mpdu.PayLoad[0],pFrmCtrl->FrameType)
        && mac_validateFrmData(pRxMpdu);

    /* Omit the packet if not valid */
    if(!bResult)
    {
        return;
    }

    /* Check the data frame */
    if (pFrmCtrl->FrameType == MAC_FRM_ACK)
    {    
        /* It's ACK, call direct to TX */
        macrx_ProcessMpdu(pRxMpdu);        
    }
    else /* For other type create task to process */
    {
        bResult = schedul_addTask(pddata_indicationTask, TASK_PER_MAC_RX, (WORD*)pRxMpdu);            
        if (bResult)
        {
            /* Increment the number of queuing pkt*/
            PhyInfo.nQueueingPkt++;
    
            PhyInfo.sCurPacket.pCurRxPacket = mem_getFreeRxPkt();            
            
            return; /* Create task */
        }   
    }

} // End of functions

/******************************************************************************/
//@FunctionName: plmeCCA_request  										  	  */
//@Description: The PLME-CCA.request primitive requests that the PLME perform 
//              a CCA as defined in 6.7.9                                     */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
PHY_STATUS    plmeCCA_request(void)
{
	/*********** Variable declaration ***********/
    PHY_STATUS      eReturn;

	/*********** Function body		 ***********/    
    eReturn = INVALID_PARAMETER;

    if ((PhyInfo.PhyState == PHY_TX_ON) || (PhyInfo.PhyState == PHY_TRX_OFF))
    {
        eReturn = PhyInfo.PhyState;
    }
    if (PhyInfo.PhyState == PHY_RX_ON)
    {
        if (CCA_IS_ACTIVE())
        {
            eReturn = PHY_IDLE;
        }
        else
        {
            eReturn = PHY_BUSY;
#ifdef ZIGBEE_STUB
            eReturn = PHY_IDLE;
#endif
        }
    }
    return eReturn;
}

/******************************************************************************/
//@FunctionName:  plmeED_request 										  	  */
//@Description:	The PLME-ED.request primitive requests that the PLME perform 
//              an ED measurement (see 6.7.7).                                */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
PHY_STATUS plmeED_request (UINT8* par_EnergyLevel)
{
	/*********** Variable declaration ***********/
    INT8        maxRssi, rssi;
    PHY_STATUS  eStatus;

	/*********** Function body		 ***********/
    eStatus = INVALID_PARAMETER;

    if(PhyInfo.PhyState == PHY_RX_ON)
    {
        maxRssi = -128;
        DISABLE_GLOBAL_INT();
        FASTSPI_GETREG(CC2420_RSSI, rssi);
        ENABLE_GLOBAL_INT();
    
        // ... and detect the peak
        maxRssi = MAX(rssi, maxRssi);

        *par_EnergyLevel =  RSSI_2_ED(maxRssi);
        eStatus = PHY_SUCCESS;
    }

    if (PhyInfo.PhyState == PHY_TX_ON || PhyInfo.PhyState == PHY_TRX_OFF)
    {
        eStatus = PhyInfo.PhyState;
    }    
    
    return eStatus;
    
} // End of functions


/******************************************************************************/
//@FunctionName:   plmeGET_request										  	  */
//@Description:	The PLME-GET.request primitive requests information about 
//              a given PHY PIB attribute.                                    */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
PHY_STATUS plmeGET_request (PHY_PIB_ATT_TYPE par_PIBAttType, UINT32* par_pPIBAttributeValue)
{
	/*********** Variable declaration ***********/
     PHY_STATUS     eStatus;

	/*********** Function body		 ***********/
    switch(par_PIBAttType)
    {
    case PHY_PIB_CURRENT_CHANNEL: 
        *par_pPIBAttributeValue = PhyInfo.PhyPIB.phyCurrentChannel;
        eStatus = PHY_SUCCESS;
    	break;
    case PHY_PIB_CHANNEL_SUPPORTED: 
        *par_pPIBAttributeValue = PhyInfo.PhyPIB.phyChannelsSupported;
        eStatus = PHY_SUCCESS;
    	break;
    case PHY_PIB_TRANSMIT_POWER: 
        *par_pPIBAttributeValue = PhyInfo.PhyPIB.phyTransmitPower;
        eStatus = PHY_SUCCESS;
        break;
    case PHY_PIB_CCA_MODE: 
        *par_pPIBAttributeValue = PhyInfo.PhyPIB.phyCcaMode;
        eStatus = PHY_SUCCESS;
        break;
    default:
        eStatus = PHY_UNSUPPORTED_ATTRIBUTE;
        break;
    }

    return eStatus;
} // End of functions

/******************************************************************************/
//@FunctionName: plmeSET_request 										  	  */
//@Description:	The PLME-SET.request primitive attempts to set the indicated  */
//              PHY PIB attribute to the given value.                         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
PHY_STATUS plmeSET_request ( PHY_PIB_ATT_TYPE par_PIBAttType, UINT32 par_PIBAttributeValue )
{
	/*********** Variable declaration ***********/
     PHY_STATUS     eStatus;
     UINT16 newFreq, oldFreq;
     BOOL sfdWasActiveBeforeStrobe;

	/*********** Function body		 ***********/
    eStatus = INVALID_PARAMETER;

    switch(par_PIBAttType)
    {
    case PHY_PIB_CURRENT_CHANNEL: 
        if ((par_PIBAttributeValue >= 0) && (par_PIBAttributeValue <= 26))
        {
            PhyInfo.PhyPIB.phyCurrentChannel = par_PIBAttributeValue;
        
            // Derive frequency programming from the given channel number
            newFreq = (UINT16) (par_PIBAttributeValue - 11); // Subtract the base channel 
            newFreq = newFreq + (newFreq << 2);       // Multiply with 5, which is the channel spacing
            newFreq = newFreq + 357 + 0x4000;         // 357 is 2405-2048, 0x4000 is LOCK_THR = 1

            // Do we need to change the frequency at all?
            DISABLE_GLOBAL_INT();
            FASTSPI_GETREG(CC2420_FSCTRL, oldFreq);
            ENABLE_GLOBAL_INT();
            if ((newFreq ^ oldFreq) & 0x03FF) 
            {        
                DISABLE_GLOBAL_INT();

                // Force RX off
                sfdWasActiveBeforeStrobe = SFD_IS_ACTIVE();                
                FASTSPI_STROBE(CC2420_SRFOFF);
        
                // Clean up if we interrupted the receiver
                if (sfdWasActiveBeforeStrobe) 
                    phyrx_ResetRx();
        
                //update the frequency register 
                FASTSPI_SETREG(CC2420_FSCTRL, newFreq);
        
                // Return to the previous RX state
                FASTSPI_STROBE(CC2420_SRXON);

                ENABLE_GLOBAL_INT();
            }   
            eStatus = PHY_SUCCESS;
        }
    	break;
    case PHY_PIB_CHANNEL_SUPPORTED: 
        //TBD??? too complicated to check this param
        PhyInfo.PhyPIB.phyChannelsSupported = par_PIBAttributeValue;
        eStatus = PHY_SUCCESS;
    	break;
    case PHY_PIB_TRANSMIT_POWER: 
        if ((par_PIBAttributeValue >= 0) && (par_PIBAttributeValue <= 0xbf))
        {
            PhyInfo.PhyPIB.phyTransmitPower = par_PIBAttributeValue;
            eStatus = PHY_SUCCESS;
        }
        break;
    case PHY_PIB_CCA_MODE: 
        if((par_PIBAttributeValue >= 1) && (par_PIBAttributeValue <= 3))
        {
            PhyInfo.PhyPIB.phyCcaMode = par_PIBAttributeValue;
            eStatus = PHY_SUCCESS;
        }
        break;
    default:
        eStatus = PHY_UNSUPPORTED_ATTRIBUTE;
        break;
    }

    return eStatus;    
}


