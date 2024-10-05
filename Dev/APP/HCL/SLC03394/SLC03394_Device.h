#ifndef SLC03394_DEVICE_H
#define SLC03394_DEVICE_H

/*********************************************************************
    Filename:       SLC03394_Device.h
    Revised:        $Date: 2005/04/13 15:41:59 $
    Revision:       $Revision: 1.3 $
    
    Description:    
    
				  Switching Load Control - Device Description

          ZigBee Document 03394r0ZB
            
    Notes:  

    Copyright (c) 2005 by Figure 8 Wireless, Inc., All Rights Reserved.
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
#define SLC03394_DEVICE_APPDEVID          0xFFFB
#define SLC03394_DEVICE_APPDEVVER         0x00

#define SLC03394_MANDATORY_IN_CLUSTERS       1
#define SLC03394_MANDATORY_OUT_CLUSTERS      0
extern const byte SLC03394_MandatoryInClusters[];

#if defined( SLC03394_OPTIONALS )
  #define SLC03394_OPTIONAL_IN_CLUSTERS      3
  extern const byte SLC03394_OptionalInClusters[];
  #define SLC03394_OPTIONAL_OUT_CLUSTERS      1
  extern const byte SLC03394_OptionalOutClusters[];
#endif  // SLC03394_OPTIONALS

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GENERAL FUNCTIONS
 */

/*
 * Device Description initialization
 */
extern void SLC03394_InitVars( void );

/*********************************************************************
 * @fn      SLC03394_ProcessKVPCmd
 *
 * @brief   Processes the incoming message for this device
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
extern void SLC03394_ProcessKVPCmd( AF_IMCOM_FRM*   pAfFrm );

/*********************************************************************
 * @fn      SLC03394_ProcessKVPCmd
 *
 * @brief   Processes the incoming message for this device
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
extern void SLC03394_ProcessMSGCmd( AF_IMCOM_FRM*   pAfFrm );

/*********************************************************************
 * DEVICE DESCRIPTION CALLBACK FUNCTIONS
 *   These functions MUST be implemented in the application
 */

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
extern byte SLC03394_RcvSET_OnOffSRC( byte endPoint, 
                                      uint16 AttribId,
                                      byte State);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SLC03394_DEVICE_H */