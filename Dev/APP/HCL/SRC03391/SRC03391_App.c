/*********************************************************************
  Filename:       SRC03391_App.c
  Revised:        $Date: 2004/12/30 16:35:21 $
  Revision:       $Revision: 1.10 $

  Description:    
				  Switch Remote Control - Application

          ZigBee Document 03391

          This application is a sample implementation of the 
          Home Controls - Lighting Profile's Switching Remote Control
          device.  This device is a switch that controls a light(DLC).
          
          This implementation is split between 2 files: 
          SRC03391_App.c and SRC03391_Device.c.  SRC03391_App.c is the
          controlling application (which contains the task), while 
          SRC03391_Device.c handles building/parsing of 
          incoming/outgoing messages.
          
          The optional program and setup input/output messages are 
          conditionally compiled in with SRC03391_OPTIONALS.
          
          Key control:
            SW1:  toggles between light on and light off
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
BYTE SRC03391_taskID;     // Applications task id
UINT8 SRC03391_NwkState;

BYTE SRC03391_state;

AF_ADDR SRC03391_DstAddr;


const ENDPOINT_DESTCRIPTOR SRC03391_epDescMandatory = 
{
    {
      SRC03391_APP_ENDPOINT_MANDATORY,  
      CHL_PROFILE_ID, 
      SRC03391_DEVICE_APPDEVID,
      SRC03391_DEVICE_APPDEVVER, 
      APPFLAG_NONE,
      SRC03391_MANDATORY_IN_CLUSTERS, 
      (BYTE*)NULL,
      SRC03391_MANDATORY_OUT_CLUSTERS, 
      (BYTE*)SRC03391_MandatoryOutClusters 
    },
    SRC03391_ProcessEvent,
    SRC03391_Init
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
void SRC03391_HandleKeys( BYTE shift, BYTE keys );
void SRC03391_ToggleState( void );

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SRC03391_Init
 *
 * @brief   Initialization function for the SRC03391 App Task.
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
void SRC03391_Init( void* arg1, void* arg2 )
{
  SRC03391_NwkState = DEV_INIT;

  // Assume LED4 starts ON (due to bind)
  SRC03391_state = OnOffSRC_ON;

  SRC03391_DstAddr.AddrMode = 0;    /*Default is indirect*/
  SRC03391_DstAddr.EP = 0;
  SRC03391_DstAddr.Addr.Short = INVALID_SHORT_ADDR;

  // Initialize the device description
  SRC03391_InitVars();

  // Register the endpoint description with the AF
  app_addEP( &SRC03391_epDescMandatory );

  // Register for all key events - This app will handle all key events
  RegisterEP( SRC03391_epDescMandatory.SimpleDescriptor.EndPoint );
  
}

/*********************************************************************
 * @fn      SRC03391_ProcessEvent
 *
 * @brief   SRC03391 Application Task event processor.  This function 
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
void SRC03391_ProcessEvent(  void* par_pMsg, void* arg2 )
{  
    ZAPP_MSG*       pMsg;
    AF_IMCOM_FRM*   pAfFrm;
    BYTE*           pTemp;            

    pMsg = par_pMsg;
    switch(pMsg->MsgType)
    {
        /*Process general event */
    case ZDO_DEVICE_STARTED:
        /* Network State */
        SRC03391_NwkState = DEV_STARTED;
    	break;
    case APF_DATA_INDICATION:
        pAfFrm = pMsg->Data;
        if (HI_UINT8(pAfFrm->Cmd[0]) == AF_FRM_MSG)
          // Process the incoming message
          SRC03391_ProcessMSGCmd( pAfFrm );
        else
          // Process the incoming message
          SRC03391_ProcessKVPCmd( pAfFrm );

        apf_releaseInComFrm(pAfFrm);
    	break;
    case APF_DATA_CONFIRM:
        break;
    case APF_KEYS:
        if(SRC03391_NwkState == DEV_STARTED)
        {
            pTemp = &pMsg->Data;
            SRC03391_HandleKeys(pTemp[0] , pTemp[1] );
        }
        break;
    case SRC03391_SEND_STATE:
        SRC03391_Set_OnOffSRC( &SRC03391_DstAddr, 
                               (ENDPOINT_DESTCRIPTOR *)&SRC03391_epDescMandatory, 
                               SRC03391_state );
        break;
        /*Application definition msg
        */
    /* Unidentified */
    default:
      break;
   }
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      SRC03391_HandleKeys
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
void SRC03391_HandleKeys( BYTE shift, BYTE keys )
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
      SRC03391_ToggleState();
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
      zdp_sendEDBindReq( SRC03391_epDescMandatory.SimpleDescriptor.EndPoint );
    }
  }
}

/*********************************************************************
 * Event Generation Functions
 */

/*********************************************************************
 * @fn      SRC03391_ChangeState
 *
 * @brief   Changed the On Off state then generates a send event.
 * 
 * NOTE:  This is the function to call when hardware detects a button
 *        press and the state of the switch is known.  If you want
 *        to act as a toggle call SRC03391_ToggleState.
 *
 * @param   Attrib - OnOffSRC_Off or OnOffSRC_On.
 *
 * @return  none
 */
void SRC03391_ChangeState( BYTE State)
{
  SRC03391_state = State;
  apf_sendAppMsg( SRC03391_epDescMandatory.SimpleDescriptor.EndPoint, 
                                  SRC03391_SEND_STATE, 0x00);
}

/*********************************************************************
 * @fn      SRC03391_ToggleState
 *
 * @brief   Toggle the On Off state then generates a send event.
 *
 * NOTE:  This is the function to call when hardware detects a button
 *        press and you want to act like a toggle switch.
 *
 * @param   none
 *
 * @return  none
 */
void SRC03391_ToggleState( void )
{
  if ( SRC03391_state == OnOffSRC_OFF )
  {
    LedOnOff(LED3,LED_ON);
    SRC03391_state = OnOffSRC_ON;
  }
  else
  {
    LedOnOff(LED3,LED_OFF);
    SRC03391_state = OnOffSRC_OFF;
  }

  apf_sendAppMsg( SRC03391_epDescMandatory.SimpleDescriptor.EndPoint, 
                                      SRC03391_SEND_STATE, 0x00);    
}

/*********************************************************************
 * DEVICE DESCRIPTION CALLBACKS
 */

/*********************************************************************
*********************************************************************/
