#ifndef DLC03395_DEVICE_H
#define DLC03395_DEVICE_H

/*********************************************************************
    Filename:       DLC03395_Device.h
    Revised:        $Date: 2004/12/30 17:08:44 $
    Revision:       $Revision: 1.2 $
    
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

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include "HomeControlLighting.h"
 
/*********************************************************************
 * CONSTANTS
 */
#define DLC03395_DEVICE_APPDEVID          0xFFFA
#define DLC03395_DEVICE_APPDEVVER         0x00

#define DLC03395_MANDATORY_IN_CLUSTERS        3
extern const byte DLC03395_MandatoryInClusters[];
#define DLC03395_MANDATORY_OUT_CLUSTERS       0

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GENERAL FUNCTIONS
 */

/*
 * Device Description initialization
 */
extern void DLC03395_InitVars( void );

/*********************************************************************
 * @fn      DLC03395_ProcessKVPCmd
 *
 * @brief   Processes the incoming message for this device
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
extern void DLC03395_ProcessKVPCmd( AF_IMCOM_FRM*   pAfFrm );

/*********************************************************************
 * @fn      DLC03395_ProcessMSGCmd
 *
 * @brief   Processes the incoming message for this device
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
extern void DLC03395_ProcessMSGCmd( AF_IMCOM_FRM*   pAfFrm );

/*********************************************************************
 * DEVICE DESCRIPTION CALLBACK FUNCTIONS
 *   These functions MUST be implemented in the application
 */

/*********************************************************************
 * @fn      DLC03395_RcvSET_OnOffDRC
 *
 * @brief   Processes a SET for the OnOffDRC command.
 *
 * @param   epInt - Intended endpoint/interface
 * @param   Attrib - OnOffDRC_Off or OnOffDRC_On.
 *
 * @return  Transaction Codes:
 *            TRANSCODE_OK, error or application code
 */
extern byte DLC03395_RcvSET_OnOffDRC( byte endPoint, 
                                      uint16 AttribId,
                                      byte State);

/*********************************************************************
 * @fn      DLC03395_RcvSET_DimBrightDRC
 *
 * @brief   Processes a SET for the DimBrightDRC command.
 *
 * @param   epInt - Intended endpoint/interface
 * @param   Attrib - DimBrightDRC_Dim or DimBrightDRC_Bright.
 * @param   level - change % of full on.
 *
 * @return  Transaction Codes:
 *            TRANSCODE_OK, error or application code
 */
extern byte DLC03395_RcvSET_DimBrightDRC( byte endPoint, 
                                   uint16 AttribId, byte Steps );

/*********************************************************************
 * @fn      DLC03395_RcvSET_PresetDRC
 *
 * @brief   Processes a SET for the PresetDRC command.
 *
 * @param   epInt - Intended endpoint/interface
 * @param   Attrib - PresetDRC_StorePreset or PresetDRC_Preset.
 *
 * @return  Transaction Codes:
 *            TRANSCODE_OK, error or application code
 */
extern byte DLC03395_RcvSET_PresetDRC( byte endPoint, 
                                       uint16 AttribId, byte percent );


/*********************************************************************
 * @fn      DLC03395_RevGET_OnOffDRC
 */
extern void DLC03395_RcvGET_OnOffDRC( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm);

/*********************************************************************
 * @fn      DLC03395_RcvGET_DimBrightDRC
 */
extern void DLC03395_RcvGET_DimBrightDRC( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm);

/*********************************************************************
 * @fn      DLC03395_RcvGET_PresetDRC
 */
extern void DLC03395_RcvGET_PresetDRC( AF_KVP*  pKvp, AF_IMCOM_FRM*   pAfFrm);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* DLC03395_DEVICE_H */