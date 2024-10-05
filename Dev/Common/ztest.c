//*******************************************************************************/
//Module: Common                                                               */   
//File Name: ztest.c                                                           */
//Description: zigbee test scenario file                                       */
//  -------------------------------------------------------------              */
//Revision History:                                                            */
//Date						 Programmer		Details                            */
//11/15/2006                                                                   */
//  -------------------------------------------------------------              */
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
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
#ifdef ZIGBEE_TEST
#if     ZIGBEE_DEVICE_FFD == TRUE  
void test_coord()
{
	/*********** Variable declaration ***********/
    volatile BOOL       gotAssociateRequest;
    volatile WORD       myShortAddr;
    WORD                panId;          // The PAN ID
    #define PANID       0x2420
    #define COORD_SHORT 0x1975
    #define CHANNEL     19
    BYTE                pBeaconPayload[8];           // The beacon payload (used initialized)
    BYTE            setAttributeValue;
    BOOL            isCoordinator;
	/*********** Function body		 ***********/
	// Initialize variables
	isCoordinator = TRUE;
	gotAssociateRequest = FALSE;
    aExtendedAddress = 0x0000004722958919;
    myShortAddr = COORD_SHORT; 
    panId = PANID;

    // Reset the MAC layer
    mlmeResetRequest(TRUE);

    // Modify necessary PIB attributes
    setAttributeValue = TRUE;
    mlmeSetRequest(MAC_ASSOCIATION_PERMIT, &setAttributeValue);
    mlmeSetRequest(MAC_RX_ON_WHEN_IDLE, &setAttributeValue);
    mlmeSetRequest(MAC_SHORT_ADDRESS, (BYTE*) &myShortAddr);
    setAttributeValue = 0;
    mlmeSetRequest(MAC_BEACON_PAYLOAD_LENGTH, &setAttributeValue);
    mlmeSetRequest(MAC_BEACON_PAYLOAD, pBeaconPayload);

    // Start as a PAN coordinator of a beacon-enabled PAN, with superframe and beacon order 4
	mlmeStartRequest(panId, CHANNEL, 4, 4, TRUE, FALSE, FALSE, FALSE,0);

    /* Wait for the data from the device */
	while (TRUE);
} // End of functions
#endif

/******************************************************************************/
//@FunctionName:  test_IndirectPkt                   	  	                  */
//@Description:	 Test funstion for sending packet                             */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
#if     ZIGBEE_DEVICE_FFD == TRUE  
void test_IndirectPkt()
{
	/*********** Variable declaration ***********/
    P_MPDU_TX       pPkt;
    ADDRESS         Addr;
	/*********** Function body		 ***********/
    mlmeResetRequest(TRUE);
    indirectTx_init();

    pPkt = mem_getFreeTxPkt();
    pPkt->Mpdu.dstAddr.Extended = 0x001;
    pPkt->Mpdu.FrmControl.DestAddMode = AM_EXTENDED_64;
        
    //indirectTx_addPkt(pPkt);

    beacon_initBeacon();

    indirectTx_controlIndirectPkt(NULL,NULL);

//    pPkt1 = indirectTx_getPkt(&Addr,AM_EXTENDED_64);
    
//    indirectTx_addPkt(pPkt);
//    indirectTx_addPkt(pPkt);
//    indirectTx_addPkt(pPkt);
    indirectTx_addPkt(pPkt);

    Addr.Extended = 0x001;  

    indirectTx_getPkt(&Addr,AM_EXTENDED_64);

    indirectTx_removePkt(pPkt);


    indirectTx_addPkt(pPkt);
    indirectTx_addPkt(pPkt);

    indirectTx_removePkt(pPkt);
} // End of functions
#endif
/******************************************************************************/
//@FunctionName:   	test_formatBeacon            		  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
#if     ZIGBEE_DEVICE_FFD == TRUE  
void test_formatBeacon()
{
	/*********** Variable declaration ***********/
    P_MPDU_TX       pPkt;
    ADDRESS         Addr;
	/*********** Function body		 ***********/
    mlmeResetRequest(TRUE);
    indirectTx_init();
    Addr.Extended = 0x001;

    pPkt = mem_getFreeTxPkt();
    pPkt->Mpdu.dstAddr.Extended = 0x001;
    indirectTx_addPkt(pPkt);

    Addr.Extended = 0x003;

    pPkt = mem_getFreeTxPkt();
    pPkt->Mpdu.dstAddr.Extended = 0x003;
    indirectTx_addPkt(pPkt);
    
    // Start as a PAN coordinator of a beacon-enabled PAN, with superframe and beacon order 4
	mlmeStartRequest(0x00, CHANNEL, 4, 4, TRUE, FALSE, FALSE, FALSE,0);
} // End of functions
#endif
/******************************************************************************/
//@FunctionName:   test_transData										  	  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void test_transData()
{
	/*********** Variable declaration ***********/
    ADDRESS     srcAddr;
    ADDRESS     dstAddr;
    BYTE        Data[2];
	/*********** Function body		 ***********/
    mlmeResetRequest(TRUE);
    srcAddr.Short       = 0x111;
    dstAddr.Extended    = 0x222;
    Data[0]=2;Data[1]=5;
	mcpsDataRequest(0x02,0x2420, &srcAddr, 
        0x02,0x2420,&dstAddr,
        0x01,Data, 0x00, 0x01);
} // End of functions

