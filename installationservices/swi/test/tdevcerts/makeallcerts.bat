@rem
@rem Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

cls
echo DO NOT RUN THIS PROGRAM - PRESS CTRL C to Cancel
echo THIS PROGRAM REGENERATES ALL OF THE TEST DEVELOPER CERTIFICATE EXTENSIONS
echo and SIS 
pause

echo Removing old stuff...
rmdir /S /Q sis certs reqs keys
if exist pkg rmdir pkg
if exist uid.txt.tmp del uid.txt.tmp /F /Q
del \epoc32\winscw\c\tswi\tdevcerts\scripts\*.ini

echo Creating new folders...
mkdir sis\winscw
mkdir sis\armv5 
mkdir certs
mkdir reqs
mkdir keys
mkdir pkg



REM Create the extension files
for %%i in (ext\*.txt) do perl -w ..\..\..\certman\tx509\data\extensions\certextbuilder.pl "%%i" "ext\%%~ni.cfg"

if exist uid.txt.tmp del uid.txt.tmp /F /Q
call makeplatformcerts.bat winscw udeb

REM UIDs MUST MATCH INI FILES SO RESET COUNTER
if exist uid.txt.tmp del uid.txt.tmp /F /Q
call makeplatformcerts.bat armv5 urel

REM BACKUP AND RESTORE TESTS MUST USE UIDS MATCHING backuprestore.ini

if exist uid.txt.tmp del uid.txt.tmp /F /Q
copy backup.uid.txt uid.txt.tmp 
call makebackuprestorecerts.bat winscw udeb

del uid.txt.tmp /F /Q
copy backup.uid.txt uid.txt.tmp 
call makebackuprestorecerts.bat armv5 urel

REM  move all SIS files to EPOC
perl -w copysis.pl --platform winscw --sis-dir sis
perl -w copysis.pl --platform armv5 --sis-dir sis

copy /y ini\backuprestore.ini \epoc32\winscw\c\tswi\tdevcerts\scripts\backuprestore.ini

REM tidy up
if exist pkg rmdir /S /Q pkg
if exist uid.txt.tmp del uid.txt.tmp /F /Q

:exit
