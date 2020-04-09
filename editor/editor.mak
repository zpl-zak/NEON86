# Microsoft Developer Studio Generated NMAKE File, Based on editor.dsp
!IF "$(CFG)" == ""
CFG=editor - Win32 Debug
!MESSAGE No configuration specified. Defaulting to editor - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "editor - Win32 Release" && "$(CFG)" != "editor - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "editor.mak" CFG="editor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "editor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "editor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "editor - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\editor.exe"

!ELSE 

ALL : "engine - Win32 Release" "$(OUTDIR)\editor.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"engine - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\CntrItem.obj"
	-@erase "$(INTDIR)\editor.obj"
	-@erase "$(INTDIR)\editor.pch"
	-@erase "$(INTDIR)\editor.res"
	-@erase "$(INTDIR)\editorDoc.obj"
	-@erase "$(INTDIR)\editorView.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\editor.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\editor.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\editor.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\editor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\editor.pdb" /machine:I386 /out:"$(OUTDIR)\editor.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\CntrItem.obj" \
	"$(INTDIR)\editor.obj" \
	"$(INTDIR)\editorDoc.obj" \
	"$(INTDIR)\editorView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\editor.res" \
	"..\engine\Release\engine.lib"

"$(OUTDIR)\editor.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\editor.exe" "$(OUTDIR)\editor.bsc"

!ELSE 

ALL : "engine - Win32 Debug" "$(OUTDIR)\editor.exe" "$(OUTDIR)\editor.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"engine - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\ChildFrm.sbr"
	-@erase "$(INTDIR)\CntrItem.obj"
	-@erase "$(INTDIR)\CntrItem.sbr"
	-@erase "$(INTDIR)\editor.obj"
	-@erase "$(INTDIR)\editor.pch"
	-@erase "$(INTDIR)\editor.res"
	-@erase "$(INTDIR)\editor.sbr"
	-@erase "$(INTDIR)\editorDoc.obj"
	-@erase "$(INTDIR)\editorDoc.sbr"
	-@erase "$(INTDIR)\editorView.obj"
	-@erase "$(INTDIR)\editorView.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\editor.bsc"
	-@erase "$(OUTDIR)\editor.exe"
	-@erase "$(OUTDIR)\editor.ilk"
	-@erase "$(OUTDIR)\editor.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\editor.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\editor.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\editor.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ChildFrm.sbr" \
	"$(INTDIR)\CntrItem.sbr" \
	"$(INTDIR)\editor.sbr" \
	"$(INTDIR)\editorDoc.sbr" \
	"$(INTDIR)\editorView.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\editor.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\editor.pdb" /debug /machine:I386 /out:"$(OUTDIR)\editor.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\CntrItem.obj" \
	"$(INTDIR)\editor.obj" \
	"$(INTDIR)\editorDoc.obj" \
	"$(INTDIR)\editorView.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\editor.res" \
	"..\engine\Debug\engine.lib"

"$(OUTDIR)\editor.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("editor.dep")
!INCLUDE "editor.dep"
!ELSE 
!MESSAGE Warning: cannot find "editor.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "editor - Win32 Release" || "$(CFG)" == "editor - Win32 Debug"
SOURCE=.\ChildFrm.cpp

!IF  "$(CFG)" == "editor - Win32 Release"


"$(INTDIR)\ChildFrm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\editor.pch"


!ELSEIF  "$(CFG)" == "editor - Win32 Debug"


"$(INTDIR)\ChildFrm.obj"	"$(INTDIR)\ChildFrm.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\editor.pch"


!ENDIF 

SOURCE=.\CntrItem.cpp

!IF  "$(CFG)" == "editor - Win32 Release"


"$(INTDIR)\CntrItem.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\editor.pch"


!ELSEIF  "$(CFG)" == "editor - Win32 Debug"


"$(INTDIR)\CntrItem.obj"	"$(INTDIR)\CntrItem.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\editor.pch"


!ENDIF 

SOURCE=.\editor.cpp

!IF  "$(CFG)" == "editor - Win32 Release"


"$(INTDIR)\editor.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\editor.pch"


!ELSEIF  "$(CFG)" == "editor - Win32 Debug"


"$(INTDIR)\editor.obj"	"$(INTDIR)\editor.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\editor.pch"


!ENDIF 

SOURCE=.\editor.rc

"$(INTDIR)\editor.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\editorDoc.cpp

!IF  "$(CFG)" == "editor - Win32 Release"


"$(INTDIR)\editorDoc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\editor.pch"


!ELSEIF  "$(CFG)" == "editor - Win32 Debug"


"$(INTDIR)\editorDoc.obj"	"$(INTDIR)\editorDoc.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\editor.pch"


!ENDIF 

SOURCE=.\editorView.cpp

!IF  "$(CFG)" == "editor - Win32 Release"


"$(INTDIR)\editorView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\editor.pch"


!ELSEIF  "$(CFG)" == "editor - Win32 Debug"


"$(INTDIR)\editorView.obj"	"$(INTDIR)\editorView.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\editor.pch"


!ENDIF 

SOURCE=.\MainFrm.cpp

!IF  "$(CFG)" == "editor - Win32 Release"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\editor.pch"


!ELSEIF  "$(CFG)" == "editor - Win32 Debug"


"$(INTDIR)\MainFrm.obj"	"$(INTDIR)\MainFrm.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\editor.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "editor - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\editor.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\editor.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\editor.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\editor.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

!IF  "$(CFG)" == "editor - Win32 Release"

"engine - Win32 Release" : 
   cd "\neon86\engine"
   $(MAKE) /$(MAKEFLAGS) /F .\engine.mak CFG="engine - Win32 Release" 
   cd "..\editor"

"engine - Win32 ReleaseCLEAN" : 
   cd "\neon86\engine"
   $(MAKE) /$(MAKEFLAGS) /F .\engine.mak CFG="engine - Win32 Release" RECURSE=1 CLEAN 
   cd "..\editor"

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

"engine - Win32 Debug" : 
   cd "\neon86\engine"
   $(MAKE) /$(MAKEFLAGS) /F .\engine.mak CFG="engine - Win32 Debug" 
   cd "..\editor"

"engine - Win32 DebugCLEAN" : 
   cd "\neon86\engine"
   $(MAKE) /$(MAKEFLAGS) /F .\engine.mak CFG="engine - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\editor"

!ENDIF 


!ENDIF 

