/*******************************************************************************/
//Module: MAC/phy_primitives                                                   */   
//File Name: phy_support.h                                                     */
//Description: 	This module will implement support function for manipulation on
//              the phy														   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//01/16/2007        pnthai          Create                                     */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef PHY_SUPPOR_H
#define PHY_SUPPOR_H

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "mac_types.h"
#include "phy_primitives.h"
#include "mac_packet.h"
#include "mac_state.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/
//Constant to access MPDU
#define     ACK_REQ_BM              0x0020
#define     CRC_CHECK_BM            0x08
#define     LENGHT_MPDU_BM          0x7F

//Constants for PSDU
#define     PHR_LENGTH              3


#define SetFifopThreshold(dwRegValue, par_nCount) \
                  dwRegValue = 0x0000;            \
                  if(macInfo.sState.nTransacType == MAC_TRANSAC_SCAN)  \
                    dwRegValue |= 0x0800;           \
                  dwRegValue |= par_nCount;       \
                  DISABLE_GLOBAL_INT();           \
                  FASTSPI_SETREG(CC2420_IOCFG0, dwRegValue); \
                  ENABLE_GLOBAL_INT() 

                                                    
#define         ReadFifo(pData,count)       \
    DISABLE_GLOBAL_INT();                   \
    SPI_ENABLE();                           \
    FASTSPI_RX_ADDR(CC2420_RXFIFO);         \
    if (count) {                            \
        do {                                \
            FASTSPI_RX(*((BYTE*) pData));   \
            pData++;                        \
        } while (--count);                  \
    }                                       \
    SPI_DISABLE();                          \
    ENABLE_GLOBAL_INT()                     \


/************************************************************************/
/*					Type Definition										*/
/************************************************************************/
typedef enum
{
    PHY_PWR_ON = 0          /* TRx is at initialization on device-power on */
} PHY_MODE;


typedef enum
{
    PHY_FIFO_EMTY,
    PHY_FIFO_OVERFLOW,
    PHY_FIFO_UNDERFLOW,
    PHY_FIFO_READ_SUCCESS,
}PHY_FIFO_STATE;
/************************************************************************/
/*					Variable declaration								*/
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
void phytrx_setPwrMode(PHY_MODE  par_Mode);


/******************************************************************************/
//@FunctionName: phyrx_turnOnRx  			    							  */
//@Description:	 Decrements the "RX on" counter, and turns off RX on a 1 -> 0 */
//               transition if SFD is low. If SFD is high, the turn-off 
//               operation is delayed by one backoff period by using          */
//               a callback                                                   */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void phyrx_turnOnRx(void* par_pParam); 

/******************************************************************************/
//@FunctionName: phyrx_turnOffTrx     					  	                  */
//@Description:	Increments the "RX on" counter, and turns on RX on 
//              a 0 -> 1 transition                                           */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void phyrx_turnOffTrx(void* par_pParam) ;

/******************************************************************************/
//@FunctionName:  phytx_turnOnTx 										  	  */
//@Description:	This function will turn on the TX                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/    
BOOL phytx_turnOnTx();

/******************************************************************************/
//@FunctionName: phyrx_forceOffTrx  										  */
//@Description:	 This task forces RX off by issuing a SRFOFF command strobe, 
//               regardless of whether the SFD pin state. If SFD was high 
//           before the command was issued, an RX engine cleanup is performed.*/
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void phyrx_forceOffTrx(void* par_pParam) ;


/******************************************************************************/
//@FunctionName: phyrx_ResetRx                      	  	                  */
//@Description:	 Resets the RX engine by clearing the RX FIFO, 
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void phyrx_ResetRx();


/******************************************************************************/
//@FunctionName: phyrx_checkAck  										  	  */
//@Description:	This function will check the need for ACK, if need, it will 
//              perform setting to send ACK                                   */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void phyrx_checkAck(P_MPDU_RX  par_pMpdu);

/******************************************************************************/
//@FunctionName:  phy_disableFIFOInt					  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void phy_disableFIFOInt(BOOL bDisable);


void            phy_WriteRam(void *pData, UINT16 address, UINT8 count, BOOL disableInterrupts);

PHY_FIFO_STATE  phy_ReadFifo(void *pData, UINT8 count, UINT8* readCount);

void            phy_WriteFifo(void *pData, UINT8 count);

#endif // !PHY_SUPPOR_H
