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
REM testcaseL001

call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataL001\functions_base.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\functions_base.sis > NUL
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataL001\functions.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\functions.sis > NUL

call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\functions_base.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness_db.txt
ECHO ***ERRORCODE*** %errorlevel% PASS>>/epoc32/winscw/c/interpretsis_test_harness_db.txt

call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\functions.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness_db.txt
ECHO ***ERRORCODE*** %errorlevel% PASS>>/epoc32/winscw/c/interpretsis_test_harness_db.txt

:END


