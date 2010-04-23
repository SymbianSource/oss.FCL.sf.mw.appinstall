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
 REM testcaseI002a
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatai002\registryflagset.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\registryflagset.sis > NUL
 call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\registryflagset.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\registryflagset.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatai002\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatai002\good-r5.key.pem  > NUL
 call interpretsis -z .\romdrive -c .\cdrive -s  \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\registryflagset.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
 if not exist \epoc32\winscw\c\sys\install\sisregistry\89334286  mkdir \epoc32\winscw\c\sys\install\sisregistry\89334286 > NUL
 if not exist \epoc32\winscw\c\sys\bin mkdir \epoc32\winscw\c\sys\bin > NUL
 if not exist \epoc32\winscw\c\sys\hash mkdir \epoc32\winscw\c\sys\hash > NUL
 copy .\cdrive\sys\install\sisregistry\89334286\ \epoc32\winscw\c\sys\install\sisregistry\89334286\ > NUL 
 copy .\cdrive\sys\bin\registryflagset.exe \epoc32\winscw\c\sys\bin\registryflagset.exe > NUL
 copy .\cdrive\sys\hash\registryflagset.exe \epoc32\winscw\c\sys\hash\registryflagset.exe > NUL
 if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
 GOTO END
:B
REM testcaseI002b
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatai002\registryflagnotset.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\registryflagnotset.sis > NUL
 call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\registryflagnotset.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\registryflagnotset.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatai002\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatai002\good-r5.key.pem  > NUL
 call interpretsis -z .\romdrive -c .\cdrive -s  \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\registryflagnotset.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
 if not exist \epoc32\winscw\c\sys\install\sisregistry\89334286  mkdir \epoc32\winscw\c\sys\install\sisregistry\89334286 > NUL
 if not exist \epoc32\winscw\c\sys\bin mkdir \epoc32\winscw\c\sys\bin > NUL
 if not exist \epoc32\winscw\c\sys\hash mkdir \epoc32\winscw\c\sys\hash > NUL
 copy .\cdrive\sys\install\sisregistry\89334286\ \epoc32\winscw\c\sys\install\sisregistry\89334286\ > NUL
 copy .\cdrive\sys\bin\registryflagnotset.exe \epoc32\winscw\c\sys\bin\registryflagnotset.exe > NUL
 copy .\cdrive\sys\hash\registryflagnotset.exe \epoc32\winscw\c\sys\hash\registryflagnotset.exe > NUL
 if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
:END