/******************************************************************************/
//@FunctionName:  test_rx 										  	          */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void test_rx()
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    mlmeResetRequest(TRUE);
    mac_setTrxState(PHY_RX_ON);
    wait(65000);
    while (TRUE)
    {
        while (CCA_IS_ACTIVE())
            wait(650);

        TOGGLE_RLED();            
        wait(6500);
    }
} // End of functions



#if     ZIGBEE_DEVICE_FFD == TRUE  
/******************************************************************************/
//@FunctionName: test_nwkNT   										  	      */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
MLME_BEACON_NOTIFY_INDICATION       BeaconNotif;
void test_nwkNT()
{
	/*********** Variable declaration ***********/
    PENDING_ADDR_SPEC*      pPendingAddr;
    SUPER_FRAME*            pSuperFrm;     
    ADDRESS                 Addr;
    CAPACITY_INFO           Cap;
	/*********** Function body		 ***********/
    BeaconNotif.AddrList[0].Short = 0x01;
    pPendingAddr = &BeaconNotif.PendAddrSpec;
    pPendingAddr->NumofShortAddr = 1;
   
    BeaconNotif.PanDescriptor.CoordAddrMode         = AM_EXTENDED_64;
    BeaconNotif.PanDescriptor.CoordAddress.Extended = 0x02;
    BeaconNotif.PanDescriptor.CoordPANId            = 0x2420;
    BeaconNotif.PanDescriptor.LogicalChannel        = 0x13;
    pSuperFrm = &BeaconNotif.PanDescriptor.SuperframeSpec;
    pSuperFrm->AssoPermit       = 1;
    pSuperFrm->SuperFrmOrder    = 2;
    pSuperFrm->BeaconOrder      = 4;
    pSuperFrm->PanCoord         = TRUE;
        
    BeaconNotif.Payload.DeviceDepth = 13;
    BeaconNotif.Payload.RouterCapacity      = 1;
    BeaconNotif.Payload.EndDeviceCapacity   = 1;

    nwk_updateBeacon(&BeaconNotif);   

    nwk_updateBeacon(&BeaconNotif);   

    Addr.Extended   = 0x013;
    Cap.DeviceType  = ZIGBEE_ROUTER;
    Cap.RxOnWhenIdle= TRUE;
    Cap.PwrSrc      = TRUE;
    nwk_addNewChild(&Addr,0x025, Cap,FALSE,FALSE);

    Addr.Extended   = 0x14;
    nwk_addNewChild(&Addr,0x025, Cap,FALSE,FALSE);

    nwk_checkNwkExisted(0x2420);

    nwk_removeChild(&Addr);

    NwkInfo.NwkJoinedDeviveType = ZIGBEE_PAN_COOR;
    nwk_findRouterInNT(0x2420,0);



} // End of functions    


