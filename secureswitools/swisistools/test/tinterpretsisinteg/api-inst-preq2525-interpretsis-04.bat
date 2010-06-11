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
REM API-INST-preq2525-interpretsis-04
REM PREQ2525 API-INST-preq2525-interpretsis-04 - Package supports FR and EN  Language selected PA and MN 

if not exist .\cdrive mkdir .\cdrive > NUL
if not exist .\romdrive mkdir .\romdrive > NUL

REM Remove installation files from any previous tests
if exist  .\cdrive\Documents\InstTest\devlangsup*.txt  del .\cdrive\Documents\InstTest\devlangsup*.txt  > NUL


call interpretsis -z .\romdrive -c .\cdrive  -s /epoc32/winscw/c/tswi/tsis/data/devlangsup_defaultlang.sis -k 5.4  -w info -I /epoc32/winscw/c/tswi/tinterpretsisinteg/preq2525-interpretsis-04/argumentfile.ini  -l /epoc32/winscw/c/interpretsis_test_harness.txt > NUL

IF NOT %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS>>/epoc32/winscw/c/interpretsis_test_harness.txt
GOTO END
:LAST
ECHO ***ERRORCODE*** %errorlevel% FAIL>>/epoc32/winscw/c/interpretsis_test_harness.txt
:END
