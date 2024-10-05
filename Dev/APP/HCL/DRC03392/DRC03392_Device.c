/*********************************************************************
  Filename:       DRC03392_Device.c
  Revised:        $Date: 2004/12/30 17:02:59 $
  Revision:       $Revision: 1.5 $

  Description:    
				  Dimmer Remote Control - Device Description

          ZigBee Document 03392r0ZB

  Notes:          

  Copyright (c) 2003 by Figure 8 Wireless, Inc., All Rights Reserved.
  Permission to use, reproduce, copy, prepare derivative works,
  modify, distribute, perform, display or sell this software and/or
  its documentation for any purpose is prohibited without the express
  written consent of Figure 8 Wireless, Inc.
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "types.h"
#include "mac_types.h"
#include "app_framework.h"
#include "zdp.h"
#include "DRC03392_App.h"
#include "DRC03392_Device.h"
#include "board.h"
#include "zdo.h"
#include "HomeControlLighting.h"
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Cluster Lists
const byte DRC03392_MandatoryOutClusters[DRC03392_MANDATORY_OUT_CLUSTERS] = 
{
  CLUSTERID_OnOffDRC,
  CLUSTERID_DimBrightDRC,
  CLUSTERID_PresetDRC
};

byte DRC03392_transID;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
 
/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */
void DRC03392_RcvSET       ( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm, byte ack );
void DRC03392_RcvSETRESP   ( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm );

void DRC03392_RcvGET       ( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm );
void DRC03392_RcvGETRESP   ( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm );

void DRC03392_RcvEVENT     ( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm, byte ack );
void DRC03392_RcvEVENTRESP ( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm );

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      DRC03392_InitVars
 *
 * @brief   Initialize any local variables.
 *
 * @param   none
 *
 * @return  none
 */
void DRC03392_InitVars( void )
{
  DRC03392_transID = 0;
}

/*********************************************************************
 * @fn      DRC03392_ProcessKVPCmd
 *
 * @brief   Processes the incoming message for this device
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void DRC03392_ProcessKVPCmd(AF_IMCOM_FRM*   pAfFrm )
{
    AF_KVP  Kvp;    

    if (!apf_getKVPTrans(pAfFrm,&Kvp,1))
        return;

	switch ( Kvp.CmdType )
	{
		case CMDTYPE_SET_ACK:
		DRC03392_RcvSET( &Kvp, pAfFrm, TRUE );
		break;

		case CMDTYPE_SET:
		DRC03392_RcvSET( &Kvp, pAfFrm, FALSE );
		break;

		case CMDTYPE_GET_ACK:
		DRC03392_RcvGET( &Kvp, pAfFrm );
		break;

		case CMDTYPE_GET_RESP:
		DRC03392_RcvGETRESP( &Kvp, pAfFrm );
		break;

		case CMDTYPE_SET_RESP:
		// Not sending with ACK for now.
		break;

		case CMDTYPE_EVENT:
		DRC03392_RcvEVENT( &Kvp, pAfFrm, FALSE );
		break;

		case CMDTYPE_EVENT_ACK:
		DRC03392_RcvEVENT( &Kvp, pAfFrm, TRUE );
		break;

		case CMDTYPE_EVENT_RESP:
		// no need, this device will not send EVENTs
		break;

	}
}

/*********************************************************************
 * @fn      DRC03392_ProcessMSGCmd
 *
 * @brief   Processes the incoming message for this device
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void DRC03392_ProcessMSGCmd( AF_IMCOM_FRM*   pAfFrm )
{

}

/*********************************************************************
 * @fn      DRC03392_RcvSET
 *
 * @brief   Process a received SET command
 *
 * @param   pkt - incoming packet
 * @param   ack - TRUE if SETwithACK 
 *
 * @return  none
 */
void DRC03392_RcvSET( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm, byte ack )
{
  byte ErrorCode;
  switch ( pAfFrm->ClusterId)
  {
    default:
      ack = TRUE;
      ErrorCode = ERRORCODE_UNSUPPORTED_ATTRIB;
      break;
  }

  if ( ack )
  {
    // Send back response message                                                         
    apf_sendSingleKVP(pKvp->TransSeqNumber,AF_CMD_SET_RSP,pKvp->AttrDataType,pKvp->AttrId,
                    ErrorCode, NULL,0, &pAfFrm->SrcAddr, pAfFrm->ClusterId,pAfFrm->DstEp,0x00);    
  }
}

/*********************************************************************
 * @fn      DRC03392_RcvSETRESP
 *
 * @brief   Process a received SET_RESPONSE command
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void DRC03392_RcvSETRESP ( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm )
{
  /* Process SET_RESPONSE HERE */

}