/******************************************************************************/
//@FunctionName:  test_sendData 										  	  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
extern UINT8    CurrAperTask;
void test_sendData()
{
	/*********** Variable declaration ***********/
    ADDRESS         aDesExAddr;
    CAPACITY_INFO   Cap;
	/*********** Function body		 ***********/
    NwkInfo.NwkState        = NWK_STATE_NORMAL_MODE;
    nwk_ib.nwkUseTreeRouting= TRUE;    
    aDesExAddr.Extended = 0x013;
    mlmeResetRequest(TRUE);
    aps_init();
    NwkInfo.pParent         = mem_getMem(sizeof(NWK_NEIGHBOR_NODE));
    NwkInfo.pParent->NetworkAddress = 0x01;
    NwkInfo.pParent->RxOnWhenIdle   = TRUE;
    nwk_neighborTable[0] = NwkInfo.pParent;
    CurrAperTask    = TASK_APER_NWK;
    /* Send message to the destination */
    //debugapp_sendData();
    nlmeJoinRequest(confPandId, FALSE, FALSE, confChannelList, confScanDuration,
                                !NodePowerDescriptor.CurrentPowerSource,
                                ((NodeDescriptor.CapabilityFlags&0x08) == 0x08), CAP_SECURITY_CAPABLE);	
} // End of functions


/******************************************************************************/
//@FunctionName:   test_mem										  	          */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void test_mem()
{
	/*********** Variable declaration ***********/
    void*   pMem;
	/*********** Function body		 ***********/
    pMem = mem_getMem(33);
    mem_freeMem(pMem);
} // End of functions
#endif

/******************************************************************************/
//@FunctionName:   test_scheduler										  	  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void test_scheduler()
{
	/*********** Variable declaration ***********/
    UINT8       Index;
	/*********** Function body		 ***********/
    schedul_init();
    schedul_addTask(test_scheduler,TASK_PER_MAC_TX,Index++);
    schedul_addTask(test_scheduler,TASK_PER_MAC_TX,Index++);
    schedul_addTask(test_scheduler,TASK_PER_MAC_TX,Index++);
    schedul_removeTask(TASK_PER_MAC_TX);
    schedul_addTask(test_scheduler,TASK_PER_MAC_TX,Index++);
    schedul_removeTask(TASK_PER_MAC_TX);
    schedul_removeTask(TASK_PER_MAC_TX);
    schedul_removeTask(TASK_PER_MAC_TX);
    schedul_removeTask(TASK_PER_MAC_TX);
} // End of functions


/******************************************************************************/
//@FunctionName:   test_timer    										  	  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void test_timer()
{
	/*********** Variable declaration ***********/
    OS_TMR*     pTestTimer;
    UINT8       nErr;
	/*********** Function body		 ***********/
    pTestTimer = OSTmrCreate(0,10,OS_TMR_OPT_ONE_SHOT,test_timer,NULL,NULL,&nErr);
    OSTmrStart(pTestTimer,&nErr);
    OSTmrStart(pTestTimer,&nErr);
} // End of functions

