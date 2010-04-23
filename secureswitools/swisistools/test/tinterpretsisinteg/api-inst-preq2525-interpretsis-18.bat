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
REM API-INST-preq2525-interpretsis-18
REM PREQ2525 API-INST-preq2525-interpretsis-18 - Base, SA, PU, SP installation for eqivalent match 

if not exist .\cdrive mkdir .\cdrive > NUL
if not exist .\romdrive mkdir .\romdrive > NUL

if not exist .\romdrive\system\data\  mkdir .\romdrive\system\data\ > NUL
if exist .\romdrive\system\data\sisregistry_*.txt  del .\romdrive\system\data\sisregistry_*.txt > NUL
::copy %SECURITYSOURCEDIR%\installationservices\swi\inc\sisregistry_5.3.txt  .\romdrive\system\data\sisregistry_5.4.txt  > c:\logs.txt
echo "have some dummy data " > .\romdrive\system\data\sisregistry_5.4.txt

dir .\romdrive\system\data\ >> c:\logs.txt
dir >> c:\logs.txt

REM Remove installation files from any previous tests
if exist  .\cdrive\Documents\InstTest\Equivalent*.txt  del .\cdrive\Documents\InstTest\Equivalent*.txt  > NUL

call interpretsis -z .\romdrive -c .\cdrive  -s /epoc32/winscw/c/tswi/tsis/data/equivalent_language_nooption.sis -w info -I /epoc32/winscw/c/tswi/tinterpretsisinteg/preq2525-interpretsis-08/argumentfile_base.ini  -l /epoc32/winscw/c/interpretsis_test_harness.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive  -s /epoc32/winscw/c/tswi/tsis/data/equivalent_language_sa_nooption.sis -w info -I /epoc32/winscw/c/tswi/tinterpretsisinteg/preq2525-interpretsis-08/argumentfile_sa.ini  -l /epoc32/winscw/c/interpretsis_test_harness.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive  -s /epoc32/winscw/c/tswi/tsis/data/equivalent_language_pu_nooption.sis -w info -I /epoc32/winscw/c/tswi/tinterpretsisinteg/preq2525-interpretsis-08/argumentfile_pu.ini  -l /epoc32/winscw/c/interpretsis_test_harness.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive  -s /epoc32/winscw/c/tswi/tsis/data/equivalent_language_sp_nooption.sis -w info -I /epoc32/winscw/c/tswi/tinterpretsisinteg/preq2525-interpretsis-08/argumentfile_sp.ini  -l /epoc32/winscw/c/interpretsis_test_harness.txt > NUL


if not exist \epoc32\winscw\c\sys\install\sisregistry\88000108 mkdir \epoc32\winscw\c\sys\install\sisregistry\88000108 > NUL
copy .\cdrive\sys\install\sisregistry\88000108\ \epoc32\winscw\c\sys\install\sisregistry\88000108\ > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL

IF NOT %errorlevel%==0 GOTO LAST
ECHO ***ERRORCODE*** %errorlevel% PASS>>/epoc32/winscw/c/interpretsis_test_harness.txt
GOTO END
:LAST
ECHO ***ERRORCODE*** %errorlevel% FAIL>>/epoc32/winscw/c/interpretsis_test_harness.txt
:END
