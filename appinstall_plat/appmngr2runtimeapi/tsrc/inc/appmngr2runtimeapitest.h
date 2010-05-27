/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   AppMngr2 Runtime API STIF test module
*
*/


#ifndef C_APPMNGR2RUNTIMEAPITEST_H
#define C_APPMNGR2RUNTIMEAPITEST_H

#include <TestScripterInternal.h>       // CScriptBase
#include <appmngr2runtimeobserver.h>    // MAppMngr2RuntimeObserver
#include "appmngr2testcommandobserver.h" // MAppMngr2TestCommandObserver
#include <f32file.h>                    // RFs

class CAppMngr2Runtime;
class CAppMngr2InfoBase;
class CAppMngr2AppInfo;
class CAppMngr2PackageInfo;
class CAppMngr2TestCommand;

#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

_LIT( KAppMngr2RuntimeApiTestModuleName, "appmngr2runtimeapitest.dll" );
_LIT( KAppMngr2RuntimeApiTestLogPath, "\\logs\\testframework\\appmngr2runtimeapitest\\" );
_LIT( KAppMngr2RuntimeApiTestLogFile, "appmngr2runtimeapitest.txt" );
_LIT( KAppMngr2RuntimeApiTestLogFileWithTitle, "appmngr2runtimeapitest_[%S].txt" );


NONSHARABLE_CLASS(CAppMngr2RuntimeApiTest) : public CScriptBase,
        public MAppMngr2RuntimeObserver, public MAppMngr2TestCommandObserver
    {
public:     // constructor and destructor
    static CAppMngr2RuntimeApiTest* NewL( CTestModuleIf& aTestModuleIf );
    ~CAppMngr2RuntimeApiTest();

public:     // from CScriptBase
    TInt RunMethodL( CStifItemParser& aItem );

public:     // from MAppMngr2RuntimeObserver
    void RefreshInstalledApps();
    void RefreshInstallationFiles();

public:     // from MAppMngr2TestCommandObserver
    void CommandResult( CAppMngr2TestCommand& aCommand, TInt aCompletionCode );
    
private:    // new functions
    CAppMngr2RuntimeApiTest( CTestModuleIf& aTestModuleIf );
    void ConstructL();
    void SendTestClassVersion();
    void Print( TRefByValue<const TDesC> aFmt, ... );
    void GetUidFromStringL( const TDesC& aString, TUid& aUid );
    CAppMngr2PackageInfo* FindInstallationPackage( const TDesC& aName );
    CAppMngr2AppInfo* FindInstalledApp( const TDesC& aName );
    void CallInfoBaseFunctionsL( CAppMngr2InfoBase& aInfo, const TDesC& aTestName );
    
    // test functions
    TInt CheckFilesExistL( CStifItemParser& aItem );
    TInt LoadAllPluginsL( CStifItemParser& aItem );
    TInt LoadPluginL( CStifItemParser& aItem );
    TInt CheckPluginsLoadedL( CStifItemParser& aItem );
    TInt LoadIconsL( CStifItemParser& aItem );
    TInt GetAdditionalDirsToScanL( CStifItemParser& aItem );
    TInt GetInstallationFilesL( CStifItemParser& aItem );
    TInt CheckInstallationFilesL( CStifItemParser& aItem );
    TInt ShowInstallationFileDetailsL( CStifItemParser& aItem );
    TInt CheckShowDetailsEndedL( CStifItemParser& aItem );
    TInt CheckNoInstallationFilesL( CStifItemParser& aItem );
    TInt StartInstallFileL( CStifItemParser& aItem );
    TInt CheckInstallEndedL( CStifItemParser& aItem );
    TInt GetInstalledAppsL( CStifItemParser& aItem );
    TInt CheckAppInstalledL( CStifItemParser& aItem );
    TInt ShowInstalledDetailsL( CStifItemParser& aItem );
    TInt CheckNoAppsInstalledL( CStifItemParser& aItem );
    TInt StartUninstallAppL( CStifItemParser& aItem );
    TInt CheckUninstallEndedL( CStifItemParser& aItem );
    
private:    // data
    RPointerArray<CAppMngr2Runtime> iPlugins;
    RPointerArray<CAppMngr2AppInfo> iInstalledApps;
    RPointerArray<CAppMngr2PackageInfo> iInstallationPackages;
    CAppMngr2TestCommand* iInstallCommand;
    CAppMngr2TestCommand* iUninstallCommand;
    CAppMngr2TestCommand* iShowDetailsCommand;
    TInt iSWInstCommonResourceFileOffset;
    TInt iAppMngr2ResourceFileOffset;
    RFs iFs;
    };

#endif  // C_APPMNGR2RUNTIMEAPITEST_H

