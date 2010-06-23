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
 REM testcaseI001a
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatai001\flagyes.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\flagyes.sis > NUL
 call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\flagyes.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\flagyes.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatai001\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatai001\good-r5.key.pem  > NUL
 call interpretsis -z .\romdrive -c .\cdrive -s  \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\flagyes.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
 if not exist \epoc32\winscw\c\sys\install\sisregistry\89334473  mkdir \epoc32\winscw\c\sys\install\sisregistry\89334473 > NUL
 if not exist \epoc32\winscw\c\sys\bin mkdir \epoc32\winscw\c\sys\bin > NUL
 if not exist \epoc32\winscw\c\sys\hash mkdir \epoc32\winscw\c\sys\hash > NUL
 copy .\cdrive\sys\install\sisregistry\89334473\ \epoc32\winscw\c\sys\install\sisregistry\89334473\ > NUL 
 copy .\cdrive\sys\bin\flagset.exe \epoc32\winscw\c\sys\bin\flagset.exe > NUL
 copy .\cdrive\sys\hash\flagset.exe \epoc32\winscw\c\sys\hash\flagset.exe > NUL
 if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL

 GOTO END
 :B
REM testcaseI001b
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatai001\flagnot.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\flagnot.sis > NUL
 call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\flagnot.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\flagnot.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatai001\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatai001\good-r5.key.pem  > NUL
 call interpretsis -z .\romdrive -c .\cdrive -s  \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\flagnot.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
 if not exist \epoc32\winscw\c\sys\install\sisregistry\89334473  mkdir \epoc32\winscw\c\sys\install\sisregistry\89334473 > NUL
 if not exist \epoc32\winscw\c\sys\bin mkdir \epoc32\winscw\c\sys\bin > NUL
 if not exist \epoc32\winscw\c\sys\hash mkdir \epoc32\winscw\c\sys\hash > NUL
 copy .\cdrive\sys\install\sisregistry\89334473\ \epoc32\winscw\c\sys\install\sisregistry\89334473\ > NUL 
 copy .\cdrive\sys\bin\flagnotset.exe \epoc32\winscw\c\sys\bin\flagnotset.exe > NUL
 copy .\cdrive\sys\hash\flagnotset.exe \epoc32\winscw\c\sys\hash\flagnotset.exe > NUL
 if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
 :END