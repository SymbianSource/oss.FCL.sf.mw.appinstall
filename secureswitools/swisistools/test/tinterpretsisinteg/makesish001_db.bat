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
REM testcaseH001
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatah001\flagyes.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\flagyes.sis > NUL
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatah001\flagnot.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\flagnot.sis > NUL
call /epoc32/winscw/c/tswi/tinterpretsisinteg/interpretsish001_db.bat
:END
