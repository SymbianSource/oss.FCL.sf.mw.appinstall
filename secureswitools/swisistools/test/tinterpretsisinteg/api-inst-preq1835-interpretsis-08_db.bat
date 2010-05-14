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
REM preq1835-interpretsis-08
REM PREQ1835 preq1835-interpretsis-08 - Verify if ROM SIS STUB and Pre-Provisioned registration files are processed. This is a positive testcase 

if not exist .\cdrive mkdir .\cdrive > NUL
if not exist .\romdrive mkdir .\romdrive > NUL
if not exist .\romdrive\private mkdir .\romdrive\private > NUL
if not exist .\romdrive\private\10003a3f mkdir .\romdrive\private\10003a3f > NUL
if not exist .\romdrive\private\10003a3f\import mkdir .\romdrive\private\10003a3f\import > NUL
if not exist .\romdrive\private\10003a3f\import\apps mkdir .\romdrive\private\10003a3f\import\apps > NUL
if not exist .\romdrive\private\10003a3f\apps mkdir .\romdrive\private\10003a3f\apps > NUL
if not exist .\romdrive\resource mkdir .\romdrive\resource > NUL
if not exist .\romdrive\resource\apps mkdir .\romdrive\resource\apps > NUL

del .\backup\romdrive\sys\install\scr\provisioned\scr.db

xcopy /E /Y backup\cdrive cdrive > NUL
xcopy /E /Y backup\romdrive romdrive > NUL

copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_preq1835_stub.sis .\romdrive\system\install\interpretsis_preq1835_stub.sis /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_preq1835_01_reg.rsc .\romdrive\private\10003a3f\import\apps\interpretsis_preq1835_01_reg.rsc /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_preq1835_02_reg.rsc .\romdrive\private\10003a3f\apps\interpretsis_preq1835_02_reg.rsc /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_preq1835_01.r01 .\romdrive\resource\apps\interpretsis_preq1835_01.r01 /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_preq1835_01.r03 .\romdrive\resource\apps\interpretsis_preq1835_01.r03 /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_preq1835_01.r04 .\romdrive\resource\apps\interpretsis_preq1835_01.r04 /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_preq1835_01.r24 .\romdrive\resource\apps\interpretsis_preq1835_01.r24 /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_preq1835_02.r03 .\romdrive\resource\apps\interpretsis_preq1835_02.r03 /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_preq183501.mbm .\romdrive\resource\apps\interpretsis_preq183501.mbm /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_preq183503.mbm .\romdrive\resource\apps\interpretsis_preq183503.mbm /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_preq183504.mbm .\romdrive\resource\apps\interpretsis_preq183504.mbm /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_preq183524.mbm .\romdrive\resource\apps\interpretsis_preq183524.mbm /Y > NUL

REM Remove installation files from any previous tests
if exist  .\cdrive\Documents\InstTest\devlangsup*.txt  del .\cdrive\Documents\InstTest\devlangsup*.txt  > NUL

copy \epoc32\release\winscw\udeb\z\sys\install\scr\provisioned\scr.db .\romdrive\sys\install\scr\provisioned\scr.db /Y > NUL

call interpretsis -z .\romdrive -c .\cdrive  -s /epoc32/winscw/c/tswi/tsis/data/interpretsis_preq1835_08.sis -w info -I /epoc32/winscw/c/tswi/tinterpretsisinteg/preq1835-interpretsis-01/argumentfile_05.ini  -l /epoc32/winscw/c/interpretsis_test_harness_db.txt > NUL

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
