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
REM testcaseF003
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataf003/preeclipsecdata.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles/preeclipsecdata.sis > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataf003/eclipsecdata.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles/eclipsecdata.sis > NUL
 call \epoc32\winscw\c\tswi\tinterpretsisinteg\interpretsisf003.bat
:END
