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
call makesis teststartlistembedded.pkg
call makesis teststartlistembedded_winscw.pkg
call signsis teststartlistembedded.sis teststartlistembedded.sisx %CERTPATH%\rd.cer %CERTPATH%\rd-key.pem
call signsis teststartlistembedded_winscw.sis teststartlistembedded_winscw.sisx %CERTPATH%\rd.cer %CERTPATH%\rd-key.pem
del teststartlistembedded.sis
del teststartlistembedded_winscw.sis
call makesis teststartlistembedded_main.pkg
call makesis teststartlistembedded_main_winscw.pkg
call signsis teststartlistembedded_main.sis teststartlistembedded_main.sisx %CERTPATH%\rd.cer %CERTPATH%\rd-key.pem
call signsis teststartlistembedded_main_winscw.sis teststartlistembedded_main_winscw.sisx %CERTPATH%\rd.cer %CERTPATH%\rd-key.pem
if not exist ..\..\data mkdir ..\..\data
if not exist ..\..\data\mmc mkdir ..\..\data\mmc
if exist ..\..\data\mmc\teststartlistembedded.sisx del /F/Q ..\..\data\mmc\teststartlistembedded.sisx
move teststartlistembedded_main.sisx ..\..\data\mmc\teststartlistembedded.sisx
if exist ..\..\data\mmc\teststartlistembedded_winscw.sisx del /F/Q ..\..\data\mmc\teststartlistembedded_winscw.sisx
move teststartlistembedded_main_winscw.sisx ..\..\data\mmc\teststartlistembedded_winscw.sisx
del teststartlistembedded_main.sis
del teststartlistembedded_main_winscw.sis
call sbs reallyclean
dir ..\..\data\mmc

:end
set CERTPATH=
