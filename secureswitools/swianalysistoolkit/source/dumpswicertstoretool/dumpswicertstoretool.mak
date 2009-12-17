# Microsoft Developer Studio Generated NMAKE File, Based on dumpswicertstoretool.dsp
!IF "$(CFG)" == ""
CFG=dumpswicertstoretool - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dumpswicertstoretool - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dumpswicertstoretool - Win32 Release" && "$(CFG)" != "dumpswicertstoretool - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dumpswicertstoretool.mak" CFG="dumpswicertstoretool - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dumpswicertstoretool - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dumpswicertstoretool - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "dumpswicertstoretool - Win32 Release"

POLICYFILEDIR=.
OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(EPOCROOT)Epoc32\tools\dumpswicertstoretool.exe"


CLEAN :
	-@erase "$(INTDIR)\certificate.obj"
	-@erase "$(INTDIR)\datalocator.obj"
	-@erase "$(INTDIR)\dumpswicertstoretool.obj"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\osinterface.obj"
	-@erase "$(INTDIR)\pfsdump.obj"
	-@erase "$(INTDIR)\streamreader.obj"
	-@erase "$(INTDIR)\swicertstoremetadata.obj"
	-@erase "$(INTDIR)\ucmp.obj"
	-@erase "$(INTDIR)\utf8.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dumpswicertstoretool.exe"
	-@erase "$(EPOCROOT)Epoc32\tools\dumpswicertstoretool.exe" 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\testcert" /I ".\\" /I "..\common" /I "..\common\utf8" /I "$(EPOCROOT)epoc32\include" /I "..\..\..\openssllib\import\inc" /I "..\..\..\swisistools\source\makesis" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "DUMPSWICERTSTORETOOL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dumpswicertstoretool.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dumpswicertstoretool.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcp.lib oldnames.lib kernel32.lib user32.lib gdi32.lib version.lib libeay32.lib zlib.lib seclib.lib Shlwapi.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\dumpswicertstoretool.pdb" /map:"$(INTDIR)\dumpswicertstoretool.map" /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\dumpswicertstoretool.exe" /libpath:"\epoc32\release\winc\rel" /libpath:"\epoc32\tools" /libpath:"..\..\..\openssllib\import\bin\rel" /libpath:"\epoc32\release\tools\rel" /opt:noref  
LINK32_OBJS= \
	"$(INTDIR)\certificate.obj" \
	"$(INTDIR)\datalocator.obj" \
	"$(INTDIR)\dumpswicertstoretool.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\osinterface.obj" \
	"$(INTDIR)\pfsdump.obj" \
	"$(INTDIR)\streamreader.obj" \
	"$(INTDIR)\swicertstoremetadata.obj" \
	"$(INTDIR)\ucmp.obj" \
	"$(INTDIR)\utf8.obj"

"$(EPOCROOT)Epoc32\tools\dumpswicertstoretool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<
 
 copy "$(INTDIR)\dumpswicertstoretool.exe" "$(EPOCROOT)Epoc32\tools\dumpswicertstoretool.exe"

!ELSEIF  "$(CFG)" == "dumpswicertstoretool - Win32 Debug"

POLICYFILEDIR=.
OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(EPOCROOT)Epoc32\release\winc\deb\dumpswicertstoretool.exe" "$(OUTDIR)\dumpswicertstoretool.bsc"


CLEAN :
	-@erase "$(INTDIR)\certificate.obj"
	-@erase "$(INTDIR)\certificate.sbr"
	-@erase "$(INTDIR)\datalocator.obj"
	-@erase "$(INTDIR)\datalocator.sbr"
	-@erase "$(INTDIR)\dumpswicertstoretool.obj"
	-@erase "$(INTDIR)\dumpswicertstoretool.sbr"
	-@erase "$(INTDIR)\options.obj"
	-@erase "$(INTDIR)\options.sbr"
	-@erase "$(INTDIR)\osinterface.obj"
	-@erase "$(INTDIR)\osinterface.sbr"
	-@erase "$(INTDIR)\pfsdump.obj"
	-@erase "$(INTDIR)\pfsdump.sbr"
	-@erase "$(INTDIR)\streamreader.obj"
	-@erase "$(INTDIR)\streamreader.sbr"
	-@erase "$(INTDIR)\swicertstoremetadata.obj"
	-@erase "$(INTDIR)\swicertstoremetadata.sbr"
	-@erase "$(INTDIR)\ucmp.obj"
	-@erase "$(INTDIR)\ucmp.sbr"
	-@erase "$(INTDIR)\utf8.obj"
	-@erase "$(INTDIR)\utf8.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dumpswicertstoretool.bsc"
	-@erase "$(OUTDIR)\dumpswicertstoretool.exe"
	-@erase "$(OUTDIR)\dumpswicertstoretool.ilk"
	-@erase "$(OUTDIR)\dumpswicertstoretool.map"
	-@erase "$(OUTDIR)\dumpswicertstoretool.pdb"
	-@erase "$(EPOCROOT)Epoc32\release\winc\deb\dumpswicertstoretool.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\testcert" /I ".\\" /I "..\common" /I "..\common\utf8" /I "$(EPOCROOT)epoc32\include" /I "..\..\..\openssllib\import\inc" /I "..\..\..\swisistools\source\makesis" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "DUMPSWICERTSTORETOOL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dumpswicertstoretool.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dumpswicertstoretool.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\certificate.sbr" \
	"$(INTDIR)\datalocator.sbr" \
	"$(INTDIR)\dumpswicertstoretool.sbr" \
	"$(INTDIR)\options.sbr" \
	"$(INTDIR)\osinterface.sbr" \
	"$(INTDIR)\pfsdump.sbr" \
	"$(INTDIR)\streamreader.sbr" \
	"$(INTDIR)\swicertstoremetadata.sbr" \
	"$(INTDIR)\ucmp.sbr" \
	"$(INTDIR)\utf8.sbr"

