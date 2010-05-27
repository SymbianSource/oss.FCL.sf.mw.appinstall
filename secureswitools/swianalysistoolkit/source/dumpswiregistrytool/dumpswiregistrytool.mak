# Microsoft Developer Studio Generated NMAKE File, Based on dumpswiregistrytool.dsp
!IF "$(CFG)" == ""
CFG=dumpswiregistrytool - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dumpswiregistrytool - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dumpswiregistrytool - Win32 Release" && "$(CFG)" != "dumpswiregistrytool - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dumpswiregistrytool.mak" CFG="dumpswiregistrytool - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dumpswiregistrytool - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dumpswiregistrytool - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "dumpswiregistrytool - Win32 Release"

POLICYFILEDIR=.
OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(EPOCROOT)Epoc32\tools\dumpswiregistrytool.exe"


CLEAN :
	-@erase "$(INTDIR)\dumpswiregistrytool.obj"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\osinterface.obj"
	-@erase "$(INTDIR)\registrypackage.obj"
	-@erase "$(INTDIR)\registrytoken.obj"
	-@erase "$(INTDIR)\siscontrollerinfo.obj"
	-@erase "$(INTDIR)\sisdependency.obj"
	-@erase "$(INTDIR)\sisembeds.obj"
	-@erase "$(INTDIR)\sisfiledescription.obj"
	-@erase "$(INTDIR)\sisproperty.obj"
	-@erase "$(INTDIR)\streamreader.obj"
	-@erase "$(INTDIR)\ucmp.obj"
	-@erase "$(INTDIR)\utf8.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dumpswiregistrytool.exe"
	-@erase "$(EPOCROOT)Epoc32\tools\dumpswiregistrytool.exe" 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "./" /I "../common"  /I "../common/utf8" /D "_WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "DUMPSWIREGISTRYTOOL" /Fp"$(INTDIR)\dumpswiregistrytool.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dumpswiregistrytool.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Shlwapi.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\dumpswiregistrytool.pdb" /debug /machine:I386 /out:"$(OUTDIR)\dumpswiregistrytool.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\dumpswiregistrytool.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\osinterface.obj" \
	"$(INTDIR)\registrypackage.obj" \
	"$(INTDIR)\registrytoken.obj" \
	"$(INTDIR)\siscontrollerinfo.obj" \
	"$(INTDIR)\sisdependency.obj" \
	"$(INTDIR)\sisembeds.obj" \
	"$(INTDIR)\sisfiledescription.obj" \
	"$(INTDIR)\sisproperty.obj" \
	"$(INTDIR)\streamreader.obj" \
	"$(INTDIR)\ucmp.obj" \
	"$(INTDIR)\utf8.obj"

"$(EPOCROOT)Epoc32\tools\dumpswiregistrytool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<
 
 copy "$(INTDIR)\dumpswiregistrytool.exe" "$(EPOCROOT)Epoc32\tools\dumpswiregistrytool.exe"

!ELSEIF  "$(CFG)" == "dumpswiregistrytool - Win32 Debug"

POLICYFILEDIR=.
OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(EPOCROOT)Epoc32\release\winc\deb\dumpswiregistrytool.exe"


CLEAN :
	-@erase "$(INTDIR)\dumpswiregistrytool.obj"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\osinterface.obj"
	-@erase "$(INTDIR)\registrypackage.obj"
	-@erase "$(INTDIR)\registrytoken.obj"
	-@erase "$(INTDIR)\siscontrollerinfo.obj"
	-@erase "$(INTDIR)\sisdependency.obj"
	-@erase "$(INTDIR)\sisembeds.obj"
	-@erase "$(INTDIR)\sisfiledescription.obj"
	-@erase "$(INTDIR)\sisproperty.obj"
	-@erase "$(INTDIR)\streamreader.obj"
	-@erase "$(INTDIR)\ucmp.obj"
	-@erase "$(INTDIR)\utf8.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dumpswiregistrytool.exe"
	-@erase "$(OUTDIR)\dumpswiregistrytool.ilk"
	-@erase "$(OUTDIR)\dumpswiregistrytool.pdb"
	-@erase "$(EPOCROOT)Epoc32\release\winc\deb\dumpswiregistrytool.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "./" /I "../common" /I "../common/utf8" /D "_WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "DUMPSWIREGISTRYTOOL" /Fp"$(INTDIR)\dumpswiregistrytool.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dumpswiregistrytool.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Shlwapi.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\dumpswiregistrytool.pdb" /debug /machine:I386 /out:"$(OUTDIR)\dumpswiregistrytool.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\dumpswiregistrytool.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\osinterface.obj" \
	"$(INTDIR)\registrypackage.obj" \
	"$(INTDIR)\registrytoken.obj" \
	"$(INTDIR)\siscontrollerinfo.obj" \
	"$(INTDIR)\sisdependency.obj" \
	"$(INTDIR)\sisembeds.obj" \
	"$(INTDIR)\sisfiledescription.obj" \
	"$(INTDIR)\sisproperty.obj" \
	"$(INTDIR)\streamreader.obj" \
	"$(INTDIR)\ucmp.obj" \
	"$(INTDIR)\utf8.obj"

"$(EPOCROOT)Epoc32\release\winc\deb\dumpswiregistrytool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<
 
 copy "$(INTDIR)\dumpswiregistrytool.exe" "$(EPOCROOT)Epoc32\release\winc\deb\dumpswiregistrytool.exe"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("dumpswiregistrytool.dep")
!INCLUDE "dumpswiregistrytool.dep"
!ELSE 
!MESSAGE Warning: cannot find "dumpswiregistrytool.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dumpswiregistrytool - Win32 Release" || "$(CFG)" == "dumpswiregistrytool - Win32 Debug"
SOURCE=.\dumpswiregistrytool.cpp

"$(INTDIR)\dumpswiregistrytool.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\options.cpp

"$(INTDIR)\options.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\common\osinterface.cpp

"$(INTDIR)\osinterface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\registrypackage.cpp

"$(INTDIR)\registrypackage.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\registrytoken.cpp

"$(INTDIR)\registrytoken.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\siscontrollerinfo.cpp

"$(INTDIR)\siscontrollerinfo.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sisdependency.cpp

"$(INTDIR)\sisdependency.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sisembeds.cpp

"$(INTDIR)\sisembeds.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sisfiledescription.cpp

"$(INTDIR)\sisfiledescription.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sisproperty.cpp

"$(INTDIR)\sisproperty.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\common\streamreader.cpp

"$(INTDIR)\streamreader.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Common\ucmp.cpp

"$(INTDIR)\ucmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\Common\utf8\utf8.cpp

"$(INTDIR)\utf8.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

