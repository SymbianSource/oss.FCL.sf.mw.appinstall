@echo off
rem
rem Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:  Creates test exe for startup list updater test module.
rem

set CERTPATH=%1
if not x%CERTPATH%x == xx goto doit
echo Usage
echo doall.bat [path to RD certificates]
goto end

:doit
echo on
call sbs
call makesis teststartlist.pkg
call makesis teststartlist_winscw.pkg
call signsis teststartlist.sis teststartlist.sisx %CERTPATH%\rd.cer %CERTPATH%\rd-key.pem
call signsis teststartlist_winscw.sis teststartlist_winscw.sisx %CERTPATH%\rd.cer %CERTPATH%\rd-key.pem
if not exist ..\..\data mkdir ..\..\data
if not exist ..\..\data\mmc mkdir ..\..\data\mmc
if exist ..\..\data\mmc\teststartlist.sisx del /F/Q ..\..\data\mmc\teststartlist.sisx
move teststartlist.sisx ..\..\data\mmc\.
if exist ..\..\data\mmc\teststartlist_winscw.sisx del /F/Q ..\..\data\mmc\teststartlist_winscw.sisx
move teststartlist_winscw.sisx ..\..\data\mmc\.
del teststartlist.sis
del teststartlist_winscw.sis
call sbs reallyclean
dir ..\..\data\mmc

:end
set CERTPATH=
