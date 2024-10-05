/*********************************************************************
  Filename:       DRC03392_App.c
  Revised:        $Date: 2005/01/11 00:02:54 $
  Revision:       $Revision: 1.14 $

  Description:    
				  Dimmer Remote Control - Application

          ZigBee Document 03392

          This application is a sample implementation of the 
          Home Controls - Lighting Profile's Dimmer Remote Control
          device.  This device is a dimming switch that controls a 
          dimming light(DLC).
          
          This implementation is split between 2 files: 
          DRC03392_App.c and DRC03392_Device.c.  DLC03392_App.c is the
          controlling application (which contains the task), while 
          DLC03392_Device.c handles building/parsing of 
          incoming/outgoing messages.
          
          The optional program and setup input/output messages are 
          conditionally compiled in with DRC03392_OPTIONALS.
          
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
#include "DRC03392_App.h"
#include "DRC03392_Device.h"
#include "board.h"
#include "zdo.h"
#include "HomeControlLighting.h"
/*********************************************************************
 * MACROS
 */
#define ABS(n) ((n < 0) ? -n : n)

/*********************************************************************
 * CONSTANTS
 */
#define READ_LEVEL_TIME 200
#define DIM_STEP        25
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
/* Network State */
UINT8 DRC03392_NwkState;

/* Destination Address */
AF_ADDR DRC03392_DstAddr;

/* OnOff */
byte DRC03392_state;

/* DimBright */
byte DRC03392_DimBright_Steps;
byte DRC03392_DimBright_Value;
uint16 DRC03392_DimBright_Attrib;
int    DiffVal;

// Store previous DRC03392_Preset state - Preset/StorePreset
uint16 DRC03392_Preset;


const ENDPOINT_DESTCRIPTOR DRC03392_epDescMandatory = 
{
    {
      DRC03392_APP_ENDPOINT_MANDATORY,  
      CHL_PROFILE_ID, 
      DRC03392_DEVICE_APPDEVID,
      DRC03392_DEVICE_APPDEVVER, 
      APPFLAG_NONE,
      DRC03392_MANDATORY_IN_CLUSTERS, 
      (byte*)NULL, 
      DRC03392_MANDATORY_OUT_CLUSTERS, 
      (byte*)DRC03392_MandatoryOutClusters 
    },
    DRC03392_ProcessEvent,
    DRC03392_Init
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
void DRC03392_HandleKeys( byte shift, byte keys );

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      DRC03392_Init
 *
 * @brief   Initialization function for the DRC03392 App Task.
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
void DRC03392_Init( void* arg1, void* arg2 )
{
  /* Network State */
  DRC03392_NwkState = DEV_INIT;

  /* OnOff */
  DRC03392_state = OnOffDRC_OFF;
  
  /* DimBright */
  DRC03392_DimBright_Steps = 0;
  DRC03392_DimBright_Value = 0;
  DRC03392_DimBright_Attrib = DimBrightDRC_DIM_ID;
  
  /* PresetDRC_Preset */
  DRC03392_Preset = PresetDRC_Preset_ID;

  /* Initialize Endpoint */
  DRC03392_DstAddr.AddrMode = 0;
  DRC03392_DstAddr.EP       = 0;
  DRC03392_DstAddr.Addr.Short= INVALID_SHORT_ADDR;
  
  // Initialize the device description
  DRC03392_InitVars();
  
  // Register the endpoint description with the AF
  app_addEP( &DRC03392_epDescMandatory );

  // Register for all key events - This app will handle all key events
  RegisterEP( DRC03392_epDescMandatory.SimpleDescriptor.EndPoint );
}

/*********************************************************************
 * @fn      DRC03392_ProcessEvent
 *
 * @brief   DRC03392 Application Task event processor.  This function 
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
void DRC03392_ProcessEvent( void* par_pMsg, void* arg2 )
{
    ZAPP_MSG*       pMsg;
    AF_IMCOM_FRM*   pAfFrm;
    BYTE*           pTemp;            
    byte            reading;

    pMsg = par_pMsg;
    switch(pMsg->MsgType)
    {
        /*Process general event */
    case ZDO_DEVICE_STARTED:
        /* Network State */
        DRC03392_NwkState = DEV_STARTED;
    	break;
    case APF_DATA_INDICATION:
        pAfFrm = pMsg->Data;

        if (HI_UINT8(pAfFrm->Cmd[0]) == AF_FRM_MSG)
            // Process the incoming message
            DRC03392_ProcessMSGCmd( pAfFrm );
        else
            DRC03392_ProcessKVPCmd( pAfFrm);

        apf_releaseInComFrm(pAfFrm);
    	break;
    case APF_DATA_CONFIRM:
        break;
    case APF_KEYS:
        if(DRC03392_NwkState == DEV_STARTED)
        {
            pTemp = &pMsg->Data;
            DRC03392_HandleKeys(pTemp[0] , pTemp[1] );
        }
        break;

        /*Application definition msg
        */
    case DRC03392_READ_LEVEL_EVT:
        break;

    /* This is the event that sends out the OnOffDRC clusterID SET command */
    case DRC03392_SEND_STATE_EVT:
        DRC03392_Set_OnOff( &DRC03392_DstAddr, 
                        (ENDPOINT_DESTCRIPTOR *)&DRC03392_epDescMandatory, 
                        OnOffDRC_OnOff,
                        DRC03392_state );
        break;

    /* This is the event that sends out the DimBright clusterID SET command */
    case DRC03392_SEND_LEVEL_EVT:
        DRC03392_Set_DimBright( &DRC03392_DstAddr, 
                              (ENDPOINT_DESTCRIPTOR *)&DRC03392_epDescMandatory,
                              DRC03392_DimBright_Attrib, 
                              DRC03392_DimBright_Steps );
        break;

    /* This is the event that sends out the PresetDRC cluster SET command */
    case DRC03392_SEND_PRESET_EVT:
        DRC03392_Set_Preset( &DRC03392_DstAddr, 
                           (ENDPOINT_DESTCRIPTOR *)&DRC03392_epDescMandatory, 
                           DRC03392_Preset );
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
 * @fn      DRC03392_HandleKeys
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
void DRC03392_HandleKeys( byte shift, byte keys )
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
        // If we are in the ON position
        if ( DRC03392_state == OnOffDRC_ON )
        {
            // check to increase
            if (DRC03392_DimBright_Value <= MAX_LIGHT_VALUE - DIM_STEP)
            {
                DRC03392_DimBright_Value = DRC03392_DimBright_Value+DIM_STEP;

                // Need to increase the brightness
                DRC03392_DimBright_Steps = (byte)DIM_STEP;
                DRC03392_DimBright_Attrib = DimBrightDRC_BRIGHT_ID;

                // Set the event to send out the message
                apf_sendAppMsg( DRC03392_epDescMandatory.SimpleDescriptor.EndPoint, 
                              DRC03392_SEND_LEVEL_EVT, 0x00);        

                Dimmer((byte)(((uint16)DRC03392_DimBright_Value * 100) / MAX_LIGHT_VALUE ));
            }

        }
    }
    
    if ( keys & EVAL_SW2 )
    {
        DRC03392_ToggleState();
    }
    
    if ( keys & EVAL_SW3 )
    {
        // If we are in the ON position
        if ( DRC03392_state == OnOffDRC_ON )
        {

            if (DRC03392_DimBright_Value >= DIM_STEP)
            {
                DRC03392_DimBright_Value = DRC03392_DimBright_Value-DIM_STEP;

                // Need to decrease the brightness
                DRC03392_DimBright_Steps =  DIM_STEP;
                DRC03392_DimBright_Attrib = DimBrightDRC_DIM_ID;

                // Set the event to send out the message
                apf_sendAppMsg( DRC03392_epDescMandatory.SimpleDescriptor.EndPoint, 
                              DRC03392_SEND_LEVEL_EVT, 0x00);        
                Dimmer((byte)(((uint16)DRC03392_DimBright_Value * 100) / MAX_LIGHT_VALUE ));
            }
        }
    }
    
    if ( keys & EVAL_SW4 )
    {        
    }
    if ( keys & EVAL_SW5 )
    {
        // Initiate an End Device Bind Request for the mandatory endpoint
        zdp_sendEDBindReq( DRC03392_epDescMandatory.SimpleDescriptor.EndPoint );
    }
  }
}

