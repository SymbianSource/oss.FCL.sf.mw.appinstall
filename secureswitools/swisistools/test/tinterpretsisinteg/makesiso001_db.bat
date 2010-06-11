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
REM testcaseO001
if not exist .\cdrive mkdir .\cdrive > NUL
if not exist .\romdrive mkdir .\romdrive > NUL
if not exist .\romdrive\private\89334471\import\ mkdir .\romdrive\private\89334471\import\ > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\testresult1.dat .\romdrive\private\89334471\import\testresult1.dat > NUL
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataa001\testfile1.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\testfile1.sis > NUL

IF NOT EXIST .\cdrive\sys\install\scr\ mkdir .\cdrive\sys\install\scr\ > NUL
call move .\romdrive\sys\install\scr\provisioned\scr.db .\cdrive\sys\install\scr\scr.db > NUL
call /epoc32/winscw/c/tswi/tinterpretsisinteg/interpretsiso001.bat > NUL
:END
