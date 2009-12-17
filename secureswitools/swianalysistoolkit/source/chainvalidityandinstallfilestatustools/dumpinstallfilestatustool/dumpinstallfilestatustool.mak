# Microsoft Developer Studio Generated NMAKE File, Based on dumpinstallfilestatustool.dsp
!IF "$(CFG)" == ""
CFG=dumpinstallfilestatustool - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dumpinstallfilestatustool - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dumpinstallfilestatustool - Win32 Release" && "$(CFG)" != "dumpinstallfilestatustool - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dumpinstallfilestatustool.mak" CFG="dumpinstallfilestatustool - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dumpinstallfilestatustool - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dumpinstallfilestatustool - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Release"

POLICYFILEDIR=.
OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(EPOCROOT)Epoc32\tools\dumpinstallfilestatustool.exe"


CLEAN :
	-@erase "$(INTDIR)\certificatechain.obj"
	-@erase "$(INTDIR)\dumpchainvaliditytool.obj"
	-@erase "$(INTDIR)\dumpinstallfilestatustool.obj"
	-@erase "$(INTDIR)\launchprocess.obj"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\osinterface.obj"
	-@erase "$(INTDIR)\siscertificatechain.obj"
	-@erase "$(INTDIR)\sisfiledata.obj"
	-@erase "$(INTDIR)\swicertstore.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dumpinstallfilestatustool.exe"
	-@erase "$(EPOCROOT)Epoc32\tools\dumpinstallfilestatustool.exe" 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /I ".\\" /I "..\..\common" /I "..\common" /I "$(EPOCROOT)epoc32\include" /I "..\..\..\..\openssllib\import\inc" /I "..\..\..\..\swisistools\source\makesis" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "GENERATE_ERRORS" /D "_UNICODE" /D "DUMPINSTALLFILESTATUSTOOL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dumpinstallfilestatustool.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /GZ /c

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dumpinstallfilestatustool.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcp.lib oldnames.lib kernel32.lib user32.lib gdi32.lib version.lib libeay32.lib zlib.lib seclib.lib Shlwapi.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\dumpinstallfilestatustool.pdb" /map:"$(INTDIR)\dumpinstallfilestatustool.map" /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\dumpinstallfilestatustool.exe" /libpath:"\epoc32\release\winc\rel" /libpath:"\epoc32\tools" /libpath:"..\..\..\..\openssllib\import\bin\rel" /libpath:"\epoc32\release\tools\rel" /opt:noref 
LINK32_OBJS= \
	"$(INTDIR)\certificatechain.obj" \
	"$(INTDIR)\dumpchainvaliditytool.obj" \
	"$(INTDIR)\dumpinstallfilestatustool.obj" \
	"$(INTDIR)\launchprocess.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\osinterface.obj" \
	"$(INTDIR)\siscertificatechain.obj" \
	"$(INTDIR)\sisfiledata.obj" \
	"$(INTDIR)\swicertstore.obj"

"$(EPOCROOT)Epoc32\tools\dumpinstallfilestatustool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<
	
	copy "$(INTDIR)\dumpinstallfilestatustool.exe" "$(EPOCROOT)Epoc32\tools\dumpinstallfilestatustool.exe"

!ELSEIF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Debug"

POLICYFILEDIR=.
OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(EPOCROOT)Epoc32\release\winc\deb\dumpinstallfilestatustool.exe" "$(OUTDIR)\dumpinstallfilestatustool.bsc"


CLEAN :
	-@erase "$(INTDIR)\certificatechain.obj"
	-@erase "$(INTDIR)\certificatechain.sbr"
	-@erase "$(INTDIR)\dumpchainvaliditytool.obj"
	-@erase "$(INTDIR)\dumpchainvaliditytool.sbr"
	-@erase "$(INTDIR)\dumpinstallfilestatustool.obj"
	-@erase "$(INTDIR)\dumpinstallfilestatustool.sbr"
	-@erase "$(INTDIR)\launchprocess.obj"
	-@erase "$(INTDIR)\launchprocess.sbr"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\options.sbr"
	-@erase "$(INTDIR)\osinterface.obj"
	-@erase "$(INTDIR)\osinterface.sbr"
	-@erase "$(INTDIR)\siscertificatechain.obj"
	-@erase "$(INTDIR)\siscertificatechain.sbr"
	-@erase "$(INTDIR)\sisfiledata.obj"
	-@erase "$(INTDIR)\sisfiledata.sbr"
	-@erase "$(INTDIR)\swicertstore.obj"
	-@erase "$(INTDIR)\swicertstore.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dumpinstallfilestatustool.bsc"
	-@erase "$(OUTDIR)\dumpinstallfilestatustool.exe"
	-@erase "$(OUTDIR)\dumpinstallfilestatustool.ilk"
	-@erase "$(OUTDIR)\dumpinstallfilestatustool.map"
	-@erase "$(OUTDIR)\dumpinstallfilestatustool.pdb"
	-@erase "$(EPOCROOT)Epoc32\release\winc\deb\dumpinstallfilestatustool.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /I ".\\" /I "..\..\common" /I "..\common" /I "$(EPOCROOT)epoc32\include" /I "..\..\..\..\openssllib\import\inc" /I "..\..\..\..\swisistools\source\makesis" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "GENERATE_ERRORS" /D "_UNICODE" /D "DUMPINSTALLFILESTATUSTOOL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dumpinstallfilestatustool.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dumpinstallfilestatustool.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\certificatechain.sbr" \
	"$(INTDIR)\dumpchainvaliditytool.sbr" \
	"$(INTDIR)\dumpinstallfilestatustool.sbr" \
	"$(INTDIR)\launchprocess.sbr" \
	"$(INTDIR)\options.sbr" \
	"$(INTDIR)\osinterface.sbr" \
	"$(INTDIR)\siscertificatechain.sbr" \
	"$(INTDIR)\sisfiledata.sbr" \
	"$(INTDIR)\swicertstore.sbr"