/*********************************************************************
 * DEVICE DESCRIPTION CALLBACKS
 */

/* --------------------------- RcvSET ------------------------- */


/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      DRC03392_ChangeState
 *
 * @brief   Changed the On Off state then generates a send event.
 * 
 * NOTE:  This is the function to call when hardware detects a button
 *        press and the state of the switch is known.  If you want
 *        to act as a toggle call DRC03392_ToggleState.
 *
 * @param   Attrib - OnOffSRC_Off or OnOffSRC_On.
 *
 * @return  none
 */
void DRC03392_ChangeState( byte State)
{
  DRC03392_state = State;
  apf_sendAppMsg( DRC03392_epDescMandatory.SimpleDescriptor.EndPoint, 
                                  DRC03392_SEND_STATE_EVT, 0x00);
}
 
/*********************************************************************
 * @fn      DRC03392_ToggleState
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
void DRC03392_ToggleState( void )
{
  if ( DRC03392_state == OnOffDRC_OFF )
  {
    Dimmer(DRC03392_DimBright_Value);
    LedOnOff(LED3,LED_ON);
    DRC03392_state = OnOffDRC_ON;
  }
  else
  {
      Dimmer(0);
    LedOnOff(LED3,LED_OFF);
    DRC03392_state = OnOffDRC_OFF;
  }
    
  apf_sendAppMsg( DRC03392_epDescMandatory.SimpleDescriptor.EndPoint, 
                                  DRC03392_SEND_STATE_EVT, 0x00);
}

/*********************************************************************
 * @fn      DRC03392_DimBright_DRC
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
void DRC03392_DimBright_DRC( uint16 DimBright_AttribId, byte DimBright_Steps )
{

  DRC03392_DimBright_Attrib = DimBright_AttribId;
  DRC03392_DimBright_Steps = DimBright_Steps;

  apf_sendAppMsg( DRC03392_epDescMandatory.SimpleDescriptor.EndPoint, 
                                  DRC03392_SEND_LEVEL_EVT, 0x00);
}

/*********************************************************************
 * @fn      DRC03392_TogglePreset
 *
 * @brief   Toggle the store/recall Preset state then generates 
 *          a send event.
 *
 * NOTE:  This is the function to call when hardware detects a button
 *        press and you want to act like a toggle switch.
 *
 * @param   none
 *
 * @return  none
 */
void DRC03392_TogglePreset( void )
{

  if ( Is_PresetDRC_Preset(DRC03392_Preset))
    DRC03392_Preset = PresetDRC_StorePreset_ID;
  else
    DRC03392_Preset = PresetDRC_Preset_ID;
    
  apf_sendAppMsg( DRC03392_epDescMandatory.SimpleDescriptor.EndPoint, 
                                  DRC03392_SEND_PRESET_EVT, 0x00);
}

/*********************************************************************
*********************************************************************/
