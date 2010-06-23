@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of the License "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/sfl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem
@ECHO OFF
REM int_nr_option_07
REM int_nr_option_07 -  SA(NR) over SA(NR). This is a positive testcase. Expected Result : SIS file is Installed. 

IF NOT EXIST .\backup\cdrive mkdir .\backup\cdrive > NUL
IF NOT EXIST .\backup\romdrive mkdir .\backup\romdrive > NUL
if not exist .\cdrive mkdir .\cdrive > NUL
if not exist .\cdrive\Documents mkdir .\cdrive\Documents > NUL
if not exist .\romdrive mkdir .\romdrive > NUL
if not exist .\romdrive\system\ mkdir .\romdrive\system\ > NUL
if not exist .\romdrive\system\install\ mkdir .\romdrive\system\install\ > NUL
REM Remove installation files from any previous tests
if exist  .\cdrive\Documents\InstTest\Equivalent*.txt  del .\cdrive\Documents\InstTest\Equivalent*.txt  > NUL

xcopy /E /Y backup\cdrive cdrive > NUL
xcopy /E /Y backup\romdrive romdrive > NUL

REM Remove installation files from any previous tests
if exist  \epoc32\winscw\c\Documents\abc*.txt  del \epoc32\winscw\c\Documents\abc*.txt  > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\80000086\*.reg del \epoc32\winscw\c\sys\install\sisregistry\80000086\*.reg > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\80000086\*.ctl del \epoc32\winscw\c\sys\install\sisregistry\80000086\*.ctl > NUL

call interpretsis -z .\romdrive -c .\cdrive  -s \epoc32\winscw\c\tswi\tsis\data\int_nr_option_01.sis +nonremovablepkg -w info -l /epoc32/winscw/c/interpretsis_test_harness_db.txt > NUL
call interpretsis -z .\romdrive -c .\cdrive  -s \epoc32\winscw\c\tswi\tsis\data\int_nr_option_01.sis +nonremovablepkg -w info -l /epoc32/winscw/c/interpretsis_test_harness_db.txt > NUL

if not %errorlevel%==0 GOTO LAST
if not exist \epoc32\winscw\c\Documents mkdir \epoc32\winscw\c\Documents
call xcopy .\cdrive\Documents \epoc32\winscw\c\Documents /S /Y > NUL
call xcopy .\cdrive\sys\install\sisregistry \epoc32\winscw\c\sys\install\sisregistry /S /Y > NUL
if exist \epoc32\winscw\c\sys\install\scr\scr.db del \epoc32\winscw\c\sys\install\scr\scr.db > NUL
copy .\romdrive\sys\install\scr\provisioned\scr.db \epoc32\winscw\c\sys\install\scr\scr.db /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL

ECHO ***ERRORCODE*** %errorlevel% PASS>>/epoc32/winscw/c/interpretsis_test_harness_db.txt
GOTO END
:LAST
ECHO ***ERRORCODE*** %errorlevel% FAIL>>/epoc32/winscw/c/interpretsis_test_harness_db.txt
:END
