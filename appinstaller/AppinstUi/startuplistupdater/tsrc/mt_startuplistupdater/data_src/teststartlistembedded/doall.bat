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
call makesis teststartlistembedded_1_sub.pkg
call makesis teststartlistembedded_1_sub_winscw.pkg
call signsis teststartlistembedded_1_sub.sis teststartlistembedded_1_sub.sisx %CERTPATH%\rd.cer %CERTPATH%\rd-key.pem
call signsis teststartlistembedded_1_sub_winscw.sis teststartlistembedded_1_sub_winscw.sisx %CERTPATH%\rd.cer %CERTPATH%\rd-key.pem
del teststartlistembedded_1_sub.sis
del teststartlistembedded_1_sub_winscw.sis

call makesis teststartlistembedded_1_main.pkg
call makesis teststartlistembedded_1_main_winscw.pkg
call signsis teststartlistembedded_1_main.sis teststartlistembedded_1_main.sisx %CERTPATH%\rd.cer %CERTPATH%\rd-key.pem
call signsis teststartlistembedded_1_main_winscw.sis teststartlistembedded_1_main_winscw.sisx %CERTPATH%\rd.cer %CERTPATH%\rd-key.pem

call makesis teststartlistembedded_2_sub.pkg
call makesis teststartlistembedded_2_sub_winscw.pkg
call signsis teststartlistembedded_2_sub.sis teststartlistembedded_2_sub.sisx %CERTPATH%\rd.cer %CERTPATH%\rd-key.pem
call signsis teststartlistembedded_2_sub_winscw.sis teststartlistembedded_2_sub_winscw.sisx %CERTPATH%\rd.cer %CERTPATH%\rd-key.pem
del teststartlistembedded_2_sub.sis
del teststartlistembedded_2_sub_winscw.sis

call makesis teststartlistembedded_2_main.pkg
call makesis teststartlistembedded_2_main_winscw.pkg
call signsis teststartlistembedded_2_main.sis teststartlistembedded_2_main.sisx %CERTPATH%\rd.cer %CERTPATH%\rd-key.pem
call signsis teststartlistembedded_2_main_winscw.sis teststartlistembedded_2_main_winscw.sisx %CERTPATH%\rd.cer %CERTPATH%\rd-key.pem

if not exist ..\..\data mkdir ..\..\data
if not exist ..\..\data\mmc mkdir ..\..\data\mmc
if exist ..\..\data\mmc\teststartlistembedded_1.sisx del /F/Q ..\..\data\mmc\teststartlistembedded_1.sisx
move teststartlistembedded_1_main.sisx ..\..\data\mmc\teststartlistembedded_1.sisx
if exist ..\..\data\mmc\teststartlistembedded_1_winscw.sisx del /F/Q ..\..\data\mmc\teststartlistembedded_1_winscw.sisx
move teststartlistembedded_1_main_winscw.sisx ..\..\data\mmc\teststartlistembedded_1_winscw.sisx
if exist ..\..\data\mmc\teststartlistembedded_2.sisx del /F/Q ..\..\data\mmc\teststartlistembedded_2.sisx
move teststartlistembedded_2_main.sisx ..\..\data\mmc\teststartlistembedded_2.sisx
if exist ..\..\data\mmc\teststartlistembedded_2_winscw.sisx del /F/Q ..\..\data\mmc\teststartlistembedded_2_winscw.sisx
move teststartlistembedded_2_main_winscw.sisx ..\..\data\mmc\teststartlistembedded_2_winscw.sisx

del teststartlistembedded_1_main.sis
del teststartlistembedded_1_main_winscw.sis
del teststartlistembedded_2_main.sis
del teststartlistembedded_2_main_winscw.sis
call sbs reallyclean
dir ..\..\data\mmc

:end
set CERTPATH=
