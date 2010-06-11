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
echo Instrumenting code (armv5 urel and winscw udeb, decision coverage)
echo.
echo ----------------------------------------------------------------------
echo.
pushd ..\..
if exist MON.sym del MON.sym
if exist MON.dat del MON.dat
if exist profile.txt del profile.txt
call qmake
call make distclean
call qmake
call ctcwrap -i d -2comp -C "EXCLUDE+moc_*.cpp" -C "EXCLUDE+*.UID.CPP" -C "SKIP_FUNCTION_NAME+SifUiInstallIndicatorPlugin::error()" -C "SKIP_FUNCTION_NAME+qt_plugin_query_verification_data" -C "SKIP_FUNCTION_NAME+qt_plugin_instance" make debug-winscw
call ctcwrap -i d -2comp -C "EXCLUDE+moc_*.cpp" -C "EXCLUDE+*.UID.CPP" -C "SKIP_FUNCTION_NAME+SifUiInstallIndicatorPlugin::error()" -C "SKIP_FUNCTION_NAME+qt_plugin_query_verification_data" -C "SKIP_FUNCTION_NAME+qt_plugin_instance" make release-armv5
popd

