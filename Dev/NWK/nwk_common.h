/*******************************************************************************/
//Module: 	NWK                                                                */   
//File Name: nwk_common.h                                                      */
//Description: 																   */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//06/14/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/
#ifndef NWK_COMMON_H
#define NWK_COMMON_H

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "mac_types.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/
/*	 Nwk header size           */
#define NWK_NPDU_HEADER_SIZE        8

/*--------------------------*/
/*	NETWORK LAYER CONSTANTS	*/
/*--------------------------*/
//SET: 1 - for Coor: need real time beacon to schedul beacon trans 
//0 -  for other, 
#define nwkcCoordinatorCapable 		1 		

#define nwkcDefaultSecurityLevel	0x00 
#define nwkcDiscoveryRetryLimit		0x03
#define nwkcMaxDepth				0x0f
#define nwkcMinHeaderOverhead		0x08
#define nwkcProtocolVersion			0x01
#define nwkcRepairThreshold			0x03
#define nwkcRouteDiscoveryTime		0x2710
#define nwkcMaxBroadcastJitter		0x40
#define nwkcInitialRREQRetries		0x03
#define nwkcRREQRetries				0x02
#define nwkcRREQRetryInterval		0xfe
#define nwkcMinRREQJitter			0x01
#define nwkcMaxRREQJitter			0X40


/*--------------------------*/
/*	TX OPTIONS																				*/
/*--------------------------*/
#define ACK_TX						0x01 
#define GTS_TX						0x02
#define IND_TX  					0x04
#define SEC_TX 						0x08

/*--------------------------*/
/*	 Scan Types             */
/*--------------------------*/
#define ED      0x01    // FFD only
#define ACTIVE  0x02    // FFD only
#define PASSIVE 0x03    
#define ORPHAN  0x04    

/* Association response status */
#define DEV_AT_CAP      0x01
#define DEV_NOT_PERMIT  0x02

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/
/*---------------------*/
/*	NWK LAYER STATUS   */	
/*---------------------*/
typedef enum{
	nwk_SUCCESS					=	0x00,
	nwk_INVALID_PARAMETER   	=	0xc1,
	nwk_INVALID_REQUEST			=	0xc2,
	nwk_NOT_PERMITTED 			=	0xc3,
	nwk_STARTUP_FAILURE 		=	0xc4,
	nwk_ALREADY_PRESENT 		=	0xc5,
	nwk_SYNC_FAILURE			=	0xc6,
	nwk_TABLE_FULL				=	0xc7,	
	nwk_UNKNOWN_DEVICE			=	0xc8,
	nwk_UNSUPPORTED_ATTRIBUTE	=	0xc9,
	nwk_NO_NETWORKS				=	0xca,
	nwk_LEAVE_UNCONFIRMED		=	0xcb,
	/*Security Processing*/
	nwk_MAX_FRM_CNTR			=	0xcc,
	nwk_NO_KEY					=	0xcd,
	nwk_BAD_CCM_OUTPUT			=	0xce,
	/*MAC dependent status*/
	nwk_DISABLE_TRX_FAILURE		=	0xcf	
}NWK_STATUS;

/*----------------------------------------------*/
/*	NETWORK LAYER INFORMATION BASE STRUCTURE    */
/*----------------------------------------------*/
typedef struct{
	UINT8	nwkSequenceNumber;													
	UINT8	nwkPassiveAckTimeout;
	UINT8	nwkMaxBroadcastRetries;
	UINT8	nwkMaxChildren;
	UINT8	nwkMaxDepth;
	UINT8	nwkMaxRouters;
	void*	nwkNeighborTable;
	UINT8	nwkBroadcastDeliveryTime;
	UINT8	nwkReportConstantCost;
	UINT8	nwkRouteDiscoveryRetriesPermitted;
	void*	nwkRouteTable;
	BOOL	nwkSymLink;						
	UINT8	nwkCapabilityInformation;		      
	BOOL	nwkUseTreeAddrAlloc;
	BOOL	nwkUseTreeRouting;
	UINT16	nwkNextAddress;
	UINT16	nwkAvailableAddresses;
	UINT16	nwkAddressIncrement;
	UINT16 	nwkTransactionPersistenceTime;
}NWK_IB;

