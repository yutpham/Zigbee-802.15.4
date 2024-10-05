/*******************************************************************************/
//Module: 	                                                                   */   
//File Name: zdp                                                               */
//Description: This file will implement ZDP functionality of Zigbee stack      */
//-----------------------------------------------------------------------------*/
//Revision History:                                                            */
//Date				Programmer		Details                                    */
//07/31/2007                                                                   */
//-----------------------------------------------------------------------------*/
//  Copyright (C) 2006 - All Rights Reserved                                   */
/*******************************************************************************/

/************************************************************************/
/*					Include files										*/
/************************************************************************/
#include "zdp.h"
#include "app_framework.h"
#include "apf_format.h"
#include "zdo.h"
/************************************************************************/
/*					Constant declaration								*/
/************************************************************************/

/************************************************************************/
/*					Type Definition										*/
/************************************************************************/


/************************************************************************/
/*					Variable declaration								*/
/************************************************************************/

/************************************************************************/
/*                      Static Function                                 */
/************************************************************************/

UINT8       zdp_trans = 0;
/************************************************************************/
/*                      Function                                        */
/************************************************************************/


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will send msg to request IEEE corresponding
//              to a NwkAddr.
//                                                                           */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS zdp_IEEEAddrReq(UINT16 NwkAddr, BYTE ReqType, 
                         UINT8 StartIndex)
{
	/*********** Variable declaration ***********/

	/*********** Function body		 ***********/
    return SUCCESS;
} // End of functions

/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will send msg to request NwkAddr corresponding
//              to a Ext address.
//                                                                            */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS zdp_NwkAddrReq( QWORD *pIEEEAddress, BYTE ReqType, 
                         UINT8 StartIndex, BOOL SecurityUse)
{
	/*********** Variable declaration ***********/
    AF_MSG      AfMsg;
    AF_FRM*     pAfFrm;
    WORD        BroadAddr;
	/*********** Function body		 ***********/
    /*Init af msg*/
    AfMsg.pData = mem_getMem(16);
    if (AfMsg.pData == NULL)
        return TRANSACTION_OVERFLOW;

    AfMsg.TransSeqNumber  = zdp_trans++;
    mem_cpy(AfMsg.pData,pIEEEAddress,ADDR_LEN_EXTENDED_64);
    AfMsg.pData[ADDR_LEN_EXTENDED_64]       = ReqType;
    AfMsg.pData[ADDR_LEN_EXTENDED_64 + 1]   = StartIndex;
    AfMsg.DataLen = ADDR_LEN_EXTENDED_64 + 1+1;

    pAfFrm = apf_initFrm(AF_FRM_MSG);
    if (pAfFrm)
    {
        if (apf_addMsg(pAfFrm,&AfMsg))
        {
            /* Send message to the destination */
             BroadAddr = BROADCAST_ADDR;
             apf_sendFrm(pAfFrm,APS_ADDR_SHORT,&BroadAddr,0,
                        0,EP_ID_ZDO,0x00);
        }
        apf_releaseFrm(pAfFrm);
        mem_freeMem(AfMsg.pData);
        return SUCCESS;
    }
    return TRANSACTION_OVERFLOW;
} // End of functions            


/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	This function will send a response to a address request       */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void zdp_NwkAddrRes(    BOOL   bIEEE, AF_ADDR* pDstAddr, BYTE Status,
                        BYTE   *IEEEAddrRemoteDev, UINT16 nwkAddr, BYTE ReqType, 
                        BYTE   NumAssocDev, BYTE   StartIndex,UINT16 *NWKAddrAssocDevList,
                        BOOL   SecurityEnable )
{
    BYTE          nTemp;
    BYTE          *buf;
    BYTE          *pBuf;
    BYTE          bufLen;
    Z_STATUS      stat;

    // Calculate buffer length needed
    bufLen = 1 +       ADDR_LEN_EXTENDED_64 + 1 + 1 + 2; 
         /* Status + IEEEAddrRemoteDev +  nwkAddr + NumAssocDev + StartIndex */

    /*Check kind of request*/
    if ( ReqType == ZDP_ADDR_REQ_EXTENDED )
        bufLen += ((NumAssocDev - StartIndex) * sizeof( UINT16 ));
    else
        NumAssocDev = 0;

    // Allocate the buffer
    buf = mem_getMem( bufLen );

    if ( buf )
    {
        // Fill in the buffer
        pBuf = buf;

        *pBuf++ = Status;

        mem_cpy( pBuf, IEEEAddrRemoteDev,ADDR_LEN_EXTENDED_64);
        pBuf   += ADDR_LEN_EXTENDED_64;

        *pBuf++ = LO_UINT16( nwkAddr );
        *pBuf++ = HI_UINT16( nwkAddr );

        *pBuf++ = NumAssocDev;
        *pBuf++ = StartIndex;

        if ( ReqType == ZDP_ADDR_REQ_EXTENDED )
        {     
            // Move past the start index
            for ( nTemp = 0; nTemp < StartIndex; nTemp++ )
            NWKAddrAssocDevList++;

            for ( nTemp = 0; nTemp < (NumAssocDev - StartIndex); nTemp++ )
            {
                *pBuf++ = LO_UINT16( *NWKAddrAssocDevList );
                *pBuf++ = HI_UINT16( *NWKAddrAssocDevList );
                NWKAddrAssocDevList++;
            }
        }
        
        // Send the message
        nTemp = (bIEEE)? IEEE_addr_rsp : NWK_addr_rsp;
        stat = apf_sendSingleMsg(buf,bufLen, zdp_trans, pDstAddr, EP_ID_ZDO, nTemp,
                                EP_ID_ZDO, 0x00 );                                  
        // Free the memory
        mem_freeMem(buf);
    }
    else
        stat = TRANSACTION_OVERFLOW;

    return stat;
}

