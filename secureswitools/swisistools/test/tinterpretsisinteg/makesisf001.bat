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
REM testcaseF001
 IF NOT EXIST .\romdrive\eclipseme\NUL mkdir .\romdrive\eclipseme > NUL
 IF NOT EXIST .\romdrive\system\install\NUL mkdir .\romdrive\system\install > NUL	
 call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\data\eclipseromdata.txt .\romdrive\eclipseme\eclipseromdata.txt > NUL
 call makesis -s \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataf001\Preeclipseromdata.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataF001\Preeclipseromdata.sis > NUL
 call copy \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataf001\*.sis .\romdrive\system\install > NUL
 call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataf001\eclipseromdata.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\eclipseromdata.sis > NUL
 call \epoc32\winscw\c\tswi\tinterpretsisinteg\interpretsisf001.bat
:END
