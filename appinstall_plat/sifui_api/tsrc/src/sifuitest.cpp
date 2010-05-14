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


//
// NOTE: This cpp file contains just generic STIF module code.
// Test case functions are defined in SifUiTestCases.cpp
//


_LIT( KWaitAndSendThread, "WaitAndSendKey" );
const TInt KWaitAndSendThreadStack = 0x1000;

class TDelayAndKeyEvent {
public:
    TTimeIntervalMicroSeconds32 iDelay;
    TKeyEvent iKeyEvent;
};


// ======== LOCAL FUNCTIONS =========

// -----------------------------------------------------------------------------
// DoSendKeyEventL( const TKeyEvent& aKeyEvent )
// -----------------------------------------------------------------------------
//
void DoSendKeyEventL( const TKeyEvent& aKeyEvent )
    {
    RWsSession wsSession;
    User::LeaveIfError( wsSession.Connect() );
    CleanupClosePushL( wsSession );
    TInt focusedGroup = wsSession.GetFocusWindowGroup();

    TWsEvent event;
    TKeyEvent& keyEvent( *event.Key() );
    TInt err;

    /*
    event.SetType( EEventKeyDown );
    event.SetTimeNow();
    keyEvent = aKeyEvent;
    err = wsSession.SendEventToWindowGroup( focusedGroup, event );
    User::LeaveIfError( err );
    */

    event.SetType( EEventKey );
    event.SetTimeNow();
    keyEvent = aKeyEvent;
    err = wsSession.SendEventToWindowGroup( focusedGroup, event );
    User::LeaveIfError( err );

    /*
    event.SetType( EEventKeyUp );
    event.SetTimeNow();
    keyEvent = aKeyEvent;
    err = wsSession.SendEventToWindowGroup( focusedGroup, event );
    User::LeaveIfError( err );
    */

    CleanupStack::PopAndDestroy( &wsSession );
    }

// -----------------------------------------------------------------------------
// WaitAndSendThreadFunc()
// -----------------------------------------------------------------------------
//
TInt WaitAndSendThreadFunc( TAny* aPtr )
    {
    TDelayAndKeyEvent* params = reinterpret_cast<TDelayAndKeyEvent*>( aPtr );
    ASSERT( params );
    TInt retVal = KErrNone;

    CTrapCleanup* cleanup = CTrapCleanup::New();
    if( !cleanup )
        {
        delete params;
        retVal = KErrNoMemory;
        }

    RThread::Rendezvous( retVal );
    if( retVal == KErrNone )
        {
        User::After( params->iDelay );
        TRAP( retVal, DoSendKeyEventL( params->iKeyEvent ) );
        }

    delete cleanup;
    delete params;
    return retVal;
    }


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
    delete iSifUi;
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

    // Read logger settings
    RSettingServer settingServer;
    TInt ret = settingServer.Connect();
    User::LeaveIfError( ret );
    TLoggerSettings loggerSettings;
    ret = settingServer.GetLoggerSettings( loggerSettings );
    User::LeaveIfError( ret );
    settingServer.Close();

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

// -----------------------------------------------------------------------------
// CSifUiTest::AsyncWaitAndSendKeyEventL()
// -----------------------------------------------------------------------------
//
void CSifUiTest::AsyncWaitAndSendKeyEventL( TTimeIntervalMicroSeconds32 aDelay,
        const TKeyEvent& aKeyEvent ) const
    {
    if( aDelay.Int() )
        {
        TDelayAndKeyEvent* paramPack = new( ELeave ) TDelayAndKeyEvent;
        paramPack->iDelay = aDelay;
        paramPack->iKeyEvent = aKeyEvent;

        RThread waitingAndSendingThread;
        TInt err = waitingAndSendingThread.Create( KWaitAndSendThread, WaitAndSendThreadFunc,
                KWaitAndSendThreadStack, NULL, reinterpret_cast< TAny* >( paramPack ) );
        if( err == KErrNone )
            {
            TRequestStatus status;
            waitingAndSendingThread.Rendezvous( status );
            waitingAndSendingThread.Resume();
            User::WaitForRequest( status );
            waitingAndSendingThread.Close();
            User::LeaveIfError( status.Int() );
            }
        else
            {
            delete paramPack;
            User::Leave( err );
            }
        }
    else
        {
        DoSendKeyEventL( aKeyEvent );
        }
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

    CTestModuleParamVer01* param = CTestModuleParamVer01::NewL();
    param->iTestThreadStackSize = 16384;    // 16K stack
    param->iTestThreadMinHeap = 4096;       // 4K heap min
    param->iTestThreadMaxHeap = 1048576;    // 1M heap max
    aTestModuleParam = param;

    return KErrNone;
    }

