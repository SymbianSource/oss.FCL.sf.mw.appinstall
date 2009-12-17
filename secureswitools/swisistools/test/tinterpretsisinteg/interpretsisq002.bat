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

:: This batch file runs the interpretsis command
@ECHO OFF
REM interpretsisq002
if not exist .\cdrive mkdir .\cdrive > NUL
if not exist .\romdrive mkdir .\romdrive > NUL
call interpretsis -p /epoc32/winscw/c/tswi/tinterpretsisinteg/testdataq002/testinterpretsisargumentsq002.txt > NUL
IF NOT %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS>>/epoc32/winscw/c/interpretsis_test_harness.txt
GOTO END
:LAST
ECHO ***ERRORCODE*** %errorlevel% FAIL>>/epoc32/winscw/c/interpretsis_test_harness.txt
:END
