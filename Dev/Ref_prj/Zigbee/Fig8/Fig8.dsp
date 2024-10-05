# Microsoft Developer Studio Project File - Name="Fig8" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Fig8 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Fig8.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Fig8.mak" CFG="Fig8 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Fig8 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Fig8 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Fig8 - Win32 Release"

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

!ELSEIF  "$(CFG)" == "Fig8 - Win32 Debug"

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

# Name "Fig8 - Win32 Release"
# Name "Fig8 - Win32 Debug"
# Begin Group "Zdo"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\Zdo\ZDApp.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\Zdo\ZDApp.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\Zdo\ZDConfig.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\Zdo\ZDConfig.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\Zdo\ZDObject.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\Zdo\ZDObject.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\Zdo\ZDProfile.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\Zdo\ZDProfile.h"
# End Source File
# End Group
# Begin Group "Nwk"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\NWK\APS.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\NWK\aps_util.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\NWK\APSMEDE.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\NWK\AssocList.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\NWK\BindingTable.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\NWK\NLMEDE.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\NWK\nwk.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\NWK\nwk_bufs.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\NWK\nwk_globals.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\NWK\nwk_globals.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\NWK\nwk_util.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\NWK\rtg.h"
# End Source File
# End Group
# Begin Group "Profile"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\Profiles\AF.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\Profiles\AF.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\Profiles\HomeControlLighting.h"
# End Source File
# End Group
# Begin Group "OSAL"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\DebugTrace.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\DebugTrace.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\MT_AF.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\MT_AF.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\MT_MAC.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\MT_MAC.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\MT_NWK.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\MT_NWK.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\MT_ZDO.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\MT_ZDO.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\MTEL.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\MTEL.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\MTSPCI.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\OSAL.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\OSAL.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\OSAL_Custom.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\OSAL_Memory.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\OSAL_Memory.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\OSAL_Nv.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\OSAL_PwrMgr.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\OSAL_PwrMgr.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\OSAL_Tasks.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\OSAL_Tasks.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\OSAL_Timers.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\OSAL_Timers.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\OSAL\ZComDef.h"
# End Source File
# End Group
# Begin Group "Zmac"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\ZMAC\ZMAC.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\ZMAC\CC2420DB\ZMac_CB_CC2420.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\ZMAC\CC2420DB\ZMAC_CC2420.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\ZMAC\CC2420DB\ZMAC_CC2420.h"
# End Source File
# End Group
# Begin Group "Zmain"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\ZMain\CC2420DB\ccf8w.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\ZMain\CC2420DB\OnBoard.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\ZMain\CC2420DB\OnBoard.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\ZMain\CC2420DB\ZMain.c"
# End Source File
# End Group
# Begin Group "GenericApp"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\Projects\Samples\GenericApp\Source\GenericApp.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\Projects\Samples\GenericApp\Source\GenericApp.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\References\Z-Stack\Projects\Samples\GenericApp\Source\OSAL_GenericApp.c"
# End Source File
# End Group
# Begin Source File

SOURCE=.\ee.ico
# End Source File
# End Target
# End Project
