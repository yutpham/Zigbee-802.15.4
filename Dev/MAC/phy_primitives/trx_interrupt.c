/*******************************************************************************/
//Module: MAC/phy_primitives                                                   */   
//File Name: trx_interrupt.c                                                   */
//Description: This file handles interrupts fom packet transmission  		   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//01/12/2007        pnthai          Create                                     */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "mac_mem.h"
#include "phy_primitives.h"
#include "phy_support.h"
#include "mac_beacon.h"
#include "zconf.h"
#include "ucos_ii.h"
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
#define             RX_STACK_SIZE                                 	   0               
#define             TMR_STACK_SIZE                                 	   0    
#define             MAX_QUEUEING_RX_PKT                                3
/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/

volatile  UINT8     TmrIntNesting;
volatile  UINT8     RxIntNesting;
/************************************************************************/
/*                      Local Function                                  */
/************************************************************************/

/******************************************************************************/
//@FunctionName: phyrx_receiveUnsecured  									  */
//@Description:	This function will receive the packet in un-secured mode      */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void phyrx_receiveUnsecured();


/******************************************************************************/
//@FunctionName:phyrx_receivePayload   										  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL phyrx_receivePayload();

/************************************************************************/
/*                      Function                                        */
/************************************************************************/


/******************************************************************************/
//@FunctionName: SIGNAL														  */
//@Description:	Timer 1 interrupt to process queued timer   				  */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
SIGNAL(SIG_OUTPUT_COMPARE1A)
{
    /*********** Variable declaration ***********/
    
    /*********** Function body		 ***********/
    //Disable global interrupt
    DISABLE_T1_COMPA_INT();    

    TmrIntNesting++;
    
    /* Activate timer and periodical task */
    OSTmr_Task(NULL);
    //schedul_runPerTasks();

    /* Other execution related to MAC layer */
    SlotInfo.bosCounter++;   /* update the BOS counter */             

    TmrIntNesting--;    

    //End of interrupt disability
    ENABLE_T1_COMPA_INT();
} // End of functions


/******************************************************************************/
//@FunctionName: SIGNAL(SIG_INPUT_CAPTURE1)
//@Description:	 This interrupt is trigged when the CC2420 SFD pin goes high  */
//               at the start of a transmitted/received frame. The ISR        */
//               generates a time stamp to be used with beacons.  			  */
//@Param                                                                      */
//Reused from C2420                                                          */   
/******************************************************************************/
SIGNAL(SIG_INPUT_CAPTURE1) {
       SlotInfo.captureTcnt = ICR1;
       SlotInfo.captureBosCounter = SlotInfo.bosCounter;
       if (TCNT1 < SlotInfo.captureTcnt) {
          SlotInfo.captureBosCounter++;
       }
       SlotInfo.captureTime = SlotInfo.bosCounterAdjustTime + (
           SlotInfo.captureBosCounter * MAC_SYMBOL_DURATION * aUnitBackoffPeriod) + 
           (UINT32) SlotInfo.captureTcnt / CLOCK_SPEED_MHZ;

#ifdef ZIGBEE_DEBUG

#endif    

} // SIGNAL(SIG_INPUT_CAPTURE1)

/******************************************************************************/
//@FunctionName: SIGNAL(SIG_OUTPUT_COMPARE1C)       	  	                  */
//@Description:	Used by the FIFOP interrupt to make sure that is enabled a    */
//              short while after it has returned.This allows for timer ticks */
//              in between, which has a lower priority than the FIFOP interrupt.*/
//	                                                                          */    
//@Param                                                                      */
//Reused from C2420                                                          */   
/******************************************************************************/

SIGNAL(SIG_OUTPUT_COMPARE1C) {
} // SIGNAL(SIG_OUTPUT_COMPARE1C)

/******************************************************************************/
//@FunctionName: SIGNAL(SIG_INTERRUPT0)                	  	                  */
//@Description:	 This interrupt will step by step build Physic layer SDU a    */
//               and necessary information. When finish, it will call         */ 
//               PD_DATA.Indication() to dispatch the packet to MAC layer     */
//	                                                                          */    
//@Param                                                                      */
/******************************************************************************/
SIGNAL(SIG_INTERRUPT0) 
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/

    // Turn off the main MAC interrupts
    DISABLE_T1_COMPA_INT();
    DISABLE_FIFOP_INT();

