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
REM testcasew001
if not exist .\cdrive mkdir .\cdrive > NUL
if not exist .\romdrive mkdir .\romdrive > NUL

call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataw001\interpretsisw001.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\interpretsisw001.sis > NUL
call /epoc32/winscw/c/tswi/tinterpretsisinteg/interpretsisw001.bat
:END
