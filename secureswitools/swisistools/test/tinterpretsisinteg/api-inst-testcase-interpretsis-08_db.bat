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
REM testcase-interpretsis-08
REM testcase testcase-interpretsis-08 - Verify if ROM SIS STUB and Pre-Provisioned registration files are processed. This is a positive testcase 

if not exist .\cdrive mkdir .\cdrive > NUL
if not exist .\cdrive\resource mkdir .\cdrive\resource > NUL
if not exist .\cdrive\resource\apps mkdir .\cdrive\resource\apps > NUL
if not exist .\romdrive mkdir .\romdrive > NUL
if not exist .\romdrive\system mkdir .\romdrive\system > NUL
if not exist .\romdrive\system\install mkdir .\romdrive\system\install > NUL
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

copy \epoc32\release\winscw\udeb\z\sys\install\scr\provisioned\scr.db .\romdrive\sys\install\scr\provisioned\scr.db /Y > NUL

copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_testcase_stub.sis .\romdrive\system\install\interpretsis_testcase_stub.sis /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_testcase_01_reg.rsc .\romdrive\private\10003a3f\import\apps\interpretsis_testcase_01_reg.rsc /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_testcase_02_reg.rsc .\romdrive\private\10003a3f\apps\interpretsis_testcase_02_reg.rsc /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_testcase_01.r01 .\romdrive\resource\apps\interpretsis_testcase_01.r01 /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_testcase_01.r03 .\romdrive\resource\apps\interpretsis_testcase_01.r03 /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_testcase_01.r04 .\romdrive\resource\apps\interpretsis_testcase_01.r04 /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_testcase_01.r24 .\romdrive\resource\apps\interpretsis_testcase_01.r24 /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_testcase_02.r03 .\romdrive\resource\apps\interpretsis_testcase_02.r03 /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_testcase01.mbm .\romdrive\resource\apps\interpretsis_testcase01.mbm /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_testcase03.mbm .\romdrive\resource\apps\interpretsis_testcase03.mbm /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_testcase04.mbm .\romdrive\resource\apps\interpretsis_testcase04.mbm /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_testcase24.mbm .\romdrive\resource\apps\interpretsis_testcase24.mbm /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_testcase01.mbm .\cdrive\resource\apps\interpretsis_testcase01.mbm /Y > NUL
copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsis_testcase03.mbm .\cdrive\resource\apps\interpretsis_testcase03.mbm /Y > NUL

REM Remove installation files from any previous tests
if exist  .\cdrive\Documents\InstTest\devlangsup*.txt  del .\cdrive\Documents\InstTest\devlangsup*.txt  > NUL

call interpretsis -z .\romdrive -c .\cdrive  -s /epoc32/winscw/c/tswi/tsis/data/interpretsis_testcase_08.sis -w info -I /epoc32/winscw/c/tswi/tinterpretsisinteg/testcase-interpretsis-01/argumentfile_05.ini  -l /epoc32/winscw/c/interpretsis_test_harness_db.txt > NUL

copy .\romdrive\sys\install\scr\provisioned\scr.db \epoc32\winscw\c\sys\install\scr\scr.db /Y > NUL

if exist  .\romdrive\system\install\interpretsis_testcase_stub.sis  del .\romdrive\system\install\interpretsis_testcase_stub.sis  > NUL
if exist  .\romdrive\private\10003a3f\import\apps\interpretsis_testcase_01_reg.rsc  del .\romdrive\private\10003a3f\import\apps\interpretsis_testcase_01_reg.rsc  > NUL
if exist  .\romdrive\private\10003a3f\apps\interpretsis_testcase_02_reg.rsc  del .\romdrive\private\10003a3f\apps\interpretsis_testcase_02_reg.rsc  > NUL
if exist  .\romdrive\resource\apps\interpretsis_testcase_01.r* del .\romdrive\resource\apps\interpretsis_testcase_01.r*  > NUL
if exist  .\romdrive\resource\apps\interpretsis_testcase_02.r* del .\romdrive\resource\apps\interpretsis_testcase_02.r*  > NUL
if exist  .\romdrive\resource\apps\interpretsis_testcase*.mbm  del .\romdrive\resource\apps\interpretsis_testcase*.mbm > NUL

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
