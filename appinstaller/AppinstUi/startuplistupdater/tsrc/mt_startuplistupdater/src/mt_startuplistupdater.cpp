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
* Description:  Startup list updater test module.
*
*/

#include "mt_startuplistupdater.h"          // CTestStartupListUpdater
#include <SettingServerClient.h>            // RSettingServer


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CTestStartupListUpdater::CTestStartupListUpdater
// ---------------------------------------------------------------------------
//
CTestStartupListUpdater::CTestStartupListUpdater( CTestModuleIf& aTestModuleIf ) :
        CScriptBase( aTestModuleIf )
    {
    }

// ---------------------------------------------------------------------------
// CTestStartupListUpdater::ConstructL
// ---------------------------------------------------------------------------
//
void CTestStartupListUpdater::ConstructL()
    {
    // Read settings to check if test case name is to be appended to file name.
    RSettingServer settingsServer;
    User::LeaveIfError( settingsServer.Connect() );
    CleanupClosePushL( settingsServer );

    TLoggerSettings loggerSettings;
    User::LeaveIfError( settingsServer.GetLoggerSettings( loggerSettings ) );

    CleanupStack::PopAndDestroy( &settingsServer );

    TFileName logFileName;
    if( loggerSettings.iAddTestCaseTitle )
        {
        TName title;
        TestModuleIf().GetTestCaseTitleL( title );
        logFileName.Format( KTestStartupListUpdaterLogFileWithTitle, &title );
        }
    else
        {
        logFileName.Copy( KTestStartupListUpdaterLogFile );
        }

    iLog = CStifLogger::NewL( KTestStartupListUpdaterLogPath, logFileName,
        CStifLogger::ETxt, CStifLogger::EFile, EFalse );
    SendTestClassVersion();
    }

// ---------------------------------------------------------------------------
// CTestStartupListUpdater::NewL
// ---------------------------------------------------------------------------
//
CTestStartupListUpdater* CTestStartupListUpdater::NewL( CTestModuleIf& aTestModuleIf )
    {
    CTestStartupListUpdater* self = new( ELeave ) CTestStartupListUpdater( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// CTestStartupListUpdater::~CTestStartupListUpdater()
// ---------------------------------------------------------------------------
//
CTestStartupListUpdater::~CTestStartupListUpdater()
    {
    Delete();
    delete iLog;
    }

// ---------------------------------------------------------------------------
// CTestStartupListUpdater::SendTestClassVersion
// ---------------------------------------------------------------------------
//
void CTestStartupListUpdater::SendTestClassVersion()
    {
    TVersion moduleVersion;
    moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
    moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
    moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;

    TFileName moduleName;
    moduleName = _L("mt_startuplistupdater.dll");

    TBool newVersionOfMethod = ETrue;
    TestModuleIf().SendTestModuleVersion( moduleVersion, moduleName, newVersionOfMethod );
    }


// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// LibEntryL()
// ---------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( CTestModuleIf& aTestModuleIf )
    {
    return reinterpret_cast< CScriptBase* >( CTestStartupListUpdater::NewL( aTestModuleIf ) );
    }

