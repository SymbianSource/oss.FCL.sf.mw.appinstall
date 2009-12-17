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
rem Set up tests using the automatic connection commdb configuration

cd \
call %SECURITYSOURCEDIR%\testframework\test\autotesting\useautocfg.bat

REM Backup the original swicertstore.dat
copy \epoc32\release\winscw\udeb\z\resource\swicertstore.dat \epoc32\release\winscw\udeb\z\resource\swicertstore_org.dat
copy \epoc32\release\winscw\urel\z\resource\swicertstore.dat \epoc32\release\winscw\urel\z\resource\swicertstore_org.dat

rem copy appropriate swicertstore
rem currently use the tsis certstore but need to change this when tests improve
copy \epoc32\winscw\c\tswi\certstore\tsis_swicertstore.dat \epoc32\release\winscw\udeb\z\resource\swicertstore.dat
copy \epoc32\winscw\c\tswi\certstore\tsis_swicertstore.dat \epoc32\release\winscw\urel\z\resource\swicertstore.dat

rem cleanup anything from previous tests.
del /s /f /q \epoc32\winscw\c\sys\install\sisregistry\
del /s /f /q \epoc32\winscw\c\Documents\InstTest\
del /s /f /q \epoc32\winscw\drive_d\Documents\InstTest\
del /s /f /q \epoc32\winscw\c\sys\install\siregbackup.lst
del /s /f /q \epoc32\winscw\c\sys\install\scr\scr.db