#if defined (ZDO_END_DEVICE_ANN)
/******************************************************************************/
//@FunctionName: zdp_EndDeviceAnnce						  	                  */
//@Description:	This function will announce the the allocated nwk addr to nwk */                
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS zdp_EndDeviceAnnce( UINT16 NwkAddr, BYTE* pIEEEAddr, BYTE SecurityEnable ) 
{
    BYTE      Msg[2 + ADDR_LEN_EXTENDED_64];
    AF_ADDR   BrtAddr;

    Msg[0] = LO_UINT16( NwkAddr );
    Msg[1] = HI_UINT16( NwkAddr );
    mem_cpy( &Msg[2], pIEEEAddr,ADDR_LEN_EXTENDED_64);

    BrtAddr.Addr.Short  = BROADCAST_ADDR;
    BrtAddr.AddrMode    = APS_ADDR_SHORT;
    return         apf_sendSingleMsg(Msg,sizeof(Msg), zdp_trans, &BrtAddr, EP_ID_ZDO, 
                                     End_Device_annce,EP_ID_ZDO, 0x00);                                  
}
#endif //!defined (ZDO_END_DEVICE_ANN)

/*********************************************************************
 * Binding
 */
#if defined ( ZDO_ENDDEVICE_BIND_REQ )
/******************************************************************************/
//@FunctionName:   										  	                  */
//@Description:	                                                              */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
void zdp_sendEDBindReq( BYTE endPoint )
{
    SIMPLE_DESCRIPTOR       *sDesc;
    ENDPOINT_DESTCRIPTOR    *epDesc;
    AF_ADDR                 Addr;    
    if ( endPoint == EP_ID_ZDO )
        return;   // Can't do for ZDO

    epDesc = app_finEp( endPoint );
    if ( epDesc )
    {
        sDesc = &epDesc->SimpleDescriptor;
        
        Addr.AddrMode   = APS_ADDR_SHORT;
        Addr.Addr.Short = 0x00;
  
        zdp_EndDeviceBindReq( &Addr, 0x00,endPoint,
                    sDesc->AppProfId,                    
                    sDesc->AppNumInClusters, sDesc->pAppInClusterList,
                    sDesc->AppNumOutClusters, sDesc->pAppOutClusterList,
                    0 );
    }
}

/******************************************************************************/
//@FunctionName:  zdp_EndDeviceBindReq					  	                  */
//@Description:	 This function will generate a end_device_bind request        */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS zdp_EndDeviceBindReq( AF_ADDR* pDstAddr, 
                                 UINT16  LocalCoordinator,
                                 BYTE    endPoint,
                                 UINT16  ProfileID,
                                 BYTE NumInClusters, BYTE *InClusterList,
                                 BYTE NumOutClusters, BYTE *OutClusterList,
                                 BYTE SecurityEnable )
{
    BYTE          *buf;
    BYTE          *pBuf;
    BYTE          bufLen;
    Z_STATUS      stat;

    // Calculate length needed
    bufLen = 2 + 1 + 2 + 1 + 1; 
    // LocalCoordinator + epIntf + ProfileID +  NumInClusters + NumOutClusters
    bufLen += (NumInClusters + NumOutClusters);

    // Allocate the buffer
    buf = mem_getMem( bufLen );

    if ( buf )
    {
        // Fill in the buffer
        pBuf = buf;

        *pBuf++ = LO_UINT16( LocalCoordinator );
        *pBuf++ = HI_UINT16( LocalCoordinator );

        *pBuf++ = endPoint;

        *pBuf++ = LO_UINT16( ProfileID );  
        *pBuf++ = HI_UINT16( ProfileID );

        *pBuf++ = NumInClusters;    /* Input cluster list */
        mem_cpy( pBuf, InClusterList, NumInClusters );
        pBuf += NumInClusters;

        *pBuf++ = NumOutClusters;   /* Output cluster list */
        mem_cpy( pBuf, OutClusterList, NumOutClusters );
        pBuf += NumOutClusters;

        // Send the message
        stat = apf_sendSingleMsg(buf,bufLen, zdp_trans, pDstAddr, 
                                 EP_ID_ZDO, End_Device_Bind_req,EP_ID_ZDO, 0x00 );                                  
        // Free the memory
        mem_freeMem( buf );
    }
    else
        stat = TRANSACTION_OVERFLOW;

    return stat;
}
#endif // ZDO_ENDDEVICE_BIND_REQ