/*----------------------------------------------*/
/*	NETWORK LAYER INFORMATION BASE ID			*/
/*----------------------------------------------*/
typedef enum {
	NWKSEQUENCENUMBER					= 0x81,									
	NWKPASSIVEACKTIMEOUT				= 0x82,
	NWKMAXBROADCASTRETRIES				= 0x83,
	NWKMAXCHILDREN						= 0x84,
	NWKMAXDEPTH							= 0x85,
	NWKMAXROUTERS						= 0x86,
	NWKNEIGHBORTABLE					= 0x87,
	NWKNETWORKBROADCASTDELIVERYTIME		= 0x88,
	NWKREPORTCOSTANTCOST				= 0x89,
	NWKROUTEDISCOVERYRETRIESPERMITTED 	= 0x8a,
	NWKROUTETABLE						= 0x8b,
	NWKSYMLINK							= 0x8e,
	NWKCAPABILITYINFORMATION			= 0x8f,		      
	NWKUSETREEADDRALLOC					= 0x90,
	NWKUSETREEROUTING					= 0x91,
	NWKNEXTADDRESS						= 0x92,
	NWKAVAILABLEADDRESSES				= 0x93,
	NWKADDRESSINCREMENT					= 0x94,
	NWKTRANSACTIONPERSISTENCETIME		= 0x95
}NWK_IB_ID;


/*----------------------------------------------*/
/*	CAPACITY INFORMATION            			*/
/*----------------------------------------------*/
typedef struct  
{
    UINT8   AlterCoord  :1;
    UINT8   DeviceType  :1;
    UINT8   PwrSrc      :1;
    UINT8   RxOnWhenIdle:1;
    UINT8   Reserved    :2;
    UINT8   SecurityCap :1;
    UINT8   AllocAddr   :1;
}CAPACITY_INFO;

/*------------------------------------------------*/
/*	Relationship types   	                      */ 
/*------------------------------------------------*/
typedef enum
{
    NWK_PARRENT = 0x00,
    NWK_CHILD,
    NWK_SIBLING,
    NWK_NO_RELATION
} NWK_NEIG_RELATION;

/*----------------------------------------------*/
/*	DEFINITION FOR NETWORK PACKET FORMAT		*/
/*----------------------------------------------*/
/* Nwk frame type */
typedef enum
{
    NWK_FRM_DATA = 0x00,
    NWK_FRM_CMD  = 0x01
} NWK_FRM_TYPE;

/* Nwk discovery type */
typedef enum
{
    NWK_DISCO_SUPPRESS = 0x00,
    NWK_DISCO_ENABLE ,
    NWK_DISCO_FORCE ,
    NWK_DISCO_RESERVED
} NWK_DISCO_TYPE;

/* Nwk command frame type */
typedef enum
{
    NWK_CMD_RREQ = 0x01,
    NWK_CMD_RREP ,
    NWK_CMD_RERR ,
    NWK_CMD_LEAVE ,
    NWK_CMD_PING,
    NWK_CMD_PING_REP
} NWK_CMD_TYPE;

/* Cmd option */
typedef struct  
{
    UINT8       Reserved:7;
    UINT8       RoutePair:1;
}NWK_ROUTE_OPTION;

typedef struct  
{
    UINT8       Reserved:6;
    UINT8       Req_Indication:1;
    UINT8       RemoveChild:1;
}NWK_LEAVE_OPTION;

/* Nwk route error type */
typedef enum
{
    NWK_RERR_NO_ROUTE = 0x00,
    NWK_RERR_TREE_FAIL ,
    NWK_RERR_NONTREE_FAIL ,
    NWK_RERR_LOW_BAT ,
    NWK_RERR_NO_CAP
} NWK_ERR_TYPE;



/* Nwk command payload */
typedef struct  
{
    NWK_CMD_TYPE        CmdType;
    NWK_ROUTE_OPTION    CmdOption;
    UINT8               RReqId;
    UINT16              DesAddr;
    UINT8               PathCost;
}NWK_RREQ_PAYLOAD_TYPE;