/*********************************************************************
 * @fn      DRC03392_RcvGET
 *
 * @brief   Process a received GET command
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void DRC03392_RcvGET( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm )
{
 
  switch ( pAfFrm->ClusterId)
  {

    default:
      /* Unknown CLUSTERID */
      break;
  }
}

/*********************************************************************
 * @fn      DRC03392_RcvGETRESP
 *
 * @brief   Process a received GET_RESPONSE command
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void DRC03392_RcvGETRESP( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm)
{
}

/*********************************************************************
 * @fn      DRC03392_RcvEVENT
 *
 * @brief   Process a received Event command
 *
 * @param   pkt - incoming packet
 * @param   ack - TRUE if EVENTwithACK 
 *
 * @return  none
 */
void DRC03392_RcvEVENT( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm, byte ack )
{
  byte ErrorCode;
  switch ( pAfFrm->ClusterId)
  {
    default:
      ack = TRUE;
      ErrorCode = ERRORCODE_UNSUPPORTED_ATTRIB;
      break;
  }

  if ( ack )
  {
    // Send back response message   
    apf_sendSingleKVP(pKvp->TransSeqNumber,CMDTYPE_EVENT_RESP,pKvp->AttrDataType,pKvp->AttrId,
                    ErrorCode, NULL,0, &pAfFrm->SrcAddr, pAfFrm->ClusterId,pAfFrm->DstEp,0x00);        
  }
}

/*********************************************************************
 * @fn      DRC03392_RcvEVENTRESP
 *
 * @brief   Process a received Event Response command
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void DRC03392_RcvEVENTRESP( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm )
{

  /* Process EVENT_RESPONSE HERE */

}

/*********************************************************************
 * @fn      DRC03392_Set_OnOff
 *
 * @brief   Sends a SET command for OnOffDRC Cluster.
 *
 * @param   dstAddr - NULL or DSTINDIRECT, if indirect
 * @param   epDesc - pointer to the originating endpoint descriptor
 * @param   Attrib - OnOffDRC_Off or OnOffDRC_On.
 *
 * @return  0 - Sent it on its way, !0 if error
 */
byte DRC03392_Set_OnOff( AF_ADDR*   pDstAddr, 
                         ENDPOINT_DESTCRIPTOR *epDesc, 
                         uint16 Attrib,
                         byte State )
{                                         
  return apf_sendSingleKVP(DRC03392_transID++,CMDTYPE_SET,DATATYPE_UINT8,Attrib,
                        SUCCESS, &State,1, pDstAddr, CLUSTERID_OnOffDRC,epDesc->SimpleDescriptor.EndPoint,0x00);          
}

/*********************************************************************
 * @fn      DRC03392_Set_DimBright
 *
 * @brief   Sends a SET command for DimBright Cluster.
 *
 * @param   dstAddr - NULL or DSTINDIRECT, if indirect
 * @param   epDesc - pointer to the originating endpoint descriptor
 * @param   Attrib - DimBrightDRC_Dim or DimBrightDRC_Bright
 * @param   percent - 0 -100 (% of full on)
 *
 * @return  0 - Sent it on its way, !0 if error
 */
byte DRC03392_Set_DimBright( AF_ADDR* pDstAddr, 
                             ENDPOINT_DESTCRIPTOR *epDesc, 
                             uint16 Attrib, byte Steps )
{

  return apf_sendSingleKVP(DRC03392_transID++,CMDTYPE_SET,DATATYPE_UINT8,Attrib,
                        SUCCESS, &Steps,1, pDstAddr, CLUSTERID_DimBrightDRC,epDesc->SimpleDescriptor.EndPoint,0x00);            
}

/*********************************************************************
 * @fn      DRC03392_Set_Preset
 *
 * @brief   Sends a SET command for Preset Cluster.
 *
 * @param   dstAddr - NULL or DSTINDIRECT, if indirect
 * @param   epDesc - pointer to the originating endpoint descriptor
 * @param   Attrib - PresetDRC_StorePreset or PresetDRC_Preset
 *
 * @return  0 - Sent it on its way, !0 if error
 */
byte DRC03392_Set_Preset( AF_ADDR*              pDstAddr, 
                          ENDPOINT_DESTCRIPTOR* epDesc, 
                          uint16 Attrib )
{

  return apf_sendSingleKVP(DRC03392_transID++,CMDTYPE_SET,DATATYPE_NO_DATA,Attrib,
                        SUCCESS, NULL, 0, pDstAddr, CLUSTERID_PresetDRC,epDesc->SimpleDescriptor.EndPoint,0x00);               
}

/*********************************************************************
*********************************************************************/
