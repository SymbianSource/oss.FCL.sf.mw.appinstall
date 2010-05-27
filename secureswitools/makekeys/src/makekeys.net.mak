# Microsoft Developer Studio Generated NMAKE File, Based on makekeys.dsp
!IF "$(CFG)" == ""
CFG=makekeys - Win32 Debug
!MESSAGE No configuration specified. Defaulting to makekeys - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "makekeys - Win32 Release" && "$(CFG)" != "makekeys - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "makekeys.mak" CFG="makekeys - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "makekeys - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "makekeys - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "makekeys - Win32 Release"

OUTDIR=.
INTDIR=.
# Begin Custom Macros
OutDir=.
# End Custom Macros

ALL : "$(OUTDIR)\$(EPOCROOT)Epoc32\tools\makekeys.exe"


CLEAN :
	-@erase "$(INTDIR)\CertificateGenerator.obj"
	-@erase "$(INTDIR)\CertificateGenerator.sbr"
	-@erase "$(INTDIR)\CertificateRequestGenerator.obj"
	-@erase "$(INTDIR)\CertificateRequestGenerator.sbr"
	-@erase "$(INTDIR)\DSAKeyGenerator.obj"
	-@erase "$(INTDIR)\DSAKeyGenerator.sbr"
	-@erase "$(INTDIR)\KeyGenerator.obj"
	-@erase "$(INTDIR)\KeyGenerator.sbr"
	-@erase "$(INTDIR)\MAKEKEYS.obj
	-@erase "$(INTDIR)\MAKEKEYS.sbr
	-@erase "$(INTDIR)\MAKEKEYS.res
	-@erase "$(INTDIR)\PARSECMD.obj
	-@erase "$(INTDIR)\PARSECMD.sbr
	-@erase "$(INTDIR)\RSAKeyGenerator.obj"
	-@erase "$(INTDIR)\RSAKeyGenerator.sbr"
	-@erase "$(INTDIR)\UTILS.OBJ"
	-@erase "$(INTDIR)\UTILS.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\makekeys.ilk"
	-@erase "$(INTDIR)\makekeys.bsc"
	-@erase "$(OUTDIR)\makekeys.pdb"
	-@erase "$(OUTDIR)\makekeys.map"
	-@erase "$(OUTDIR)\$(EPOCROOT)Epoc32\tools\makekeys.exe"


CPP_PROJ=/nologo /MT /W3 /EHsc /O2 /I "..\..\openssllib\import\inc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Y- /FD /c 

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


BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\makekeys.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\MAKEKEYS.sbr" \
	"$(INTDIR)\PARSECMD.sbr" \
	"$(INTDIR)\UTILS.sbr" \
	"$(INTDIR)\CertificateGenerator.sbr" \
	"$(INTDIR)\CertificateRequestGenerator.sbr" \
	"$(INTDIR)\DSAKeyGenerator.sbr" \
	"$(INTDIR)\KeyGenerator.sbr" \
	"$(INTDIR)\RSAKeyGenerator.sbr" \
	
LINK32=link.exe
LINK32_FLAGS=wincrypto.lib libcmt.lib libcpmt.lib oldnames.lib kernel32.lib /nologo /subsystem:console /pdb:none /map:"$(INTDIR)\makekeys.map" /machine:I386 /nodefaultlib /out:"$(EPOCROOT)Epoc32\tools\makekeys.exe" /libpath:"..\..\..\..\..\os\unref\orphan\comgen\security\wincrypto\import\bin\rel" 
LINK32_OBJS= \
	"$(INTDIR)\CertificateGenerator.obj" \
	"$(INTDIR)\CertificateRequestGenerator.obj" \
	"$(INTDIR)\DSAKeyGenerator.obj" \
	"$(INTDIR)\KeyGenerator.obj" \
	"$(INTDIR)\MAKEKEYS.OBJ" \
	"$(INTDIR)\PARSECMD.OBJ" \
	"$(INTDIR)\RSAKeyGenerator.obj" \
	"$(INTDIR)\UTILS.OBJ"

"$(OUTDIR)\$(EPOCROOT)Epoc32\tools\makekeys.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\$(EPOCROOT)Epoc32\tools\makekeys.exe"
	del makekeys.obj
	del makekeys.map
	del makekeys.pch
	del parsecmd.obj
	del utils.obj
	del certificategenerator.obj
	del certificateRequestgenerator.obj
	del dsakeygenerator.obj
	del keygenerator.obj
	del rsakeygenerator.obj
	del vc60.idb

