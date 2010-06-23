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
rem Description:  Module tests for SW install progress indicator.
rem

echo ----------------------------------------------------------------------
echo.
echo Running module tests
echo.
echo ----------------------------------------------------------------------
echo.
call make run

echo.
echo.
echo ----------------------------------------------------------------------
echo.
echo Processing test results
echo.
echo ----------------------------------------------------------------------
echo.
find "[QTestLib]" %TEMP%\epocwind.out > mt_results.txt
start mt_results.txt
type mt_results.txt
echo.
pushd ..\..
call ctcpost MON.sym MON.dat -p profile.txt
call ctc2html -i profile.txt
popd

