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
REM testcase-interpretsis-05
REM testcase testcase-interpretsis-05 - Install sis. This is a positive testcase 

if not exist .\cdrive mkdir .\cdrive > NUL
if not exist .\romdrive mkdir .\romdrive > NUL

REM xcopy /E /Y backup\cdrive cdrive > NUL
REM xcopy /E /Y backup\romdrive romdrive > NUL


REM Remove installation files from any previous tests
if exist  .\cdrive\Documents\InstTest\devlangsup*.txt  del .\cdrive\Documents\InstTest\devlangsup*.txt  > NUL
if not exist .\romdrive\sys\install\scr\provisioned mkdir .\romdrive\sys\install\scr\provisioned > NUL
if exist  .\romdrive\sys\install\scr\provisioned\scr.db  del .\romdrive\sys\install\scr\provisioned\scr.db  > NUL
copy \epoc32\release\winscw\udeb\z\sys\install\scr\provisioned\scr.db .\romdrive\sys\install\scr\provisioned\scr.db /Y > NUL

call interpretsis -z .\romdrive -c .\cdrive  -a /epoc32/release/winscw/udeb/z/private/10003a3f/test -w info -l /epoc32/winscw/c/interpretsis_test_harness_db.txt > NUL

if not exist \epoc32\winscw\c\sys\install\scr mkdir \epoc32\winscw\c\sys\install\scr > NUL
if exist  \epoc32\winscw\c\sys\install\scr\scr.db  del \epoc32\winscw\c\sys\install\scr\scr.db  > NUL
copy .\romdrive\sys\install\scr\provisioned\scr.db \epoc32\winscw\c\sys\install\scr\scr.db /Y > NUL

copy \epoc32\release\winscw\udeb\z\private\10003a3f\test\tstap*_reg.rsc \epoc32\release\winscw\udeb\z\private\10003a3f\apps /Y > NUL

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
