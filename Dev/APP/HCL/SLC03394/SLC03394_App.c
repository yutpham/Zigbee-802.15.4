/*********************************************************************
  Filename:       SLC03394_App.c
  Revised:        $Date: 2004/12/30 16:39:52 $
  Revision:       $Revision: 1.10 $

  Description:    
				  Switching Load Control - Application

          ZigBee Document 03394

          This application is a sample implementation of the 
          Home Controls - Lighting Profile's Switching Load Controller
          device.  This device is a light and can be controlled by a 
          switch (DRC).
          
          This implementation is split between 2 files: 
          SLC03394_App.c and SLC03394_Device.c.  SLC03394_App.c is the
          controlling application (which contains the task), while 
          SLC03394_Device.c handles building/parsing of 
          incoming/outgoing messages.
          
          Optional messages (input and output) are conditionally 
          compiled with SLC03394_OPTIONALS.  The incoming optional
          messages from the light sensor (LSM) contain semi-precision
          fields, which will increase code size by pulling in floating
          point math library functions.  An improvement, to this 
          implementation, would be to use a table (instead of calculation)
          for incoming light level comparisons. 

          Key control:
            SW1:  
            SW2:  initiates end device binding
            SW3:  
            SW4:  initiates a match description request
          
  Notes:          

  Copyright (c) 2004 by Figure 8 Wireless, Inc., All Rights Reserved.
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
#include "HomeControlLighting.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
#define SLC03394_MIN_LIGHT_LIMIT 600.0

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
/* Network State */
UINT8 SLC03394_NwkState;

/* Destination Address */
AF_ADDR SLC03394_DstAddr;

/* Current State of the device - OnOff */
BYTE SLC03394_State;     

/* StatusSLC attributes */
uint16 SLC03394_StatusSLC_AttribId;
uint16 SLC03394_StatusSLC_Data;

/* Saved data for GET */
BYTE saved_OnOffSRC_OnOff;

const ENDPOINT_DESTCRIPTOR SLC03394_epDescMandatory = 
{
    {
      SLC03394_APP_ENDPOINT_MANDATORY,  
      CHL_PROFILE_ID, 
      SLC03394_DEVICE_APPDEVID,
      SLC03394_DEVICE_APPDEVVER, 
      APPFLAG_NONE,
      SLC03394_MANDATORY_IN_CLUSTERS, 
      (BYTE*)SLC03394_MandatoryInClusters,
      SLC03394_MANDATORY_OUT_CLUSTERS,
      (BYTE*)NULL
    },
    SLC03394_ProcessEvent,
    SLC03394_Init    
};

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
void SLC03394_HandleKeys( BYTE shift, BYTE keys );

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SLC03394_Init
 *
 * @brief   Initialization function for the SLC03394 App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be 
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void SLC03394_Init( void* arg1, void* arg2 )
{

  SLC03394_NwkState = DEV_INIT;
  SLC03394_State = OnOffSRC_OFF;

  SLC03394_DstAddr.AddrMode = INVALID_SHORT_ADDR;
  SLC03394_DstAddr.EP = 0;
  SLC03394_DstAddr.Addr.Short = 0;
  
  // Initialize the device description
  SLC03394_InitVars();

  // Register the endpoint description with the AF
  app_addEP( &SLC03394_epDescMandatory );

  // Register for all key events - This app will handle all key events
  RegisterEP( SLC03394_epDescMandatory.SimpleDescriptor.EndPoint );

}

/*********************************************************************
 * @fn      SLC03394_ProcessEvent
 *
 * @brief   SLC03394 Application Task event processor.  This function 
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
void SLC03394_ProcessEvent( void* par_pMsg, void* arg2 )
{
    ZAPP_MSG*       pMsg;
    AF_IMCOM_FRM*   pAfFrm;
    BYTE*           pTemp;            
    BYTE            reading;

    pMsg = par_pMsg;
    switch(pMsg->MsgType)
    {
        /*Process general event */
    case ZDO_DEVICE_STARTED:
        /* Network State */
        SLC03394_NwkState = DEV_STARTED;
    	break;
    case APF_DATA_INDICATION:
        pAfFrm = pMsg->Data;

        if (HI_UINT8(pAfFrm->Cmd[0]) == AF_FRM_MSG)
            // Process the incoming message
            SLC03394_ProcessMSGCmd( pAfFrm );
        else
            SLC03394_ProcessKVPCmd( pAfFrm);

        apf_releaseInComFrm(pAfFrm);
    	break;
    case APF_DATA_CONFIRM:
        break;
    case APF_KEYS:
        if(SLC03394_NwkState == DEV_STARTED)
        {
            pTemp = &pMsg->Data;
            SLC03394_HandleKeys(pTemp[0] , pTemp[1] );
        }
        break;

        /*Application definition msg
        */
    case SLC03394_STATE_CHANGED_EVT:
        if ( SLC03394_State == OnOffSRC_ON )
          LedOnOff( LED3, LED_ON );
        else
          LedOnOff( LED3, LED_OFF );

        break;

        /* Unidentified */
        default:
          break;
    }
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      SLC03394_HandleKeys
 *
 * @brief   Handles all key events for this device.
 * 
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 EVAL_SW4
 *                 EVAL_SW3
 *                 EVAL_SW2
 *                 EVAL_SW1
 *
 * @return  none
 */
