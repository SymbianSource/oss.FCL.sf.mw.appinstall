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
REM testcaseF006
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataf006/ownedfolder.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles/ownedfolder.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataf006/nomyfolder.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles/nomyfolder.sis > NUL
 call \epoc32\winscw\c\tswi\tinterpretsisinteg\interpretsisf006_db.bat A
GOTO END
:B
REM testcaseF006Defect
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataf006/privatefolderdef.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles/privatefolderdef.sis > NUL
 call \epoc32\winscw\c\tswi\tinterpretsisinteg\interpretsisf006_db.bat B
GOTO END
:END
