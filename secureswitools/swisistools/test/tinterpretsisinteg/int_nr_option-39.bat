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
REM int_nr_option_39
REM int_nr_option_39 - Install a SIS file with NR CLI option and +mcardalone option set. This is a positive testcase. Expected Result : SIS file is Installed.

IF NOT EXIST .\backup\cdrive mkdir .\backup\cdrive > NUL
IF NOT EXIST .\backup\romdrive mkdir .\backup\romdrive > NUL
if not exist .\cdrive mkdir .\cdrive > NUL
if not exist .\cdrive\Documents mkdir .\cdrive\Documents > NUL
if not exist .\romdrive mkdir .\romdrive > NUL
if not exist .\romdrive\system\ mkdir .\romdrive\system\ > NUL
if not exist .\romdrive\system\install\ mkdir .\romdrive\system\install\ > NUL
if not exist \epoc32\winscw\e mkdir \epoc32\winscw\e

xcopy /E /Y backup\cdrive cdrive > NUL
xcopy /E /Y backup\romdrive romdrive > NUL

REM Remove installation files from any previous tests
if exist \epoc32\winscw\c\Documents\abc*.txt  del \epoc32\winscw\c\Documents\abc*.txt  > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\80000086\*.reg del \epoc32\winscw\c\sys\install\sisregistry\80000086\*.reg > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\80000086\*.ctl del \epoc32\winscw\c\sys\install\sisregistry\80000086\*.ctl > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\80000087\*.reg del \epoc32\winscw\c\sys\install\sisregistry\80000087\*.reg > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\80000087\*.ctl del \epoc32\winscw\c\sys\install\sisregistry\80000087\*.ctl > NUL
if exist  \epoc32\winscw\e\Documents\abc*.txt  del \epoc32\winscw\e\Documents\abc*.txt  > NUL
if exist \epoc32\winscw\e\sys\install\sisregistry\80000086\*.reg del \epoc32\winscw\e\sys\install\sisregistry\80000086\*.reg > NUL
if exist \epoc32\winscw\e\sys\install\sisregistry\80000086\*.ctl del \epoc32\winscw\e\sys\install\sisregistry\80000086\*.ctl > NUL


call interpretsis -z .\romdrive -c .\cdrive  -s \epoc32\winscw\c\tswi\tsis\data\int_nr_option_01.sis  +e +mcardalone +nonremovablepkg -w info -i \epoc32\winscw\c\tswi\tinterpretsisinteg\parameters_file\int_nr_option_38.txt -l /epoc32/winscw/c/interpretsis_test_harness.txt > NUL

if not %errorlevel%==0 GOTO LAST
if not exist \epoc32\winscw\c\Documents mkdir \epoc32\winscw\c\Documents
call xcopy .\cdrive\Documents \epoc32\winscw\c\Documents /S /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
if exist \epoc32\winscw\e\sys\install\sisregistry\backup.lst del \epoc32\winscw\e\sys\install\sisregistry\backup.lst > NUL
if not exist  \epoc32\winscw\e\Documents\abc1.txt  GOTO LAST
@rem Cleaning all residue file
if exist  \epoc32\winscw\e\Documents\abc*.txt  del \epoc32\winscw\e\Documents\abc*.txt  > NUL
if exist \epoc32\winscw\e\sys\install\sisregistry\80000086\*.reg del \epoc32\winscw\e\sys\install\sisregistry\80000086\*.reg > NUL
if exist \epoc32\winscw\e\sys\install\sisregistry\80000086\*.ctl del \epoc32\winscw\e\sys\install\sisregistry\80000086\*.ctl > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\80000086 rmdir \epoc32\winscw\c\sys\install\sisregistry\80000086 > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\80000087 rmdir \epoc32\winscw\c\sys\install\sisregistry\80000087 > NUL
if exist \epoc32\winscw\e\sys\install\sisregistry\80000086 rmdir \epoc32\winscw\e\sys\install\sisregistry\80000086 > NUL
if exist \epoc32\winscw\e\sys\install\sisregistry rmdir \epoc32\winscw\e\sys\install\sisregistry > NUL
if exist \epoc32\winscw\e\sys\install rmdir \epoc32\winscw\e\sys\install > NUL
if exist \epoc32\winscw\e\sys rmdir \epoc32\winscw\e\sys > NUL
if exist \epoc32\winscw\e\private\10202dce\*.sis del \epoc32\winscw\e\private\10202dce\*.sis > NUL
if exist \epoc32\winscw\e\private\10202dce rmdir \epoc32\winscw\e\private\10202dce > NUL
if exist \epoc32\winscw\e\private rmdir \epoc32\winscw\e\private > NUL
if exist \epoc32\winscw\e\Documents rmdir \epoc32\winscw\e\Documents > NUL
if exist \epoc32\winscw\e rmdir \epoc32\winscw\e > NUL

ECHO ***ERRORCODE*** %errorlevel% PASS>>/epoc32/winscw/c/interpretsis_test_harness.txt
GOTO END
:LAST
ECHO ***ERRORCODE*** %errorlevel% FAIL>>/epoc32/winscw/c/interpretsis_test_harness.txt
:END
