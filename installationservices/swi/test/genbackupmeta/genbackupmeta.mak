# Microsoft Developer Studio Generated NMAKE File, Based on genbackupmeta.dsp
!IF "$(CFG)" == ""
CFG=genbackupmeta - Win32 Debug
!MESSAGE No configuration specified. Defaulting to genbackupmeta - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "genbackupmeta - Win32 Release" && "$(CFG)" != "genbackupmeta - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "genbackupmeta - Win32 Release"

OUTDIR=.\Release
POLICYFILEDIR=.
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(EPOCROOT)Epoc32\release\winc\deb\genbackupmeta.exe"


CLEAN :
	-@erase "$(INTDIR)\cmdparser.obj"
	-@erase "$(INTDIR)\details.obj"
	-@erase "$(INTDIR)\exceptionhandler.obj"
	-@erase "$(INTDIR)\filecontents.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\metagenerator.obj"
	-@erase "$(INTDIR)\preamble.obj"
	-@erase "$(INTDIR)\siscompressed.obj"
	-@erase "$(INTDIR)\streamwriter.obj"
	-@erase "$(INTDIR)\tlv.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(EPOCROOT)Epoc32\release\winc\deb\genbackupmeta.exe"


"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I ".\source\common" /I ".\inc" /I "$(EPOCROOT)epoc32\include" /I "..\..\..\..\secureswitools\swianalysistoolkit\source\common" /I "..\..\inc\swi" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\genbackupmeta.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=$(EPOCROOT)EPOC32\RELEASE\WINC\REL\zlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\genbackupmeta.pdb" /machine:I386 /out:"$(OUTDIR)\genbackupmeta.exe" /libpath:"\epoc32\release\winc\rel" /libpath:"\epoc32\release\tools\rel" /libpath:"\epoc32\tools" 
LINK32_OBJS= \
	"$(INTDIR)\cmdparser.obj" \
	"$(INTDIR)\details.obj" \
	"$(INTDIR)\exceptionhandler.obj" \
	"$(INTDIR)\filecontents.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\metagenerator.obj" \
	"$(INTDIR)\preamble.obj" \
	"$(INTDIR)\siscompressed.obj" \
	"$(INTDIR)\streamwriter.obj" \
	"$(INTDIR)\tlv.obj" \
	"$(INTDIR)\utils.obj"

"$(EPOCROOT)Epoc32\release\winc\deb\genbackupmeta.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

	
   	copy "$(INTDIR)\genbackupmeta.exe" "$(EPOCROOT)Epoc32\release\winc\deb\genbackupmeta.exe"

!ELSEIF  "$(CFG)" == "genbackupmeta - Win32 Debug"

OUTDIR=.\Debug
POLICYFILEDIR=.
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(EPOCROOT)Epoc32\release\winc\deb\genbackupmeta.exe" "$(OUTDIR)\genbackupmeta.bsc"


CLEAN :
	-@erase "$(INTDIR)\cmdparser.obj"
	-@erase "$(INTDIR)\cmdparser.sbr"
	-@erase "$(INTDIR)\details.obj"
	-@erase "$(INTDIR)\details.sbr"
	-@erase "$(INTDIR)\exceptionhandler.obj"
	-@erase "$(INTDIR)\exceptionhandler.sbr"
	-@erase "$(INTDIR)\filecontents.obj"
	-@erase "$(INTDIR)\filecontents.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\metagenerator.obj"
	-@erase "$(INTDIR)\metagenerator.sbr"
	-@erase "$(INTDIR)\preamble.obj"
	-@erase "$(INTDIR)\preamble.sbr"
	-@erase "$(INTDIR)\siscompressed.obj"
	-@erase "$(INTDIR)\siscompressed.sbr"
	-@erase "$(INTDIR)\streamwriter.obj"
	-@erase "$(INTDIR)\streamwriter.sbr"
	-@erase "$(INTDIR)\tlv.obj"
	-@erase "$(INTDIR)\tlv.sbr"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\utils.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\genbackupmeta.bsc"
	-@erase "$(OUTDIR)\genbackupmeta.exe"
	-@erase "$(OUTDIR)\genbackupmeta.ilk"
	-@erase "$(OUTDIR)\genbackupmeta.pdb"
	-@erase "$(EPOCROOT)Epoc32\release\winc\deb\genbackupmeta.exe"
	-@erase "$(OUTDIR)\genbackupmeta.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I ".\inc" /I "$(EPOCROOT)epoc32\include" /I "..\..\..\..\secureswitools\swianalysistoolkit\source\common" /I "..\..\inc\swi" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\genbackupmeta.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\cmdparser.sbr" \
	"$(INTDIR)\details.sbr" \
	"$(INTDIR)\exceptionhandler.sbr" \
	"$(INTDIR)\filecontents.sbr" \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\metagenerator.sbr" \
	"$(INTDIR)\preamble.sbr" \
	"$(INTDIR)\siscompressed.sbr" \
	"$(INTDIR)\streamwriter.sbr" \
	"$(INTDIR)\tlv.sbr" \
	"$(INTDIR)\utils.sbr"

