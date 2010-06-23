/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: This file contains Hardcoded module implementation.
*
*/


#include <Stiftestinterface.h>
#include <SettingServerClient.h>
#include <sifui.h>                      // CSifUi
#include "SifUiTest.h"

const TInt KTestStackSize = 0x8000;     // 32K stack
const TInt KTestMinHeap = 0x4000;       // 16K heap min
const TInt KTestMaxHeap = 0x200000;     // 2M heap max


// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CSifUiTest::NewL()
// -----------------------------------------------------------------------------
//
CSifUiTest* CSifUiTest::NewL()
    {
    CSifUiTest* self = new (ELeave) CSifUiTest;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CSifUiTest::~CSifUiTest()
// -----------------------------------------------------------------------------
//
CSifUiTest::~CSifUiTest()
    {
    iLog = NULL;
    delete iStdLog;
    delete iTCLog;
    iFs.Close();

    if( iScheduler )
        {
        CActiveScheduler::Install( NULL );
        delete iScheduler;
        }
    }

// -----------------------------------------------------------------------------
// CSifUiTest::InitL()
// -----------------------------------------------------------------------------
//
TInt CSifUiTest::InitL( TFileName& /*aIniFile*/, TBool /*aFirstTime*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSifUiTest::GetTestCasesL()
// -----------------------------------------------------------------------------
//
TInt CSifUiTest::GetTestCasesL( const TFileName& /*aConfig*/,
        RPointerArray<TTestCaseInfo>& aTestCases )
    {
    for( TInt index = 0; Case( index ).iMethod != NULL; ++index )
        {
        TTestCaseInfo* newCase = new( ELeave ) TTestCaseInfo();
        CleanupStack::PushL( newCase );
        newCase->iCaseNumber = index;
        newCase->iTitle.Copy( Case(index).iCaseName );
        aTestCases.AppendL( newCase );
        CleanupStack::Pop( newCase );
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSifUiTest::RunTestCaseL
// -----------------------------------------------------------------------------
//
TInt CSifUiTest::RunTestCaseL( const TInt aCaseNumber,
        const TFileName& /*aConfig*/, TTestResult& aResult )
    {
    if( !iVersionLogged )
        {
        SendTestModuleVersion();
        iVersionLogged = ETrue;
        }

    TInt returnValue = KErrNone;

    TCaseInfo caseInfo = Case ( aCaseNumber );
    if( caseInfo.iMethod != NULL )
        {
        _LIT( KLogStartTC, "Starting testcase [%S]" );
        iLog->Log( KLogStartTC, &caseInfo.iCaseName );

        // Switch to use test case (TC) logger if necessary
        if( iAddTestCaseTitleToLogName )
            {
            if( iTCLog )
                {
                delete iTCLog;
                iTCLog = NULL;
                }

            TName title;
            TestModuleIf().GetTestCaseTitleL( title );
            TFileName logFileName;
            logFileName.Format( KSifUiTestLogFileWithTitle, &title );
            iTCLog = CStifLogger::NewL( KSifUiTestLogPath, logFileName );
            iLog = iTCLog;
            }

        // Run test case
        iMethod = caseInfo.iMethod;
        TRAPD( err, returnValue  = ( this->*iMethod )( aResult ) );

        // Restore standard log and close test case logger
        if( iAddTestCaseTitleToLogName )
            {
            iLog = iStdLog;
            delete iTCLog;
            iTCLog = NULL;
            }

        // Leave if the case execution failed
        User::LeaveIfError( err );
        }
    else
        {
        returnValue = KErrNotFound;
        }

    // Return case execution status (not the result of the case execution)
    return returnValue;
    }

//-----------------------------------------------------------------------------
// CSifUiTest::SendTestModuleVersion
//-----------------------------------------------------------------------------
//
void CSifUiTest::SendTestModuleVersion()
    {
    TVersion moduleVersion;
    moduleVersion.iMajor = TEST_MODULE_VERSION_MAJOR;
    moduleVersion.iMinor = TEST_MODULE_VERSION_MINOR;
    moduleVersion.iBuild = TEST_MODULE_VERSION_BUILD;

    TFileName moduleName;
    moduleName = _L("SifUiTest.dll");

    TBool newVersionOfMethod = ETrue;
    TestModuleIf().SendTestModuleVersion( moduleVersion, moduleName, newVersionOfMethod );
    }

// -----------------------------------------------------------------------------
// CSifUiTest::CSifUiTest()
// -----------------------------------------------------------------------------
//
CSifUiTest::CSifUiTest()
    {
    }

// -----------------------------------------------------------------------------
// CSifUiTest::ConstructL()
// -----------------------------------------------------------------------------
//
void CSifUiTest::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );

    RSettingServer settingsServer;
    User::LeaveIfError( settingsServer.Connect() );
    CleanupClosePushL( settingsServer );

    TLoggerSettings loggerSettings;
    User::LeaveIfError( settingsServer.GetLoggerSettings( loggerSettings ) );

    CleanupStack::PopAndDestroy( &settingsServer );

    iAddTestCaseTitleToLogName = loggerSettings.iAddTestCaseTitle;
    iStdLog = CStifLogger::NewL( KSifUiTestLogPath, KSifUiTestLogFile );
    iLog = iStdLog;

    _LIT( KLogStart, "SifUiTest starts" );
    iLog->Log( KLogStart );

    iVersionLogged = EFalse;

    if( !CActiveScheduler::Current() )
        {
        iScheduler = new( ELeave ) CActiveScheduler;
        CActiveScheduler::Install( iScheduler );
        }
    }

// -----------------------------------------------------------------------------
// CSifUiTest::ErrorCodeString()
// -----------------------------------------------------------------------------
//
TPtrC CSifUiTest::ErrorCodeString( TInt aErrorCode ) const
    {
    switch( aErrorCode )
        {
        case KErrNone:
            return _L("KErrNone");
        case KErrNotFound:
            return _L("KErrNotFound");
        case KErrGeneral:
            return _L("KErrGeneral");
        case KErrCancel:
            return _L("KErrCancel");
        case KErrNoMemory:
            return _L("KErrNoMemory");
        case KErrNotSupported:
            return _L("KErrNotSupported");
        case KErrArgument:
            return _L("KErrArgument");
        case KErrTotalLossOfPrecision:
            return _L("KErrTotalLossOfPrecision");
        case KErrBadHandle:
            return _L("KErrBadHandle");
        case KErrOverflow:
            return _L("KErrOverflow");
        case KErrUnderflow:
            return _L("KErrUnderflow");
        case KErrAlreadyExists:
            return _L("KErrAlreadyExists");
        case KErrPathNotFound:
            return _L("KErrPathNotFound");
        case KErrDied:
            return _L("KErrDied");
        case KErrInUse:
            return _L("KErrInUse");
        case KErrServerTerminated:
            return _L("KErrServerTerminated");
        case KErrServerBusy:
            return _L("KErrServerBusy");
        case KErrCompletion:
            return _L("KErrCompletion");
        case KErrNotReady:
            return _L("KErrNotReady");
        case KErrUnknown:
            return _L("KErrUnknown");
        case KErrCorrupt:
            return _L("KErrCorrupt");
        case KErrAccessDenied:
            return _L("KErrAccessDenied");
        case KErrLocked:
            return _L("KErrLocked");
        case KErrWrite:
            return _L("KErrWrite");
        case KErrDisMounted:
            return _L("KErrDisMounted");
        case KErrEof:
            return _L("KErrEof");
        case KErrDiskFull:
            return _L("KErrDiskFull");
        case KErrBadDriver:
            return _L("KErrBadDriver");
        case KErrBadName:
            return _L("KErrBadName");
        case KErrCommsLineFail:
            return _L("KErrCommsLineFail");
        case KErrCommsFrame:
            return _L("KErrCommsFrame");
        case KErrCommsOverrun:
            return _L("KErrCommsOverrun");
        case KErrCommsParity:
            return _L("KErrCommsParity");
        case KErrTimedOut:
            return _L("KErrTimedOut");
        case KErrCouldNotConnect:
            return _L("KErrCouldNotConnect");
        case KErrCouldNotDisconnect:
            return _L("KErrCouldNotDisconnect");
        case KErrDisconnected:
            return _L("KErrDisconnected");
        case KErrBadLibraryEntryPoint:
            return _L("KErrBadLibraryEntryPoint");
        case KErrBadDescriptor:
            return _L("KErrBadDescriptor");
        case KErrAbort:
            return _L("KErrAbort");
        case KErrTooBig:
            return _L("KErrTooBig");
        case KErrDivideByZero:
            return _L("KErrDivideByZero");
        case KErrBadPower:
            return _L("KErrBadPower");
        case KErrDirFull:
            return _L("KErrDirFull");
        case KErrHardwareNotAvailable:
            return _L("KErrHardwareNotAvailable");
        case KErrSessionClosed:
            return _L("KErrSessionClosed");
        case KErrPermissionDenied:
            return _L("KErrPermissionDenied");
        case KErrExtensionNotSupported:
            return _L("KErrExtensionNotSupported");
        case KErrCommsBreak:
            return _L("KErrCommsBreak");
        case KErrNoSecureTime:
            return _L("KErrNoSecureTime");
        }
    return _L("UNKNOWN ERROR CODE");
    }

// -----------------------------------------------------------------------------
// CSifUiTest::SetResult()
// -----------------------------------------------------------------------------
//
void CSifUiTest::SetResult( TTestResult& aResult, const TInt aReturnCode ) const
    {
    aResult.SetResult( aReturnCode, ErrorCodeString( aReturnCode ) );
    }


// ==== OTHER EXPORTED FUNCTIONS ====

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point
// -----------------------------------------------------------------------------
//
EXPORT_C CTestModuleBase* LibEntryL()
    {
    return CSifUiTest::NewL();
    }

// -----------------------------------------------------------------------------
// SetRequirements handles test module parameters
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SetRequirements( CTestModuleParam*& aTestModuleParam,
        TUint32& aParameterValid )
    {
    aParameterValid = KStifTestModuleParameterChanged;

    CTestModuleParamVer01* param = NULL;
    TRAPD( error, param = CTestModuleParamVer01::NewL() );
    if( !error )
        {
        param->iTestThreadStackSize = KTestStackSize;
        param->iTestThreadMinHeap = KTestMinHeap;
        param->iTestThreadMaxHeap = KTestMaxHeap;
        aTestModuleParam = param;
        }

    return error;
    }

