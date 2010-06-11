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
REM testcaseB006
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab006\ai1.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\ai1.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab006\bi1.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\bi1.sis  > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab006\ci1.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\ci1.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab006\di1.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\di1.sis  > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab006\ai2.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\ai2.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab006\bi2.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\bi2.sis  > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab006\ci2.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\ci2.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab006\di2.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\di2.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab006\ai3.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\ai3.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab006\bi3.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\bi3.sis  > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab006\ci3.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\ci3.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab006\di3.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\di3.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab006\ei3.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\ei3.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab006\fi3.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\fi3.sis > NUL
:END
