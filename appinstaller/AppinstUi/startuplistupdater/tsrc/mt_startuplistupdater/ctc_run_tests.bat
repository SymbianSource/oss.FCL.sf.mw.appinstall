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
rem Description:  Module tests for startup list updater.
rem

echo ----------------------------------------------------------------------
echo.
echo Running module tests
echo.
echo ----------------------------------------------------------------------
echo.
echo Note: module test can be run also manually. Start STIF UI and run
echo test cases listed in conf\mt_startuplistupdater.cfg file. Use ctcpost
echo and ctc2html commands to process the CTC output file (MON.dat).
echo.
pushd ..\..\group
call \epoc32\release\winscw\udeb\ATSInterface.exe -testmodule testscripter -config c:\testframework\mt_startuplistupdater.cfg

echo.
echo.
echo ----------------------------------------------------------------------
echo.
echo Processing test results
echo.
echo ----------------------------------------------------------------------
echo.
call ctcpost MON.sym MON.dat -p profile.txt
call ctc2html -i profile.txt
popd
