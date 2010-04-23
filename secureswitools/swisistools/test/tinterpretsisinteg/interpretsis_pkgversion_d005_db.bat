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

@ECHO OFF
REM testdata_pkgversion_d005
REM CR1125 SEC-SWI-PKGVER-D005 - Installing a SIS containing an EXISTS condition with a valid argument string

if not exist .\cdrive mkdir .\cdrive > NUL
if not exist .\romdrive mkdir .\romdrive > NUL

xcopy /E /Y backup\cdrive cdrive > NUL
xcopy /E /Y backup\romdrive romdrive > NUL

call interpretsis -z .\romdrive -c .\cdrive  -s /epoc32/winscw/c/tswi/tsis/data/testexpressions_version4.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness_db.txt > NUL

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
