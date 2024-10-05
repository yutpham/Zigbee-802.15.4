/*******************************************************************************/
//Module: MAC/phy_primitives                                                   */   
//File Name: phy_primitives.h                                                  */
//Description: Describes type definitions for PHY primitives				   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//12/27/2006        pnthai          Create                                     */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef PHY_PRIMITIVES_H
#define PHY_PRIMITIVES_H

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "mac_types.h"
#include "mac_packet.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/


/************************************************************************/
/*					Type Definition										*/
/************************************************************************/

//The result of the request to transmit a packet.
typedef enum 
{
    PHY_BUSY = 0x00, 
    PHY_BUSY_RX,
    PHY_BUSY_TX, 
    PHY_FORCE_TRX_OFF,
    PHY_IDLE,
    PHY_INVALID_PARAMETER,
    PHY_RX_ON,
    PHY_SUCCESS,
    PHY_TRX_OFF,
    PHY_TX_ON,
    PHY_UNSUPPORTED_ATTRIBUTE
}PHY_STATUS;

//PHY PIB type
typedef enum 
{
    PHY_PIB_CURRENT_CHANNEL =0x00,
    PHY_PIB_CHANNEL_SUPPORTED,
    PHY_PIB_TRANSMIT_POWER,
    PHY_PIB_CCA_MODE
}PHY_PIB_ATT_TYPE;


//Define type of the phy packet
typedef struct 
{
    INT8        nLength;
    P_MPDU_RX   pCurRxPacket;
    UINT8       nHrLength;
}PHY_RX_PACKET;

typedef PHY_RX_PACKET*  P_PHY_RX_PACKET;

// The PHY PIB comprises the attributes required to manage the PHY of a device.
typedef struct 
{
    UINT8  phyCurrentChannel;
    UINT32 phyChannelsSupported;     
    UINT8  phyTransmitPower;
    UINT8  phyCcaMode;
}PHY_PIB;

//State of packet reception process
typedef enum   
{
    PHY_RX_STATE_PHR = 0,
    PHY_RX_STATE_PAYLOAD,
    PHY_RX_STATE_FCS,
    PHY_RX_STATE_SUCCESS,
    PHY_RX_STATE_RESET,
    PHY_RX_STATE_DISCARD
}PHY_RX_STATE;

//Type define all information needed of working of Phy.
typedef struct  
{
    PHY_PIB                 PhyPIB;
#ifndef ZIGBEE_DEBUG
    volatile  
#endif
    PHY_STATUS    PhyState;

    // This counter decides whether or not the receiver is on (greater than 0) or off (0)
    UINT8                   nrxOnCounter;
    
    // Variable to control FIFO interrupt
    BOOL                    bFifopIntOff;
    
    // mlmeRxEnableRequest parameters
    UINT32                  rxEnableOnDuration;

    //Number of pkt is queuing
    UINT8                   nQueueingPkt;

    //Pointer to current active packet
    PHY_RX_PACKET           sCurPacket;    
    PHY_RX_STATE            eState;     //state of the current receiving packet
}PHY_INFO;

/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
extern PHY_INFO     PhyInfo;
/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/******************************************************************************/
//@FunctionName:  pddata_request 										  	  */
//@Description:	 The PD-DATA.request primitive requests the transfer of an 
//               MPDU (i.e., PSDU) from the MAC sublayer to                   */
//               the local PHY entity.                                        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
PHY_STATUS pddata_request(MPDU* par_pMpdu);


/******************************************************************************/
//@FunctionName: pddata_indication  										 */
//@Description:	The PD-DATA.indication primitive indicates the transfer of an 
//              MPDU (i.e., PSDU) from the PHY to the                         */
//              local MAC sublayer entity.                                    */
//	            Don't care Link quality                                       */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void pddata_indication(P_MPDU_RX pRxMpdu,  UINT8 pPduLinkQuality);


/******************************************************************************/
//@FunctionName:  plmeCCA_request 										  	  */
//@Description: The PLME-CCA.request primitive requests that the PLME perform 
//              a CCA as defined in 6.7.9                                     */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
PHY_STATUS    plmeCCA_request(void);


/******************************************************************************/
//@FunctionName:  plmeED_request 										  	  */
//@Description:	The PLME-ED.request primitive requests that the PLME perform 
//              an ED measurement (see 6.7.7).                                */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/

PHY_STATUS plmeED_request (UINT8* par_EnergyLevel);

/******************************************************************************/
//@FunctionName:  plmeGET_request 										  	  */
//@Description:	The PLME-GET.request primitive requests information about 
//              a given PHY PIB attribute.                                    */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
PHY_STATUS plmeGET_request (PHY_PIB_ATT_TYPE par_PIBAttType, UINT32* par_pPIBAttributeValue);



/******************************************************************************/
//@FunctionName: plmeSET_request  										  	  */
//@Description:	The PLME-SET.request primitive attempts to set the indicated  */
//              PHY PIB attribute to the given value.                         */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
PHY_STATUS plmeSET_request ( PHY_PIB_ATT_TYPE PIBAttType, UINT32 PIBAttributeValue );





#endif // ! PHY_PRIMITIVES_H