/******************************************************************************/
//@FunctionName:   test_indiData    										  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
NPDU  npdu;
MCPS_DATA_INDICATION indiData;
void test_indiData()
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    npdu.DesAddr = 0;
    npdu.FrmType = NWK_FRM_DATA;
    indiData.pMsdu = &npdu;
    mcpsDataIndication(&indiData);
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
extern UINT8    CurrAperTask;
void test_app()
{
	/*********** Variable declaration ***********/
    ADDRESS         aDesExAddr;
    CAPACITY_INFO   Cap;
	/*********** Function body		 ***********/
    NwkInfo.NwkState        = NWK_STATE_NORMAL_MODE;
    nwk_ib.nwkUseTreeRouting= TRUE;    
    aDesExAddr.Extended = 0x013;
    mlmeResetRequest(TRUE);
    aps_init();
    NwkInfo.pParent         = mem_getMem(sizeof(NWK_NEIGHBOR_NODE));
    NwkInfo.pParent->NetworkAddress = 0x01;
    NwkInfo.pParent->RxOnWhenIdle   = TRUE;
    nwk_neighborTable[0] = NwkInfo.pParent;
    CurrAperTask    = TASK_APER_NWK;
    /* Send message to the destination */
//#ifndef TYPE_COOR
    //debugapp_searchCoord(TRUE);
    zdp_NwkAddrRes(FALSE, 0x01, SUCCESS,&aExtendedAddress,NwkInfo.NwkAddr,
                               ZDP_ADDR_REQ_SINGLE, 0, 0,NULL,FALSE);
//#endif
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void test_addrMap()
{
	/*********** Variable declaration ***********/
    BOOL                Status;
    UINT16              Short;
    ADDR_MAP_ENTRY*     pMap;
	/*********** Function body		 ***********/
    aps_init();
    Short = 0x1325;
    Status = ib_addAddrEntry(Short,&aExtendedAddress);
    pMap   = ib_searchAddrEntry(Short,NULL);
    pMap   = ib_searchAddrEntry(INVALID_SHORT_ADDR,&aExtendedAddress);
    Short = 0x1327;
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void test_testBingdingTbl()
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
	/*********** Variable declaration ***********/
    BOOL                Status;
    UINT16              Short;
    BINDING_ENTRY*      pEntry;
	/*********** Function body		 ***********/
    aps_init();
    Short   = 0x1325;
    Status  = ib_addBindingEntry(Short,1,Short+1,2,2,&Short);
//    ib_restartIterativeBindingEntry();
    pEntry = ib_searchIterativeBindingEntry(Short,1,NULL);
    ib_removeBindingEntry(pEntry);
    Short = 0x1327;    
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void test_binding()
{
	/*********** Variable declaration ***********/
    QWORD   SrcAddr;
    QWORD   DesAddr;
	/*********** Function body		 ***********/
    SrcAddr = 0x13;
    DesAddr = 0x25;
    apsmeBindRequest(&SrcAddr,07,1,&DesAddr,25);
    apsmeBindRequest(&SrcAddr,07,3,&DesAddr,25);    
    apsmeBindRequest(&SrcAddr,07,5,&DesAddr,25);
    DesAddr = 0x26;
    apsmeBindRequest(&SrcAddr,07,5,&DesAddr,25);
    DesAddr = 0x25;
    apsmeUnbindRequest(&SrcAddr,07,3,&DesAddr,25);
    apsmeUnbindRequest(&SrcAddr,07,1,&DesAddr,25);
    DesAddr = 0x26;
    apsmeUnbindRequest(&SrcAddr,07,5,&DesAddr,25);
    DesAddr = 0x25;
    ib_searchBindingLink(&SrcAddr,07,5,&DesAddr,25);
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void test_indirectDataReq()
{
	/*********** Variable declaration ***********/
    AF_ADDR         aDesExAddr;
    ADDRESS         Addr;
    CAPACITY_INFO   Cap;
    BYTE            InClusteIdList[3];
    BYTE            OutClusteIdList[3];
	/*********** Function body		 ***********/
    NwkInfo.NwkState        = NWK_STATE_NORMAL_MODE;
    nwk_ib.nwkUseTreeRouting= TRUE;    
    mlmeResetRequest(TRUE);
    aps_init();
    NwkInfo.pParent         = mem_getMem(sizeof(NWK_NEIGHBOR_NODE));
    NwkInfo.pParent->NetworkAddress = 0x01;
    NwkInfo.pParent->RxOnWhenIdle   = TRUE;
    nwk_neighborTable[0] = NwkInfo.pParent;
    CurrAperTask    = TASK_APER_NWK;
    /* Send message to the destination */
//#ifndef TYPE_COOR
    aDesExAddr.Addr.Short   = 0x013;
    aDesExAddr.EP           = EP_ID_ZDO;
    aDesExAddr.AddrMode     = APS_ADDR_SHORT;
    InClusteIdList[0] = 30;
    InClusteIdList[1] = 31;
    InClusteIdList[2] = 32;
    OutClusteIdList[0] = 60;
    OutClusteIdList[1] = 61;
    OutClusteIdList[2] = 62;
    zdp_EndDeviceBindReq(&aDesExAddr, 0x01, 0x02,111,3,InClusteIdList,3,OutClusteIdList,FALSE);

    aDesExAddr.Addr.Short = 0x025;
    aDesExAddr.EP         = EP_ID_ZDO;
    aDesExAddr.AddrMode   = APS_ADDR_SHORT;
    InClusteIdList[0] = 60;
    InClusteIdList[1] = 71;
    InClusteIdList[2] = 82;
    OutClusteIdList[0] = 30;
    OutClusteIdList[1] = 71;
    OutClusteIdList[2] = 82;
    zdp_EndDeviceBindReq(&aDesExAddr, 0x01, 0x03,111,3,InClusteIdList,3,OutClusteIdList,FALSE);

    Addr.Short       = 0x013;
    apsdeDataRequest(APS_ADDR_INDIRECT,&Addr,0,111,30,0x02,8,&aExtendedAddress,0x00,FALSE,DEFAULT_RADIUS);
    
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
extern DATA_REC            ApsRecList[MAX_APS_DATA_QUEUE_SIZE];
extern DATA_QUEUE          ApsDataHandler;
void test_queueRec()
{
	/*********** Variable declaration ***********/
    UINT8   tHandle;
    BYTE    Temp[4];
	/*********** Function body		 ***********/
    /*init data queue*/
    dataqueue_init(&ApsDataHandler,ApsRecList,MAX_APS_DATA_QUEUE_SIZE);
    
    tHandle = dataqueue_getFreeSlot(&ApsDataHandler);                          
    dataqueue_addRec(&ApsDataHandler,Temp,4,tHandle,
                    76,77);

    tHandle = dataqueue_getFreeSlot(&ApsDataHandler);                          
    dataqueue_addRec(&ApsDataHandler,Temp,4,tHandle,
                    78,78);

    tHandle = dataqueue_getFreeSlot(&ApsDataHandler);                          
    dataqueue_addRec(&ApsDataHandler,Temp,4,tHandle,
                    79,79);

    tHandle = dataqueue_getFreeSlot(&ApsDataHandler);                          
    dataqueue_addRec(&ApsDataHandler,Temp,4,tHandle,
                    79,79);

    dataqueue_removeRec(&ApsDataHandler,7);
    dataqueue_removeRec(&ApsDataHandler,2);
    dataqueue_removeRec(&ApsDataHandler,1);
    dataqueue_removeRec(&ApsDataHandler,0);
    dataqueue_removeRec(&ApsDataHandler,3);
    dataqueue_removeRec(&ApsDataHandler,3);
} // End of functions


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
 
byte DLC03395_RcvSET_OnOffDRC( byte endPoint, 
                               uint16 AttribId,
                               byte State)
 {
     AttribId = mem_getMem(32);
 }
void test_KVP()
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    BYTE            Trans[6]={0x11,0x2e,0x11,0x00,0x00,0xff};
    AF_IMCOM_FRM*   pFrm;
    AF_KVP          Kvp;
    pFrm = mem_getMem(32);
    mem_cpy(&pFrm->Cmd,Trans,6);
    apf_getKVPTrans(pFrm,&Kvp,1);
    DLC03395_RcvSET_OnOffDRC(1,2,*(Kvp.pData));
} // End of functions

#endif //!ZIGBEE_TEST