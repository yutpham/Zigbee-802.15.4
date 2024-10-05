/*******************************************************************************/
//Module: MAC/phy_primitives                                                   */   
//File Name: phy_support.c                                                     */
//Description: 	This module will implement support function for manipulation on
//              the phy														   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//01/16/2007        pnthai          Create                                     */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "phy_support.h"
#include "phy_primitives.h"
#include "mac_state.h"
#include "zconf.h"
#include "mac_packet.h"
#include "indirect_tx.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/
/* The number of microseconds used to power up 
   the CC2420 voltage regulator
*/
#define MPM_VREG_TURN_ON_TIME           600
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
//@FunctionName:  phytrx_setPwrMode 										  */
//@Description:	 This function will be call to control power of the TRX       */
//               Currently, only support on PWR_ON mode
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void phytrx_setPwrMode(PHY_MODE  par_Mode)
{
	/*********** Variable declaration ***********/
    BYTE            spiStatus;
	/*********** Function body		 ***********/
    switch(par_Mode)
    {
    case PHY_PWR_ON:

        /*Initialize*/
        MAC_PORT_INIT();
        SPI_INIT(); 
        FIFOP_INT_INIT();                                 /*FIFOP interrupt initialization*/
        DB_PERIPHERAL_PORT_INIT();                        /* Initialize peripheral ports */           
        
        /* Initialize the C2420*/
        SET_RESET_ACTIVE(); 
        SET_VREG_ACTIVE();        
        wait(MPM_VREG_TURN_ON_TIME);

        SET_RESET_INACTIVE();       
        FASTSPI_STROBE(CC2420_SXOSCON);  
#ifdef ZIGBEE_STUB
        /*no wait*/
#else
        /* Wait for oscillator become stable */
        do {
            DISABLE_GLOBAL_INT();
            FASTSPI_UPD_STATUS(spiStatus);
            ENABLE_GLOBAL_INT();
        } while (!(spiStatus & CC2420_XOSC16M_STABLE_BM));
#endif
        /*Setting PHY state */
        PhyInfo.bFifopIntOff = FALSE;
        PhyInfo.eState       = PHY_RX_STATE_PHR;
        PhyInfo.PhyState     = PHY_TRX_OFF;

    	break;
    default:
        break;
    }
} // End of functions

/******************************************************************************/
//@FunctionName: phyrx_turnOffRx  										  */
//@Description:	 Decrements the "RX on" counter, and turns off RX on a 1 -> 0 */
//               transition if SFD is low. If SFD is high, the turn-off 
//               operation is delayed by one backoff period by using          */
//               a callback (TBD)                                             */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void phyrx_turnOffTrx(void* par_pParam) 
{
    DISABLE_GLOBAL_INT();

    phyrx_ResetRx();
    
    FASTSPI_STROBE(CC2420_SRFOFF);

    /*Disable interrupt */
    DISABLE_FIFOP_INT();

    //reset state
    PhyInfo.PhyState     = PHY_TRX_OFF;
    PhyInfo.eState       = PHY_RX_STATE_PHR;

    ENABLE_GLOBAL_INT();
} // phyrx_DecrOnCounter


/******************************************************************************/
//@FunctionName: phyrx_turnOnRx      					  	                  */
//@Description:	Increments the "RX on" counter, and turns on RX on 
//              a 0 -> 1 transition                                           */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void phyrx_turnOnRx(void* par_pParam) {
    DISABLE_GLOBAL_INT();

    FASTSPI_STROBE(CC2420_SRXON);
    /*Disable interrupt */
    ENABLE_FIFOP_INT();    
    
    PhyInfo.PhyState     = PHY_RX_ON;
    PhyInfo.eState       = PHY_RX_STATE_PHR;
    
    ENABLE_GLOBAL_INT();
} // phyrx_IncrOnCounter


void phyrx_initOnRx() {   
    PhyInfo.PhyState     = PHY_RX_ON;
    PhyInfo.eState       = PHY_RX_STATE_PHR;   
    /*Disable interrupt */
    ENABLE_FIFOP_INT();    
} // phyrx_IncrOnCounter

/******************************************************************************/
//@FunctionName: phytx_turnOnTx  										  	   */
//@Description:	This function will turn on the TX                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/    
BOOL phytx_turnOnTx()
{
	/*********** Variable declaration ***********/
    BOOL            bStatus;
    BYTE            nTemp;
	/*********** Function body		 ***********/
    DISABLE_GLOBAL_INT();

   // Flush the FIFO
    phyrx_ResetRx();
    FASTSPI_STROBE(CC2420_SFLUSHTX);
    
    // Start TX
    FASTSPI_STROBE(CC2420_STXON);

    // Was TX started?
    FASTSPI_UPD_STATUS(nTemp);
#ifdef ZIGBEE_STUB
    nTemp = 0xFF;
#endif
    if (!(nTemp & CC2420_TX_ACTIVE_BM)) 
    {
        bStatus = FALSE;
    }
    else
    {
        bStatus = TRUE;
        PhyInfo.PhyState     = PHY_TX_ON;
        PhyInfo.eState       = PHY_RX_STATE_PHR;
    }
    ENABLE_GLOBAL_INT();            

    return bStatus;
} // End of functions