"$(OUTDIR)\dumpinstallfilestatustool.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=libcp.lib oldnames.lib kernel32.lib user32.lib gdi32.lib version.lib libeay32.lib zlib.lib seclib.lib Shlwapi.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\dumpinstallfilestatustool.pdb" /map:"$(INTDIR)\dumpinstallfilestatustool.map" /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\dumpinstallfilestatustool.exe" /libpath:"\epoc32\release\winc\rel" /libpath:"\epoc32\tools" /libpath:"..\..\..\..\openssllib\import\bin\rel" /libpath:"\epoc32\release\tools\rel" /opt:noref 
LINK32_OBJS= \
	"$(INTDIR)\certificatechain.obj" \
	"$(INTDIR)\dumpchainvaliditytool.obj" \
	"$(INTDIR)\dumpinstallfilestatustool.obj" \
	"$(INTDIR)\launchprocess.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\osinterface.obj" \
	"$(INTDIR)\siscertificatechain.obj" \
	"$(INTDIR)\sisfiledata.obj" \
	"$(INTDIR)\swicertstore.obj"

"$(EPOCROOT)Epoc32\release\winc\deb\dumpinstallfilestatustool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<
	
	copy "$(INTDIR)\dumpinstallfilestatustool.exe" "$(EPOCROOT)Epoc32\release\winc\deb\dumpinstallfilestatustool.exe"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("dumpinstallfilestatustool.dep")
!INCLUDE "dumpinstallfilestatustool.dep"
!ELSE 
!MESSAGE Warning: cannot find "dumpinstallfilestatustool.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dumpinstallfilestatustool - Win32 Release" || "$(CFG)" == "dumpinstallfilestatustool - Win32 Debug"
SOURCE=..\common\certificatechain.cpp

!IF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Release"


"$(INTDIR)\certificatechain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Debug"


"$(INTDIR)\certificatechain.obj"	"$(INTDIR)\certificatechain.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\common\dumpchainvaliditytool.cpp

!IF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Release"


"$(INTDIR)\dumpchainvaliditytool.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Debug"


"$(INTDIR)\dumpchainvaliditytool.obj"	"$(INTDIR)\dumpchainvaliditytool.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\dumpinstallfilestatustool.cpp

!IF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Release"


"$(INTDIR)\dumpinstallfilestatustool.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Debug"


"$(INTDIR)\dumpinstallfilestatustool.obj"	"$(INTDIR)\dumpinstallfilestatustool.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\common\launchprocess.cpp

!IF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Release"


"$(INTDIR)\launchprocess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Debug"


"$(INTDIR)\launchprocess.obj"	"$(INTDIR)\launchprocess.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\options.cpp

!IF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Release"


"$(INTDIR)\options.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Debug"


"$(INTDIR)\options.obj"	"$(INTDIR)\options.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\..\common\osinterface.cpp

!IF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Release"


"$(INTDIR)\osinterface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Debug"


"$(INTDIR)\osinterface.obj"	"$(INTDIR)\osinterface.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\common\siscertificatechain.cpp

!IF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Release"


"$(INTDIR)\siscertificatechain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Debug"


"$(INTDIR)\siscertificatechain.obj"	"$(INTDIR)\siscertificatechain.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\sisfiledata.cpp

!IF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Release"


"$(INTDIR)\sisfiledata.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Debug"


"$(INTDIR)\sisfiledata.obj"	"$(INTDIR)\sisfiledata.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\common\swicertstore.cpp

!IF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Release"


"$(INTDIR)\swicertstore.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dumpinstallfilestatustool - Win32 Debug"


"$(INTDIR)\swicertstore.obj"	"$(INTDIR)\swicertstore.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

