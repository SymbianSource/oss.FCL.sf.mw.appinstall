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
echo DO NOT RUN THIS PROGRAM - PRESS CTRL C to Cancel
echo THIS PROGRAM REGENERATES ALL OF THE SIS FILES USED TO TEST DEVELOPER CERTIFICATE EXTENSIONS
pause
rmdir /S /Q sis certs reqs keys pkg ini
del uid.txt.tmp /F /Q

mkdir sis\winscw
mkdir sis\armv5 
mkdir certs
mkdir reqs
mkdir keys
mkdir pkg
mkdir ini

REM Create the extension files
for %%i in (ext\*.txt) do perl -w ..\..\..\certman\tx509\data\extensions\certextbuilder.pl "%%i" "ext\%%~ni.cfg"

call makeplatform.bat winscw

REM UIDs MUST MATCH INI FILES SO RESET COUNTER
del uid.txt.tmp /F /Q
del ini\* /F /Q

call makeplatform.bat armv5

:exit
