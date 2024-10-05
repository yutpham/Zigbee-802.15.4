/*******************************************************************************/
//Module: APP/DebugApp                                                         */   
//File Name: zigbee_app                                                        */
//Description: 	This component implement the behavior of Application Task      */
//              Behavior of Application Task is Zigbee Profile - specific.     */
//              Change is component to adapted to the specific App.            */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//03/07/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

#ifndef ZIGBEE_APP_H
#define ZIGBEE_APP_H


/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "types.h"
#include "nwk.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/


/************************************************************************/
/*					Type Definition										*/
/************************************************************************/
/* General message for all application
    - Zigbee maintains the conformance of message type between APF, APS, ZDO and NWK
        + NWW message type starts from 0x01-0x1F
        + ZDO message type starts from 0x20-0x3F
        + APS message type starts from 0x40-0x4F
        + APF message type starts from 0x50-0x5F
        + From 0x60 is application definition types
*/
typedef enum
{
    DAPP_SEND_DATA = 0x60   
}DAPP_MSG_TYPE;


/*Endpoint id*/
#define     EP_ID_DEBUGAPP       0x01       /* Zigbee debug application */
/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
extern    WORD                                  AppPanID;
/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/


/************************************************************************/
/*                      Function                                        */
/************************************************************************/


/******************************************************************************/
//@FunctionName:   app_init  							  	                  */
//@Description:	 This function is called to initialize application at the     */
//               first time                                                   */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    debugapp_init(void* arg1, void* arg2);

#endif  //!ZIGBEE_APP_H