/******************************************************************************/
//@FunctionName: phyrx_ResetRx                      	  	                  */
//@Description:	 Resets the RX engine by clearing the RX FIFO,                */
//TBC                                                                         */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void phyrx_ResetRx()
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/    
    DISABLE_GLOBAL_INT();
    
    // Flush the RX FIFO twice 
    FASTSPI_STROBE(CC2420_SFLUSHRX);
    FASTSPI_STROBE(CC2420_SFLUSHRX);                
    
    // read the current packet from the C2420
    //TBD

    // Reset the RX engine state
    PhyInfo.eState      = PHY_RX_STATE_PHR;

    DISABLE_GLOBAL_INT();
} // End of functions

//-------------------------------------------------------------------------------------------------------
//  void phy_WriteRam(void *pData, UINT16 address, UINT8 count, BOOL disableInterrupts)
//
//  DESCRIPTION:
//      Write data to the CC2420 RAM-space
//      Used for setup of addresses and security
//
//  PARAMETERS:
//      void *pData
//          Pointer to data to be written
//      UINT16 address
//          CC2420 RAM Address
//      UNIT8 count
//          Number of bytes to be written
//      BOOL disableInterrupts
//          Disable global interrupts while writing?
//-------------------------------------------------------------------------------------------------------
void phy_WriteRam(void *pData, UINT16 address, UINT8 count, BOOL disableInterrupts) {
    if (count) {
        do {
            if (disableInterrupts) DISABLE_GLOBAL_INT();
            SPI_ENABLE();
            FASTSPI_TX(0x80 | (address & 0x7F)); \
            FASTSPI_TX((address >> 1) & 0xC0); \
            do {
                FASTSPI_TX(*((BYTE*) pData));
                pData++;
            } while ((--count) & 0x07);
            SPI_DISABLE();
            if (disableInterrupts) ENABLE_GLOBAL_INT();

            address += 8;
        } while (count);
    }
} // msupWriteRam




//-------------------------------------------------------------------------------------------------------
//  void phy_ReadFifo(void *pData, UINT8 count)
//
//  DESCRIPTION:
//      Reads data from the CC2420 RX FIFO
//
//  PARAMETERS:
//      void *pData
//          Pointer to data to be read
//      UNIT8 count
//          Number of bytes to be read
//-------------------------------------------------------------------------------------------------------
PHY_FIFO_STATE phy_ReadFifo(void *pData, UINT8 count, UINT8* readCount) {
    DISABLE_GLOBAL_INT();
    SPI_ENABLE();
    FASTSPI_RX_ADDR(CC2420_RXFIFO);
    *readCount = count;
    if (count) {
        do {
            FASTSPI_RX(*((BYTE*) pData));
            pData++;
        } while (--count);
    }
    /*Check number of read bytes*/
    *readCount -= count;

    SPI_DISABLE();
    ENABLE_GLOBAL_INT();

    return PHY_FIFO_READ_SUCCESS;
} // phy_ReadFifo

//-------------------------------------------------------------------------------------------------------
//  void phy_WriteFifo(void *pData, UINT8 count)
//
//  DESCRIPTION:
//      Write data to the CC2420 TX FIFO
//
//  PARAMETERS:
//      void *pData
//          Pointer to data to be written
//      UNIT8 count
//          Number of bytes to be written
//-------------------------------------------------------------------------------------------------------
void phy_WriteFifo(void *pData, UINT8 count) {
    if (count) {
        do {
            DISABLE_GLOBAL_INT();
            SPI_ENABLE();
            FASTSPI_TX_ADDR(CC2420_TXFIFO);
            do {
                FASTSPI_TX(*((BYTE*) pData));
                pData++;
            } while ((--count) & 0x07);
            SPI_DISABLE();
            ENABLE_GLOBAL_INT();
        } while (count);
    }
} // phy_WriteFifo


/******************************************************************************/
//@FunctionName:phyrx_checkAck   						  	                  */
//@Description:	This function will check the need for ACK, if need, it will 
//              perform setting to send ACK                                   */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void phyrx_checkAck(P_MPDU_RX  par_pMpdu)
{
	/*********** Variable declaration ***********/
    
	/*********** Function body		 ***********/
    /*ACK request*/
    if (par_pMpdu->Mpdu.FrmControl.ACKRequest) 
    {                    
        
        DISABLE_GLOBAL_INT();
#if     ZIGBEE_DEVICE_FFD == TRUE  
        if (SFD_IS_ACTIVE()) 
        {

            if (PKT_IS_PENDING())
            {
                FASTSPI_STROBE(CC2420_SACKPEND);
            }       
            else
                FASTSPI_STROBE(CC2420_SACK);
        }
#else
        if (SFD_IS_ACTIVE()) 
        {
            FASTSPI_STROBE(CC2420_SACK);
        }
#endif     //!ZIGBEE_DEVICE_FFD == TRUE  

        ENABLE_GLOBAL_INT();
    }
} // End of functions

/******************************************************************************/
//@FunctionName: phy_disableFIFOInt  										  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void phy_disableFIFOInt(BOOL bDisable)
{
    DISABLE_GLOBAL_INT();
    if (bDisable)
    {
        PhyInfo.bFifopIntOff = bDisable;
        DISABLE_FIFOP_INT();
    }
    else
    {
        FASTSPI_STROBE(CC2420_SFLUSHRX);        // Flush the RX FIFO twice 
        FASTSPI_STROBE(CC2420_SFLUSHRX);                
        ENABLE_FIFOP_INT();
    }
    ENABLE_GLOBAL_INT();
}
