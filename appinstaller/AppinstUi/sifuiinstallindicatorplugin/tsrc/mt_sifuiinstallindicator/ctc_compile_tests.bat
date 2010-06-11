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
echo Compiling module tests
echo.
echo ----------------------------------------------------------------------
echo.
call qmake
call make debug-winscw
call make release-armv5

