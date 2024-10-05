#ifndef SRC03391_DEVICE_H
#define SRC03391_DEVICE_H

/*********************************************************************
    Filename:       SRC03391_Device.h
    Revised:        $Date: 2004/12/30 16:28:14 $
    Revision:       $Revision: 1.3 $
    
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
#define SRC03391_DEVICE_APPDEVID          0xFFFE
#define SRC03391_DEVICE_APPDEVVER         0x00

#define SRC03391_MANDATORY_IN_CLUSTERS       0
#define SRC03391_MANDATORY_OUT_CLUSTERS      1
extern const byte SRC03391_MandatoryOutClusters[];


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GENERAL FUNCTIONS
 */

/*
 * Device Description initialization
 */
extern void SRC03391_InitVars( void );

/*********************************************************************
 * @fn      SRC03391_ProcessKVPCmd
 *
 * @brief   Processes the incoming message for this device
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
extern void SRC03391_ProcessKVPCmd( AF_IMCOM_FRM*   pAfFrm );

/*********************************************************************
 * @fn      SRC03391_ProcessMSGCmd
 *
 * @brief   Processes the incoming message for this device
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
extern void SRC03391_ProcessMSGCmd( AF_IMCOM_FRM*   pAfFrm );

/*********************************************************************
 * DEVICE DESCRIPTION CALLBACK FUNCTIONS
 *   These functions MUST be implemented in the application
 */


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
extern byte SRC03391_Set_OnOffSRC(AF_ADDR*   pDstAddr, 
                            ENDPOINT_DESTCRIPTOR *epDesc,
                         byte State);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SRC03391_DEVICE_H */