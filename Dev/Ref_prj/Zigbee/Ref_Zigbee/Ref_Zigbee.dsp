# Microsoft Developer Studio Project File - Name="Ref_Zigbee" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Ref_Zigbee - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Ref_Zigbee.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Ref_Zigbee.mak" CFG="Ref_Zigbee - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Ref_Zigbee - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Ref_Zigbee - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Ref_Zigbee - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Ref_Zigbee - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Ref_Zigbee - Win32 Release"
# Name "Ref_Zigbee - Win32 Debug"
# Begin Group "MAC"

# PROP Default_Filter ""
# Begin Group "mac_primitives"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\MAC\mac_primitive\mac_mcps.c
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\mac_primitive\mac_mlme.c
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\mac_primitive\mac_mlme.h
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\mac_primitive\mac_state.c
# End Source File
# End Group
# Begin Group "mac_tx"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\MAC\mac_tx\csma-ca.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\MAC\mac_tx\csma-ca.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\mac_tx\indirect_tx.c
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\mac_tx\indirect_tx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\mac_tx\mac_tx.c
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\mac_tx\mac_tx.h
# End Source File
# End Group
# Begin Group "interfaces"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\MAC\mac_interfaces\mac_primitives.h
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\mac_primitive\mac_state.h
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\mac_interfaces\mac_types.h
# End Source File
# End Group
# Begin Group "PHY"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\MAC\phy_primitives\phy_primitives.c
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\phy_primitives\phy_primitives.h
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\phy_primitives\phy_support.c
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\phy_primitives\phy_support.h
# End Source File
# End Group
# Begin Group "HAL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\MAC\hal\hal.h
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\hal\hal_cc2420.h
# End Source File
# End Group
# Begin Group "beacon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\MAC\beacon\mac_beacon.c
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\beacon\mac_beacon.h
# End Source File
# End Group
# Begin Group "INT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\MAC\phy_primitives\trx_interrupt.c
# End Source File
# End Group
# Begin Group "mac_rx"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\MAC\mac_rx\mac_rx.c
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\mac_rx\mac_rx.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\MAC\common\mac_mem.c
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\common\mac_mem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\common\mac_packet.c
# End Source File
# Begin Source File

SOURCE=..\..\..\MAC\common\mac_packet.h
# End Source File
# End Group
# End Group
# Begin Group "NWK"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\NWK\data_queue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\NWK\data_queue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\NWK\nwk.c
# End Source File
# Begin Source File

SOURCE=..\..\..\NWK\nwk.h
# End Source File
# Begin Source File

SOURCE=..\..\..\NWK\nwk_common.c
# End Source File
# Begin Source File

SOURCE=..\..\..\NWK\nwk_common.h
# End Source File
# Begin Source File

SOURCE=..\..\..\NWK\nwk_data.c
# End Source File
# Begin Source File

SOURCE=..\..\..\NWK\nwk_data.h
# End Source File
# Begin Source File

SOURCE=..\..\..\NWK\nwk_msg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\NWK\nwk_nlme.c
# End Source File
# Begin Source File

SOURCE=..\..\..\NWK\nwk_routing.c
# End Source File
# Begin Source File

SOURCE=..\..\..\NWK\nwk_routing.h
# End Source File
# End Group
# Begin Group "Com"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Common\board.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Common\board.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Common\types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Common\zconf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\Common\zconf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Common\zigbee.c
# End Source File
# End Group
# Begin Group "Ref"

# PROP Default_Filter ""
# Begin Source File

SOURCE=C:\WinAVR\avr\include\avr\iom128.h
# End Source File
# End Group
# Begin Group "Stub"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Common\zigbee_test.c
# End Source File
# End Group
# Begin Group "OS-HAL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\OS\linklist.c
# End Source File
# Begin Source File

SOURCE=..\..\..\OS\linklist.h
# End Source File
# Begin Source File

SOURCE=..\..\..\OS\nv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\OS\nv.h
# End Source File
# Begin Source File

SOURCE=..\..\..\OS\ucos\os_cfg_r.h
# End Source File
# Begin Source File

SOURCE=..\..\..\OS\ucos\os_cpu.h
# End Source File
# Begin Source File

SOURCE=..\..\..\OS\ucos\os_mem.c
# End Source File
# Begin Source File

SOURCE=..\..\..\OS\ucos\os_tmr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\OS\queue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\OS\queue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\OS\schedul.c
# End Source File
# Begin Source File

SOURCE=..\..\..\OS\schedul.h
# End Source File
# Begin Source File

