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
REM testcaseF009
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataf009\newexe.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\newexe.sis > NUL
 call signsis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\newexe.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\newexe.sis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.pem  \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataj001\good-r5.key.pem  > NUL
 call interpretsis -z .\romdrive -c .\cdrive -s  \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\newexe.sis -w info -l /epoc32/winscw/c/interpretsis_test_harness_db.txt > NUL
if exist \epoc32\winscw\c\sys\install\sisregistry\backup.lst del \epoc32\winscw\c\sys\install\sisregistry\backup.lst > NUL
:END
