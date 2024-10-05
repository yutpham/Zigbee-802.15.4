/*********************************************************************
  Filename:       DLC03395_App.c
  Revised:        $Date: 2004/12/30 17:08:44 $
  Revision:       $Revision: 1.12 $

  Description:    
				  Dimmer Load Controller - Application

          ZigBee Document 03395
          
          This application is a sample implementation of the 
          Home Controls - Lighting Profile's Dimmer Load Controller
          device.  This device is a dimming light and can be 
          controlled by a dimming switch (DRC).
          
          This implementation is split between 2 files: 
          DLC03395_App.c and DLC03395_Device.c.  DLC03395_App.c is the
          controlling application (which contains the task), while 
          DLC03395_Device.c handles building/parsing of 
          incoming/outgoing messages.
          
          Optional messages (input and output) are conditionally 
          compiled with DLC03395_OPTIONALS.  The incoming optional
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

#define MIN_LIGHT_VALUE    0
#define MAX_LIGHT_VALUE  255

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
/* Destination Address */
AF_ADDR DLC03395_DstAddr;

/* Network State */
UINT8 DLC03395_NwkState;

/* OnOff State of the device */
byte DLC03395_state;

/* Current light level */
byte DimBrightDRC_Light_value;     
/* Current DimBrightDRC */
uint16 DimBrightDRC_AttribId;

/* The minimun light level limit. If LSM goes below this, light turns ON */
#define DLC03395_MIN_LIGHT_LIMIT 600.0

/* PresetDRC stored values */
byte storedState;
byte stored_Light_value;
uint16 stored_AttribId;

/*********************************************************************
 * These variables are used to store the information of the current state.
 * This information needs to be memorized for GETwithACK and GETwithRESPONSE
 */

/* Mandatory */
byte saved_OnOffDRC_OnOff;          
byte saved_DimBrightDRC_DimBright;  
byte saved_PresetDRC_StorePreset;   
byte saved_PresetDRC_Preset;        

