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

pushd mt_startuplistupdater
call ctc_instrument_code.bat
call ctc_compile_tests.bat
call ctc_run_tests.bat
call ctc_remove_instrumentation.bat
popd
