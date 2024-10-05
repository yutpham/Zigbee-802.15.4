/*******************************************************************************/
//Module: 	                                                                   */   
//File Name: zigbee_app                                                        */
//Description: 	This file will implement a simple application on zigbee network
/*              - Scenario: 
                    + Endpoint device will search for Nwk-Address of a
                    a device with external address is defined in aExDesAddr
                    + After received the nwk address of the destination it will
                    send data to that node
                - To customize 
                    + aExDesAddr for the destination coordinator
                    + Change parameter of debugapp_sendData   for the data to send
                    + Change body of function debugapp_processReceivedData to process
                    data
                    + Change the data rate by changing DATA_SENDING_INTERVAL

NOTE:   Memory of the system is limited. Check the memory after you add any static
        variables.
        To check it, 
            - open the file zigbee.sym in the folder that contains makefile
            - Check the location of variable before __bss_end. The location
              must be smaller than 008010ff.        
*/
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//03/07/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef DEBUGAPP_H
#define DEBUGAPP_H
/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "types.h"
#include "schedul.h"
#include "app_framework.h"
#include "aps_ib.h"
#include "zdp.h"
#include "apf_format.h"
#include "aps.h"
#include "debugapp.h"
#include "hal_cc2420.h"
#include "zdo.h"
#include "ucos_ii.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/
#define         DATA_SENDING_INTERVAL                       10 /*Second*/
/************************************************************************/
/*					Type Definition										*/
/************************************************************************/
typedef enum
{
    APP_INIT            = 0x01,
    APP_SEARCH_ADDR,
    APP_SEND_DATA
}APP_STATE;
/************************************************************************/
/*                      Variables Declaration                           */
/************************************************************************/
UINT8       nTransSeq   = 0;
WORD        aDesShorAddr= INVALID_SHORT_ADDR;
UINT8       AppState    = APP_INIT;

/*Customizable variable: Change this value for the destination coord*/
QWORD       aExDesAddr    = 0x5555;


/* Debug application */
void    debugapp_task(void* arg1, void* arg2);
void    debugapp_init(void* arg1, void* arg2);
/*Endpoint descriptor*/

ENDPOINT_DESTCRIPTOR        DebugApp_Descriptor =
{
    {EP_ID_DEBUGAPP,0,0,0,0,0,0,0,0},       /* DEBUG_APP simple descriptors */
    debugapp_task,                           /* Task and init functions*/
    debugapp_init
};

/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/

/************************************************************************/
/*                      Function                                        */
/************************************************************************/


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description: This function will be call to process the received data that 
//              is sent by others DebugApp                                    */
//	                                                                          */    
//@Param        Note: buffer will be removed after calling of this function
//              To store data, copy it to other location
//	                                                                          */   
/******************************************************************************/
void    debugapp_processReceivedData(void*  pBuffer, UINT8 nLen)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    /*Change this to customize data processing*/    
    if (*(QWORD*)pBuffer == 0x3333)
        TOGGLE_OLED();   /*Just for demo: toggle led if it is addr of device*/
} // End of functions






/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	 This function will send data from endpoint the coordinators  */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    debugapp_sendData(void*  pBuffer, UINT8 nLen)
{
	/*********** Variable declaration ***********/
    AF_MSG      AfMsg;
    AF_FRM*     pAfFrm;
	/*********** Function body		 ***********/
    /*Init af msg*/
    AfMsg.DataLen           = nLen;
    AfMsg.TransSeqNumber    = nTransSeq++;
    AfMsg.pData             = pBuffer; /**/

    pAfFrm = apf_initFrm(AF_FRM_MSG);
    if (pAfFrm)
    {
        if (apf_addMsg(pAfFrm,&AfMsg))
        {
            /* Send message to the destination */
             apf_sendFrm(pAfFrm,APS_ADDR_SHORT,&aDesShorAddr,EP_ID_DEBUGAPP,0,EP_ID_DEBUGAPP,0x00);
        }
        apf_releaseFrm(pAfFrm);
    }
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    debugapp_createTimer(void* pTimer, void* arg2)
{
	/*********** Variable declaration ***********/  
    ZAPP_MSG        Msg;
	/*********** Function body		 ***********/
    Msg.EndPointId  = EP_ID_DEBUGAPP;
    Msg.MsgType     = DAPP_SEND_DATA;
    /*Send control the app*/
    mtimer_creatTmr(&Msg,(UINT32)DATA_SENDING_INTERVAL*1000,TRUE,TASK_APER_APP);
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    debugapp_searchCoord(BOOL bCreateTimer)
{
	/*********** Variable declaration ***********/
    ADDR_MAP_ENTRY*     pAddMap;
	/*********** Function body		 ***********/
    if (aDesShorAddr != INVALID_SHORT_ADDR)
        return;

    /*Search for coor.*/
    aDesShorAddr = nwk_getShortAddInNT(&aExDesAddr);
    if (aDesShorAddr != INVALID_SHORT_ADDR)
    {
        AppState = APP_SEND_DATA;
        return;
    }
    else /*Search in the APS*/
    {
        pAddMap = ib_searchAddrEntry(INVALID_SHORT_ADDR,&aExDesAddr);
        if (pAddMap)
        {
            TOGGLE_OLED();
            AppState        = APP_SEND_DATA;
            aDesShorAddr    = pAddMap->ShortAddr;
            return;
        }
    }

    /*Search addr*/
    AppState = APP_SEARCH_ADDR;
    zdp_NwkAddrReq(&aExDesAddr,ZDP_ADDR_REQ_SINGLE,1,1);

    if (bCreateTimer)
        debugapp_createTimer(NULL,NULL);             /*To check later*/
} // End of functions
/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	 This is the main function of Application Task                */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    debugapp_task(void* arg1, void* arg2)
{
	/*********** Variable declaration ***********/
    ZAPP_MSG*       pMsg;
    AF_IMCOM_FRM*   pAfFrm;
	/*********** Function body		 ***********/
    pMsg = (ZAPP_MSG*)arg1;

    switch(pMsg->MsgType)
    {
        /*Process general event */
    case ZDO_DEVICE_STARTED:
#ifdef Z_SOURCE
        debugapp_searchCoord(TRUE); /*Created periodical timer*/
#endif
    	break;
    case APF_DATA_INDICATION:
        pAfFrm = pMsg->Data;
        debugapp_processReceivedData(&pAfFrm->Cmd[1+1+1/*Header*/],
                                     &pAfFrm->Cmd[2/*Lenght of Msg*/]);
        apf_releaseInComFrm(pAfFrm);
    	break;
    case APF_DATA_CONFIRM:
        break;

        /*Application definition msg
        */
    case DAPP_SEND_DATA:       
        switch(AppState)
        {
        case APP_SEARCH_ADDR:
            debugapp_searchCoord(FALSE); /*Timer created*/
        	break;
        case APP_SEND_DATA:
            /*Do somthing*/

            /*Change this input parameter to customize data to send*/
            debugapp_sendData(&aExtendedAddress, sizeof(QWORD));
        	break;
        default:
            break;
        }            
        break;
    default:
        break;
    }       
} // End of functions


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	 This function is called to initialize application at the     */
//               first timer                                                  */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    debugapp_init(void* arg1, void* arg2)
{
    /* Add the EP */
    app_addEP(&DebugApp_Descriptor);

    /*Initialize of the application */
} // End of functions



#endif //DEBUGAPP_H
