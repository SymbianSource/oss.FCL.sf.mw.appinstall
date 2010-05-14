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
REM testcaseB001g
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataB001\dep7c.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\dep7c.sis > NUL
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataB001\depdat1c.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\depdat1c.sis > NUL
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataB001\depdat2c.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\depdat2c.sis > NUL
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataB001\depdat3c.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\depdat3c.sis > NUL

call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\depdat1c.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\depdat2c.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\depdat3c.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
call interpretsis -z .\romdrive -c .\cdrive -s  \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\dep7c.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
IF NOT %errorlevel%==-1 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS>>/epoc32/winscw/c/interpretsis_test_harness.txt
GOTO END
:LAST
ECHO ***ERRORCODE*** %errorlevel% FAIL>>/epoc32/winscw/c/interpretsis_test_harness.txt
:END
