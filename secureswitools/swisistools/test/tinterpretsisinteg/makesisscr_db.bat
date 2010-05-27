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

:: This batch file preares & installs the sis files using interpretsis
@ECHO OFF

GOTO %1

:A
REM testdatascr001
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatascr\base_r.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis > NUL
call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatascr\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatascr\good-r5.key.pem > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness_db.txt > NUL
if not exist \epoc32\winscw\c\sys\install\sisregistry\89334307 mkdir \epoc32\winscw\c\sys\install\sisregistry\89334307 > NUL
copy .\cdrive\sys\install\sisregistry\89334307\ \epoc32\winscw\c\sys\install\sisregistry\89334307\ /Y > NUL
copy .\romdrive\sys\install\scr\provisioned\scr.db \epoc32\winscw\c\sys\install\scr\scr.db /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst >NUL
GOTO END
:B
REM testdatascr002
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatascr\base_r.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis > NUL
call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatascr\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatascr\good-r5.key.pem > NUL
call interpretsis -z .\romdrive -c .\cdrive -f -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness_db.txt > NUL
if not exist \epoc32\winscw\c\sys\install\sisregistry\89334307 mkdir \epoc32\winscw\c\sys\install\sisregistry\89334307 > NUL
copy .\cdrive\sys\install\sisregistry\89334307\ \epoc32\winscw\c\sys\install\sisregistry\89334307\ /Y > NUL
copy .\romdrive\sys\install\scr\provisioned\scr.db \epoc32\winscw\c\sys\install\scr\scr.db /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
GOTO END
:C
REM testdatascr003
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatascr\base_r.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis > NUL
call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatascr\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatascr\good-r5.key.pem > NUL
call /epoc32/winscw/c/tswi/tinterpretsisinteg/interpretsisscr_db.bat
GOTO END
:D
REM testdatascr004
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatascr\hiddensa.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\hiddensa.sis > NUL
call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\hiddensa.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\hiddensa.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatascr\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatascr\good-r5.key.pem > NUL
call interpretsis -z .\romdrive -c .\cdrive -f -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\hiddensa.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness_db.txt > NUL
if not exist \epoc32\winscw\c\sys\install\sisregistry\802730D7 mkdir \epoc32\winscw\c\sys\install\sisregistry\802730D7 > NUL
copy .\cdrive\sys\install\sisregistry\802730D7\ \epoc32\winscw\c\sys\install\sisregistry\802730D7\ /Y > NUL
copy .\romdrive\sys\install\scr\provisioned\scr.db \epoc32\winscw\c\sys\install\scr\scr.db /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
GOTO END
:E
REM testdatascr005
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatascr\base_r.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis > NUL
call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatascr\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatascr\good-r5.key.pem > NUL
call interpretsis -z .\romdrive -c .\cdrive -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness_db.txt > NUL
if not exist \epoc32\winscw\c\sys\install\sisregistry\89334307 mkdir \epoc32\winscw\c\sys\install\sisregistry\89334307 > NUL
copy .\cdrive\sys\install\sisregistry\89334307\ \epoc32\winscw\c\sys\install\sisregistry\89334307\ /Y > NUL
copy .\romdrive\sys\install\scr\provisioned\scr.db \epoc32\winscw\c\sys\install\scr\scr.db /Y > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst >NUL
:END