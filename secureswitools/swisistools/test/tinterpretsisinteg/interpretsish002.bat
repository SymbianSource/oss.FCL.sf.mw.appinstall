@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

:: This batch file creates sis file/files
@ECHO OFF

GOTO %1

:A
REM testcaseH002a
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataH002\diag1silent.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag1silent.sis > NUL
call interpretsis -z .\romdrive -c .\cdrive  -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag1silent.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
IF NOT %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS >>/epoc32/winscw/c/interpretsis_test_harness.txt
GOTO END

:B
REM testcaseH002b
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataH002\diag1.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag1.sis > NUL
call interpretsis -z .\romdrive -c .\cdrive  -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag1.sis  -w info -l \epoc32\winscw\c\tswi\tinterpretsisinteg\logs\info1.txt
IF NOT %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS >>/epoc32/winscw/c/interpretsis_test_harness.txt
GOTO END
:C
REM testcaseH002c
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataH002\diag2.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag2.sis > NUL
call interpretsis -z .\romdrive -c .\cdrive  -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag2.sis -w warn  -l \epoc32\winscw\c\tswi\tinterpretsisinteg\logs\warning1.txt 
IF NOT %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS >>/epoc32/winscw/c/interpretsis_test_harness.txt
GOTO END
:D
REM testcaseH002d
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataH002\diag1Warning.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag1Warning.sis > NUL
call interpretsis -z .\romdrive -c .\cdrive  -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag1Warning.sis  -w warn  -l \epoc32\winscw\c\tswi\tinterpretsisinteg\logs\warning2.txt
IF NOT %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS >>/epoc32/winscw/c/interpretsis_test_harness.txt
GOTO END
:E
REM testcaseH002e
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataH002\diag3.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag3.sis > NUL
call interpretsis -z .\romdrive -c .\cdrive  -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag3.sis -w error  -l \epoc32\winscw\c\tswi\tinterpretsisinteg\logs\error1.txt
IF NOT %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS >>/epoc32/winscw/c/interpretsis_test_harness.txt
GOTO END
:F
REM testcaseH002f
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataH002\diag1Error.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag1Error.sis > NUL
call interpretsis -z .\romdrive -c .\cdrive  -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag1Error.sis -w error  -l \epoc32\winscw\c\tswi\tinterpretsisinteg\logs\error2.txt
IF NOT %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS >>/epoc32/winscw/c/interpretsis_test_harness.txt
GOTO END
:G
REM testcaseH002g
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataH002\diag1Error.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag1Error.sis > NUL
call interpretsis -z .\romdrive -c .\cdrive  -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag1Error.sis  -w off -l \epoc32\winscw\c\tswi\tinterpretsisinteg\logs\silent1.txt
IF NOT %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS >>/epoc32/winscw/c/interpretsis_test_harness.txt
GOTO END
:H
REM testcaseH002h
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataH002\diag1Error.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag1Error.sis > NUL
call interpretsis -z .\romdrive -c .\cdrive  -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag1Error.sis  -l \epoc32\winscw\c\tswi\tinterpretsisinteg\logs\error3.txt
IF NOT %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS >>/epoc32/winscw/c/interpretsis_test_harness.txt
GOTO END
:I
REM testcaseH002i
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataH002\diag1Warning.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag1Warning.sis > NUL
call interpretsis -z .\romdrive -c .\cdrive  -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag1Warning.sis  -l \epoc32\winscw\c\tswi\tinterpretsisinteg\logs\warning3.txt
IF NOT %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS >>/epoc32/winscw/c/interpretsis_test_harness.txt
GOTO END
:J
REM testcaseH002j
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataH002\diag1Warning.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag1Warning.sis > NUL
call interpretsis -z .\romdrive -c .\cdrive  -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag1Warning.sis -w info  -l \epoc32\winscw\c\tswi\tinterpretsisinteg\logs\info2.txt
IF NOT %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS >>/epoc32/winscw/c/interpretsis_test_harness.txt
GOTO END
:K
REM testcaseH002k
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataH002\diag4.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag4.sis > NUL
call interpretsis -z .\romdrive -c .\cdrive  -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\diag4.sis -w info  -l \epoc32\winscw\c\tswi\tinterpretsisinteg\logs\info3.txt
IF NOT %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS >>/epoc32/winscw/c/interpretsis_test_harness.txt
GOTO END
:LAST
ECHO ***ERRORCODE*** %errorlevel% FAIL>>/epoc32/winscw/c/interpretsis_test_harness.txt
:END
