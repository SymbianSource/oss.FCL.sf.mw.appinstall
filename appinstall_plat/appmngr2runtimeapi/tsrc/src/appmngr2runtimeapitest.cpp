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


#include "appmngr2runtimeapitest.h"     // CRuntimeApiTest
#include "appmngr2testcommand.h"        // CAppMngr2TestCommand
#include <StifLogger.h>                 // CStifLogger
#include <SettingServerClient.h>        // RSettingServer
#include <ecom/ecom.h>                  // REComSession
#include <appmngr2runtime.h>            // AppMngr2 Runtime API
#include <appmngr2appinfo.h>            // AppMngr2 Runtime API
#include <appmngr2packageinfo.h>        // AppMngr2 Runtime API
#include <appmngr2recognizedfile.h>     // AppMngr2 Runtime API
#include <appmngr2driveutils.h>         // AppMngr2 Runtime API
#include <appmngr2common.hrh>           // AppMngr2 Runtime API
#include <appmngr2cleanuputils.h>       // CleanupResetAndDestroyPushL
#include <apmstd.h>                     // TDataType
#include <caf/caf.h>                    // ContentAccess
#include <bautils.h>                    // BaflUtils
#include <AknIconArray.h>               // CAknIconArray
#include <gulicon.h>                    // CGulIcon
#include <data_caging_path_literals.hrh> // KDC_APP_RESOURCE_DIR

const TInt KMaxLineLength = 256;
const TInt KGranularity = 8;

_LIT( KSWInstCommonUIResourceFileName, "SWInstCommonUI.rsc" );
_LIT( KAppMngr2UIResourceFileName, "appmngr2.rsc" );
_LIT( KAppRscFormat, "z:%S%S" );

_LIT( KAppMngr2Tag, "AppMngr2" );
_LIT( KTestStartFormat, "%S: start" );
_LIT( KTestDoneFormat, "%S: done (error %d)" );
_LIT( KResultFormat, "%S: %S, result %d" );
_LIT( KNotFoundFormat, "%S: %S not found" );


// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::NewL()
// -----------------------------------------------------------------------------
//
CAppMngr2RuntimeApiTest* CAppMngr2RuntimeApiTest::NewL( CTestModuleIf& aTestModuleIf )
    {
    CAppMngr2RuntimeApiTest* self = new (ELeave) CAppMngr2RuntimeApiTest( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::~CAppMngr2RuntimeApiTest()
// -----------------------------------------------------------------------------
//
CAppMngr2RuntimeApiTest::~CAppMngr2RuntimeApiTest()
    {
    delete iInstallCommand;
    delete iUninstallCommand;
    delete iShowDetailsCommand;
    iInstalledApps.ResetAndDestroy();
    iInstallationPackages.ResetAndDestroy();
    iPlugins.ResetAndDestroy();
    
    CCoeEnv* coeEnv = CCoeEnv::Static();
    if( coeEnv )
        {
        if( iSWInstCommonResourceFileOffset )
            {
            coeEnv->DeleteResourceFile( iSWInstCommonResourceFileOffset );
            }
        if( iAppMngr2ResourceFileOffset )
            {
            coeEnv->DeleteResourceFile( iAppMngr2ResourceFileOffset );
            }
        }
    
    delete iLog;
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::RunMethodL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::RunMethodL( CStifItemParser& aItem )
    {
    static TStifFunctionInfo const KFunctions[] =
        {
        ENTRY( "CheckFilesExist", CAppMngr2RuntimeApiTest::CheckFilesExistL ),
        ENTRY( "LoadAllPlugins", CAppMngr2RuntimeApiTest::LoadAllPluginsL ),
        ENTRY( "LoadPlugin", CAppMngr2RuntimeApiTest::LoadPluginL ),
        ENTRY( "CheckPluginsLoaded", CAppMngr2RuntimeApiTest::CheckPluginsLoadedL ),
        ENTRY( "LoadIcons", CAppMngr2RuntimeApiTest::LoadIconsL ),
        ENTRY( "GetAdditionalDirsToScan", CAppMngr2RuntimeApiTest::GetAdditionalDirsToScanL ),
        ENTRY( "GetInstallationFiles", CAppMngr2RuntimeApiTest::GetInstallationFilesL ),
        ENTRY( "CheckInstallationFiles", CAppMngr2RuntimeApiTest::CheckInstallationFilesL ),
        ENTRY( "ShowInstallationFileDetails", CAppMngr2RuntimeApiTest::ShowInstallationFileDetailsL  ),
        ENTRY( "CheckShowDetailsEnded", CAppMngr2RuntimeApiTest::CheckShowDetailsEndedL ),
        ENTRY( "CheckNoInstallationFiles", CAppMngr2RuntimeApiTest::CheckNoInstallationFilesL ),
        ENTRY( "StartInstallFile", CAppMngr2RuntimeApiTest::StartInstallFileL ),
        ENTRY( "CheckInstallEnded", CAppMngr2RuntimeApiTest::CheckInstallEndedL ),
        ENTRY( "GetInstalledApps", CAppMngr2RuntimeApiTest::GetInstalledAppsL ),
        ENTRY( "CheckAppInstalled", CAppMngr2RuntimeApiTest::CheckAppInstalledL ),
        ENTRY( "ShowInstalledDetails", CAppMngr2RuntimeApiTest::ShowInstalledDetailsL  ),
        ENTRY( "CheckNoAppsInstalled", CAppMngr2RuntimeApiTest::CheckNoAppsInstalledL ),
        ENTRY( "StartUninstallApp", CAppMngr2RuntimeApiTest::StartUninstallAppL ),
        ENTRY( "CheckUninstallEnded", CAppMngr2RuntimeApiTest::CheckUninstallEndedL )
        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );
    return RunInternalL( KFunctions, count, aItem );
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::RefreshInstalledApps()
// -----------------------------------------------------------------------------
//
void CAppMngr2RuntimeApiTest::RefreshInstalledApps()
    {
    _LIT( KFunctionName, "RefreshInstalledApps()" );
    Print( KFunctionName );
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::RefreshInstallationFiles()
// -----------------------------------------------------------------------------
//
void CAppMngr2RuntimeApiTest::RefreshInstallationFiles()
    {
    _LIT( KFunctionName, "RefreshInstallationFiles()" );
    Print( KFunctionName );
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::CommandResult()
// -----------------------------------------------------------------------------
//
void CAppMngr2RuntimeApiTest::CommandResult( CAppMngr2TestCommand& aCommand,
        TInt aCompletionCode )
    {
    _LIT( KCommandResultFormat, "async command completed (%S), result %d" );
    if( &aCommand == iInstallCommand )
        {
        _LIT( KCommandName, "install" );
        Print( KCommandResultFormat, &KCommandName, aCompletionCode );
        }
    else if( &aCommand == iUninstallCommand )
        {
        _LIT( KCommandName, "uninstall" );
        Print( KCommandResultFormat, &KCommandName, aCompletionCode );
        }
    else
        {
        _LIT( KCommandName, "UNKNOWN" );
        Print( KCommandResultFormat, &KCommandName, aCompletionCode );
        }
    Signal();       // releases "waittestclass" command in cfg file
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::CAppMngr2RuntimeApiTest()
// -----------------------------------------------------------------------------
//
CAppMngr2RuntimeApiTest::CAppMngr2RuntimeApiTest( CTestModuleIf& aTestModuleIf )
        : CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::ConstructL()
// -----------------------------------------------------------------------------
//
void CAppMngr2RuntimeApiTest::ConstructL()
    {
    // Logger settings define if test case name must be appended to log file name
    RSettingServer settingServer;
    User::LeaveIfError( settingServer.Connect() );
    CleanupClosePushL( settingServer );
    TLoggerSettings loggerSettings;
    User::LeaveIfError( settingServer.GetLoggerSettings( loggerSettings ) );
    CleanupStack::PopAndDestroy( &settingServer );

    User::LeaveIfError( iFs.Connect() );
    
    TFileName logFileName;
    if( loggerSettings.iAddTestCaseTitle )
        {
        TName title;
        TestModuleIf().GetTestCaseTitleL( title );
        logFileName.Format( KAppMngr2RuntimeApiTestLogFileWithTitle, &title );
        }
    else
        {
        logFileName.Copy( KAppMngr2RuntimeApiTestLogFile );
        }

    iLog = CStifLogger::NewL( KAppMngr2RuntimeApiTestLogPath, logFileName,
        CStifLogger::ETxt, CStifLogger::EFile, EFalse );
    
    SendTestClassVersion();

    _LIT( KRuntimeApiTest, "CAppMngr2RuntimeApiTest::ConstructL()" );
    Print( KTestStartFormat, &KRuntimeApiTest );
    
    // Ensure that CCoeEnv exists, RuntimeAPI needs it
    CCoeEnv* coeEnv = CCoeEnv::Static();
    _LIT( KRuntimeApiTestCoeEnvFormat, "%S: coeEnv = 0x%08x");
    Print( KRuntimeApiTestCoeEnvFormat, &KRuntimeApiTest, coeEnv );
    User::LeaveIfNull( coeEnv );
    
    // Load resource files used by AppMngr2 Runtime API and plug-ins
    TFileName* fullName = TAppMngr2DriveUtils::NearestResourceFileLC(
            KSWInstCommonUIResourceFileName, iFs );
    _LIT( KRuntimeApiTestResourceFileFormat, "%S: resource file %S");
    Print( KRuntimeApiTestResourceFileFormat, &KRuntimeApiTest, fullName );
    iSWInstCommonResourceFileOffset = coeEnv->AddResourceFileL( *fullName );
    CleanupStack::PopAndDestroy( fullName );
    
    fullName = new (ELeave) TFileName;
    CleanupStack::PushL( fullName );
    fullName->Format( KAppRscFormat, &KDC_APP_RESOURCE_DIR, &KAppMngr2UIResourceFileName );
    BaflUtils::NearestLanguageFile( iFs, *fullName );
    Print( KRuntimeApiTestResourceFileFormat, &KRuntimeApiTest, fullName );
    iAppMngr2ResourceFileOffset = coeEnv->AddResourceFileL( *fullName );
    CleanupStack::PopAndDestroy( fullName );
    
    _LIT( KCompleteFormat, "%S: complete");
    Print( KCompleteFormat, &KRuntimeApiTest );
    }

//-----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::SendTestClassVersion()
//-----------------------------------------------------------------------------
//
void CAppMngr2RuntimeApiTest::SendTestClassVersion()
    {
    TVersion moduleVersion;
    moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
    moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
    moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;

    TFileName moduleName;
    moduleName = KAppMngr2RuntimeApiTestModuleName;

    TBool newVersionOfMethod = ETrue;
    TestModuleIf().SendTestModuleVersion( moduleVersion, moduleName, newVersionOfMethod );
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::Print()
// -----------------------------------------------------------------------------
//
void CAppMngr2RuntimeApiTest::Print( TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf<KMaxLineLength> buf;
    buf.FormatList( aFmt, list );
    TestModuleIf().Printf( 0, KAppMngr2Tag, buf );
    iLog->Log( buf );
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::GetUidFromStringL()
// -----------------------------------------------------------------------------
//
void CAppMngr2RuntimeApiTest::GetUidFromStringL( const TDesC& aString, TUid& aUid )
    {
    TUint32 uidValue;
    TLex lex( aString );
    User::LeaveIfError( lex.Val( uidValue, EHex ) );
    aUid.iUid = uidValue;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::FindInstallationPackage()
// -----------------------------------------------------------------------------
//
CAppMngr2PackageInfo* CAppMngr2RuntimeApiTest::FindInstallationPackage(
        const TDesC& aName )
    {
    TInt count = iInstallationPackages.Count();
    for( TInt index = 0; index < count; index++ )
        {
        if( iInstallationPackages[ index ]->Name().CompareC( aName ) == 0 )
            {
            return iInstallationPackages[ index ];
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::FindInstalledApp()
// -----------------------------------------------------------------------------
//
CAppMngr2AppInfo* CAppMngr2RuntimeApiTest::FindInstalledApp(
        const TDesC& aName )
    {
    TInt count = iInstalledApps.Count();
    for( TInt index = 0; index < count; index++ )
        {
        if( iInstalledApps[ index ]->Name().CompareC( aName ) == 0 )
            {
            return iInstalledApps[ index ];
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::CallInfoBaseFunctionsL()
// -----------------------------------------------------------------------------
//
void CAppMngr2RuntimeApiTest::CallInfoBaseFunctionsL( CAppMngr2InfoBase& aInfo,
        const TDesC& aTestName )
    {
    // Call CAppMngr2InfoBase and CAppMngr2Runtime members to get better code coverage.
    CAppMngr2Runtime& runtime( aInfo.Runtime() );
    MAppMngr2RuntimeObserver& runtimeObserver( runtime.Observer() );
    TInt testResult = KErrNone;
    
    RPointerArray<CEikMenuPaneItem::SData> menuPaneData;
    CleanupResetAndDestroyPushL( menuPaneData );
    TRAP( testResult, aInfo.GetMenuItemsL( menuPaneData ) );
    _LIT( KGetMenuItemsFormat, "%S: GetMenuItemsL, count %d, result %d" );
    Print( KGetMenuItemsFormat, &aTestName, menuPaneData.Count(), testResult );
    User::LeaveIfError( testResult );
    CleanupStack::PopAndDestroy( &menuPaneData );
    
    TBool isDetailsSupported = aInfo.SupportsGenericCommand( EAppMngr2CmdViewDetails );
    _LIT( KIsDetailsSupportedFormat, "%S: SupportsGenericCommand returned %d" );
    Print( KIsDetailsSupportedFormat, &aTestName, isDetailsSupported );
    
    TInt resourceId;
    TInt commandId;
    TRAP( testResult, aInfo.GetMiddleSoftkeyCommandL( resourceId, commandId ) );
    _LIT( KGetMiddleSoftkeyCmdFormat, "%S: GetMiddleSoftkeyCommandL, result %d" );
    Print( KGetMiddleSoftkeyCmdFormat, &aTestName, testResult );
    User::LeaveIfError( testResult );
                
    TBool isShowOnTop = aInfo.IsShowOnTop();
    _LIT( KIsShowOnTopFormat, "%S: IsShowOnTop returned %d" );
    Print( KIsShowOnTopFormat, &aTestName, isShowOnTop );
    
    TInt iconIndex = aInfo.IconIndex();
    _LIT( KIconIndexFormat, "%S: IconIndex returned %d" );
    Print( KIconIndexFormat, &aTestName, iconIndex );

    CGulIcon* icon = NULL;
    TRAP( testResult, icon = aInfo.SpecificIconL() );
    delete icon;
    icon = NULL;
    _LIT( KSpecificIconFormat, "%S: SpecificIcon, result %d" );
    Print( KSpecificIconFormat, &aTestName, testResult );
    if( testResult != KErrNone && testResult != KErrNotSupported )
        {
        User::Leave( testResult );
        }
    
    TInt indIconIndex = aInfo.IndicatorIconIndex();
    _LIT( KIndIconIndexFormat, "%S: IndicatorIconIndex returned %d" );
    Print( KIndIconIndexFormat, &aTestName, indIconIndex );

    TRAP( testResult, icon = aInfo.SpecificIndicatorIconL() );
    delete icon;
    icon = NULL;
    _LIT( KSpecificIndIconFormat, "%S: SpecificIndicatorIcon, result %d" );
    Print( KSpecificIndIconFormat, &aTestName, testResult );
    if( testResult != KErrNone && testResult != KErrNotSupported )
        {
        User::Leave( testResult );
        }
    
    TAppMngr2Location location = aInfo.Location();
    _LIT( KLocationFormat, "%S: Location returned %d" );
    Print( KLocationFormat, &aTestName, location );

    TDriveUnit driveUnit = aInfo.LocationDrive();
    _LIT( KLocationDriveFormat, "%S: LocationDrive returned %S" );
    TDriveName driveName( driveUnit.Name() );
    Print( KLocationDriveFormat, &aTestName, &driveName );
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::CheckFilesExistL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::CheckFilesExistL( CStifItemParser& aItem )
    {
    _LIT( KTestName, "CheckFilesExist" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;

    TPtrC dirName;
    User::LeaveIfError( aItem.GetNextString( dirName ) );
    
    TPtrC fileName;
    while( aItem.GetNextString( fileName ) == KErrNone )
        {
        TParse parse;
        User::LeaveIfError( parse.SetNoWild( fileName, &dirName, NULL ) );
        TPtrC fullName = parse.FullName();
        TEntry entry;
        testResult = iFs.Entry( fullName, entry );
        Print( KResultFormat, &KTestName, &fileName, testResult );
        }
    
    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::LoadAllPluginsL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::LoadAllPluginsL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestName, "LoadAllPlugins" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;
    iPlugins.ResetAndDestroy();
    
    RImplInfoPtrArray implInfoArray;
    CleanupResetAndDestroyPushL( implInfoArray  );
    REComSession::ListImplementationsL( KAppMngr2PluginInterface, implInfoArray );
    
    TInt count = implInfoArray.Count();
    _LIT( KPluginCountFormat, "%S: %d plugins found" );
    Print( KPluginCountFormat, &KTestName, count );

    for( TInt index = 0; index < count; index++ )
        {
        CImplementationInformation* implInfo = implInfoArray[ index ];
        CAppMngr2Runtime* plugin = NULL;
        TPtrC pluginName = implInfo->DisplayName();
        TUid pluginUid = implInfo->ImplementationUid();
        TRAP( testResult, plugin = CAppMngr2Runtime::NewL( pluginUid, *this ) );
        _LIT( KLoadingFormat,  "%S: index %d, loading %S (0x%08x), result %d" );
        Print( KLoadingFormat, &KTestName, index, &pluginName, pluginUid.iUid, testResult );
        if( testResult == KErrNone )
            {
            CleanupStack::PushL( plugin );
            iPlugins.AppendL( plugin );
            CleanupStack::Pop( plugin );
            }
        }

    CleanupStack::PopAndDestroy( &implInfoArray );
    REComSession::FinalClose();

    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::LoadPluginL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::LoadPluginL( CStifItemParser& aItem )
    {
    _LIT( KTestName, "LoadPlugin" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;

    TPtrC string;
    User::LeaveIfError( aItem.GetNextString ( string ) );
    TUid uid;
    GetUidFromStringL( string, uid );
    _LIT( KPluginCountFormat, "%S: plugin UID 0x%08x" );
    Print( KPluginCountFormat, &KTestName, uid.iUid );
    
    RImplInfoPtrArray implInfoArray;
    CleanupResetAndDestroyPushL( implInfoArray  );
    REComSession::ListImplementationsL( KAppMngr2PluginInterface, implInfoArray );
    
    CImplementationInformation* implInfo;
    TInt count = implInfoArray.Count();
    TBool found = EFalse;
    for( TInt index = 0; index < count && !found; index++ )
        {
        implInfo = implInfoArray[ index ];
        if( implInfo->ImplementationUid() == uid )
            {
            CAppMngr2Runtime* plugin = NULL;
            found = ETrue;

            _LIT( KLoadingFormat, "%S: loading %S" );
            Print( KLoadingFormat, &KTestName, &( implInfo->DisplayName() ) );
            TRAP( testResult, plugin = CAppMngr2Runtime::NewL(
                    implInfo->ImplementationUid(), *this ) );
            if( testResult == KErrNone )
                {
                CleanupStack::PushL( plugin );
                iPlugins.AppendL( plugin );
                CleanupStack::Pop( plugin );
                }
            }
        }
    if( !found )
        {
        _LIT( KPluginNotFound, "%S: plugin not found" );
        Print( KPluginNotFound, &KTestName );
        testResult = KErrNotFound;
        }

    CleanupStack::PopAndDestroy( &implInfoArray );
    REComSession::FinalClose();

    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::CheckPluginsLoadedL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::CheckPluginsLoadedL( CStifItemParser& aItem )
    {
    _LIT( KTestName, "CheckPluginsLoaded" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;

    TPtrC pluginUidString;
    while( aItem.GetNextString( pluginUidString ) == KErrNone )
        {
        TUid uid;
        GetUidFromStringL( pluginUidString, uid );
        
        TBool found = EFalse;
        for( TInt index = 0; index < iPlugins.Count() && !found; index++ )
            {
            found = ( iPlugins[ index ]->RuntimeUid() == uid );
            }
        
        _LIT( KCheckPluginsLoadedFormat, "%S: 0x%08x, result %d" );
        Print( KCheckPluginsLoadedFormat, &KTestName, uid.iUid, found );
        if( !found )
            {
            testResult = KErrNotFound;
            }
        }

    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::LoadIconsL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::LoadIconsL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestName, "LoadIcons" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;

    for( TInt pluginIndex = 0; pluginIndex < iPlugins.Count(); pluginIndex++ )
        {
        CAppMngr2Runtime* plugin = iPlugins[ pluginIndex ];
        TInt pluginId = plugin->RuntimeUid().iUid;
        
        CAknIconArray* iconArray = new (ELeave) CAknIconArray( KGranularity );
        CleanupStack::PushL( iconArray );
        TRAP( testResult, plugin->LoadIconsL( *iconArray ) );
        _LIT( KLoadIconsFormat, "%S: plugin 0x%08x, icons count %d, error %d" );
        Print( KLoadIconsFormat, &KTestName, pluginId, iconArray->Count(), testResult );
        CleanupStack::PopAndDestroy( iconArray );
        }

    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::GetAdditionalDirsToScanL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::GetAdditionalDirsToScanL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestName, "GetAdditionalDirsToScan" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;

    for( TInt pluginIndex = 0; pluginIndex < iPlugins.Count(); pluginIndex++ )
        {
        CAppMngr2Runtime* plugin = iPlugins[ pluginIndex ];
        TInt pluginId = plugin->RuntimeUid().iUid;
        
        RPointerArray<HBufC> dirs;
        CleanupResetAndDestroyPushL( dirs );
        TRAP( testResult, plugin->GetAdditionalDirsToScanL( iFs, dirs ) );
        _LIT( KDirsToScanFormat, "%S: plugin 0x%08x, additional dirs, count %d, error %d" );
        Print( KDirsToScanFormat, &KTestName, pluginId, dirs.Count(), testResult );
        if( testResult == KErrNone )
            {
            for( TInt dirIndex = 0; dirIndex < dirs.Count(); dirIndex++ )
                {
                _LIT( KScanDirFormat, "%S: plugin 0x%08x, dir %S" );
                Print( KScanDirFormat, &KTestName, pluginId, dirs[ dirIndex ] );
                }
            }        
        CleanupStack::PopAndDestroy( &dirs );
        }
        
    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::GetInstallationFilesL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::GetInstallationFilesL( CStifItemParser& aItem )
    {
    _LIT( KTestName, "GetInstallationFiles" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;
    iInstallationPackages.ResetAndDestroy();
    
    TPtrC dirName;
    User::LeaveIfError( aItem.GetNextString( dirName ) );
    _LIT( KParameterFormat, "%S: dir %S" );
    Print( KParameterFormat, &KTestName, &dirName );
    
    RPointerArray<CAppMngr2RecognizedFile> fileArray;
    CleanupResetAndDestroyPushL( fileArray );
    
    // recognize files in given directory
    CDir* dir = NULL;
    User::LeaveIfError( iFs.GetDir( dirName, KEntryAttNormal, ESortNone, dir ) );
    CleanupStack::PushL( dir );
    TInt dirCount = dir->Count();
    for( TInt dirIndex = 0; dirIndex < dirCount; dirIndex++ )
        {
        const TChar KBackslash = '\\';
        const TInt KBackslashLen = 1;

        const TEntry& file = ( *dir )[ dirIndex ];
        
        // full file name
        HBufC* fullName = HBufC::NewLC( dirName.Length() + KBackslashLen + file.iName.Length() );
        TPtr namePtr( fullName->Des() );
        namePtr.Copy( dirName );
        if( dirName.LocateReverse( KBackslash ) != ( dirName.Length() - 1 ) )
            {
            namePtr.Append( KBackslash );
            }
        namePtr.Append( file.iName );

        // data type
        HBufC* mimeType = HBufC::NewLC( KMaxDataTypeLength );
        ContentAccess::CContent* content = ContentAccess::CContent::NewLC( *fullName,
                ContentAccess::EContentShareReadWrite );
        TPtr mimePtr( mimeType->Des() );
        content->GetStringAttribute( ContentAccess::EMimeType, mimePtr );
        
        _LIT( KFileTypeFormat, "%S: file %S, type %S" );
        Print( KFileTypeFormat, &KTestName, fullName, mimeType );

        // create new recognized file
        CAppMngr2RecognizedFile* recFile = CAppMngr2RecognizedFile::NewL( fullName, mimeType );
        CleanupStack::PopAndDestroy( content );
        CleanupStack::Pop( 2, fullName );  // mimeType and fullName now owned by recFile
        
        // append it into array
        CleanupStack::PushL( recFile );
        fileArray.AppendL( recFile );
        CleanupStack::Pop( recFile );
        }
    CleanupStack::PopAndDestroy( dir );
    dir = NULL;

    for( TInt pluginIndex = 0; pluginIndex < iPlugins.Count(); pluginIndex++ )
        {
        CAppMngr2Runtime* plugin = iPlugins[ pluginIndex ];
        TInt pluginId = plugin->RuntimeUid().iUid;
        
        CDataTypeArray* dataTypeArray = new (ELeave) CDataTypeArray( KGranularity );
        CleanupStack::PushL( dataTypeArray );
        TRAP( testResult, plugin->GetSupportedDataTypesL( *dataTypeArray ) );
        _LIT( KSuppDataTypeFormat, "%S: plugin 0x%08x, data type count %d, error %d" );
        Print( KSuppDataTypeFormat, &KTestName, pluginId, dataTypeArray->Count(), testResult );
        if( testResult == KErrNone )
            {
            // copy those recognized files that matches plugin data types in pluginFiles
            RPointerArray<CAppMngr2RecognizedFile> pluginFiles;
            CleanupResetAndDestroyPushL( pluginFiles );
            for( TInt dataTypeIndex = 0; dataTypeIndex < dataTypeArray->Count(); dataTypeIndex++ )
                {
                for( TInt fileIndex = 0; fileIndex < fileArray.Count(); fileIndex++ )
                    {
                    if( fileArray[ fileIndex ]->DataType() == (*dataTypeArray)[ dataTypeIndex ] )
                        {
                        _LIT( KPluginGetsFileFormat, "%S: plugin 0x%08x, matching file %S" );
                        Print( KPluginGetsFileFormat, &KTestName, pluginId,
                                &( fileArray[ fileIndex ]->FileName() ) );
    
                        CAppMngr2RecognizedFile* recFile = CAppMngr2RecognizedFile::NewL(
                                fileArray[ fileIndex ]->FileName(),
                                fileArray[ fileIndex ]->DataType() );
                        CleanupStack::PushL( recFile );
                        pluginFiles.Append( recFile );
                        CleanupStack::Pop( recFile );
                        }
                    }
                }
            _LIT( KPluginFilesFormat, "%S: plugin 0x%08x, total %d files" );
            Print( KPluginFilesFormat, &KTestName, pluginId, pluginFiles.Count() );
            
            // get installation files from plugin
            RPointerArray<CAppMngr2PackageInfo> packageArray;
            CleanupResetAndDestroyPushL( packageArray );
            TRequestStatus status;
            TRAP( testResult, plugin->GetInstallationFilesL( packageArray, pluginFiles, iFs, status ) );
            _LIT( KInstFilesReqFormat, "%S: plugin 0x%08x, inst files req, error %d, status %d" );
            Print( KInstFilesReqFormat, &KTestName, pluginId, testResult, status.Int() );
            if( testResult == KErrNone )
                {
                User::WaitForRequest( status );
                _LIT( KInstFilesCompleteFormat, "%S: plugin 0x%08x, package count %d, status %d" );
                Print( KInstFilesCompleteFormat, &KTestName, pluginId, packageArray.Count(),
                        status.Int() );
                
                // move items to iInstallationPackages array
                for( TInt i = packageArray.Count() - 1; i >= 0; i-- )
                    {
                    CAppMngr2PackageInfo* packageInfo = packageArray[ i ];
                    packageArray.Remove( i );
                    CleanupStack::PushL( packageInfo );
                    iInstallationPackages.AppendL( packageInfo );
                    CleanupStack::Pop( packageInfo );
                    }
                _LIT( KTotalInstFileCountFormat, "%S: total package count %d" );
                Print( KTotalInstFileCountFormat, &KTestName, iInstallationPackages.Count() );

                if( status.Int() )
                    {
                    testResult = status.Int();
                    }
                }

            CleanupStack::PopAndDestroy( &packageArray );
            CleanupStack::PopAndDestroy( &pluginFiles );
            }
        
        CleanupStack::PopAndDestroy( dataTypeArray );
        }
    
    CleanupStack::PopAndDestroy( &fileArray );

    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::CheckInstallationFilesL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::CheckInstallationFilesL( CStifItemParser& aItem )
    {
    _LIT( KTestName, "CheckInstallationFiles" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;

    TPtrC nameString;
    while( aItem.GetNextString( nameString ) == KErrNone )
        {
        CAppMngr2PackageInfo* info = FindInstallationPackage( nameString );
        TBool found = ( info != NULL );
        
        _LIT( KCheckInstFileFormat, "%S: file %S, result %d" );
        Print( KCheckInstFileFormat, &KTestName, &nameString, found );
        if( found )
            {
            CallInfoBaseFunctionsL( *info, KTestName );
            }
        else
            {
            Print( KNotFoundFormat, &KTestName, &nameString );
            testResult = KErrNotFound;
            }
        }
    
    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::ShowInstallationFileDetailsL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::ShowInstallationFileDetailsL( CStifItemParser& aItem )
    {
    _LIT( KTestName, "ShowInstallationFileDetails" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;

    if( iShowDetailsCommand )
        {
        User::Leave( KErrInUse );
        }

    TPtrC nameString;
    while( aItem.GetNextString( nameString ) == KErrNone )
        {
        CAppMngr2PackageInfo* info = FindInstallationPackage( nameString );
        if( info )
            {
            iShowDetailsCommand = CAppMngr2TestCommand::NewL( *info, *this );
            TRAP( testResult, iShowDetailsCommand->HandleCommandL( EAppMngr2CmdViewDetails ) );
            _LIT( KShowInstFileDetailsFormat, "%S: show details request %S, result %d" );
            Print( KShowInstFileDetailsFormat, &KTestName, &nameString, testResult );
            }
        else
            {
            Print( KNotFoundFormat, &KTestName, &nameString );
            testResult = KErrNotFound;
            }
        }
    
    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::CheckShowDetailsEndedL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::CheckShowDetailsEndedL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestName, "CheckShowDetailsEnded" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;

    if( !iShowDetailsCommand )
        {
        User::Leave( KErrNotFound );
        }

    if( iShowDetailsCommand->IsActive() )
        {
        User::Leave( KErrNotReady );
        }
    testResult = iShowDetailsCommand->iStatus.Int();
    _LIT( KCheckShowDetailsEndedFormat, "%S: show details completed, status %d" );
    Print( KCheckShowDetailsEndedFormat, &KTestName, testResult );
    
    delete iShowDetailsCommand;
    iShowDetailsCommand = NULL;
    
    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::CheckNoInstallationFilesL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::CheckNoInstallationFilesL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestName, "CheckNoInstallationFiles" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;
    
    TInt count = iInstallationPackages.Count();
    if( count != 0 )
        {
        testResult = KErrTooBig;
        }
    _LIT( KInstallationFilesEmptyFormat, "%S: file count %d" );
    Print( KInstallationFilesEmptyFormat, &KTestName, count );
    
    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::StartInstallFileL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::StartInstallFileL( CStifItemParser& aItem )
    {
    _LIT( KTestName, "StartInstallFile" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;
    
    if( iInstallCommand )
        {
        User::Leave( KErrInUse );
        }

    TPtrC nameString;
    while( aItem.GetNextString( nameString ) == KErrNone )
        {
        CAppMngr2PackageInfo* info = FindInstallationPackage( nameString );
        if( info )
            {
            iInstallCommand = CAppMngr2TestCommand::NewL( *info, *this );
            TRAP( testResult, iInstallCommand->HandleCommandL( EAppMngr2CmdInstall ) );
            _LIT( KInstallFileFormat, "%S: install request %S, result %d" );
            Print( KInstallFileFormat, &KTestName, &nameString, testResult );
            }
        else
            {
            Print( KNotFoundFormat, &KTestName, &nameString );
            testResult = KErrNotFound;
            }
        }
    
    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::CheckInstallEndedL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::CheckInstallEndedL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestName, "CheckInstallEnded" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;

    if( !iInstallCommand )
        {
        User::Leave( KErrNotFound );
        }

    if( iInstallCommand->IsActive() )
        {
        User::Leave( KErrNotReady );
        }
    testResult = iInstallCommand->iStatus.Int();
    _LIT( KCheckInstallEndedFormat, "%S: install completed, status %d" );
    Print( KCheckInstallEndedFormat, &KTestName, testResult );
    
    delete iInstallCommand;
    iInstallCommand = NULL;
    
    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::GetInstalledAppsL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::GetInstalledAppsL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestName, "GetInstalledApps" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;
    iInstalledApps.ResetAndDestroy();
    
    TInt count = iPlugins.Count();
    for( TInt index = 0; index < count; index++ )
        {
        CAppMngr2Runtime* plugin = iPlugins[ index ];
        TInt pluginId = plugin->RuntimeUid().iUid;

        RPointerArray<CAppMngr2AppInfo> appArray;
        CleanupResetAndDestroyPushL( appArray );
        
        TRequestStatus status;
        TRAP( testResult, plugin->GetInstalledAppsL( appArray, iFs, status ) );
        _LIT( KRequestedFormat, "%S: plugin 0x%08x, apps requested, error %d, status %d" );
        Print( KRequestedFormat, &KTestName, pluginId, testResult, status.Int() );
        if( testResult == KErrNone )
            {
            User::WaitForRequest( status );
            _LIT( KLoadedFormat, "%S: plugin 0x%08x, apps count %d, status %d" );
            TInt count = appArray.Count();
            Print( KLoadedFormat, &KTestName, pluginId, count, status.Int() );
            
            // move items to iInstalledApps array
            for( TInt i = count - 1; i >= 0; i-- )
                {
                CAppMngr2AppInfo* appInfo = appArray[ i ];
                appArray.Remove( i );
                CleanupStack::PushL( appInfo );
                iInstalledApps.AppendL( appInfo );
                CleanupStack::Pop( appInfo );
                }
            _LIT( KTotalInstAppCountFormat, "%S: total installed apps count %d" );
            Print( KTotalInstAppCountFormat, &KTestName, iInstalledApps.Count() );
            
            if( status.Int() )
                {
                testResult = status.Int();
                }
            }

        CleanupStack::PopAndDestroy( &appArray );
        }
    
    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::CheckAppInstalledL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::CheckAppInstalledL( CStifItemParser& aItem )
    {
    _LIT( KTestName, "CheckAppInstalled" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;

    TPtrC nameString;
    while( aItem.GetNextString( nameString ) == KErrNone )
        {
        CAppMngr2AppInfo* info = FindInstalledApp( nameString );
        TBool found = ( info != NULL );
        
        Print( KResultFormat, &KTestName, &nameString, found );
        if( found )
            {
            CallInfoBaseFunctionsL( *info, KTestName );
            }
        else
            {
            Print( KNotFoundFormat, &KTestName, &nameString );
            testResult = KErrNotFound;
            }
        }
    
    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::ShowInstalledDetailsL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::ShowInstalledDetailsL( CStifItemParser& aItem )
    {
    _LIT( KTestName, "ShowInstalledDetails" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;

    if( iShowDetailsCommand )
        {
        User::Leave( KErrInUse );
        }

    TPtrC nameString;
    while( aItem.GetNextString( nameString ) == KErrNone )
        {
        CAppMngr2AppInfo* info = FindInstalledApp( nameString );
        if( info )
            {
            iShowDetailsCommand = CAppMngr2TestCommand::NewL( *info, *this );
            TRAP( testResult, iShowDetailsCommand->HandleCommandL( EAppMngr2CmdViewDetails ) );
            _LIT( KShowInstalledDetailsFormat, "%S: show details request %S, result %d" );
            Print( KShowInstalledDetailsFormat, &KTestName, &nameString, testResult );
            }
        else
            {
            Print( KNotFoundFormat, &KTestName, &nameString );
            testResult = KErrNotFound;
            }
        }
    
    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::CheckNoAppsInstalledL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::CheckNoAppsInstalledL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestName, "CheckNoAppsInstalled" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;
    
    TInt count = iInstalledApps.Count();
    if( count != 0 )
        {
        testResult = KErrTooBig;
        }
    _LIT( KInstalledAppsEmptyFormat, "%S: app count %d" );
    Print( KInstalledAppsEmptyFormat, &KTestName, count );
    
    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::StartUninstallAppL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::StartUninstallAppL( CStifItemParser& aItem )
    {
    _LIT( KTestName, "StartUninstallApp" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;

    if( iUninstallCommand )
        {
        User::Leave( KErrInUse );
        }

    TPtrC nameString;
    while( aItem.GetNextString( nameString ) == KErrNone )
        {
        CAppMngr2AppInfo* info = FindInstalledApp( nameString );
        if( info )
            {
            iUninstallCommand = CAppMngr2TestCommand::NewL( *info, *this );
            TRAP( testResult, iUninstallCommand->HandleCommandL( EAppMngr2CmdUninstall ) );
            _LIT( KUninstallAppFormat, "%S: uninstall request %S, result %d" );
            Print( KUninstallAppFormat, &KTestName, &nameString, testResult );
            }
        else
            {
            Print( KNotFoundFormat, &KTestName, &nameString );
            testResult = KErrNotFound;
            }
        }
    
    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

// -----------------------------------------------------------------------------
// CAppMngr2RuntimeApiTest::CheckUninstallEndedL()
// -----------------------------------------------------------------------------
//
TInt CAppMngr2RuntimeApiTest::CheckUninstallEndedL( CStifItemParser& /*aItem*/ )
    {
    _LIT( KTestName, "CheckUninstallEnded" );
    Print( KTestStartFormat, &KTestName );
    TInt testResult = KErrNone;

    if( !iUninstallCommand )
        {
        User::Leave( KErrNotFound );
        }

    if( iUninstallCommand->IsActive() )
        {
        User::Leave( KErrNotReady );
        }
    testResult = iUninstallCommand->iStatus.Int();
    _LIT( KCheckUninstallEndedFormat, "%S: uninstall completed, status %d" );
    Print( KCheckUninstallEndedFormat, &KTestName, testResult );
    
    delete iUninstallCommand;
    iUninstallCommand = NULL;
    
    Print( KTestDoneFormat, &KTestName, testResult );
    return testResult;
    }

