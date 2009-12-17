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
@echo off
cls
echo THIS PROGRAM REGENERATES ALL OF THE SIS FILES USED TO TEST DEVELOPER CERTIFICATE EXTENSIONS
echo IT IS CALLED FROM MAKETESTSIS.FIL...

set PLATFORM=%1
set CFG=%2

rem Remove only SIS\%PLATFORM% and PKG 
rem Leave out  CERT, REQS and KEYS 'cause they were created during manual generation of certificates
rem INI folder already contains backuprestore.ini and distribution.policy files
if exist sis\%PLATFORM% rmdir /S /Q sis\%PLATFORM%
if exist pkg rmdir /S /Q pkg else mkdir pkg
if exist uid.txt.tmp del uid.txt.tmp /F /Q
del \epoc32\winscw\c\tswi\tdevcerts\scripts\*.ini

mkdir sis\%PLATFORM%
mkdir pkg



REM Create the extension files
for %%i in (ext\*.txt) do perl -w ..\..\..\certman\tx509\data\extensions\certextbuilder.pl "%%i" "ext\%%~ni.cfg"

if exist uid.txt.tmp del uid.txt.tmp /F /Q
call makeplatform.bat %PLATFORM% %CFG%

REM BACKUP AND RESTORE TESTS MUST USE UIDS MATCHING backuprestore.ini
if exist uid.txt.tmp del uid.txt.tmp /F /Q
copy backup.uid.txt uid.txt.tmp 
call makebackuprestore.bat %PLATFORM% %CFG%

REM  move all SIS files to EPOC
perl -w copysis.pl --platform %PLATFORM% --sis-dir sis

copy /y ini\backuprestore.ini \epoc32\winscw\c\tswi\tdevcerts\scripts\backuprestore.ini

REM tidy up
if exist pkg rmdir /S /Q pkg
if exist uid.txt.tmp del uid.txt.tmp /F /Q

:exit