"$(OUTDIR)\genbackupmeta.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(EPOCROOT)EPOC32\RELEASE\WINC\REL\zlib.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\genbackupmeta.pdb" /debug /machine:I386 /nodefaultlib:"libc" /out:"$(OUTDIR)\genbackupmeta.exe" /pdbtype:sept /libpath:"\epoc32\release\winc\rel" /libpath:"\epoc32\release\tools\rel" /libpath:"\epoc32\tools" 
LINK32_OBJS= \
	"$(INTDIR)\cmdparser.obj" \
	"$(INTDIR)\details.obj" \
	"$(INTDIR)\exceptionhandler.obj" \
	"$(INTDIR)\filecontents.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\metagenerator.obj" \
	"$(INTDIR)\preamble.obj" \
	"$(INTDIR)\siscompressed.obj" \
	"$(INTDIR)\streamwriter.obj" \
	"$(INTDIR)\tlv.obj" \
	"$(INTDIR)\utils.obj"

"$(EPOCROOT)Epoc32\release\winc\deb\genbackupmeta.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<
	
	copy "$(INTDIR)\genbackupmeta.exe" "$(EPOCROOT)Epoc32\release\winc\deb\genbackupmeta.exe"

!ENDIF 

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


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("genbackupmeta.dep")
!INCLUDE "genbackupmeta.dep"
!ELSE 
!MESSAGE Warning: cannot find "genbackupmeta.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "genbackupmeta - Win32 Release" || "$(CFG)" == "genbackupmeta - Win32 Debug"
SOURCE=.\source\cmdparser.cpp

!IF  "$(CFG)" == "genbackupmeta - Win32 Release"


"$(INTDIR)\cmdparser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "genbackupmeta - Win32 Debug"


"$(INTDIR)\cmdparser.obj"	"$(INTDIR)\cmdparser.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\source\details.cpp

!IF  "$(CFG)" == "genbackupmeta - Win32 Release"


"$(INTDIR)\details.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "genbackupmeta - Win32 Debug"


"$(INTDIR)\details.obj"	"$(INTDIR)\details.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\source\exceptionhandler.cpp

!IF  "$(CFG)" == "genbackupmeta - Win32 Release"


"$(INTDIR)\exceptionhandler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "genbackupmeta - Win32 Debug"


"$(INTDIR)\exceptionhandler.obj"	"$(INTDIR)\exceptionhandler.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\source\filecontents.cpp

!IF  "$(CFG)" == "genbackupmeta - Win32 Release"


"$(INTDIR)\filecontents.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "genbackupmeta - Win32 Debug"


"$(INTDIR)\filecontents.obj"	"$(INTDIR)\filecontents.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\source\main.cpp

!IF  "$(CFG)" == "genbackupmeta - Win32 Release"


"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "genbackupmeta - Win32 Debug"


"$(INTDIR)\main.obj"	"$(INTDIR)\main.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\source\metagenerator.cpp

!IF  "$(CFG)" == "genbackupmeta - Win32 Release"


"$(INTDIR)\metagenerator.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "genbackupmeta - Win32 Debug"


"$(INTDIR)\metagenerator.obj"	"$(INTDIR)\metagenerator.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\source\preamble.cpp

!IF  "$(CFG)" == "genbackupmeta - Win32 Release"


"$(INTDIR)\preamble.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "genbackupmeta - Win32 Debug"


"$(INTDIR)\preamble.obj"	"$(INTDIR)\preamble.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\source\siscompressed.cpp

!IF  "$(CFG)" == "genbackupmeta - Win32 Release"


"$(INTDIR)\siscompressed.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "genbackupmeta - Win32 Debug"


"$(INTDIR)\siscompressed.obj"	"$(INTDIR)\siscompressed.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\source\streamwriter.cpp

!IF  "$(CFG)" == "genbackupmeta - Win32 Release"


"$(INTDIR)\streamwriter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "genbackupmeta - Win32 Debug"


"$(INTDIR)\streamwriter.obj"	"$(INTDIR)\streamwriter.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\source\tlv.cpp

!IF  "$(CFG)" == "genbackupmeta - Win32 Release"


"$(INTDIR)\tlv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "genbackupmeta - Win32 Debug"


"$(INTDIR)\tlv.obj"	"$(INTDIR)\tlv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\source\utils.cpp

!IF  "$(CFG)" == "genbackupmeta - Win32 Release"


"$(INTDIR)\utils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "genbackupmeta - Win32 Debug"


"$(INTDIR)\utils.obj"	"$(INTDIR)\utils.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