void SLC03394_HandleKeys( BYTE shift, BYTE keys )
{
  // Shift is used to make each button/switch dual purpose.  
  if ( shift )
  {
    if ( keys & EVAL_SW1 )
    {
    }
    if ( keys & EVAL_SW2 )
    {
    }
    if ( keys & EVAL_SW3 )
    {
    }
    if ( keys & EVAL_SW4 )
    {
    }
  }
  else
  {
    if ( keys & EVAL_SW1 )
    {
    }
    
    if ( keys & EVAL_SW2 )
    {
    }
    
    if ( keys & EVAL_SW3 )
    {
    }
    
    if ( keys & EVAL_SW4 )
    {
    }
    if ( keys & EVAL_SW5 )
    {
        // Initiate an End Device Bind Request for the mandatory endpoint
        zdp_sendEDBindReq( SLC03394_epDescMandatory.SimpleDescriptor.EndPoint );
    }
  }
}

/*********************************************************************
 * DEVICE DESCRIPTION CALLBACKS
 */

/*---------------------------- RcvSET -------------------------------*/

/*********************************************************************
 * @fn      SLC03394_RcvSET_OnOffSRC
 *
 * @brief   Processes a SET for the OnOffSRC command.
 *
 * @param   epInt - Intended endpoint/interface
 * @param   Attrib - OnOffSRC_Off or OnOffSRC_On.
 *
 * @return  Transaction Codes:
 *            TRANSCODE_OK, error or application code
 */
BYTE SLC03394_RcvSET_OnOffSRC( BYTE endPoint, 
                               uint16 AttribId,
                               BYTE State)
{
  BYTE leds;
  if (State == OnOffSRC_ON)
  {
    SLC03394_State = OnOffSRC_ON;
  }
  else if (State == OnOffSRC_OFF)
  {
    SLC03394_State = OnOffSRC_OFF;
  }
  else if (State == OnOffSRC_TOGGLE)
  {
    // Toggle, if current state is ON --> OFF, OFF --> ON
    if (SLC03394_State == OnOffSRC_ON)
      SLC03394_State = OnOffSRC_OFF;
    else
      SLC03394_State = OnOffSRC_ON;
  }
  else
  {
    return ERRORCODE_INVALID_DATA;
  } 
  apf_sendAppMsg( SLC03394_epDescMandatory.SimpleDescriptor.EndPoint, 
                  SLC03394_STATE_CHANGED_EVT, 0x00);          
  return ( ERRORCODE_SUCCESS );
}

/*---------------------------- RcvGET -------------------------------*/
/*********************************************************************
 * @fn      SLC03394_RcvGET_OnOffSRC
 *
 * @brief   Process RcvGET for OnOffSRC
 *
 * @param   AttribId
 * @param   State
 *
 * @return Transaction Code
 *
 */
void SLC03394_RcvGET_OnOffSRC(AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm)
{
  BYTE ErrorCode             = ERRORCODE_SUCCESS;
  BYTE AttribDataType        = DATATYPE_NO_DATA;
  BYTE *Data                 = (BYTE*)NULL;
  BYTE DataLength            = 0;

  switch (pKvp->AttrId)
  {
    case OnOffSRC_OnOff:
      Data = &saved_OnOffSRC_OnOff;
      DataLength = sizeof (saved_OnOffSRC_OnOff);
      AttribDataType = DATATYPE_UINT8;
      break;

    default:
      ErrorCode = ERRORCODE_UNSUPPORTED_ATTRIB;
      break;
  }
  apf_sendSingleKVP(pKvp->TransSeqNumber,AF_CMD_GET_RSP,AttribDataType,pKvp->AttrId,
                    ErrorCode, Data,DataLength, &pAfFrm->SrcAddr, pAfFrm->ClusterId,pAfFrm->DstEp,0x00);

}



/*********************************************************************
*********************************************************************/
