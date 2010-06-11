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
rem Description:  Module tests for SIF UI API.
rem

echo ----------------------------------------------------------------------
echo.
echo Running module tests
echo.
echo ----------------------------------------------------------------------
echo.
echo Note that closing dialogs automatically is not implemented yet.
echo.
call \epoc32\release\winscw\udeb\ATSInterface.exe -testmodule sifuitest

