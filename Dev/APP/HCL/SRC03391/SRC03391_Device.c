/*********************************************************************
  Filename:       SRC03391_Device.c
  Revised:        $Date: 2004/12/30 16:28:14 $
  Revision:       $Revision: 1.6 $

  Description:    
				  Switch Remote Control - Device Description

          ZigBee Document 03391r0ZB

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
#include "SRC03391_App.h"
#include "SRC03391_Device.h"
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
const BYTE SRC03391_MandatoryOutClusters[SRC03391_MANDATORY_OUT_CLUSTERS] = 
{
  CLUSTERID_OnOffSRC
};

BYTE SRC03391_TransSeqNumber;

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
void SRC03391_RcvSET( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm, BYTE ack );
void SRC03391_RcvSETRESP( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm);

void SRC03391_RcvGET( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm );
void SRC03391_RcvGETRESP( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm);

void SRC03391_RcvEVENT( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm, BYTE ack );
void SRC03391_RcvEVENTRESP( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm);

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SRC03391_InitVars
 *
 * @brief   Initialize any local variables.
 *
 * @param   none
 *
 * @return  none
 */
void SRC03391_InitVars( void )
{
  SRC03391_TransSeqNumber = 0;
}

/*********************************************************************
 * @fn      SRC03391_ProcessKVPCmd
 *
 * @brief   Processes the incoming message for this device
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void SRC03391_ProcessKVPCmd( AF_IMCOM_FRM*   pAfFrm )
{

  AF_KVP  Kvp;    

  if (!apf_getKVPTrans(pAfFrm,&Kvp,1))

    return;
  switch ( Kvp.CmdType )
  {
    case CMDTYPE_SET_ACK:
      SRC03391_RcvSET( &Kvp, pAfFrm, TRUE );
      break;

    case CMDTYPE_SET:
      SRC03391_RcvSET( &Kvp, pAfFrm, FALSE );
      break;

    case CMDTYPE_GET_ACK:
      SRC03391_RcvGET( &Kvp, pAfFrm );
      break;

    case CMDTYPE_SET_RESP:
      // Not sending with ACK for now.
      break;

    case CMDTYPE_EVENT:
      SRC03391_RcvEVENT( &Kvp, pAfFrm, FALSE );
      break;

    case CMDTYPE_EVENT_ACK:
      SRC03391_RcvEVENT( &Kvp, pAfFrm, TRUE );
      break;

    case CMDTYPE_EVENT_RESP:
      // no need, this device will not send EVENTs
      break;
  }
}

/*********************************************************************
 * @fn      SRC03391_ProcessMSGCmd
 *
 * @brief   Processes the incoming message for this device
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void SRC03391_ProcessMSGCmd( AF_IMCOM_FRM*   pAfFrm )
{
}

/*********************************************************************
 * @fn      SRC03391_RcvSET
 *
 * @brief   Process a received SET command
 *
 * @param   pkt - incoming packet
 * @param   ack - TRUE if SETwithACK 
 *
 * @return  none
 */
void SRC03391_RcvSET( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm, BYTE ack )
{
  BYTE ErrorCode;
  switch ( pAfFrm->ClusterId )
  {
    default:
      ack = TRUE;
      ErrorCode = ERRORCODE_UNSUPPORTED_ATTRIB;
      break;
  }

  if ( ack )
  {
    apf_sendSingleKVP(pKvp->TransSeqNumber,CMDTYPE_SET_RESP,pKvp->AttrDataType,pKvp->AttrId,
                    ErrorCode, NULL,0, &pAfFrm->SrcAddr, pAfFrm->ClusterId,pAfFrm->DstEp,0x00);    
    
  }
}

/*********************************************************************
 * @fn      SRC03391_RcvSETRESP
 *
 * @brief   Process a received SET RESPONSE command
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void SRC03391_RcvSETRESP( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm)
{
  /* Process GET_RESPONSE HERE */
}

/*********************************************************************
 * @fn      SRC03391_RcvGET
 *
 * @brief   Process a received GET command
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void SRC03391_RcvGET( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm )
{

  switch ( pAfFrm->ClusterId )
  {
    default:
      /* Unknown Cluster_ID */
      break;
  }
}

/*********************************************************************
 * @fn      SRC03391_RcvGETRESP
 *
 * @brief   Process a received GET_RESPONSE command
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void SRC03391_RcvGETRESP( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm)
{
  /* Process GET_RESPONSE HERE */
}

/*********************************************************************
 * @fn      SRC03391_RcvEVENT
 *
 * @brief   Process a received Event command
 *
 * @param   pkt - incoming packet
 * @param   ack - TRUE if EVENTwithACK 
 *
 * @return  none
 */
void SRC03391_RcvEVENT( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm, BYTE ack )
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
    apf_sendSingleKVP(pKvp->TransSeqNumber,CMDTYPE_EVENT_RESP,pKvp->AttrDataType,pKvp->AttrId,
                    result, NULL,0, &pAfFrm->SrcAddr, pAfFrm->ClusterId,pAfFrm->DstEp,0x00);    
  }
}

/*********************************************************************
 * @fn      SRC03391_RcvEVENTRESP
 *
 * @brief   Process a received EVENT_RESPONSE command
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
void SRC03391_RcvEVENTRESP( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm)
{
  /* Process GET_RESPONSE HERE */
}

/*********************************************************************
 * @fn      SRC03391_Set_OnOffSRC
 *
 * @brief   Sends a SET command for OnOffSRC Cluster.
 *
 * @param   dstAddr - NULL or DSTINDIRECT, if indirect
 * @param   epDesc - pointer to the originating endpoint descriptor
 * @param   State - OnOffSRC_Off or OnOffSRC_On.
 *
 * @return  0 - Sent it on its way, !0 if error
 */
BYTE SRC03391_Set_OnOffSRC( AF_ADDR*   pDstAddr, 
                            ENDPOINT_DESTCRIPTOR *epDesc,
                            BYTE State )
{

    // Send back response message                                                         
  return apf_sendSingleKVP(SRC03391_TransSeqNumber++,CMDTYPE_SET,DATATYPE_UINT8,OnOffSRC_OnOff,
                        SUCCESS, &State, 1, pDstAddr, CLUSTERID_OnOffSRC,epDesc->SimpleDescriptor.EndPoint,0x00);
}

/*********************************************************************
*********************************************************************/