!ELSEIF  "$(CFG)" == "makekeys - Win32 Debug"

OUTDIR=.
INTDIR=.
# Begin Custom Macros
OutDir=.
# End Custom Macros

ALL : "$(OUTDIR)\$(EPOCROOT)Epoc32\release\winc\deb\makekeys.exe"


CLEAN :
	-@erase "$(INTDIR)\CertificateGenerator.obj"
	-@erase "$(INTDIR)\CertificateGenerator.sbr"
	-@erase "$(INTDIR)\CertificateRequestGenerator.obj"
	-@erase "$(INTDIR)\CertificateRequestGenerator.sbr"
	-@erase "$(INTDIR)\DSAKeyGenerator.obj"
	-@erase "$(INTDIR)\DSAKeyGenerator.sbr"
	-@erase "$(INTDIR)\KeyGenerator.obj"
	-@erase "$(INTDIR)\KeyGenerator.sbr"
	-@erase "$(INTDIR)\MAKEKEYS.OBJ"
	-@erase "$(INTDIR)\MAKEKEYS.sbr"
	-@erase "$(INTDIR)\PARSECMD.OBJ"
	-@erase "$(INTDIR)\PARSECMD.sbr"
	-@erase "$(INTDIR)\RSAKeyGenerator.obj"
	-@erase "$(INTDIR)\RSAKeyGenerator.sbr"
	-@erase "$(INTDIR)\UTILS.OBJ"
	-@erase "$(INTDIR)\UTILS.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\$(EPOCROOT)Epoc32\release\winc\deb\makekeys.exe"
	-@erase "$(OUTDIR)\makekeys.ilk"
	-@erase "$(OUTDIR)\makekeys.pdb"
	-@erase "$(INTDIR)\makekeys.bsc"
	-@erase "$(INTDIR)\makekeys.map"

CPP_PROJ=/nologo /MT /W3 /Gm /EHsc /ZI /Od /I "..\..\openssllib\import\inc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Y- /FD /GZ /c 

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


BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\makekeys.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\MAKEKEYS.sbr" \
	"$(INTDIR)\PARSECMD.sbr" \
	"$(INTDIR)\UTILS.sbr" \
	"$(INTDIR)\CertificateGenerator.sbr" \
	"$(INTDIR)\CertificateRequestGenerator.sbr" \
	"$(INTDIR)\DSAKeyGenerator.sbr" \
	"$(INTDIR)\KeyGenerator.sbr" \
	"$(INTDIR)\RSAKeyGenerator.sbr" \
	
LINK32=link.exe
LINK32_FLAGS=wincrypto.lib libcmtd.lib libcpmtd.lib oldnames.lib kernel32.lib /nologo /subsystem:console /pdb:none /machine:I386 /nodefaultlib /out:"$(EPOCROOT)Epoc32\release\winc\deb/makekeys.exe" /libpath:"..\..\..\..\..\os\unref\orphan\comgen\security\wincrypto\import\bin\deb" 
LINK32_OBJS= \
	"$(INTDIR)\CertificateGenerator.obj" \
	"$(INTDIR)\CertificateRequestGenerator.obj" \
	"$(INTDIR)\DSAKeyGenerator.obj" \
	"$(INTDIR)\KeyGenerator.obj" \
	"$(INTDIR)\MAKEKEYS.OBJ" \
	"$(INTDIR)\PARSECMD.OBJ" \
	"$(INTDIR)\RSAKeyGenerator.obj" \
	"$(INTDIR)\UTILS.OBJ"

"$(OUTDIR)\$(EPOCROOT)Epoc32\release\winc\deb\makekeys.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=remove intermediate files
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\$(EPOCROOT)Epoc32\release\winc\deb\makekeys.exe"
	del makekeys.obj
	del makekeys.map
	del makekeys.pch
	del parsecmd.obj
	del utils.obj
	del certificategenerator.obj
	del certificateRequestgenerator.obj
	del dsakeygenerator.obj
	del keygenerator.obj
	del rsakeygenerator.obj
	del vc60.idb
	del vc60.pdb

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
!IF EXISTS("makekeys.dep")
!INCLUDE "makekeys.dep"
!ELSE 
!MESSAGE Warning: cannot find "makekeys.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "makekeys - Win32 Release" || "$(CFG)" == "makekeys - Win32 Debug"
SOURCE=.\MAKEKEYS.CPP

"$(INTDIR)\MAKEKEYS.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PARSECMD.CPP

"$(INTDIR)\PARSECMD.OBJ" : $(SOURCE) "$(INTDIR)"

!ENDIF 

