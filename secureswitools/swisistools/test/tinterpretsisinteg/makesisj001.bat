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

:: This batch file copies sis file/files to sisfiles folder
@ECHO OFF

GOTO %1

:A
 REM testcaseJ001a
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\base_r.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\upgrade_sa_r.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\upgrade_sa_r.sis > NUL
 call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.key.pem  > NUL
 call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\upgrade_sa_r.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\upgrade_sa_r.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.key.pem  > NUL
 call interpretsis -z .\romdrive -c .\cdrive -s  \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
 if not exist \epoc32\winscw\c\sys\install\sisregistry\89334307 mkdir \epoc32\winscw\c\sys\install\sisregistry\89334307 > NUL
 copy .\cdrive\sys\install\sisregistry\89334307\ \epoc32\winscw\c\sys\install\sisregistry\89334307\ > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
GOTO END
:B   
 REM testcaseJ001b
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\base_nr.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_nr.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\upgrade_sa_r.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\upgrade_sa_r.sis > NUL
 call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_nr.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_nr.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.key.pem  > NUL
 call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\upgrade_sa_r.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\upgrade_sa_r.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.key.pem  > NUL
 call interpretsis -z .\romdrive -c .\cdrive -s  \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_nr.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
 if not exist \epoc32\winscw\c\sys\install\sisregistry\89334307 mkdir \epoc32\winscw\c\sys\install\sisregistry\89334307 > NUL
 copy .\cdrive\sys\install\sisregistry\89334307\ \epoc32\winscw\c\sys\install\sisregistry\89334307\ > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
GOTO END
:C
 REM testcaseJ001c
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\base_r.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\upgrade_sa_nr.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\upgrade_sa_nr.sis > NUL
 call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.key.pem  > NUL
 call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\upgrade_sa_nr.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\upgrade_sa_nr.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.key.pem  > NUL
 call interpretsis -z .\romdrive -c .\cdrive -s  \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_r.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
 if not exist \epoc32\winscw\c\sys\install\sisregistry\89334307 mkdir \epoc32\winscw\c\sys\install\sisregistry\89334307 > NUL
 copy .\cdrive\sys\install\sisregistry\89334307\ \epoc32\winscw\c\sys\install\sisregistry\89334307\ > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
GOTO END
:D
 REM testcaseJ001d
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\base_nr.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_nr.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\upgrade_sa_nr.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\upgrade_sa_nr.sis > NUL
 call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_nr.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_nr.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.key.pem  > NUL
 call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\upgrade_sa_nr.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\upgrade_sa_nr.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.key.pem  > NUL
 call interpretsis -z .\romdrive -c .\cdrive -s  \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\base_nr.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
 if not exist \epoc32\winscw\c\sys\install\sisregistry\89334307 mkdir \epoc32\winscw\c\sys\install\sisregistry\89334307 > NUL
 copy .\cdrive\sys\install\sisregistry\89334307\ \epoc32\winscw\c\sys\install\sisregistry\89334307\ > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
:END
