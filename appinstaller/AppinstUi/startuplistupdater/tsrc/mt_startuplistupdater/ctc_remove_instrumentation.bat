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
echo Removing instrumentation
echo.
echo ----------------------------------------------------------------------
echo.
pushd ..\..\group
call sbs reallyclean
call sbs -c armv5_urel
call sbs -c winscw_udeb
popd

