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
REM testcaseB002
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab002\depdat1z.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\depdat1z.sis
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab002\depdat2z.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\depdat2z.sis
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab002\depdat3z.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\depdat3z.sis
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab002\dep1z.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\dep1z.sis
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab002\dep2z.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\dep2z.sis
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab002\dep3z.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\dep3z.sis
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab002\dep4z.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\dep4z.sis
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab002\dep5z.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\dep5z.sis
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab002\dep6z.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\dep6z.sis
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab002\dep7z.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\dep7z.sis
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab002\dep8z.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\dep8z.sis
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatab002\dep9z.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\dep9z.sis
:END