#ifdef ZIGBEE_DEBUG

#endif

    /* just implemented unsecured reception*/  
    while ((PhyInfo.nQueueingPkt < MAX_QUEUEING_RX_PKT)
            && FIFOP_IS_ACTIVE()
            &&PhyInfo.PhyState == PHY_RX_ON)
    {
        phyrx_receiveUnsecured(&PhyInfo);
    }

    
    /* flush the Rx when
     - Invalid state
     - Too many packet
     - Overflow */
    if((PhyInfo.PhyState != PHY_RX_ON)
        || (PhyInfo.nQueueingPkt >= MAX_QUEUEING_RX_PKT)
        ||(FIFOP_IS_ACTIVE()&&!FIFO_IS_ACTIVE()))  
    {
        phyrx_ResetRx();       
    }


#ifdef ZIGBEE_DEBUG
        
#endif

    ENABLE_FIFOP_INT();    

    /* Enable timer */
    if (!TmrIntNesting)
    {
        ENABLE_T1_COMPA_INT();
    }
} // End of functions


/******************************************************************************/
//@FunctionName: phyrx_receiveUnsecured  									  */
//@Description:	This function will receive the packet in un-secured mode      */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void phyrx_receiveUnsecured()
{
	/*********** Variable declaration ***********/
    P_MPDU_RX           pRxPacket;   
    FRAME_CONTROL*      pFrmCtrl;
    WORD                readCount;
    BYTE*               pTemp;

	/*********** Function body		 ***********/

#ifdef ZIGBEE_STUB
    
#endif

    /* Allocate the pkt*/
    if (PhyInfo.sCurPacket.pCurRxPacket == 0)
    {
        PhyInfo.sCurPacket.pCurRxPacket = mem_getFreeRxPkt();
    }

    if (PhyInfo.sCurPacket.pCurRxPacket == 0)    
    {
        PhyInfo.eState = PHY_RX_STATE_RESET;
    }
    else
    {
        pRxPacket = PhyInfo.sCurPacket.pCurRxPacket;
        pFrmCtrl = (FRAME_CONTROL*)&PhyInfo.sCurPacket.pCurRxPacket->Mpdu;
    }


    switch (PhyInfo.eState) {
    case PHY_RX_STATE_PHR:

#ifdef ZIGBEE_DEBUG
        SET_YLED();
#endif
        //receive the length of the MPDU      
        pTemp = (BYTE*)&PhyInfo.sCurPacket.nLength;
        readCount = 1;
        ReadFifo(pTemp,readCount);        
        //receive the header
        pTemp = (BYTE*)&PhyInfo.sCurPacket.pCurRxPacket->Mpdu;
        readCount = MAC_HR_LEN;
        ReadFifo(pTemp,readCount);                        

        /* validate the packet */
        PhyInfo.sCurPacket.nHrLength = mac_getMpduHrLength(pFrmCtrl);
        if (PhyInfo.sCurPacket.nLength < PhyInfo.sCurPacket.nHrLength+MAC_FCS_LEN)
        {
            PhyInfo.eState = PHY_RX_STATE_RESET;
			break;
        }

        // This packet will be acknowledged if the ack request bit is set
        phyrx_checkAck(pRxPacket);

        /* Check if the frame has payload : ACK */
        if (PhyInfo.sCurPacket.nLength == MAC_HR_LEN+MAC_FCS_LEN
            && pFrmCtrl->FrameType == MAC_FRM_ACK)
		{
            PhyInfo.eState = PHY_RX_STATE_FCS;
		}
        else
        {
           //set threshold to receive the payload
            PhyInfo.eState = PHY_RX_STATE_PAYLOAD;
            SetFifopThreshold(readCount,PhyInfo.sCurPacket.nLength-MAC_HR_LEN-MAC_FCS_LEN-1);    
        }

#ifdef ZIGBEE_DEBUG	     
#endif // _DEBUG

        /* If data is ready, read it*/
        if (!FIFOP_IS_ACTIVE() && PhyInfo.eState == PHY_RX_STATE_PAYLOAD)
        {
            break;
        }	     
    
    case PHY_RX_STATE_PAYLOAD:    
        
        //receive the payload of the MPDU
        phyrx_receivePayload();
        
        /* Next state */        
        PhyInfo.eState = PHY_RX_STATE_FCS;
        break;
    default:
        break;
    }

    /* Read the FCS */
    if (PhyInfo.eState == PHY_RX_STATE_FCS)
    {
        //receive the FCS of the MPDU
        pTemp = (BYTE*)&pRxPacket->Mpdu.Fcs;
        readCount = MAC_FCS_LEN;
        ReadFifo(pTemp,readCount);        

        //Check CRC state
        if(pRxPacket->Mpdu.Fcs.CRCCheck)
        {
            //CRC check is not ok, discard this packet.
            PhyInfo.eState = PHY_RX_STATE_DISCARD;
        }
        //else //crc check is ok, 
        {
            PhyInfo.eState = PHY_RX_STATE_SUCCESS;
        }     
    }
        

    /* End of the receiving process, indicate the data or clean up*/
    switch (PhyInfo.eState) {
    case PHY_RX_STATE_SUCCESS:       

        //Call PD-DATA.indication, LinkQuality will be calculated on the upper layer
        pddata_indication(pRxPacket,0);

        //next state 
        PhyInfo.eState = PHY_RX_STATE_PHR;
        //Next state will receive a new packet
        SetFifopThreshold(readCount,MAC_HR_LEN+1);            

#ifdef ZIGBEE_DEBUG
        CLR_YLED();
#endif

        break;
    case PHY_RX_STATE_RESET:
        phyrx_ResetRx();
        /*No break*/
    case PHY_RX_STATE_DISCARD:

#ifdef ZIGBEE_DEBUG
        CLR_YLED();
#endif
        //next state 
        PhyInfo.eState = PHY_RX_STATE_PHR;
        //Next state will receive a new packet
        SetFifopThreshold(readCount,MAC_HR_LEN+1);

        break;    
    default:
        break;
    }

} // End of functions


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL phyrx_receivePayload()
{
	/*********** Variable declaration ***********/
    FRAME_CONTROL*          pFrameCtrl;
    MPDU*                   pMpdu;
    BYTE*                   pTemp;
    UINT8                   TemLen;
	/*********** Function body		 ***********/
    
    pFrameCtrl      = (FRAME_CONTROL*)&PhyInfo.sCurPacket.pCurRxPacket->Mpdu;
    pMpdu           =  &PhyInfo.sCurPacket.pCurRxPacket->Mpdu;


    //check the des address mode
    if (pFrameCtrl->DestAddMode)
    {

        //copy  PAN ID and address.
        pTemp  = &pMpdu->dstPanId;
        TemLen = MAC_PAN_ID_LEN;
        ReadFifo(pTemp, TemLen);

        pTemp  = &pMpdu->dstAddr;
        if (pFrameCtrl->DestAddMode == AM_SHORT_16)
        {
            TemLen = ADDR_LEN_SHORT_16;

        }
        else if (pFrameCtrl->DestAddMode == AM_EXTENDED_64)
        {
            TemLen = ADDR_LEN_EXTENDED_64;
        }
        else
            return FALSE;

        ReadFifo(pTemp, TemLen);
    }
    

    //check the src address mode
    if(pFrameCtrl->SourceAddMode)
    {
        //copy src PAN ID and address.
        if (!pFrameCtrl->IntraPAN)
        {
            pTemp  = &pMpdu->srcPanId;
            TemLen = MAC_PAN_ID_LEN;
            ReadFifo(pTemp, TemLen);
        }

        pTemp  = &pMpdu->srcAddr;
        if (pFrameCtrl->SourceAddMode == AM_SHORT_16)
        {
            TemLen = ADDR_LEN_SHORT_16;        
        }
        else if (pFrameCtrl->SourceAddMode == AM_EXTENDED_64)
        {
            TemLen = ADDR_LEN_EXTENDED_64;
        }
        else
            return FALSE;

        ReadFifo(pTemp, TemLen);
    }
   

    /*Set the length*/
    pMpdu->nLength = PhyInfo.sCurPacket.nLength - PhyInfo.sCurPacket.nHrLength - MAC_FCS_LEN;

    pTemp  = (BYTE*)pMpdu->PayLoad;
    TemLen = pMpdu->nLength;
    ReadFifo(pTemp, TemLen);
    
    //here, alway return true
    return TRUE;

} // End of functions
