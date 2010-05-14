@rem
@rem Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

REM Copy the files required to run the script
del /s /f /q \epoc32\release\winscw\udeb\z\sys\bin\tswisidupgradeexe.exe

del /s /f /q \epoc32\release\winscw\urel\z\sys\bin\tswisidupgradeexe.exe

del /s /f /q \epoc32\winscw\c\tswi\tbackuprestore\backup*.*
del /s /f /q \epoc32\winscw\c\tswi\tbackuprestore\metadata*

copy /y \epoc32\release\winscw\udeb\z\resource\temp\swicertstore.dat \epoc32\release\winscw\udeb\z\resource\swicertstore.dat
copy /y \epoc32\release\winscw\urel\z\resource\temp\swicertstore.dat \epoc32\release\winscw\urel\z\resource\swicertstore.dat


rem cleanup anything from previous tests.
RMDIR /s /q \epoc32\release\winscw\udeb\z\resource\temp
del /f /q \epoc32\winscw\c\sys\install\scr\scr.*
del /s /f /q \epoc32\winscw\c\sys\install\sisregistry\802730A0
del /s /f /q \epoc32\winscw\c\sys\install\sisregistry\80000077
del /s /f /q \epoc32\winscw\c\sys\install\sisregistry\80000076
del /s /f /q \epoc32\winscw\c\sys\install\sisregistry\
del /s /f /q \epoc32\winscw\c\sys\bin\
del /s /f /q \epoc32\winscw\c\sys\hash\
