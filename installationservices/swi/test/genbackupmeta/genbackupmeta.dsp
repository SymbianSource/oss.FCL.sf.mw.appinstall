# Microsoft Developer Studio Project File - Name="genbackupmeta" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=genbackupmeta - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "genbackupmeta.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "genbackupmeta.mak" CFG="genbackupmeta - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "genbackupmeta - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "genbackupmeta - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "genbackupmeta - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\source\common" /I ".\inc" /I "$(EPOCROOT)epoc32\include" /I "..\..\..\..\secureswitools\swianalysistoolkit\source\common" /I "..\..\inc\swi" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 $(EPOCROOT)EPOC32\RELEASE\WINC\REL\zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"\epoc32\release\winc\rel" /libpath:"\epoc32\release\tools\rel" /libpath:"\epoc32\tools"

!ELSEIF  "$(CFG)" == "genbackupmeta - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\inc" /I "$(EPOCROOT)epoc32\include" /I "..\..\..\..\secureswitools\swianalysistoolkit\source\common" /I "..\..\inc\swi" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(EPOCROOT)EPOC32\RELEASE\WINC\REL\zlib.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libc" /pdbtype:sept /libpath:"\epoc32\release\winc\rel" /libpath:"\epoc32\release\tools\rel" /libpath:"\epoc32\tools"

!ENDIF 

# Begin Target

# Name "genbackupmeta - Win32 Release"
# Name "genbackupmeta - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\source\cmdparser.cpp
# End Source File
# Begin Source File

SOURCE=.\source\details.cpp
# End Source File
# Begin Source File

SOURCE=.\source\exceptionhandler.cpp
# End Source File
# Begin Source File

SOURCE=.\source\filecontents.cpp
# End Source File
# Begin Source File

SOURCE=.\source\main.cpp
# End Source File
# Begin Source File

SOURCE=.\source\metagenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\source\preamble.cpp
# End Source File
# Begin Source File

SOURCE=.\source\siscompressed.cpp
# End Source File
# Begin Source File

SOURCE=.\source\streamwriter.cpp
# End Source File
# Begin Source File

SOURCE=.\source\tlv.cpp
# End Source File
# Begin Source File

SOURCE=.\source\utils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\inc\cmdparser.h
# End Source File
# Begin Source File

SOURCE=.\inc\details.h
# End Source File
# Begin Source File

SOURCE=.\inc\exceptionhandler.h
# End Source File
# Begin Source File

SOURCE=.\inc\filecontents.h
# End Source File
# Begin Source File

SOURCE=.\inc\metagenerator.h
# End Source File
# Begin Source File

SOURCE=.\inc\preamble.h
# End Source File
# Begin Source File

SOURCE=.\inc\siscompressed.h
# End Source File
# Begin Source File

SOURCE=.\inc\streamwriter.h
# End Source File
# Begin Source File

SOURCE=.\inc\tlv.h
# End Source File
# Begin Source File

SOURCE=.\inc\utils.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
