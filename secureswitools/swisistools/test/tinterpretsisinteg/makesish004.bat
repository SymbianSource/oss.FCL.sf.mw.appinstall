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
 
 REM testcaseH004
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatah004\trustok.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\trustok.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatah004\trustselfsigned.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\trustselfsigned.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatah004\trustsignedroot.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\trustsignedroot.sis > NUL
 call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\trustok.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\trustok.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatah004\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatah004\good-r5.key.pem  > NUL
 call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\trustselfsigned.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\trustselfsigned.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatah004\cert-gen.cer  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatah004\key-gen.key ajay > NUL
 call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\trustsignedroot.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\trustsignedroot.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatah004\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatah004\good-r5.key.pem  > NUL
 call interpretsis -z .\romdrive -c .\cdrive -s  \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\trustok.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness.txt
 if not exist \epoc32\winscw\c\sys\install\sisregistry\89334284 mkdir \epoc32\winscw\c\sys\install\sisregistry\89334284 > NUL
 copy .\cdrive\sys\install\sisregistry\89334284\ \epoc32\winscw\c\sys\install\sisregistry\89334284\ > NUL
 copy .\cdrive\trustok.txt \epoc32\winscw\c\trustok.txt > NUL
 if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
:END