"$(OUTDIR)\dumpswicertstoretool.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=libcp.lib oldnames.lib kernel32.lib user32.lib gdi32.lib version.lib libeay32.lib zlib.lib seclib.lib Shlwapi.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\dumpswicertstoretool.pdb" /map:"$(INTDIR)\dumpswicertstoretool.map" /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\dumpswicertstoretool.exe" /libpath:"\epoc32\release\winc\rel" /libpath:"\epoc32\tools" /libpath:"..\..\..\openssllib\import\bin\rel" /libpath:"\epoc32\release\tools\rel" /opt:noref 
LINK32_OBJS= \
	"$(INTDIR)\certificate.obj" \
	"$(INTDIR)\datalocator.obj" \
	"$(INTDIR)\dumpswicertstoretool.obj" \
	"$(INTDIR)\options.obj" \
	"$(INTDIR)\osinterface.obj" \
	"$(INTDIR)\pfsdump.obj" \
	"$(INTDIR)\streamreader.obj" \
	"$(INTDIR)\swicertstoremetadata.obj" \
	"$(INTDIR)\ucmp.obj" \
	"$(INTDIR)\utf8.obj"

"$(EPOCROOT)Epoc32\release\winc\deb\dumpswicertstoretool.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<
 
 copy "$(INTDIR)\dumpswicertstoretool.exe" "$(EPOCROOT)Epoc32\release\winc\deb\dumpswicertstoretool.exe"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("dumpswicertstoretool.dep")
!INCLUDE "dumpswicertstoretool.dep"
!ELSE 
!MESSAGE Warning: cannot find "dumpswicertstoretool.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dumpswicertstoretool - Win32 Release" || "$(CFG)" == "dumpswicertstoretool - Win32 Debug"
SOURCE=.\certificate.cpp

!IF  "$(CFG)" == "dumpswicertstoretool - Win32 Release"


"$(INTDIR)\certificate.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dumpswicertstoretool - Win32 Debug"


"$(INTDIR)\certificate.obj"	"$(INTDIR)\certificate.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\datalocator.cpp

!IF  "$(CFG)" == "dumpswicertstoretool - Win32 Release"


"$(INTDIR)\datalocator.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dumpswicertstoretool - Win32 Debug"


"$(INTDIR)\datalocator.obj"	"$(INTDIR)\datalocator.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dumpswicertstoretool.cpp

!IF  "$(CFG)" == "dumpswicertstoretool - Win32 Release"


"$(INTDIR)\dumpswicertstoretool.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dumpswicertstoretool - Win32 Debug"


"$(INTDIR)\dumpswicertstoretool.obj"	"$(INTDIR)\dumpswicertstoretool.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\options.cpp

!IF  "$(CFG)" == "dumpswicertstoretool - Win32 Release"


"$(INTDIR)\options.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dumpswicertstoretool - Win32 Debug"


"$(INTDIR)\options.obj"	"$(INTDIR)\options.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\common\osinterface.cpp

!IF  "$(CFG)" == "dumpswicertstoretool - Win32 Release"


"$(INTDIR)\osinterface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dumpswicertstoretool - Win32 Debug"


"$(INTDIR)\osinterface.obj"	"$(INTDIR)\osinterface.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pfsdump.cpp

!IF  "$(CFG)" == "dumpswicertstoretool - Win32 Release"


"$(INTDIR)\pfsdump.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dumpswicertstoretool - Win32 Debug"


"$(INTDIR)\pfsdump.obj"	"$(INTDIR)\pfsdump.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\common\streamreader.cpp

!IF  "$(CFG)" == "dumpswicertstoretool - Win32 Release"


"$(INTDIR)\streamreader.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dumpswicertstoretool - Win32 Debug"


"$(INTDIR)\streamreader.obj"	"$(INTDIR)\streamreader.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\swicertstoremetadata.cpp

!IF  "$(CFG)" == "dumpswicertstoretool - Win32 Release"


"$(INTDIR)\swicertstoremetadata.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dumpswicertstoretool - Win32 Debug"


"$(INTDIR)\swicertstoremetadata.obj"	"$(INTDIR)\swicertstoremetadata.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\common\ucmp.cpp

!IF  "$(CFG)" == "dumpswicertstoretool - Win32 Release"


"$(INTDIR)\ucmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dumpswicertstoretool - Win32 Debug"


"$(INTDIR)\ucmp.obj"	"$(INTDIR)\ucmp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\common\utf8\utf8.cpp

!IF  "$(CFG)" == "dumpswicertstoretool - Win32 Release"


"$(INTDIR)\utf8.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dumpswicertstoretool - Win32 Debug"


"$(INTDIR)\utf8.obj"	"$(INTDIR)\utf8.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