SOURCE=..\..\..\OS\static_timer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\OS\static_timer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\OS\ucos\ucos_ii.c
# End Source File
# Begin Source File

SOURCE=..\..\..\OS\ucos\ucos_ii.h
# End Source File
# Begin Source File

SOURCE=..\..\..\OS\utils.c
# End Source File
# Begin Source File

SOURCE=..\..\..\OS\utils.h
# End Source File
# End Group
# Begin Group "APL"

# PROP Default_Filter ""
# Begin Group "APS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\APL\APS\aps.c
# End Source File
# Begin Source File

SOURCE=..\..\..\APL\APS\aps.h
# End Source File
# Begin Source File

SOURCE=..\..\..\APL\APS\aps_ib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\APL\APS\aps_ib.h
# End Source File
# End Group
# Begin Group "APF"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\APL\APF\apf_format.c
# End Source File
# Begin Source File

SOURCE=..\..\..\APL\APF\apf_format.h
# End Source File
# Begin Source File

SOURCE=..\..\..\APL\APF\app_framework.c
# End Source File
# Begin Source File

SOURCE=..\..\..\APL\APF\app_framework.h
# End Source File
# End Group
# End Group
# Begin Group "DEBUGAPP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\APP\DebugApp\debugapp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\APP\DebugApp\debugapp.h
# End Source File
# End Group
# Begin Group "Test"

# PROP Default_Filter ""
# Begin Group "target"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Test\SYSTEM\taget\ep_conf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Test\SYSTEM\taget\ep_conf.h
# End Source File
# End Group
# Begin Group "Coor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Test\APP\coor\ep_conf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Test\APP\coor\ep_conf.h
# End Source File
# End Group
# Begin Group "Device"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Test\APP\device\ep_conf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Test\APP\device\ep_conf.h
# End Source File
# End Group
# Begin Group "Router"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\Test\APP\router\ep_conf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Test\APP\router\ep_conf.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\Common\ztest.c
# End Source File
# End Group
# Begin Group "ZDO"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\APL\ZDO\zdo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\APL\ZDO\zdo.h
# End Source File
# Begin Source File

SOURCE=..\..\..\APL\ZDO\zdo_app_interface.c
# End Source File
# Begin Source File

SOURCE=..\..\..\APL\ZDO\zdo_app_interface.h
# End Source File
# End Group
# Begin Group "ZDP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\APL\ZDP\zdp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\APL\ZDP\zdp.h
# End Source File
# End Group
# Begin Group "HCL"

# PROP Default_Filter ""
# Begin Group "DLC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\APP\HCL\DLC03395\DLC03395_App.c
# End Source File
# Begin Source File

SOURCE=..\..\..\APP\HCL\DLC03395\DLC03395_App.h
# End Source File
# Begin Source File

SOURCE=..\..\..\APP\HCL\DLC03395\DLC03395_Device.c
# End Source File
# Begin Source File

SOURCE=..\..\..\APP\HCL\DLC03395\DLC03395_Device.h
# End Source File
# End Group
# Begin Group "DRC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\APP\HCL\DRC03392\DRC03392_App.c
# End Source File
# Begin Source File

SOURCE=..\..\..\APP\HCL\DRC03392\DRC03392_App.h
# End Source File
# Begin Source File

SOURCE=..\..\..\APP\HCL\DRC03392\DRC03392_Device.c
# End Source File
# Begin Source File

SOURCE=..\..\..\APP\HCL\DRC03392\DRC03392_Device.h
# End Source File
# End Group
# Begin Group "SRC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\APP\HCL\SRC03391\SRC03391_App.c
# End Source File
# Begin Source File

SOURCE=..\..\..\APP\HCL\SRC03391\SRC03391_App.h
# End Source File
# Begin Source File

SOURCE=..\..\..\APP\HCL\SRC03391\SRC03391_Device.c
# End Source File
# Begin Source File

SOURCE=..\..\..\APP\HCL\SRC03391\SRC03391_Device.h
# End Source File
# End Group
# Begin Group "SLC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\APP\HCL\SLC03394\SLC03394_App.c
# End Source File
# Begin Source File

SOURCE=..\..\..\APP\HCL\SLC03394\SLC03394_App.h
# End Source File
# Begin Source File

SOURCE=..\..\..\APP\HCL\SLC03394\SLC03394_Device.c
# End Source File
# Begin Source File

SOURCE=..\..\..\APP\HCL\SLC03394\SLC03394_Device.h
# End Source File
# End Group
# End Group
# End Target
# End Project
