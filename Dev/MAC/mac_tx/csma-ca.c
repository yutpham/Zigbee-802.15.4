/*******************************************************************************/
//Module: MAC\mac_tx                                                           */   
//File Name: csma-ca.c                                                         */
//Description: Implements csma-ca mechanism    					  			   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//01/08/2007                                                   */
//------------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "csma-ca.h"
#include "mac_primitives.h"
#include "phy_primitives.h"
#include "schedul.h"
#include "static_timer.h"
#include "phy_support.h"
#include "mac_beacon.h"
#include "mac_tx.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/
#define MAC_CSMA_CW         1


/************************************************************************/
/*					Type Definition										*/
/************************************************************************/


/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/

/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/

/******************************************************************************/
//@FunctionName:  csmaca_locateBeaconTimeout			  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    csmaca_locateBeaconTimeout(void* args, void*    par_pParam);

/******************************************************************************/
//@FunctionName: csmaca_setupBackOffs   										*/
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL csmaca_setupBackOffs(CSMACA_INFO*     par_csmaca);

/******************************************************************************/
//@FunctionName: csmaca_restartFromDelay  										*/
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void csmaca_restartFromDelay(void* args1, void* args2)
{
    //restart the TX task
    schedul_activate(TASK_PER_MAC_TX);
}


/******************************************************************************/
//@FunctionName: csmaca_setupBackoffCallback  								  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL csmaca_setupBackoffCallback(CSMACA_INFO* par_pCsmaca) 
{
    timer_startTimer( restartFromDelay, NULL, 
                    (UINT32)((par_pCsmaca->nRandomBackoff==0)?1:par_pCsmaca->nRandomBackoff));
    //suspend, and restart next time by timers
    schedul_suspend(TASK_PER_MAC_TX);
    return TRUE;
} 

/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/******************************************************************************/
//@FunctionName:  csmaca_initCsmaca 										  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void csmaca_initCsmaca(CSMACA_INFO* par_pCsmaca, PKT_INFO* par_pPktInfo, BOOL par_bSlotted)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    //reset internal value of csma-ca
    par_pCsmaca->nb = 0;
    par_pCsmaca->cw = MAC_CSMA_CW;
    par_pCsmaca->be = mpib.macMinBE;
    par_pCsmaca->bSlotted           = par_bSlotted;
    par_pCsmaca->pPktInfo           = par_pPktInfo;
    par_pCsmaca->bWaitBeaconForTx   = FALSE;
    par_pCsmaca->nRandomBackoff     = 0xFF;

    if (par_bSlotted && mpib.macBattLifeExt) {
        par_pCsmaca->be = MIN(2, par_pCsmaca->be);    
    }

    par_pCsmaca->eState    = CSMACA_STATE_INIT;
    par_pCsmaca->eCurrentResult = CSMACA_RESULT_NO_ERR;    

} // End of functions

