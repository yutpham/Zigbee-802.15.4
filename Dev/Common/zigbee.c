/*******************************************************************************/
//Module: 	Common                                                             */   
//File Name: zigbee.c                                                          */
//Description: This component is the main() program to test the MAC layer      */
//           - zigbee_task is the task for NWK, APS, and ZDO                   */
//             Here are main characteristics of this task
/*                  - This is aperiodic task
                    - Message in queue contains the indication and confirms 
                    from MAC layer
                    - To inform the task in upper layer (Application layers) a
                    message will be send to their queues
                    - Application task call directly functions in the NWK, APS,
                    and ZDO layers
                NWM, APS, and ZDO have to take this mechanism into account to
                manage the coherence in their states.
              - app_task is the task for endpoint of zigbee. This task 
              will activate handler function of each registerd endpoint        */ 
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//03/06/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/


/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "mac_state.h"
#include "nwk.h"
#include "nwk_msg.h"
#include "app_framework.h"
#include "zdo.h"
#include "schedul.h"
#include "static_timer.h"
#include "phy_primitives.h"
#include "hal.h"
#include "utils.h"
#include "ucos_ii.h"
#include "nv.h"
#include "aps.h"
#include "mac_beacon.h"
#include "mac_tx.h"
#include "mac_state.h"
#include "nwk.h"
#include "nwk_msg.h"
#include "app_framework.h"
#include "zdo.h"
#include "schedul.h"
#include "static_timer.h"
#include "phy_primitives.h"
#include "hal.h"
#include "utils.h"
#include "ucos_ii.h"
#include "mac_types.h"
#include "mac_beacon.h"
#include "aps.h"
#include "nwk_common.h"
#include "aps_ib.h"
#include "zdp.h"
#include "data_queue.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/
#define             OS_GLOBALS                          
#define             STACK_SIZE_MAIN                            450    
/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
BYTE                AppStk[STACK_SIZE_MAIN];
/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/

/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/******************************************************************************/
//@FunctionName: putData     							  	                  */
//@Description:	Logging                                                       */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/

/*----------------------------------------------------------------------------
                                STUFF FUNCTION
 ----------------------------------------------------------------------------*/
UINT8  nCount =0;
#ifdef ZIGBEE_DEBUG
BYTE*   DataBuf;
void putData(BYTE* data, UINT8 size)
{
    mem_cpy(DataBuf+nCount,data, size);
    nCount+=size;
    if (nCount == 12)
    {
        ENABLE_GLOBAL_INT();
    }
}
#endif // _DEBUG
void recordByte(UINT8 value)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    nCount++;
    nv_write(0x0B+nCount,&value,1);
} // End of functions

#ifdef PROGRAMMED_EXT_ADDR
void programEEPROM()
{
    ADDRESS     addr;
    addr.Extended = PROGRAMMED_EXT_ADDR;
    nv_write(EXT_ADDR_ADDRESS,&addr,EXT_ADDR_LEN);
    while (TRUE)
        SET_RLED();
}
#endif
/******************************************************************************/
//@FunctionName: err									  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void err(ERROR   Type,
         P_FNC   FncLocation,
         BYTE*   pData,
         UINT8   nLength)
{
	/*********** Variable declaration ***********/
    UINT8       a = 1;
	/*********** Function body		 ***********/
    switch(Type)
    {
    case ERR_UNRECOVERABLE:
        /*Record the error*/
        while (a)    /*Blink red Led*/
        {
            wait(50000);
            TOGGLE_RLED();
        }
    	break;
    case ERR_DEBUG:
        break;
    default:
        break;
    }
} // End of functions

/*----------------------------------------------------------------------------
                                APP TASK
 ----------------------------------------------------------------------------*/

/******************************************************************************/
//@FunctionName: app_task    							  	                  */
//@Description:	 This is the main function of Application Task                */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    app_task(void* arg1, void* arg2)
{
	/*********** Variable declaration ***********/
    ZAPP_MSG*    pMsg;
	/*********** Function body		 ***********/
    pMsg = (ZAPP_MSG*)arg1;

    /*General handling*/
    if (pMsg->MsgType == APF_POLL_BOARD)
    {
        pollBoard();
        return;
    }
    else
    {
        /* Call appropriate end point */
        for (int i=1;i<ZIGBEE_NUM_EP;i++)
        {   
            if (EndpointDescriptors[i] &&
                (EndpointDescriptors[i]->SimpleDescriptor.EndPoint == pMsg->EndPointId))
                EndpointDescriptors[i]->pHandler(pMsg,NULL);	
        }
    }
} // End of functions

/******************************************************************************/
//@FunctionName:  app_init  							  	                  */
//@Description:	 This function is called to initialize application at the     */
//               first timer                                                  */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void    app_init()
{
	/*********** Variable declaration ***********/    
	/*********** Function body		 ***********/

    /* Initialize all the endpoint */
    for (int i=0;i<ZIGBEE_NUM_EP;i++)
        if (EndpointDescriptors[i])
            EndpointDescriptors[i]->pInit(NULL,NULL);

} // End of functions

/*----------------------------------------------------------------------------
                                ZIGBEE TASK
 ----------------------------------------------------------------------------*/

/******************************************************************************/
//@FunctionName:   zigbee_init							  	                  */
//@Description:	 This function will be call to first time initialization of   */
//               Zigbee                                                       */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void zigbee_init()
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/

#ifndef ZIGBEE_DEBUG
    /*NV initialization*/
    nv_read(EXT_ADDR_ADDRESS,&aExtendedAddress,EXT_ADDR_LEN);        
    if ( aExtendedAddress == 0xFFFFFFFFFFFFFFFF )  /*EEPROM is erased*/
        //err(ERR_UNRECOVERABLE,NULL,NULL,0);
        aExtendedAddress = 0x9999;          /*Default address*/
