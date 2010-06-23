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
REM testcaseF004
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataf004/preeclipsecexe.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles/preeclipsecexe.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataf004/eclipsecexe.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles/eclipsecexe.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataf004/eclipsecbinexe.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles/eclipsecbinexe.sis > NUL
 call \epoc32\winscw\c\tswi\tinterpretsisinteg\interpretsisf004_db.bat
:END
