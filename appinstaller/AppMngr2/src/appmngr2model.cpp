/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Model implementation
*
*/


#include "appmngr2model.h"              // CAppMngr2Model
#include "appmngr2modelobserver.h"      // CAppMngr2ModelObserver
#include "appmngr2infomaker.h"          // CAppMngr2InfoMaker
#include "appmngr2appinfomaker.h"       // CAppMngr2AppInfoMaker
#include "appmngr2packageinfomaker.h"   // CAppMngr2PackageInfoMaker
#include "appmngr2appinfoarray.h"       // CAppMngr2AppInfoArray
#include "appmngr2packageinfoarray.h"   // CAppMngr2PackageInfoArray
#include "appmngr2scanner.h"            // CAppMngr2Scanner
#include "appmngr2pluginholder.h"       // CAppMngr2PluginHolder
#include <appmngr2runtime.h>            // CAppMngr2Runtime
#include <appmngr2infobase.h>           // CAppMngrInfoBase
#include <appmngr2appinfo.h>            // CAppMngr2AppInfo
#include <appmngr2packageinfo.h>        // CAppMngr2PackageInfo
#include <appmngr2recognizedfile.h>     // CAppMngr2RecognizedFile
#include <appmngr2cleanuputils.h>       // CleanupResetAndDestroyPushL
#include <appmngr2driveutils.h>         // TAppMngr2DriveUtils
#include <appmngr2common.hrh>           // generic command ids
#include <ecom/implementationinformation.h>  // CImplementationInformation
#include <ecom/ecom.h>                  // REComSession
#include <AknIconArray.h>               // CAknIconArray
#include <gulicon.h>                    // CGulIcon
#include <driveinfo.h>                  // DriveInfo
#include <pathinfo.h>                   // Pathinfo
#include <centralrepository.h>          // CRepository
#include <SWInstallerInternalCRKeys.h>  // KCRUidSWInstallerLV
#include <AknsUtils.h>                  // AknsUtils
#include <appmngr2.mbg>                 // icon IDs

_LIT( KAppMngr2BitmapFile, "appmngr2.mif" );
_LIT( KDriveSpec, "%c:" );


// ======== LOCAL FUNCTIONS =========

