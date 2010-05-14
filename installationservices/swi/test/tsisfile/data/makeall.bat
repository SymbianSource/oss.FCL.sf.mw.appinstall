@rem
@rem Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
@echo on

REM construct appropriate directories

if not exist \epoc32\drive_d				 mkdir \epoc32\drive_d
if not exist \epoc32\drive_d\system			 mkdir \epoc32\drive_d\system
if not exist \epoc32\drive_d\system\install  		 mkdir \epoc32\drive_d\system\install
if not exist \epoc32\winscw\c\system		 	 mkdir \epoc32\winscw\c\system
if not exist \epoc32\winscw\c\system\install 		 mkdir \epoc32\winscw\c\system\install
if not exist \epoc32\winscw\c\PlatformTest	 	 mkdir \epoc32\winscw\c\PlatformTest

call /epoc32/tools/createsis create -pass 1234 files\2-2selfsigneddata.pkg

ECHO Running makesis on all package files and copying them to their destination
perl -S makeandsign.pl

echo makesis DONE

if not exist \epoc32\winscw\c\tswi\tsis\data	 	 mkdir \epoc32\winscw\c\tswi\tsis\data
if not exist \epoc32\winscw\c\tswi\tsis\scripts	 	 mkdir \epoc32\winscw\c\tswi\tsis\scripts

copy ..\scripts\tsis.script \epoc32\winscw\c\tswi\tsis\scripts
copy ..\scripts\tsis.ini \epoc32\winscw\c\tswi\tsis\scripts

REM Unfortunately we can't sign corrupted SIS files

perl makecorrupted.pl

copy ..\scripts\tsis_signed.script \epoc32\winscw\c\tswi\tsis\scripts
copy ..\scripts\tsis_signed.ini \epoc32\winscw\c\tswi\tsis\scripts

cd signedsis
echo Running signsis.pl
perl signsis.pl
cd ..

copy ..\scripts\tsistestdata.iby ..\..\..\inc\
copy ..\scripts\tsistestdata.iby \epoc32\rom\include
copy ..\scripts\tsissignedtestdata.iby ..\..\..\inc\
copy ..\scripts\tsissignedtestdata.iby \epoc32\rom\include
