# Microsoft Developer Studio Project File - Name="ref" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ref - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ref.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ref.mak" CFG="ref - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ref - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ref - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ref - Win32 Release"

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

!ELSEIF  "$(CFG)" == "ref - Win32 Debug"

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

# Name "ref - Win32 Release"
# Name "ref - Win32 Debug"
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\Compiler.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\console.c
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\Console.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\Debug.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\generic.h
# End Source File
# Begin Source File

SOURCE=C:\WinAVR\avr\include\avr\iom128.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\MSPI.c
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\MSPI.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\NeighborTable.c
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\NeighborTable.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\sralloc.c
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\sralloc.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\Tick.c
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\Tick.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\zAPL.c
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\zAPL.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\zAPS.c
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\zAPS.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\zdo.c
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\zdo.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\zHCLighting.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\zigbee.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\zMAC.c
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\zMAC.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\zMCDemo.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\zNVM.c
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\zNVM.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\ZNWK.c
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\zNWK.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\zPHY.h
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\zPHYCC2420.c
# End Source File
# Begin Source File

SOURCE=C:\MpZBee\Source\Stack\ZPHYCC2420.h
# End Source File
# End Target
# End Project
