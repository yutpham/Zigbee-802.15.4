/*********************************************************************
  Filename:       DLC03395_Device.c
  Revised:        $Date: 2004/12/30 17:08:44 $
  Revision:       $Revision: 1.5 $

  Description:    
				  Dimmer Load Control - Device Description

          ZigBee Document 03395r0ZB

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
#include "DLC03395_App.h"
#include "DLC03395_Device.h"
#include "board.h"
#include "zdo.h"
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
const byte DLC03395_MandatoryInClusters[DLC03395_MANDATORY_IN_CLUSTERS] = 
{
  CLUSTERID_OnOffDRC,
  CLUSTERID_DimBrightDRC,
  CLUSTERID_PresetDRC
};

byte DLC03395_transID;

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
void DLC03395_RcvSET      ( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm, byte ack );
void DLC03395_RcvSETRESP  ( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm);

void DLC03395_RcvGET      ( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm );
void DLC03395_RcvGETRESP  ( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm);

void DLC03395_RcvEVENT    ( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm, byte ack );
void DLC03395_RcvEVENTRESP( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm);

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      DLC03395_InitVars
 *
 * @brief   Initialize any local variables.
 *
 * @param   none
 *
 * @return  none
 */
void DLC03395_InitVars( void )
{
  DLC03395_transID = 0;
}

/*********************************************************************
 * @fn      DLC03395_ProcessKVPCmd
 *
 * @brief   Processes the incoming message for this device
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void DLC03395_ProcessKVPCmd( AF_IMCOM_FRM*   pAfFrm )
{
    AF_KVP  Kvp;    

    if (!apf_getKVPTrans(pAfFrm,&Kvp,1))
        return;
 
    switch ( Kvp.CmdType)
    {
      case CMDTYPE_SET:
        DLC03395_RcvSET(&Kvp, pAfFrm, FALSE );
        break;

      case CMDTYPE_SET_ACK:
        DLC03395_RcvSET( &Kvp, pAfFrm, TRUE );
        break;
      
      case CMDTYPE_SET_RESP:
        // Not sending with ACK for now.
        break;

      case CMDTYPE_GET_ACK:
        DLC03395_RcvGET( &Kvp, pAfFrm );
        break;

      case CMDTYPE_GET_RESP:
        DLC03395_RcvGETRESP ( &Kvp, pAfFrm );
        break;
     
      case CMDTYPE_EVENT:
        DLC03395_RcvEVENT( &Kvp, pAfFrm, FALSE );
        break;

      case CMDTYPE_EVENT_ACK:
        DLC03395_RcvEVENT( &Kvp, pAfFrm, TRUE );
        break;

      case CMDTYPE_EVENT_RESP:
        break;

    }
}

/*********************************************************************
 * @fn      DLC03395_ProcessMSGCmd
 *
 * @brief   Processes the incoming message for this device
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void DLC03395_ProcessMSGCmd( AF_IMCOM_FRM*   pAfFrm )
{

}

/*********************************************************************
 * @fn      DLC03395_RcvSET
 *
 * @brief   Process a received SET command
 *
 * @param   pkt - incoming packet
 * @param   ack - TRUE if SETwithACK 
 *
 * @return  none
 */
void DLC03395_RcvSET( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm , byte ack )
{
  byte result;
  uint16 AttribId;
  byte ep;

  ep = pAfFrm->DstEp;
  AttribId = pKvp->AttrId;

  switch ( pAfFrm->ClusterId)
  {
    case CLUSTERID_OnOffDRC:     
      result = DLC03395_RcvSET_OnOffDRC( ep, AttribId, *(pKvp->pData) );
      break;

    case CLUSTERID_DimBrightDRC:
      result = DLC03395_RcvSET_DimBrightDRC( ep, AttribId, *(pKvp->pData) );
      break;

    case CLUSTERID_PresetDRC:
      result = DLC03395_RcvSET_PresetDRC( ep, AttribId, *(pKvp->pData) );
      break;
    
    default:         
      ack = TRUE;
      result = AF_ERR_UNSUPPORTED_ATTRIB;
      break;
  }

  if ( ack )
  {
    // Send back response message                                        
    apf_sendSingleKVP(pKvp->TransSeqNumber,AF_CMD_SET_RSP,pKvp->AttrDataType,pKvp->AttrId,
                    result, NULL,0, &pAfFrm->SrcAddr, pAfFrm->ClusterId,pAfFrm->DstEp,0x00);    
  }
}

/*********************************************************************
 * @fn      DLC03395_RcvSETRESP
 *
 * @brief   Process a received SET_RESPONSE command
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void DLC03395_RcvSETRESP ( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm )
{

}

/*********************************************************************
 * @fn      DLC03395_RcvGET
 *
 * @brief   Process a received GET command
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void DLC03395_RcvGET( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm )
{ 
  switch ( pAfFrm->ClusterId)
  {
    case CLUSTERID_OnOffDRC:
      DLC03395_RcvGET_OnOffDRC( pKvp, pAfFrm );
      break;

    case CLUSTERID_DimBrightDRC:
      DLC03395_RcvGET_DimBrightDRC( pKvp, pAfFrm );
      break;

    case CLUSTERID_PresetDRC:
      DLC03395_RcvGET_PresetDRC(  pKvp, pAfFrm );
      break;

    default:
      /* Unknown Cluster_ID */
      break;
  }

}

/*********************************************************************
 * @fn      DLC03395_RcvGETRESP
 *
 * @brief   Process a received GET_RESPONSE command
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void DLC03395_RcvGETRESP( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm)
{

  /* Process GET_RESPONSE HERE */

}

/*********************************************************************
 * @fn      DLC03395_RcvEVENT
 *
 * @brief   Process a received Event command
 *
 * @param   pkt - incoming packet
 * @param   ack - TRUE if EVENTwithACK 
 *
 * @return  none
 */
void DLC03395_RcvEVENT( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm, byte ack )
{
  byte result;
  switch ( pAfFrm->ClusterId )
  {
    default:
      ack = TRUE;
      result = AF_ERR_UNSUPPORTED_ATTRIB;
      break;
  }

  if ( ack )
  {
    // Send back response message                          
    apf_sendSingleKVP(pKvp->TransSeqNumber,AF_CMD_EVENT_RSP,pKvp->AttrDataType,pKvp->AttrId,
                    result, NULL,0, &pAfFrm->SrcAddr, pAfFrm->ClusterId,pAfFrm->DstEp,0x00);        
  }
}

/*********************************************************************
 * @fn      DLC03395_RcvEVENTRESP
 *
 * @brief   Process a received Event Response command
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void DLC03395_RcvEVENTRESP( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm )
{
  /* Process the EVENT_RESPONSE HERE */

}

/*********************************************************************
*********************************************************************/