const ENDPOINT_DESTCRIPTOR DLC03395_epDescMandatory = 
{
    { DLC03395_APP_ENDPOINT_MANDATORY,  
      CHL_PROFILE_ID, 
      DLC03395_DEVICE_APPDEVID,
      DLC03395_DEVICE_APPDEVVER, 
      APPFLAG_NONE,
      DLC03395_MANDATORY_IN_CLUSTERS, 
      (byte*)DLC03395_MandatoryInClusters,
      DLC03395_MANDATORY_OUT_CLUSTERS, 
      (byte*)NULL 
    },
    DLC03395_ProcessEvent,
    DLC03395_Init   
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
void DLC03395_HandleKeys( byte shift, byte keys );

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      DLC03395_Init
 *
 * @brief   Initialization function for the DLC03395 App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notification ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be 
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void DLC03395_Init(void* arg1, void* arg2)
{
  /* Initialize taskID and NetworkState */
  DLC03395_NwkState = DEV_INIT;

  /* Initialize current state of the device */
  DLC03395_state = OnOffDRC_OFF;
  
  /* Initialize DimBrightDRC attributes */
  DimBrightDRC_Light_value = MIN_LIGHT_VALUE;
  DimBrightDRC_AttribId = DimBrightDRC_BRIGHT_ID;
  
  /* Initialize PresetDRC attribites */
  storedState = OnOffDRC_OFF;
  stored_Light_value = MIN_LIGHT_VALUE;
  stored_AttribId = DimBrightDRC_DIM_ID;

  /* Initialize Endpoint */
  DLC03395_DstAddr.AddrMode = 0;
  DLC03395_DstAddr.EP       = 0;
  DLC03395_DstAddr.Addr.Short= INVALID_SHORT_ADDR;
  
  // Initialize the device description
  DLC03395_InitVars();

  // Initialize stored information
  DLC03395_InitSavedInfo();

  // Register for all key events - This app will handle all key events
  RegisterEP( DLC03395_epDescMandatory.SimpleDescriptor.EndPoint );
 
}

/*********************************************************************
 * @fn      DLC03395_ProcessEvent
 *
 * @brief   DLC03395 Application Task event processor.  This function 
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
void DLC03395_ProcessEvent( void* par_pMsg, void* arg2 )
{
    ZAPP_MSG*       pMsg;
    AF_IMCOM_FRM*   pAfFrm;
    BYTE*           pTemp;            
    byte            value;
    byte            ledPct;

    pMsg = par_pMsg;
    switch(pMsg->MsgType)
    {
        /*Process general event */
    case ZDO_DEVICE_STARTED:
    	break;
    case APF_DATA_INDICATION:
        pAfFrm = pMsg->Data;
        if (HI_UINT8(pAfFrm->Cmd[0]) == AF_FRM_MSG)
            // Process the incoming message
            DLC03395_ProcessMSGCmd( pAfFrm );
        else
        {
            DLC03395_ProcessKVPCmd( pAfFrm);
        }

        apf_releaseInComFrm(pAfFrm);
    	break;
    case APF_DATA_CONFIRM:
        break;
    case APF_KEYS:
        TOGGLE_RLED();
        pTemp = &pMsg->Data;
        DLC03395_HandleKeys(pTemp[0] , pTemp[1] );
        break;

        /*Application definition msg
        */
    case DLC03395_STATE_CHANGED_EVT:
        // This is where we calculate the wanted light level and turn on the lamp.
        // Are we in an ON state
        if ( DLC03395_state == OnOffDRC_ON )
        {
            LedOnOff( LED3, LED_ON );
            value = DimBrightDRC_Light_value;
            ledPct = (byte)(((uint16)value * 100) / MAX_LIGHT_VALUE );
            Dimmer( ledPct );      
        }
        else
        {
            Dimmer( 0 );
            LedOnOff( LED3, LED_OFF );
        }
        break;
    default:
        break;
    }       
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      DLC03395_HandleKeys
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
void DLC03395_HandleKeys( byte shift, byte keys )
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
        zdp_sendEDBindReq( DLC03395_epDescMandatory.SimpleDescriptor.EndPoint );
    }
  }
}

/*********************************************************************
 * DEVICE DESCRIPTION CALLBACKS
 */

/*---------------------------- RcvSET -------------------------------*/

/*********************************************************************
 * @fn      DLC03395_RcvSET_OnOffDRC
 *
 * @brief   Processes a SET for the OnOffDRC command.
 *
 * @param   endPoint - Intended endpoint
 * @param   Attrib - OnOffDRC_Off or OnOffDRC_On.
 *
 * @return  Transaction Codes:
 *            SUCCESS, error or application code
 */
byte DLC03395_RcvSET_OnOffDRC( byte endPoint, 
                               uint16 AttribId,
                               byte State)
{
  // Put code to process this command HERE.
 
  /* Save this for GET */
  saved_OnOffDRC_OnOff = State; 
  if (State == OnOffDRC_ON)
  {
    DLC03395_state = OnOffDRC_ON;    
  }
  else if (State == OnOffDRC_OFF)
  {
    DLC03395_state = OnOffDRC_OFF;
  }
  else if (State == OnOffDRC_TOGGLE)
  {
    // Toggle, if current state is ON --> OFF, OFF --> ON
    if (DLC03395_state == OnOffDRC_ON)
      DLC03395_state = OnOffDRC_OFF;
    else
      DLC03395_state = OnOffDRC_ON;
  }

  apf_sendAppMsg( DLC03395_APP_ENDPOINT_MANDATORY, DLC03395_STATE_CHANGED_EVT, 0x00);
  
  return ( SUCCESS );
}