/******************************************************************************/
//@FunctionName:csmaca_process							  	                  */
//@Description:	Main task to perform csma-ca procedure                        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void csmaca_process(CSMACA_INFO* par_pcsmaca)
{
	/*********** Variable declaration ***********/
    BOOL            bEnoughTime;
	/*********** Function body		 ***********/
    switch(par_pcsmaca->eState)
    {
    case CSMACA_STATE_UNDEFINED:
        //do nothing here 
    	break;
    case CSMACA_STATE_INIT:  

        if (par_pcsmaca->nRandomBackoff == 0xFF) /*Initialization*/
            //locate back-off period
		    bEnoughTime = csmaca_setupBackOffs(par_pcsmaca);
        else
            bEnoughTime = TRUE;                 /*Recovered from delay*/

        /* Check time to transmit in this CAP */
        if (bEnoughTime == TRUE) 
        {
            //Starting of delay period
            csmaca_setupBackoffCallback(par_pcsmaca);           
            par_pcsmaca->eCurrentResult = CSMACA_RESULT_NO_ERR;
            par_pcsmaca->eState         = CSMACA_STATE_RANDOM_DELAY;     
        }
        else 
        {        
            // CSMA-CA should not be resumed in the next super-frame for indirect packets!
            if (par_pcsmaca->pPktInfo->txOptions & TX_OPT_INDIRECT) 
            {
                par_pcsmaca->eCurrentResult =   CSMACA_RESULT_SHORT_TIME;
                return;
            } 
            else
            {
                //Request to locate beacon if beacon is not tracked
                if (! BeaconInfo.bTrackBeaconReq    //beacon is not locating
                    && ! BeaconInfo.bTrackBeacon )    //or not tracked beacon 
                    mac_setTrxState(PHY_RX_ON);

                if (par_pcsmaca->pPktInfo->bToCoord) /*Locate beacon only if to coor.*/
                    /* Start timer to wait */
                    timer_startTimer(locateBeaconTimeout,(void*)par_pcsmaca,
                                    ((UINT32) (aBaseSuperframeDuration / aUnitBackoffPeriod) << (UINT32) mpib.macBeaconOrder));

                par_pcsmaca->bWaitBeaconForTx   = TRUE;
                par_pcsmaca->eCurrentResult     = CSMACA_RESULT_NO_ERR;
                par_pcsmaca->eState             = CSMACA_STATE_INIT;     
            }
        }

    	break;
    case CSMACA_STATE_RANDOM_DELAY:
        //after the random delay of 2expBE-1, performing the CCA
    case CSMACA_STATE_TURNON_RX:        
        // Make sure that the FIFOP interrupt is off during the CSMA-CA procedure 
        // and the timing-critical TX FIFO access
        phy_disableFIFOInt(TRUE);

        // Turn on RX to be able to do the CCA check in the CSMA-CA algorithm
        mac_setTrxState(PHY_RX_ON);

        par_pcsmaca->eState         = CSMACA_STATE_PERFORM_CCA;  
        par_pcsmaca->eCurrentResult = CSMACA_RESULT_NO_ERR;        

        break;
    case CSMACA_STATE_PERFORM_CCA: //perform CCA        
        if (par_pcsmaca->bSlotted == TRUE)      //slotted
        {
            // CCA check on the back-off slot boundary
            WAIT_FOR_BOUNDARY();
            if (plmeCCA_request() == PHY_IDLE)
            {
                par_pcsmaca->cw--;
                if(par_pcsmaca->cw == 0)
                    par_pcsmaca->eCurrentResult = CSMACA_RESULT_SUCCESS;
            } 
            else 
            {
                par_pcsmaca->nb++;
                par_pcsmaca->cw = MAC_CSMA_CW;
                par_pcsmaca->be = MIN(par_pcsmaca->be + 1, aMaxBE);

                if (par_pcsmaca->nb > mpib.macMaxCSMABackoffs)
                    par_pcsmaca->eCurrentResult = CSMACA_RESULT_FAILURE; /*channel access failure*/
                else
                {   
                    /* Try again from beginning*/
                    par_pcsmaca->eState         = CSMACA_STATE_INIT;
                    par_pcsmaca->nRandomBackoff = 0xFF;
                    par_pcsmaca->eCurrentResult = CSMACA_RESULT_NO_ERR;
                }
            }
        }
        //un-slotted, check CCA immediately
        else
        {
            if (plmeCCA_request() == PHY_IDLE)
                par_pcsmaca->eCurrentResult = CSMACA_RESULT_SUCCESS;
            else 
            {
                par_pcsmaca->nb++;
                par_pcsmaca->be = MIN(par_pcsmaca->be + 1, aMaxBE);

                if (par_pcsmaca->nb > mpib.macMaxCSMABackoffs)
                    par_pcsmaca->eCurrentResult = CSMACA_RESULT_FAILURE; /*channel access failure*/
                else
                {
                    /* Try again from beginning*/
                    par_pcsmaca->eState         = CSMACA_STATE_INIT;
                    par_pcsmaca->nRandomBackoff = 0xFF;
                    par_pcsmaca->eCurrentResult = CSMACA_RESULT_NO_ERR;
                }
            }            
        }

        mac_setTrxState(PHY_TRX_OFF);
        //End of CCA, enable the FIFO interrupt
        phy_disableFIFOInt(FALSE);

    	break;
    default:
        break;
    }

} // End of function