#endif
    /* Initialize NWK layer */
    nwk_init();    
   
    /* Initialize Mac layer */
    mac_init();

} // End of functions


/******************************************************************************/
//@FunctionName:  zigbee_task   						  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void  zigbee_task(BYTE*  par_pMsg, void* args)
{
	/*********** Variable declaration ***********/
    ZAPP_MSG*  pMsg;
	/*********** Function body		 ***********/

    /* Dispatch msg to specific handler
    */
    pMsg = par_pMsg;
    if (par_pMsg[0] < MSG_NWK)
        nwk_processNwkEvent(par_pMsg,NULL);
    else if (par_pMsg[0] < MSG_ZDO) 
        apf_dispatchEvent(par_pMsg);
    else if (par_pMsg[0] < MSG_APS) 
        nwk_processApsEvent(par_pMsg,NULL);
    
} // End of functions


/*----------------------------------------------------------------------------
                                MAIN
 ----------------------------------------------------------------------------*/

/******************************************************************************/
//@FunctionName:  main           						  	                  */
//@Description:	Startup sequence and main loop.                               */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void main(void) 
{       

#ifdef ZIGBEE_TEST
    UINT32           capRequired;
    UINT32          nOffset;
    UINT8           nTemp;
    UINT32          nBeaconDuration;
    ZAPP_MSG        Msg;
#endif

#ifdef PROGRAMMED_EXT_ADDR
    programEEPROM();
#endif

	/*Set stack for main program, Timer INT will override this area*/
    SP      = &AppStk[STACK_SIZE_MAIN - 1];
  
    OS_ENTER_CRITICAL();          

    OSTmr_Init();   /* initialize os. */
    OS_MemInit();

    schedul_init(); /* initialize scheduler */
    mem_init();     /* initialize memory partitions */
    mtimer_reset(); /* initialize msg timer*/

    zigbee_init();  /* initialize zigbee stack */           /*BUG, do not change the order of this initialization*/
    zigbee_InitEP();/* Initialize endpoint*/
#ifndef ZIGBEE_TEST
    app_init();     /* initialize app. */
#endif
    random_init();  /* initialize random generator */
    timer_init();   /* initialize timer */

    InitBoard();    /* Board initialization*/
    OS_EXIT_CRITICAL();    

#ifndef ZIGBEE_TEST

    while(TRUE)
    {
        /* run a-periodical task*/
        schedul_runAperTasks();
    }
 
#else
    
    WORD SrcAddr = 0x01a5;
    UINT8 SrcEndPoint= 0x1;
    UINT8 ClusterId = 0x100;
    WORD DesAddr = 0x0001;
    INT8 DesEndPoint = 0x02;
    BINDING_ENTRY*      pEntry;

    //Perform a binding request
    apsmeBindRequest(SrcAddr,SrcEndPoint, ClusterId, DesAddr,DesEndPoint);

    //Check if a binding record is added to the Binding List
    pEntry = ib_searchIterativeBindingEntry(SrcAddr,1,NULL);
        
#endif
	while(true);
} // main











































// /*Test for the MAC primitives */
// MAC_SCAN_RESULT ScanResult;
// mlmeScanRequest(ACTIVE_SCAN, 0x00080000, 6, ScanResult);
// 
// UINT32          SetRequestValue;
// SetRequestValue = TRUE;
// mlmeSetRequest(MAC_ASSOCIATION_PERMIT,&SetRequestValue)
// 
// 
// /*Test for the NETWORK primitive */
// UINT16 PanID = 0x1325;
// BOOL JoinAsRouter  = FALSE;
// BOOL RejoinNetwork = FALSE;
// DWORD ScanChannels = 0x00080000; /*channel 19*/
// UINT8 ScanDuration = 6;
// UINT8 PowerSource  = FALSE;
// BOOL RxOnWhenIdle  = TRUE;
// UINT8 MACSecurity  = FALSE;
// 
// 
// nlmeJoinRequest(PanID,JoinAsRouter,RejoinNetwork, ScanChannels,
//                 ScanDuration,PowerSource, RxOnWhenIdle, MACSecurity);
// 
// UINT16 DstAddr = 0x5555;
// BYTE NsduLength= 0x13;
// BYTE Nsdu = NULL;
// BYTE NsduHandle = 1;
// BYTE Radius = DEFAULT_RADIUS;
// BYTE DiscoverRoute = FALSE;
// BOOL SecurityEnable =FALSE;
// 
// nldeDataRequest(DstAddr, NsduLength,Nsdu, NsduHandle, Radius, DiscoverRoute, SecurityEnable);
// 
// 
// /* Test for the APP */
// 
// //apsmeBindRequest : This test demonstrate for Application support layer
// ,,,,
// WORD SrcAddr = 0x01a5;
// UINT8 SrcEndPoint= 0x1;
// UINT8 ClusterId = 0x100;
// WORD DesAddr = 0x0001;
// INT8 DesEndPoint = 0x02;
// apsmeBindRequest(SrcAddr,SrcEndPoint, ClusterId, DesAddr,DesEndPoint);
// 
// //apf_sendSingleMsg: This test demonstrate for Application Framework
// BYTE* pData = "Test data";
// UINT8 nDataLen = 9; //size of "Test data"
// UINT8 TransSeq  = 0x010;
// AF_ADDR DstAddr;
// DstAddr EP = 0;
// BYTE DstEndpoint = 0x01;
// BYTE ClusterId = 0x100;
// BYTE SrcEndpoint = 0x11;
// BYTE TxOptions = 0x00;
// 
// apf_sendSingleMsg(pData, nDataLen,TransSeq , &DstAddr, DstEndpoint, ClusterId, SrcEndpoint, TxOptions);