/*********************************************************************
 * @fn      DLC03395_RcvSET_DimBrightDRC
 *
 * @brief   Processes a SET for the DimBrightDRC command.
 *
 * @param   endPoint - Intended endpoint
 * @param   Attrib - DimBrightDRC_Dim or DimBrightDRC_Bright.
 * @param   level - change % of full on.
 *
 * @return  Transaction Codes:
 *            SUCCESS, error or application code
 */
byte DLC03395_RcvSET_DimBrightDRC( byte endPoint, 
                                   uint16 AttribId,
                                   byte Steps )
{
  int16 newLevel;

  if ( Steps == 0x00 )
    // Force to minimum
    newLevel = MIN_LIGHT_VALUE - 1;
  else if ( Steps == 0xFF )
    // Force to maximum
    newLevel = MAX_LIGHT_VALUE + 1;
  else
    newLevel = DimBrightDRC_Light_value;


  /* Save this for GET */
  saved_DimBrightDRC_DimBright = Steps;

  if ( Is_DimBrightDRC_DIM(AttribId) )
    // Reduce the current level according to Steps
    newLevel -= Steps;
  else if ( Is_DimBrightDRC_BRIGHT(AttribId))
    // Increase the current level according to Steps
    newLevel += Steps;
  else
    return ( AF_ERR_UNSUPPORTED_ATTRIB );
  
  // Keep final value within limits
  if ( newLevel < MIN_LIGHT_VALUE )
    DimBrightDRC_Light_value = MIN_LIGHT_VALUE;
  else if ( newLevel > MAX_LIGHT_VALUE )
    DimBrightDRC_Light_value = MAX_LIGHT_VALUE;
  else
    DimBrightDRC_Light_value = (byte)newLevel;

  // Save the last action GETwithAck
  DimBrightDRC_AttribId = AttribId;
      
  apf_sendAppMsg( DLC03395_APP_ENDPOINT_MANDATORY, DLC03395_STATE_CHANGED_EVT, 0x00);
    
  return ( SUCCESS );
}

/*********************************************************************
 * @fn      DLC03395_RcvSET_PresetDRC
 *
 * @brief   Processes a SET for the PresetDRC command.
 *
 * @param   endPoint - Intended endpoint
 * @param   Attrib - PresetDRC_StorePreset or PresetDRC_Preset.
 *
 * @return  Transaction Codes:
 *            SUCCESS, error or application code
 */
byte DLC03395_RcvSET_PresetDRC( byte endPoint, 
                                uint16 AttribId,
                                byte Percent)
{
  byte status = SUCCESS;
  
  // Put code to process this command HERE.
  if ( AttribId == PresetDRC_StorePreset_ID )
  {
    // Save off the settings
    storedState = DLC03395_state;
    stored_Light_value = DimBrightDRC_Light_value;
    stored_AttribId = DimBrightDRC_AttribId;

    // Save the value for GET
    saved_PresetDRC_StorePreset = TRUE;
  }
  //else if ( AttribId == PresetDRC_Preset )
  else if (Is_PresetDRC_Preset(AttribId))
  {
    // Use the stored settings
    DLC03395_state = storedState;
    DimBrightDRC_Light_value = stored_Light_value;
    DimBrightDRC_AttribId = stored_AttribId;
    
    // Save the value for GET
    saved_PresetDRC_StorePreset = FALSE;
    saved_PresetDRC_Preset = Percent;

    apf_sendAppMsg( DLC03395_APP_ENDPOINT_MANDATORY, DLC03395_STATE_CHANGED_EVT, 0x00);
  }
  else
    status = AF_ERR_UNSUPPORTED_ATTRIB;

  return ( status );
}




/*---------------------------- RcvGET -------------------------------*/

/*********************************************************************
 * @fn      DLC03395_RcvGET_OnOffDRC
 *
 * @brief
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 *
 */