/******************************************************************************/
//@FunctionName: csmaca_setupBackOffs  										  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL csmaca_setupBackOffs(CSMACA_INFO*     par_csmaca) 
{
	/*********** Variable declaration ***********/
    BOOL    bReturn;
    UINT32  nCap;
    INT32   bosLeft;
    UINT8   randomBackoff;
	/*********** Function body		 ***********/

    // Check for validity of indirect packets
    //TBD
        
    // Calculate the random back-off period
    randomBackoff = ramdom_getRandomByte() & (BM(par_csmaca->be) - 1);

    // Calculate the number of FULL back-off slots left in the CAP (including the two/three periods required to prepare for transmission
    if (mpib.macSuperframeOrder == 15 || !par_csmaca->bSlotted ) {
        bosLeft = 0;
    } else if (BeaconInfo.bBatteryLifeExt && par_csmaca->pPktInfo->bToCoord) {
        //        <----------------- The CAP duration ----------------->   <------- Used CAP ------>   < Delay + overhead + duration >                                                                                         
        bosLeft = (BeaconInfo.nBeaconDuration+ mpib.macBattLifeExtPeriods) - (SlotInfo.bosCounter + 1) -     (randomBackoff + 2 + 1);
    } else 
    {
        nCap    = (((aBaseSlotDuration / aUnitBackoffPeriod) * (UINT32) (BeaconInfo.nFinalCapSlot + (UINT32) 1)) << (UINT32) mpib.macSuperframeOrder);
        if (MacTx.pPacket->PktInfo.bToCoord) /*To parent*/
        {
            //        <- The CAP duration > <------- Used CAP ------>   <-------- Delay + overhead + duration -------->   <-- Beacon margin -->                                                                  
            bosLeft =  nCap                 - (SlotInfo.bosCounter + 1) - (randomBackoff + 2 + par_csmaca->pPktInfo->nDuration) - beacon_calBeaconMargin(); 
        }
        else /*To child  or other*/
        {
            /*        <- The CAP duration >   <------ Used CAP ------->        <-------- Delay + overhead + duration -------->  <-- Beacon margin --> */
            bosLeft = nCap                   - (BeaconInfo.nBeaconDuration+ 1) - (randomBackoff + 2 + par_csmaca->pPktInfo->nDuration) - beacon_calBeaconMargin(); 
        }
    }

    // Subtract an extra back-off period if slotted CSMA-CA is used (2 CCA checks instead of 1)
    if (par_csmaca->bSlotted) bosLeft--;

    // Default values...
    par_csmaca->bWaitBeaconForTx = FALSE;

    if (bosLeft >= 0)
        bReturn = TRUE; /*Enough time to transmit*/
    else
        bReturn = FALSE;

    par_csmaca->nRandomBackoff = randomBackoff;

    return bReturn;
} // csmaSetStartupTime


/******************************************************************************/
//@FunctionName:  csmaca_locateBeaconTimeout 								  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    csmaca_locateBeaconTimeout(void* args,void*    par_pParam)
{
	/*********** Variable declaration ***********/
    CSMACA_INFO*        pCsmaca;
	/*********** Function body		 ***********/
    pCsmaca = (CSMACA_INFO*)par_pParam;

    if(BeaconInfo.bTrackBeaconReq)
    {
        //Locate beacon.
        timer_startTimer(locateBeaconTimeout,(void*)pCsmaca,
                         ((UINT32) (aBaseSuperframeDuration / aUnitBackoffPeriod) << (UINT32) mpib.macBeaconOrder));
    }
    else
    {
        //the beacon is lost
        pCsmaca->bWaitBeaconForTx = FALSE;
        schedul_activate(TASK_PER_MAC_TX);
        //switch back to un-slotted Csmaca
        pCsmaca->bSlotted   = FALSE;
        pCsmaca->nb         = 0;
        pCsmaca->be         = mpib.macMinBE;
        pCsmaca->bSlotted   = FALSE;
        BeaconInfo.bBeaconLost = TRUE;
    }
    
} // End of functions

