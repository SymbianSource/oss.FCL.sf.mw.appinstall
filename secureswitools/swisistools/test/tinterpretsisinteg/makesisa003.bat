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
REM testcaseA003
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataa003\indir1\dira.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\indir1\dira.sis 
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataa003\indir1\dirb.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\indir1\dirb.sis
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataa003\indir1\dirc.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\indir1\dirc.sis
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataa003\indir2\dira.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\indir2\dira.sis
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataa003\indir2\dirb.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\indir2\dirb.sis
call makesis \epoc32\winscw\c\tswi\tinterpretsisinteg\testdataa003\indir2\dirc.pkg \epoc32\winscw\c\tswi\tinterpretsisinteg\sisfiles\indir2\dirc.sis
call /epoc32/winscw/c/tswi/tinterpretsisinteg/interpretsisa003.bat
:END
