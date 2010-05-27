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
REM testcaseM0011
if not exist .\cdrive mkdir .\cdrive > NUL
if not exist .\romdrive mkdir .\romdrive > NUL
IF NOT EXIST .\romdrive\system\install\ mkdir .\romdrive\system\install\ > NUL
IF NOT EXIST .\romdrive\private\81111106\import\ mkdir .\romdrive\private\81111106\import\ > NUL
call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\interpretsism.txt .\romdrive\private\81111106\import\interpretsism.txt > NUL

call makesis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatam0011\interpretsisstubm0011.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\interpretsisstubm0011.sis > NUL

call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\interpretsisstubm0011.sis .\romdrive\system\install\interpretsisstubm0011.sis > NUL

call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdatam0011\interpretsism0011.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\interpretsism0011.sis > NUL
call /epoc32/winscw/c/tswi/tinterpretsisinteg/interpretsism0011_db.bat
:END