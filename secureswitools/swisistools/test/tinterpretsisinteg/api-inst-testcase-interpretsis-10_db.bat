@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
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
@ECHO OFF
REM testcase-interpretsis-10
REM testcase testcase-interpretsis-10 - Verify that scr panic do not happens with corrupt scr.db.

if not exist .\cdrive mkdir .\cdrive > NUL
if not exist .\romdrive mkdir .\romdrive > NUL
if not exist .\romdrive\system mkdir .\romdrive\system > NUL
if not exist .\romdrive\system\install mkdir .\romdrive\system\install > NUL

REM xcopy /E /Y backup\cdrive cdrive > NUL
REM xcopy /E /Y backup\romdrive romdrive > NUL


REM Remove installation files from any previous tests
if exist  .\cdrive\Documents\InstTest\devlangsup*.txt  del .\cdrive\Documents\InstTest\devlangsup*.txt  > NUL
if not exist .\romdrive\sys\install\scr\provisioned mkdir .\romdrive\sys\install\scr\provisioned > NUL
if exist  .\romdrive\sys\install\scr\provisioned\scr.db  del .\romdrive\sys\install\scr\provisioned\scr.db  > NUL
copy \epoc32\release\winscw\udeb\z\tusif\tscr\data\scr-corruptdb.db .\romdrive\sys\install\scr\provisioned\scr.db /Y > NUL

call scrtool -d .\romdrive\sys\install\scr\provisioned\scr.db -p \epoc32\winscw\c\tswi\tinterpretsisinteg\preprovision_appproperty_opaquedata.xml > NUL

if not exist \epoc32\winscw\c\sys\install\scr mkdir \epoc32\winscw\c\sys\install\scr > NUL
if exist  \epoc32\winscw\c\sys\install\scr\scr.db  del \epoc32\winscw\c\sys\install\scr\scr.db  > NUL
copy .\romdrive\sys\install\scr\provisioned\scr.db \epoc32\winscw\c\sys\install\scr\scr.db /Y > NUL

IF NOT EXIST .\backup\cdrive mkdir .\backup\cdrive > NUL
IF NOT EXIST .\backup\romdrive mkdir .\backup\romdrive > NUL
xcopy /E /Y cdrive backup\cdrive > NUL
xcopy /E /Y romdrive backup\romdrive > NUL

IF NOT %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS>>/epoc32/winscw/c/interpretsis_test_harness_db.txt
GOTO END
:LAST
ECHO ***ERRORCODE*** %errorlevel% FAIL>>/epoc32/winscw/c/interpretsis_test_harness_db.txt
:END