// ---------------------------------------------------------------------------
// FindDataType()
// ---------------------------------------------------------------------------
//
TBool FindDataType( const TDataType& aDataType, CDataTypeArray& aArray )
    {
    TInt count = aArray.Count();
    for( TInt index = 0; index < count; index++ )
        {
        if( aArray[ index ] == aDataType )
            {
            return ETrue;
            }
        }
    return EFalse;
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2Model::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2Model* CAppMngr2Model::NewL( RFs& aFsSession,
        MAppMngr2ModelObserver& aObserver )
    {
    CAppMngr2Model* self = new (ELeave) CAppMngr2Model( aFsSession, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::~CAppMngr2Model()
// ---------------------------------------------------------------------------
//
CAppMngr2Model::~CAppMngr2Model()
    {
    FLOG( "CAppMngr2Model::~CAppMngr2Model" );
    iClosing = ETrue;   // to disable notifications

    Cancel();
    delete iApaAppListNotifier;
    iInfoMakers.ResetAndDestroy();
    delete iScanner;
    delete iInstalledApps;
    delete iInstallationFiles;
    iPlugins.ResetAndDestroy();     // unloads plugin DLLs
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::AppInfoCount()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2Model::AppInfoCount() const
    {
    return iInstalledApps->Count();
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::AppInfo()
// ---------------------------------------------------------------------------
//
CAppMngr2AppInfo& CAppMngr2Model::AppInfo( TInt aIndex ) const
    {
    return *( reinterpret_cast< CAppMngr2AppInfo* >( iInstalledApps->At( aIndex ) ) );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::PackageInfoCount()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2Model::PackageInfoCount() const
    {
    return iInstallationFiles->Count();
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::PackageInfo()
// ---------------------------------------------------------------------------
//
CAppMngr2PackageInfo& CAppMngr2Model::PackageInfo( TInt aIndex ) const 
    {
    return *( reinterpret_cast< CAppMngr2PackageInfo* >( iInstallationFiles->At( aIndex ) ) );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::LoadIconsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::LoadIconsL( CAknIconArray& aIconArray )
    {
    LoadDefaultIconsL( aIconArray );
    
    TInt pluginCount = iPlugins.Count();
    for( TInt index = 0; index < pluginCount; index++ )
        {
        iPlugins[ index ]->LoadIconsL( aIconArray );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::GetIconIndexesL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::GetIconIndexesL( TUid aUid, TInt& aIconIndexBase, TInt& aIconIndexMax ) const
    {
    TInt pluginCount = iPlugins.Count();
    for( TInt index = 0; index < pluginCount; index++ )
        {
        if( iPlugins[ index ]->Runtime().RuntimeUid() == aUid )
            {
            aIconIndexBase = iPlugins[ index ]->IconIndexBase();
            aIconIndexMax = iPlugins[ index ]->IconIndexMax();
            return;
            }
        }
    User::Leave( KErrNotFound );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::HandleCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::HandleCommandL( CAppMngr2InfoBase& aInfo, TInt aCommand )
    {
    FLOG( "CAppMngr2Model::HandleCommandL( %d ), IsActive() = %d", aCommand, IsActive() );
    
    if( !IsActive() )
        {
        // About to start plugin specific command. Note that when the command completes
        // we need to call CAppMngr2InfoBase::CommandComplete() function. For this (and
        // to indicate that there is a plugin specific command on-going) we take the
        // address of CAppMngr2InfoBase item into iActiveItem. Because plugin specific
        // command (like delete or uninstall) may trigger new scanning, it is necessary
        // to disable iInstalledApps and iInstallationFiles array refreshing temporarily.
        // When refreshing is disabled, the item which address is stored in iActiveItem
        // is maintained in arrays and we can call iActiveItem->CommandComplete() when
        // the command is complete. If arrays could be refreshed during plugin specific
        // command, then calling iActiveItem->CommandComplete() would panic.
        if( iObs.InstalledAppsDisplayed() )
            {
            iInstalledApps->DisableRefreshNotificationsL();
            }
        if( iObs.InstallationFilesDisplayed() )
            {
            iInstallationFiles->DisableRefreshNotificationsL();
            }

        iActiveItem = &aInfo;
        iActiveCommand = aCommand;
        FLOG( "CAppMngr2Model::HandleCommandL, iActiveItem = 0x%08x '%S'",
                iActiveItem, &( iActiveItem->Name() ) ); 
        TRAPD( err, iActiveItem->HandleCommandL( aCommand, iStatus ) );
        FLOG( "CAppMngr2Model::HandleCommandL, command started, err = %d", err );
        SetActive();
        if( err )
            {
            TRequestStatus* statusPtr = &iStatus;
            User::RequestComplete( statusPtr, err );
            }
        }
    // Ignore the command silently if already active. This may happen when user
    // cancels previous installation command. Because cancelling installation can
    // take long time, user may be able to issue new commands while model is still
    // active (i.e. previous installation command is being cancalled).
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::StartFetchingInstallationFilesL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::StartFetchingInstallationFilesL()
    {
    FLOG( "CAppMngr2Model::StartFetchingInstallationFilesL" );
    FLOG_PERF_START( FetchInstallationFiles )
    
    // Installation files cache must be enabled until scanner has completed.
    // This ensures that scanner has time to call GetInstallationFilesL() for
    // each plugin and for each directory before the first call completes.
    // If the first call completes before scanner has made all these requets,
    // cache will be turned off and partial results are displayed.
    iInstallationFiles->IncrementCacheUseStartingNewRoundL();

    iScanner->StartScanningL();
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::StartFetchingInstalledAppsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::StartFetchingInstalledAppsL()
    {
    FLOG( "CAppMngr2Model::StartFetchingInstalledAppsL" );
    FLOG_PERF_START( FetchInstalledApps )

    // Additional cache increment to ensure that iInstalledApps cache is
    // used until GetInstalledAppsL() function is called for each plugin.
    // Without this, the fastest plugin might get it's list complete before
    // other IncrementCacheUseL() calls and iInstalledApps would display
    // partial list.
    iInstalledApps->IncrementCacheUseStartingNewRoundL();
    
    TInt pluginCount = iPlugins.Count();
    for( TInt pluginIndex = 0; pluginIndex < pluginCount; pluginIndex++ )
        {
        CAppMngr2AppInfoMaker* appInfoMaker = CAppMngr2AppInfoMaker::NewLC(
                iPlugins[ pluginIndex ]->Runtime(), *this, iFs );
        
        TRAPD( err, appInfoMaker->StartGettingInstalledAppsL() );
        FLOG( "CAppMngr2Model::StartFetchingInstalledAppsL, plugin 0x%08x, err = %d",
                iPlugins[ pluginIndex ]->Runtime().RuntimeUid().iUid, err );
        if( err == KErrNone )
            {
            iInfoMakers.AppendL( appInfoMaker );
            CleanupStack::Pop( appInfoMaker );
            iInstalledApps->IncrementCacheUseL();
            }
        else
            {
            CleanupStack::PopAndDestroy( appInfoMaker );
            }
        }

    // All GetInstalledAppsL() requests have been issued
    iInstalledApps->DecrementCacheUse();
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::DoCancel()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::DoCancel()
    {
    FLOG( "CAppMngr2Model::DoCancel, iActiveItem = 0x%08x", iActiveItem );
    
    if( iActiveItem )
        {
        iActiveItem->CancelCommand();
        iActiveItem = NULL;
        if( !iClosing )
            {
            TRAP_IGNORE( iInstalledApps->EnableRefreshNotificationsL() );
            TRAP_IGNORE( iInstallationFiles->EnableRefreshNotificationsL() );
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::RunL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::RunL()
    {
    FLOG( "CAppMngr2Model::RunL, iActiveItem = 0x%08x, iStatus = %d",
            iActiveItem, iStatus.Int() );

    if( iActiveItem )
        {
        TInt completionCode = iStatus.Int();
        TRAPD( err, iActiveItem->HandleCommandResultL( completionCode ) );
        FLOG( "CAppMngr2Model::RunL, HandleCommandResultL err = %d", err );
        CAppMngr2InfoBase* itemToDelete = iActiveItem;
        iActiveItem = NULL;

        // Enable refresh notifications. No need to check which view is active
        // because notifications can be enabled even if they were not disabled.
        iInstalledApps->EnableRefreshNotificationsL();
        iInstallationFiles->EnableRefreshNotificationsL();

        // Leave on error. This displays error note (if error notes are enabled).
        User::LeaveIfError( err );
        
        // If the command is EAppMngr2CmdUninstall or EAppMngr2CmdRemove, and it
        // completed without errors, then we remove the current item immediatelty
        // from the displayed list. Otherwise it may take quite long time until
        // the item is removed from the UI, and in worst case it may not be removed
        // at all.
        // For example when an installation file is deleted, scanner notices change
        // in directories and re-scans everything. UI is updated when the scanning
        // completes. Delay between delete command and UI update (deleted item is
        // removed from UI) depends on how quickly scanning completes.
        if( completionCode == KErrNone )
            {
            if( iActiveCommand == EAppMngr2CmdUninstall && iObs.InstalledAppsDisplayed() )
                {
                iInstalledApps->ImmediateDelete( itemToDelete );
                }
            if( iActiveCommand == EAppMngr2CmdRemove && iObs.InstallationFilesDisplayed() )
                {
                iInstallationFiles->ImmediateDelete( itemToDelete );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::RefreshInstalledApps()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::RefreshInstalledApps()
    {
    FLOG( "CAppMngr2Model::RefreshInstalledApps" );
    
    TRAP_IGNORE( StartFetchingInstalledAppsL() );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::RefreshInstallationFiles()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::RefreshInstallationFiles()
    {
    FLOG( "CAppMngr2Model::RefreshInstallationFiles" );

    TRAP_IGNORE( StartFetchingInstallationFilesL() );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::ScanningResultL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::ScanningResultL( RPointerArray<CAppMngr2RecognizedFile>& aResult )
    {
    FLOG( "CAppMngr2Model::ScanningResultL, begin: aResult.Count() = %d", aResult.Count() );
    
    // Split recognition result array into smaller (plugin specific) arrays. Plugin
    // specific arrays are maintained by CAppMngr2PackageInfoMaker objects, so one
    // CAppMngr2PackageInfoMaker object is needed for each plugin that has recognized
    // files.
    TInt pluginCount = iPlugins.Count();
    for( TInt pluginIndex = 0; pluginIndex < pluginCount; pluginIndex++ )
        {
        CAppMngr2PackageInfoMaker* packageInfoMaker = CAppMngr2PackageInfoMaker::NewLC(
                iPlugins[ pluginIndex ]->Runtime(), *this, iFs );

        TInt fileCount = 0;
        TInt resultCount = aResult.Count();
        for( TInt resultIndex = resultCount - 1; resultIndex >= 0; resultIndex-- )
            {
            CAppMngr2RecognizedFile* recFile = aResult[ resultIndex ];
            if( FindDataType( recFile->DataType(), iPlugins[ pluginIndex ]->DataTypes() ) )
                {
                packageInfoMaker->AddFileL( recFile );  // takes ownership
                aResult.Remove( resultIndex );
                fileCount++;
                }
            }

        FLOG( "CAppMngr2Model::ScanningResultL, plugin 0x%08x: fileCount = %d",
                iPlugins[ pluginIndex ]->Runtime().RuntimeUid().iUid, fileCount );
        if( fileCount > 0 )
            {
            packageInfoMaker->StartGettingInstallationFilesL();
            iInfoMakers.AppendL( packageInfoMaker );
            CleanupStack::Pop( packageInfoMaker );
            iInstallationFiles->IncrementCacheUseL();
            }
        else
            {
            CleanupStack::PopAndDestroy( packageInfoMaker );
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::ScanningComplete()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::ScanningComplete()
    {
    FLOG( "CAppMngr2Model::ScanningComplete" );
    
    iInstallationFiles->DecrementCacheUse();
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::DirectoryChangedL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::DirectoryChangedL( const TDesC& /*aChangedDir*/ )
    {
    FLOG( "CAppMngr2Model::DirectoryChangedL" );
    
    // This might be improved by scanning the changed directory only. Model
    // could record which items are got from which directory, so that it could
    // remove those items that were created from the changed directory and
    // re-scan only the changed directory. Scanner should also support
    // scanning one directory only. Now, we just scan all again.
    StartFetchingInstallationFilesL();
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::HandleAppListEvent()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::HandleAppListEvent( TInt /*aEvent*/ )
    {
    FLOG( "CAppMngr2Model::HandleAppListEvent" );
    
    TRAP_IGNORE( StartFetchingInstalledAppsL() );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::NewAppsCreatedL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::NewAppsCreatedL( const CAppMngr2InfoMaker& aMaker,
        RPointerArray<CAppMngr2AppInfo>& aAppInfos )
    {
    FLOG( "CAppMngr2Model::NewAppsCreatedL, plugin 0x%08x: packageCount = %d",
            aMaker.RuntimeUid().iUid, aAppInfos.Count() );
    
    iInstalledApps->AddItemsInOrderL( aAppInfos );
    iInstalledApps->DecrementCacheUse();
    CloseInfoMaker( aMaker );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::ErrorInCreatingAppsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::ErrorInCreatingAppsL( const CAppMngr2InfoMaker& aMaker,
#ifdef FLOG_DEBUG_TRACES
        TInt aError )
#else
        TInt /*aError*/ )
#endif
    {
    FLOG( "CAppMngr2Model::ErrorInCreatingAppsL, plugin 0x%08x: error = %d",
            aMaker.RuntimeUid().iUid, aError );
    
    iInstalledApps->DecrementCacheUse();
    CloseInfoMaker( aMaker );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::NewPackagesCreatedL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::NewPackagesCreatedL( const CAppMngr2InfoMaker& aMaker,
            RPointerArray<CAppMngr2PackageInfo>& aPackageInfos )
    {
    FLOG( "CAppMngr2Model::NewPackagesCreatedL, plugin 0x%08x: packageCount = %d",
            aMaker.RuntimeUid().iUid, aPackageInfos.Count() );
    
    iInstallationFiles->AddItemsInOrderL( aPackageInfos );
    iInstallationFiles->DecrementCacheUse();
    CloseInfoMaker( aMaker );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::ErrorInCreatingPackagesL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::ErrorInCreatingPackagesL( const CAppMngr2InfoMaker& aMaker,
#ifdef FLOG_DEBUG_TRACES
        TInt aError )
#else
        TInt /*aError*/ )
#endif
    {
    FLOG( "CAppMngr2Model::ErrorInCreatingPackagesL, plugin 0x%08x: error = %d",
            aMaker.RuntimeUid().iUid, aError );
    
    iInstallationFiles->DecrementCacheUse();
    CloseInfoMaker( aMaker );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::ArrayContentChanged()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::ArrayContentChanged( CAppMngr2InfoArray* aArray,
        TInt aMoreRefreshesExpected )
    {
    if( aArray == iInstalledApps )
        {
        FLOG_PERF_STOP( FetchInstalledApps )
        FLOG_PERF_PRINT( FetchInstalledApps )
        iObs.InstalledAppsChanged( aMoreRefreshesExpected );
        }
    if( aArray == iInstallationFiles )
        {
        FLOG_PERF_STOP( FetchInstallationFiles )
        FLOG_PERF_PRINT( FetchInstallationFiles )
        iObs.InstallationFilesChanged( aMoreRefreshesExpected );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::CAppMngr2Model()
// ---------------------------------------------------------------------------
//
CAppMngr2Model::CAppMngr2Model( RFs& aFsSession, MAppMngr2ModelObserver& aObserver )
        : CActive( CActive::EPriorityStandard ), iFs( aFsSession ), iObs( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::ConstructL()
    {
    FLOG( "CAppMngr2Model::ConstructL" );
    iClosing = EFalse;

    iInstalledApps = CAppMngr2AppInfoArray::NewL( *this );
    iInstallationFiles = CAppMngr2PackageInfoArray::NewL( *this );
    
    FLOG_PERF_STATIC_BEGIN( LoadPluginsL )
    LoadPluginsL();
    FLOG_PERF_STATIC_END( LoadPluginsL )
    FLOG_PERF_STATIC_BEGIN( CreateScannerL )
    CreateScannerL();
    FLOG_PERF_STATIC_END( CreateScannerL )
    FLOG_PERF_STATIC_BEGIN( FetchDataTypesL )
    FetchDataTypesL();
    FLOG_PERF_STATIC_END( FetchDataTypesL )

    // start monitoring changes in applications list
    iApaAppListNotifier = CApaAppListNotifier::NewL( this, CActive::EPriorityStandard );
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::LoadDefaultIconsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::LoadDefaultIconsL( CAknIconArray& aIconArray )
    {
    FLOG( "CAppMngr2Model::LoadDefaultIconsL" );
    
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    HBufC* bitmapFile = TAppMngr2DriveUtils::FullBitmapFileNameLC( KAppMngr2BitmapFile, iFs );
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    CGulIcon* icon = NULL;
    
    // Note that icons can be graphically-skinned (icon graphic defined in theme)
    // or color-skinned (icon colors change depending on background color defined
    // in theme). Normal icons are graphically-skinned and indicator icons are
    // color-skinned. AknsUtils::CreateGulIconL() creates graphically-skinned icons,
    // and AknsUtils::CreateColorIconL() creates color-skinned icons. Hence both
    // of these functions are used below.

    // Icon 0: EAppMngr2IconIndex_QgnIndiAmInstMmcAdd
    // Indicator icon for items stored/installed in memory card
    AknsUtils::CreateColorIconLC( skinInstance, 
            KAknsIIDQgnIndiMmcAdd, KAknsIIDQsnIconColors, EAknsCIQsnIconColorsCG13,
            bitmap, mask, *bitmapFile, 
            EMbmAppmngr2Qgn_indi_mmc_add,
            EMbmAppmngr2Qgn_indi_mmc_add_mask,
            KRgbBlack );
    icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );
    CleanupStack::Pop( 2 );   // bitmap and mask, order is varying
    CleanupStack::PushL( icon );
    aIconArray.AppendL( icon );
    CleanupStack::Pop( icon );

    // Icon 1: EAppMngr2IconIndex_QgnIndiFmgrMsAdd
    // Indicator icon for items stored/installed in mass memory
    AknsUtils::CreateColorIconLC( skinInstance, 
            KAknsIIDQgnIndiFmgrMsAdd, KAknsIIDQsnIconColors, EAknsCIQsnIconColorsCG13,
            bitmap, mask, *bitmapFile, 
            EMbmAppmngr2Qgn_indi_fmgr_ms_add,
            EMbmAppmngr2Qgn_indi_fmgr_ms_add_mask,
            KRgbBlack );
    icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );
    CleanupStack::Pop( 2 );   // bitmap and mask, order may vary
    CleanupStack::PushL( icon );
    aIconArray.AppendL( icon );
    CleanupStack::Pop( icon );

    // Icon 2: EAppMngr2IconIndex_QgnPropUnknown
    // List icon for items that are not known
    icon = AknsUtils::CreateGulIconL( skinInstance,
            KAknsIIDQgnPropUnknown, *bitmapFile,
            EMbmAppmngr2Qgn_prop_unknown, 
            EMbmAppmngr2Qgn_prop_unknown_mask );
    CleanupStack::PushL( icon );
    aIconArray.AppendL( icon );
    CleanupStack::Pop( icon );

    CleanupStack::PopAndDestroy( bitmapFile );
    
    // Additionally some unknown indicator icon could be defined.
    // Now, if some plugin gives incorrect index fox indicator icon,
    // then no indicator icon is displayed.
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::LoadPluginsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::LoadPluginsL()
    {
    FLOG( "CAppMngr2Model::LoadPluginsL" );
    
    RImplInfoPtrArray implInfoArray;
    CleanupResetAndDestroyPushL( implInfoArray  );
    REComSession::ListImplementationsL( KAppMngr2PluginInterface, implInfoArray );

    CAppMngr2PluginHolder* pluginHolder = NULL;
    CImplementationInformation* implInfo;
    TInt count = implInfoArray.Count();
    for( TInt index = 0; index < count; index++ )
        {
        implInfo = implInfoArray[ index ];
        FLOG( "CAppMngr2Model::LoadPluginsL, loading %S", &( implInfo->DisplayName() ) );
        FLOG_PERF_STATIC_BEGIN( DoLoadPluginL );
        TRAPD( err, pluginHolder = DoLoadPluginL( implInfo->ImplementationUid() ) );
        FLOG_PERF_STATIC_END( DoLoadPluginL );
        FLOG( "CAppMngr2Model::DoLoadPluginL( 0x%08x ), err = %d",
                implInfo->ImplementationUid().iUid, err );
        if( err == KErrNone )
            {
            CleanupStack::PushL( pluginHolder );
            iPlugins.AppendL( pluginHolder );
            CleanupStack::Pop( pluginHolder );
            }
        }

    CleanupStack::PopAndDestroy( &implInfoArray );
    REComSession::FinalClose();
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::DoLoadPluginL()
// ---------------------------------------------------------------------------
//
CAppMngr2PluginHolder* CAppMngr2Model::DoLoadPluginL( TUid aUid )
    {
    CAppMngr2Runtime* plugin = CAppMngr2Runtime::NewL( aUid, *this );
    CleanupStack::PushL( plugin );
    CAppMngr2PluginHolder* holder = new (ELeave) CAppMngr2PluginHolder( plugin );
    CleanupStack::Pop( plugin );
    return holder;
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::CreateScannerL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::CreateScannerL()
    {
    iScanner = CAppMngr2Scanner::NewL( *this );

    TDriveList driveList;
    TInt driveCount = 0;
    TInt err = DriveInfo::GetUserVisibleDrives( iFs, driveList, driveCount );
    User::LeaveIfError( err );

    // Default directories
    TFileName path;
    TInt driveListLength = driveList.Length();
    for( TInt driveNumber = 0; driveNumber < driveListLength; driveNumber++ )
        {
        if( driveList[ driveNumber ] )
            {
            TUint driveStatus = 0;
            err = DriveInfo::GetDriveStatus( iFs, driveNumber, driveStatus );
            User::LeaveIfError( err );

            if( !( driveStatus & DriveInfo::EDriveRemote ) )
                {
                err = PathInfo::GetFullPath( path, driveNumber, PathInfo::EInstallsPath );
                User::LeaveIfError( err );
                iScanner->AddDirectoryL( path );
                }
            }
        }

    // Additional plug-in specific directories
    TInt pluginCount = iPlugins.Count();
    for( TInt pluginIndex = 0; pluginIndex < pluginCount; pluginIndex++ )
        {
        RPointerArray<HBufC> dirsToScan;
        CleanupResetAndDestroyPushL( dirsToScan );
        TRAPD( err, iPlugins[ pluginIndex ]->Runtime().GetAdditionalDirsToScanL( iFs,
                dirsToScan ) );
        if( err == KErrNone )
            {
            TInt dirCount = dirsToScan.Count();
            for( TInt dirIndex = 0; dirIndex < dirCount; dirIndex++ )
                {
                TPtrC dirName = *( dirsToScan[ dirIndex ] );
                iScanner->AddDirectoryL( dirName );
                }
            }
        CleanupStack::PopAndDestroy( &dirsToScan );
        }
    
    // KSWInstallerPackageFolder directory if defined in CenRep
    CRepository* cenrep = CRepository::NewLC( KCRUidSWInstallerLV );
    err = cenrep->Get( KSWInstallerPackageFolder, path );
    CleanupStack::PopAndDestroy( cenrep );
    if( err == KErrNone )
        {
        TParsePtr parse( path );
        if( parse.DrivePresent() )
            {
            iScanner->AddDirectoryL( path );
            }
        else if( parse.PathPresent() )
            {
            TDriveList driveList;
            TInt driveCount = 0;
            err = DriveInfo::GetUserVisibleDrives( iFs, driveList, driveCount );
            if( err == KErrNone )
                {
                TInt driveListLength = driveList.Length();
                for( TInt driveNumber = 0; driveNumber < driveListLength; driveNumber++ )
                    {
                    if( driveList[ driveNumber ] )
                        {
                        TUint driveStatus = 0;
                        err = DriveInfo::GetDriveStatus( iFs, driveNumber, driveStatus );
                        if( err == KErrNone && !( driveStatus & DriveInfo::EDriveRemote ) )
                            {
                            TChar driveLetter;
                            if( RFs::DriveToChar( driveNumber, driveLetter ) == KErrNone )
                                {
                                TFileName fullPath;
                                fullPath.Format( KDriveSpec, static_cast<TUint>( driveLetter ) );
                                fullPath.Append( path ); 
                                iScanner->AddDirectoryL( fullPath );
                                }
                            }
                        }
                    }
                }
            }
        else
            {
            // ignore empty values, and values that does not contain valid path
            }
        }
    else
        {
        if( err != KErrNotFound )
            {
            User::Leave( err );
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::FetchDataTypesL()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::FetchDataTypesL()
    {
    FLOG( "CAppMngr2Model::FetchDataTypesL" );
    
    TInt pluginCount = iPlugins.Count();
    for( TInt pluginIndex = 0; pluginIndex < pluginCount; pluginIndex++ )
        {
        FLOG( "CAppMngr2Model::FetchDataTypesL: pluginIndex = %d", pluginIndex );
        TRAP_IGNORE( iPlugins[ pluginIndex ]->FetchDataTypesL() );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2Model::CloseInfoMaker()
// ---------------------------------------------------------------------------
//
void CAppMngr2Model::CloseInfoMaker( const CAppMngr2InfoMaker& aMaker )
    {
    const CAppMngr2InfoMaker* makerToClose = &aMaker;
    for( TInt index = iInfoMakers.Count() - 1; index >= 0; index-- )
        {
        CAppMngr2InfoMaker* maker = iInfoMakers[ index ]; 
        if( maker == makerToClose )
            {
            iInfoMakers.Remove( index );
            delete maker;
            break;
            }
        }
    }