void DLC03395_RcvGET_OnOffDRC ( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm )
{
  byte ErrorCode       = SUCCESS;
  byte *Data           = (byte*)NULL;
  byte DataLength      = 0;
  byte AttribDataType  = AF_ATT_NO_DATA;


  switch (pKvp->AttrId)
  {
    case OnOffDRC_OnOff:
      Data = &saved_OnOffDRC_OnOff;
      DataLength = sizeof (saved_OnOffDRC_OnOff);
      AttribDataType = AF_ATT_UINT8;
      break;

    default:
      ErrorCode = AF_ERR_UNSUPPORTED_ATTRIB;
      break; 
  }
  // Send out the response
  apf_sendSingleKVP(pKvp->TransSeqNumber,AF_CMD_GET_RSP,AttribDataType,pKvp->AttrId,
                    ErrorCode, Data,DataLength, &pAfFrm->SrcAddr, pAfFrm->ClusterId,pAfFrm->DstEp,0x00);

}

/*********************************************************************
 * @fn      DLC03395_RcvGET_DimBrightDRC
 *
 * @brief
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 *
 */
void DLC03395_RcvGET_DimBrightDRC ( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm )
{ 
  byte ErrorCode         = SUCCESS;
  byte *Data             = (byte*)NULL;
  byte DataLength        = 0;
  byte AttribDataType    = AF_ATT_NO_DATA;

  switch (pKvp->AttrId&0xFF00)
  {
    case DimBrightDRC_DIM_ID:
    case DimBrightDRC_BRIGHT_ID:
      Data = &saved_DimBrightDRC_DimBright;
      DataLength = sizeof (saved_DimBrightDRC_DimBright);
      AttribDataType = AF_ATT_UINT8;
      break;

    default:
      ErrorCode = AF_ERR_UNSUPPORTED_ATTRIB;
      break;
  }

  // Send out the response                        
                        
  apf_sendSingleKVP(pKvp->TransSeqNumber,AF_CMD_GET_RSP,AttribDataType,pKvp->AttrId,
                    ErrorCode, Data,DataLength, &pAfFrm->SrcAddr, pAfFrm->ClusterId,pAfFrm->DstEp,0x00);                                      
}


/*********************************************************************
 * @fn      DLC03395_RcvGET_PresetDRC
 *
 * @brief
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 *
 */
void DLC03395_RcvGET_PresetDRC ( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm )
{
 
  byte ErrorCode        = SUCCESS;
  byte *Data            = (byte*)NULL;
  byte DataLength       = 0;
  byte AttribDataType   = AF_ATT_NO_DATA;
  
  switch (pKvp->AttrId&0xFF00)
  {
    case PresetDRC_StorePreset_ID:
      Data = &saved_PresetDRC_StorePreset;
      DataLength = sizeof (saved_PresetDRC_StorePreset);
      AttribDataType = AF_ATT_UINT8;
      break;
    
    case PresetDRC_Preset_ID:
      Data = &saved_PresetDRC_Preset;
      DataLength = sizeof (saved_PresetDRC_Preset);
      AttribDataType = AF_ATT_UINT8;
      break;

    default:
      ErrorCode = AF_ERR_UNSUPPORTED_ATTRIB;
      break;
  }
  // Send out the response                      
  apf_sendSingleKVP(pKvp->TransSeqNumber,AF_CMD_GET_RSP,AttribDataType,pKvp->AttrId,
                    ErrorCode, Data,DataLength, &pAfFrm->SrcAddr, pAfFrm->ClusterId,pAfFrm->DstEp,0x00);                                      
}

/* -------------------------- SET commands -------------------------*/

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      DLC03395_InitSavedInfo
 */
void DLC03395_InitSavedInfo()
{

  /* Mandatory */
  saved_OnOffDRC_OnOff                     = NULL;                        
  saved_DimBrightDRC_DimBright             = NULL;             
  saved_PresetDRC_StorePreset              = NULL;   
  saved_PresetDRC_Preset                   = NULL;          
}

/*********************************************************************
*********************************************************************/