typedef struct  
{
    NWK_CMD_TYPE        CmdType;
    NWK_ROUTE_OPTION    CmdOption;
    UINT8               RReqId;
    UINT16              OrigAddr;
    UINT16              ResAddr;
    UINT8               PathCost;
}NWK_RREP_PAYLOAD_TYPE;

typedef struct  
{
    NWK_CMD_TYPE        CmdType;
    NWK_ERR_TYPE        ErrCode;
    UINT16              DesAddr;
}NWK_RERR_PAYLOAD_TYPE;

typedef struct  
{
    NWK_CMD_TYPE        CmdType;
    NWK_LEAVE_OPTION    CmdOption;
}NWK_LEAVE_PAYLOAD_TYPE;


/* NPDU format */
typedef struct  
{
    /*Network header- Control field*/
    UINT16          FrmType:2;
    UINT16          ProtocolVer:4;
    UINT16          DiscoverRoute:2;
    UINT16          Reserved1:1;
    UINT16          Security:1;
    UINT16          Reserved2:6;
    /*Network header- Routing field*/
    UINT16          DesAddr;
    UINT16          SrcAddr;
    UINT8           Radius;
    UINT8           SeqNumber;
    /* Network Payload */
    UINT8           Payload;
}NPDU;

typedef NPDU*      P_NPDU;

/*-----------------------*/
/* NETWORK DESCRIPTOR	 */				
/*-----------------------*/
typedef struct
{
	UINT16 PanID;
	BYTE LogicalChannel;
	BYTE StackProfile;
	BYTE ZigbeeVersion;
	BYTE BeaconOrder;
	BYTE SuperframeOrder;
	BOOL PermitJoining;
}NWK_DESC;

/*-----------------------*/
/*	NEIGHBOR TABLE		 */
/*-----------------------*/
typedef struct{
	UINT16	PanID;
	QWORD	ExtendedAddress;
	WORD    NetworkAddress;
	BYTE 	DeviceType;
	BOOL	RxOnWhenIdle;
	BYTE	Relationship;
	BYTE	Depth;
	BYTE	BeaconOrder;
	BOOL 	PermitJoining;
	BYTE 	PotentialParent;
	BYTE 	LogicalChannel;
	DWORD 	IncomingBeaconTimestamp;
	DWORD	BeaconTxTimeOffset;
    /* Custom parameter */
}NWK_NEIGHBOR_NODE;



/*----------------------------------------------*/
/*	DEFINITION FOR NETWORK STATE        		*/
/*----------------------------------------------*/
typedef enum  
{
    /* Mode of Nwk layer */
    NWK_STATE_INIT      =0,
    NWK_STATE_NORMAL_MODE,

    /* State in initialization of Nwk layer */
    NWK_STATE_FORMATION,
    NWK_STATE_DISCOVERY,
    NWK_STATE_JOIN,
    NWK_STATE_REJOIN,
    NWK_STATE_JOIN_SUCESS,
    NWK_STATE_START_ROUTER,

    NWK_STATE_SYNC,
    
    NWK_STATE_FAILED
}NWK_STATE;

/*------------------------------------------------*/
/*	NON-STANDARD PARAMETERS	                      */ 
/*------------------------------------------------*/
typedef struct{
    UINT8   NwkState;
    UINT8   CurRequest;
    NWK_NEIGHBOR_NODE* pParent;	
	UINT16	NwkPANId;
    UINT8   NwkChannel;
    UINT8   NwkJoinedDeviveType;
	WORD   	NwkAddr;
	BOOL 	SecurityEnable;
    UINT8   NwkSuperFrmOrder;
    UINT8   NwkBeaconOrder;
#if ZIGBEE_DEVICE_FFD == 1
    /* Specification of nwk */
    UINT32  BeaconOffset;
	BOOL    BatLifeExt;
    UINT32  NwkChannelList;
    BEACON_PAYLOAD BcnPayload;
#endif    
}NWK_INFO;



