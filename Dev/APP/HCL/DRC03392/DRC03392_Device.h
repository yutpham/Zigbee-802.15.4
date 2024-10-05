#ifndef DRC03392_DEVICE_H
#define DRC03392_DEVICE_H

/*********************************************************************
    Filename:       DRC03392_Device.h
    Revised:        $Date: 2004/12/30 17:02:59 $
    Revision:       $Revision: 1.2 $
    
    Description:    
    
				  Dimmer Remote Control - Device Description

          ZigBee Document 03392r0ZB
            
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
 
/*********************************************************************
 * CONSTANTS
 */
#define DRC03392_DEVICE_APPDEVID            0xFFFD
#define DRC03392_DEVICE_APPDEVVER           0x00

#define DRC03392_MANDATORY_IN_CLUSTERS      0
#define DRC03392_MANDATORY_OUT_CLUSTERS     3
extern const byte DRC03392_MandatoryOutClusters[];


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GENERAL FUNCTIONS
 */

/*
 * Device Description initialization
 */
extern void DRC03392_InitVars( void );

/*********************************************************************
 * @fn      DRC03392_ProcessKVPCmd
 *
 * @brief   Processes the incoming message for this device
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
extern void DRC03392_ProcessKVPCmd( AF_IMCOM_FRM*   pAfFrm );


/*********************************************************************
 * @fn      DLC03395_ProcessMSGCmd
 *
 * @brief   Processes the incoming message for this device
 *
 * @param   pkt - incoming packet
 *
 * @return  none
 */
extern void DRC03392_ProcessMSGCmd( AF_IMCOM_FRM*   pAfFrm );

/*********************************************************************
 * DEVICE DESCRIPTION CALLBACK FUNCTIONS
 *   These functions MUST be implemented in the application
 */

/*********************************************************************
 * SEND MESSAGE FUNCTIONS
 */

/*********************************************************************
 * @fn      DRC03392_Set_OnOff
 *
 * @brief   Sends a SET command for OnOffDRC Cluster.
 *
 * @param   dstAddr - NULL or DSTINDIRECT, if indirect
 * @param   epDesc - pointer to the originating endpoint descriptor
 * @param   Attrib - OnOffDRC_Off or OnOffDRC_On.
 *
 * @return  0 - Sent it on its way, !0 if error
 */
extern byte DRC03392_Set_OnOff( AF_ADDR*   pDstAddr, 
                                ENDPOINT_DESTCRIPTOR *epDesc, 
                                uint16 Attrib,
                                byte State);

/*********************************************************************
 * @fn      DRC03392_Set_DimBright
 *
 * @brief   Sends a SET command for DimBright Cluster.
 *
 * @param   dstAddr - NULL or DSTINDIRECT, if indirect
 * @param   epDesc - pointer to the originating endpoint descriptor
 * @param   Attrib - DimBrightDRC_Dim or DimBrightDRC_Bright
 * @param   percent - 0 -100 (% of full on)
 *
 * @return  0 - Sent it on its way, !0 if error
 */
extern byte DRC03392_Set_DimBright( AF_ADDR*   pDstAddr, 
                                    ENDPOINT_DESTCRIPTOR *epDesc, 
                                    uint16 Attrib, byte Steps );

/*********************************************************************
 * @fn      DRC03392_Set_Preset
 *
 * @brief   Sends a SET command for Preset Cluster.
 *
 * @param   dstAddr - NULL or DSTINDIRECT, if indirect
 * @param   epDesc - pointer to the originating endpoint descriptor
 * @param   Attrib - PresetDRC_StorePreset or PresetDRC_Preset
 *
 * @return  0 - Sent it on its way, !0 if error
 */
extern byte DRC03392_Set_Preset( AF_ADDR*   pDstAddr, 
                                 ENDPOINT_DESTCRIPTOR *epDesc, 
                                 uint16 Attrib );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* DRC03392_DEVICE_H */