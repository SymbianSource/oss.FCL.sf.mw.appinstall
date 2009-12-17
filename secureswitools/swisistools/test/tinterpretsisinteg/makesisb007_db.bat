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

:: This batch file creates sis file/files
@ECHO OFF
REM testcaseB007
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab007\ai1.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\ai1.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab007\bi1.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\bi1.sis  > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab007\ai2.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\ai2.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab007\bi2.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\bi2.sis > NUL 
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab007\ai3.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\ai3.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab007\bi3.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\bi3.sis > NUL 
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab007\ci3.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\ci3.sis > NUL
 call /epoc32/winscw/c/tswi/tinterpretsisinteg/interpretsisb007_db.bat
 :END