#if defined ( ZDO_ENDDEVICE_BIND_RSP )
/******************************************************************************/
//@FunctionName: zdp_EDBindRes						  	                      */
//@Description:	 This function will generate ED_bin_ to the source            */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS zdp_EDBindRes(AF_ADDR* pDstAddr, UINT8 Trans, UINT8    Status)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    // Send the message        
    return apf_sendSingleMsg(&Status,sizeof(Status), Trans, pDstAddr, 
                            EP_ID_ZDO, End_Device_Bind_rsp ,
                            EP_ID_ZDO, 0x00 ); 
} // End of functions
#endif //!defined ( ZDO_ENDDEVICE_BIND_RSP )


#if defined ( ZDO_ENDDEVICE_BIND_UNBIND_REQ )
/******************************************************************************/
//@FunctionName: zdp_BindUnbindReq						  	                  */
//@Description:	This function will generate a bin/unbind request and send
//              to the coor.                                                  */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS zdp_BindUnbindReq( BOOL BindOrUnbind, AF_ADDR* pDstAddr, 
                              BYTE *pSourceAddr, BYTE SrcEndPoint,
                              BYTE ClusterID, 
                              BYTE *pDestinationAddr, BYTE DstEndPoint,
                              BYTE SecurityEnable )
{
    BYTE            *pBuf;
    BYTE            buf;
    BYTE            bufLen;
    Z_STATUS        stat;

    // Calculate length needed
    bufLen = ADDR_LEN_EXTENDED_64 + 1 + 1 + ADDR_LEN_EXTENDED_64 + 1; 
    // SourceAddr + SrcEPIntf + ClusterID +  DestinationAddr + DstEPIntf

    // Allocate the buffer
    buf = mem_getMem( bufLen );

    if ( buf )
    {
        pBuf = buf;
        mem_cpy( pBuf, pSourceAddr );
        *pBuf++ = SrcEndPoint;

        *pBuf++ = ClusterID;

        mem_cpy( pBuf, pDestinationAddr,ADDR_LEN_EXTENDED_64);
        *pBuf = DstEndPoint;

        // Send the message        
        stat = apf_sendSingleMsg(buf,bufLen, zdp_trans, pDstAddr, 
                                EP_ID_ZDO, (BindOrUnbind==TRUE)?Bind_req:Unbind_req ,
                                EP_ID_ZDO, 0x00 );  
        // Free the memory
        mem_freeMem( buf );
    }
    else
        stat = TRANSACTION_OVERFLOW;

    return stat;
}
#endif // ZDO_ENDDEVICE_BIND_UNBIND_REQ 

#if defined ( ZDO_ENDDEVICE_BIND_UNBIND_REQ )
/******************************************************************************/
//@FunctionName: zdp_BindUnBindRes						  	                  */
//@Description:	 This function will generate bin_res/unbind_res to the source */
//	                                                                          */    
//@Param                                                                      */
//	                                                                          */   
/******************************************************************************/
Z_STATUS zdp_BindUnBindRes(AF_ADDR* pDstAddr, UINT8 Trans, UINT8    Status, BOOL    Bind)
{
	/*********** Variable declaration ***********/
	/*********** Function body		 ***********/
    // Send the message        
    return apf_sendSingleMsg(&Status,sizeof(Status), Trans, pDstAddr, 
                            EP_ID_ZDO, (Bind==TRUE)?Bind_rsp:Unbind_rsp ,
                            EP_ID_ZDO, 0x00 ); 
} // End of functions
#endif // ZDO_ENDDEVICE_BIND_UNBIND_REQ 
