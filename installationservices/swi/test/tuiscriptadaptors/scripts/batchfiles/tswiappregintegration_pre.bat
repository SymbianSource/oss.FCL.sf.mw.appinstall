@rem
@rem Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of the License "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem

rem Copy the list of files to Z in winscw
md \epoc32\release\winscw\udeb\z\private\10003a3f\import\apps
md \epoc32\release\winscw\urel\z\private\10003a3f\import\apps
copy /y \epoc32\release\winscw\udeb\swiappregintegration_base01.exe									\epoc32\release\winscw\udeb\z\sys\bin\swiappregintegration_base01.exe
copy /y \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\data\swiappregintegration_base01\swiappregintegration_base01_reg.rsc	\epoc32\release\winscw\udeb\z\private\10003a3f\import\apps\swiappregintegration_base01_reg.rsc
copy /y \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\data\swiappregintegration_base01\swiappregintegration_base01_loc.rsc	\epoc32\release\winscw\udeb\z\resource\apps\swiappregintegration_base01_loc.rsc
copy /y \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\data\swiappregintegration01\swiappregintegration.mbm			\epoc32\release\winscw\udeb\z\resource\apps\swiappregintegration_base01_rsc.mbm
copy /y \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\data\swiappregintegration_base01\swiappregintegration_base01_loc.r01	\epoc32\release\winscw\udeb\z\resource\apps\swiappregintegration_base01_loc.r01
copy /y \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\data\swiappregintegration01\swiappregintegration.mbm			\epoc32\release\winscw\udeb\z\resource\apps\swiappregintegration_base01_01.mbm
copy /y \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\data\swiappregintegration_base01\swiappregintegration_base01_loc.r02	\epoc32\release\winscw\udeb\z\resource\apps\swiappregintegration_base01_loc.r02
copy /y \epoc32\release\winscw\udeb\z\tswi\tuiscriptadaptors\data\swiappregintegration01\swiappregintegration.mbm			\epoc32\release\winscw\udeb\z\resource\apps\swiappregintegration_base01_02.mbm

copy /y \epoc32\release\winscw\urel\swiappregintegration_base01.exe									\epoc32\release\winscw\urel\z\sys\bin\swiappregintegration_base01.exe
copy /y \epoc32\release\winscw\urel\z\tswi\tuiscriptadaptors\data\swiappregintegration_base01\swiappregintegration_base01_reg.rsc	\epoc32\release\winscw\urel\z\private\10003a3f\import\apps\swiappregintegration_base01_reg.rsc
copy /y \epoc32\release\winscw\urel\z\tswi\tuiscriptadaptors\data\swiappregintegration_base01\swiappregintegration_base01_loc.rsc	\epoc32\release\winscw\urel\z\resource\apps\swiappregintegration_base01_loc.rsc
copy /y \epoc32\release\winscw\urel\z\tswi\tuiscriptadaptors\data\swiappregintegration01\swiappregintegration.mbm			\epoc32\release\winscw\urel\z\resource\apps\swiappregintegration_base01_rsc.mbm
copy /y \epoc32\release\winscw\urel\z\tswi\tuiscriptadaptors\data\swiappregintegration_base01\swiappregintegration_base01_loc.r01	\epoc32\release\winscw\urel\z\resource\apps\swiappregintegration_base01_loc.r01
copy /y \epoc32\release\winscw\urel\z\tswi\tuiscriptadaptors\data\swiappregintegration01\swiappregintegration.mbm			\epoc32\release\winscw\urel\z\resource\apps\swiappregintegration_base01_01.mbm
copy /y \epoc32\release\winscw\urel\z\tswi\tuiscriptadaptors\data\swiappregintegration_base01\swiappregintegration_base01_loc.r02	\epoc32\release\winscw\urel\z\resource\apps\swiappregintegration_base01_loc.r02
copy /y \epoc32\release\winscw\urel\z\tswi\tuiscriptadaptors\data\swiappregintegration01\swiappregintegration.mbm			\epoc32\release\winscw\urel\z\resource\apps\swiappregintegration_base01_02.mbm

rem Copy the stub sis file

copy /y \epoc32\winscw\c\tswi\tuiscriptadaptors\scripts\swiappregintegration_base01_stub.sis \epoc32\release\winscw\udeb\z\system\install\swiappregintegration_base01_stub.sis
copy /y \epoc32\winscw\c\tswi\tuiscriptadaptors\scripts\swiappregintegration_base01_stub.sis \epoc32\release\winscw\urel\z\system\install\swiappregintegration_base01_stub.sis

del /s /f /q \epoc32\winscw\c\sys\install
del /s /f /q \epoc32\winscw\c\sys\bin
del /s /f /q \epoc32\winscw\c\sys\hash
del /s /f /q \epoc32\winscw\c\resource\apps
del /s /f /q \epoc32\winscw\c\private\10003a3f\import\apps