/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/
/* Network layer information*/
extern NWK_INFO        NwkInfo;
/* Network Information Base */
extern NWK_IB          nwk_ib;

extern NWK_DESC*       nwk_desc[MAX_NWK_DESC_SIZE];


extern NWK_NEIGHBOR_NODE*  nwk_neighborTable[NWK_NEIGHBOR_TBLE_SIZE];
/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/


/************************************************************************/
/*                      Function                                        */
/************************************************************************/

/******************************************************************************/
//@FunctionName: This function will 
//                  - Format the NPDU data at the beginning of the Nsdu
//                    this memory already spared for NWK header
//                  - Get new memory block (32 bytes) for NPDU command, format
//                    it  
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
P_NPDU nwk_makeNpdu(NWK_FRM_TYPE FrmType, BYTE NsduLength, BYTE *Nsdu);

/******************************************************************************/
//@FunctionName: This function will be called in the scanning procedure to    */
//               update the neighbor table and network table                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void  nwk_updateBeacon(MLME_BEACON_NOTIFY_INDICATION* BcnNotif);

/******************************************************************************/
//@FunctionName: This function will check in the nwk Description table if
//               there exist such a network with the same PANID               */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL  nwk_checkNwkExisted(UINT16    PanId);

/******************************************************************************/
//@FunctionName: This function will check if we should chose the channel for  */
//               a new network. The choice depends on implementation
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT8  nwk_choseChannel(UINT32 ScanChannel);

/******************************************************************************/
//@FunctionName: This function will search in the neighbor table a router 
//               to join								  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
NWK_NEIGHBOR_NODE*  nwk_findRouterInNT(UINT16   PanId, DWORD  ScanChannels);

/******************************************************************************/
//@FunctionName: This function will search in the neighbor table a node that
//               has the same 64-bits address
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
NWK_NEIGHBOR_NODE*  nwk_findNodeInNT(ADDRESS*    pExAddr);


/******************************************************************************/
//@FunctionName: This function check if a  node is parent                     */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL nwk_IsParent(ADDRESS*  pExAddr);

/******************************************************************************/
//@FunctionName: This function check if a  node is child                      */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL nwk_IsChild(ADDRESS*  pExAddr);

/******************************************************************************/
//@FunctionName: This function will get extended addr from short add of 
//               a node in the NT (neighbors table)                           */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
ADDRESS*    nwk_getExAddInNT(UINT16 ShortAddr);

/******************************************************************************/
//@FunctionName: This function will get extended addr from short add of 
//               a node in the NT (neighbors table)                           */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
UINT16     nwk_getShortAddInNT(ADDRESS* pExAddr);

/******************************************************************************/
//@FunctionName: Set relationship to a node in the NT   	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void      nwk_setRelationship(QWORD* pExAddr, NWK_NEIG_RELATION Relationship);


/******************************************************************************/
//@FunctionName: This function will return next child in the NT               */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
NWK_NEIGHBOR_NODE*      nwk_getNextChild(BOOL bFirstChild);


/******************************************************************************/
//@FunctionName: This function will check last entry  of the neighbor table   */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/

BYTE      nwk_lastEntry();


/******************************************************************************/
//@FunctionName:This function if the entry already exist on the neighbor table*/
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL	nwk_TableExist(ADDRESS DeviceAddr);


/******************************************************************************/
//@FunctionName: Set relationship to a node in the NT   	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
NWK_STATUS  nwk_addNewChild(QWORD* pDevAddress, WORD NwkAddr,CAPACITY_INFO CapInfor, 
                            BOOL securityUse, UINT8 aclEntry);

/******************************************************************************/
//@FunctionName: This function will remove a child in the NT due to a 
//               disassociation cmd                                           */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
NWK_STATUS nwk_removeChild(QWORD* pDevAddress);

/******************************************************************************/
//@FunctionName: This function will check if there exist such a nwk in the 
//               network desctiptor. After that, it will release the memory
//               holding by nwk desc    				  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
BOOL nwk_checkNwk(WORD  PanID, DWORD    ChannelList);

#endif //!NWK_COMMON_H
