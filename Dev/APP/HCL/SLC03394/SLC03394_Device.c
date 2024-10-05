/*********************************************************************
  Filename:       SLC03394_Device.c
  Revised:        $Date: 2004/12/30 16:39:52 $
  Revision:       $Revision: 1.4 $

  Description:    
				  Switching Load Control - Device Description

          ZigBee Document 03394r0ZB

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
#include "SLC03394_App.h"
#include "SLC03394_Device.h"
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
const BYTE SLC03394_MandatoryInClusters[SLC03394_MANDATORY_IN_CLUSTERS] = 
{
  CLUSTERID_OnOffSRC,
};

BYTE SLC03394_transID;

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
void SLC03394_RcvSET( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm, BYTE ack );
void SLC03394_RcvSETRESP( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm);

void SLC03394_RcvGET( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm );
void SLC03394_RcvGETRESP( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm);

void SLC03394_RcvEVENT( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm, BYTE ack );
void SLC03394_RcvEVENTRESP( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm);

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SLC03394_InitVars
 *
 * @brief   Initialize any local variables.
 *
 * @param   none
 *
 * @return  none
 */
void SLC03394_InitVars( void )
{
  SLC03394_transID = 0;
}

/*********************************************************************
 * @fn      SLC03394_ProcessKVPCmd
 *
 * @brief   Processes the incoming message for this device
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void SLC03394_ProcessKVPCmd( AF_IMCOM_FRM*   pAfFrm )
{
  AF_KVP  Kvp;    

  if (!apf_getKVPTrans(pAfFrm,&Kvp,1))
    return;

  switch ( Kvp.CmdType )
  {
    case CMDTYPE_SET_ACK:
      SLC03394_RcvSET( &Kvp, pAfFrm, TRUE );
      break;

    case CMDTYPE_SET:
      SLC03394_RcvSET( &Kvp, pAfFrm, FALSE );
      break;

    case CMDTYPE_GET_ACK:
      SLC03394_RcvGET( &Kvp, pAfFrm );
      break;

    case CMDTYPE_GET_RESP:
      SLC03394_RcvGETRESP( &Kvp, pAfFrm );
      break;

    case CMDTYPE_SET_RESP:
      // Not sending with ACK for now.
      break;

    case CMDTYPE_EVENT:
      SLC03394_RcvEVENT( &Kvp, pAfFrm, FALSE );
      break;

    case CMDTYPE_EVENT_ACK:
      SLC03394_RcvEVENT( &Kvp, pAfFrm, TRUE );
      break;

    case CMDTYPE_EVENT_RESP:
      break;
  }
}

/*********************************************************************
 * @fn      SLC03394_ProcessMSGCmd
 *
 * @brief   Processes the incoming message for this device
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void SLC03394_ProcessMSGCmd( AF_IMCOM_FRM*   pAfFrm )
{
}

/*********************************************************************
 * @fn      SLC03394_RcvSET
 *
 * @brief   Process a received SET command
 *
 * @param   pkt - incoming packet
 * @param   ack - TRUE if SETwithACK 
 *
 * @return  none
 */
void SLC03394_RcvSET( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm, BYTE ack )
{
  BYTE result;
  uint16 AttribId;
  BYTE ep;

  ep = pAfFrm->DstEp;
  AttribId = pKvp->AttrId;

  switch ( pAfFrm->ClusterId )
  {
    case CLUSTERID_OnOffSRC:
      result = SLC03394_RcvSET_OnOffSRC( ep, AttribId, *(pKvp->pData) );
      break;
    
    default:         
      ack = TRUE;
      result = ERRORCODE_UNSUPPORTED_ATTRIB;
      break;

  }

  if ( ack )
  {
    // Send back response message
    // Send back response message                                        
    apf_sendSingleKVP(pKvp->TransSeqNumber,AF_CMD_SET_RSP,pKvp->AttrDataType,pKvp->AttrId,
                    result, NULL,0, &pAfFrm->SrcAddr, pAfFrm->ClusterId,pAfFrm->DstEp,0x00);          
                              
  }
}

/*********************************************************************
 * @fn      SLC03394_RcvSETRESP
 *
 * @brief   Process a received SET RESPONSE command
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void SLC03394_RcvSETRESP( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm)
{
  /* Process GET_RESPONSE HERE */

}


/*********************************************************************
 * @fn      SLC03394_RcvGET
 *
 * @brief   Process a received GET command
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void SLC03394_RcvGET( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm )
{

  switch ( pAfFrm->ClusterId )
  {
    case CLUSTERID_OnOffSRC:
      SLC03394_RcvGET_OnOffSRC( pKvp, pAfFrm);
      break;

    default:
      /* Undefined ClusterID */
      break;
  }
}

/*********************************************************************
 * @fn      SLC03394_RcvGETRESP
 *
 * @brief   Process a received GET_RESPONSE command
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void SLC03394_RcvGETRESP( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm)
{

  /* Process GET_RESPONSE HERE */

}
/*********************************************************************
 * @fn      SLC03394_RcvEVENT
 *
 * @brief   Process a received Event command
 *
 * @param   pkt - incoming packet
 * @param   ack - TRUE if EVENTwithACK 
 *
 * @return  none
 */
void SLC03394_RcvEVENT( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm, BYTE ack )
{
  BYTE result;
  switch ( pAfFrm->ClusterId )
  {
    default:
      ack = TRUE;
      result = ERRORCODE_UNSUPPORTED_ATTRIB;
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
 * @fn      SLC03394_RcvEVENTRESP
 *
 * @brief   Process a received EVENT RESPONSE command
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void SLC03394_RcvEVENTRESP( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm)
{
  /* Process GET_RESPONSE HERE */

}

/*********************************************************************
*********************************************************************/
