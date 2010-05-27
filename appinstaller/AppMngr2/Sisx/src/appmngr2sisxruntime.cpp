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
* Description:   Native SISX runtime type for AppMngr2
*
*/


#include "appmngr2sisxruntime.h"        // CAppMngr2SisxRuntime
#include "appmngr2sisxappinfo.h"        // CAppMngr2SisxAppInfo
#include "appmngr2sisxpackageinfo.h"    // CAppMngr2SisxPackageInfo
#include "appmngr2sisxunknownlist.h"    // CAppMngr2SisxUnknownList
#include "appmngr2sisxswimonitor.h"     // CAppMngr2SisxSwiMonitor
#include <appmngr2runtimeobserver.h>    // MAppMngr2RuntimeObserver
#include <appmngr2recognizedfile.h>     // CAppMngr2RecognizedFile
#include <appmngr2cleanuputils.h>       // CleanupResetAndDestroyPushL
#include <appmngr2debugutils.h>         // FLOG macros
#include <swi/sisregistryentry.h>       // RSisRegistryEntry
#include <swi/sisregistrypackage.h>     // CSisRegistryPackage
#include <data_caging_path_literals.hrh> // KDC_APP_BITMAP_DIR
#include <AknIconArray.h>               // CAknIconArray
#include <AknsUtils.h>                  // AknsUtils
#include <gulicon.h>                    // CGulIcon
#include <eikenv.h>                     // CEikonEnv
#include <driveinfo.h>                  // DriveInfo
#include <f32file.h>                    // RFs
#include <SWInstDefs.h>                 // MIME types
#include <appmngr2sisx.mbg>             // icon IDs

_LIT( KAppMngr2SisxIconFileNameMif, "AppMngr2Sisx.mif" );
_LIT( KDriveSpec, "%c:" );
_LIT8( KSisInstallerUidType, "x-epoc/x-app268436505" );

const TInt KUidLength = 8;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2SisxRuntime::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxRuntime* CAppMngr2SisxRuntime::NewL( MAppMngr2RuntimeObserver &aObserver )
    {
    CAppMngr2SisxRuntime* self = new (ELeave) CAppMngr2SisxRuntime( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxRuntime::~CAppMngr2SisxRuntime()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxRuntime::~CAppMngr2SisxRuntime()
    {
    delete iSwiMonitor;
    iSisRegistrySession.Close();
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxRuntime::LoadIconsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxRuntime::LoadIconsL( CAknIconArray& aIconArray )
    {
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    HBufC* iconFilePath = NULL;
    
    iconFilePath = FullBitmapFileNameLC( KAppMngr2SisxIconFileNameMif );

    // Icon loading order in must be the same in which SISX icons
    // are defined in TAppMngr2SisxIcons enum in appmngr2sisx.hrh.
    
    // Icon 0: SIS application icon, EQgnMenuAmSis
    CGulIcon* icon = AknsUtils::CreateGulIconL( skinInstance,
            KAknsIIDQgnMenuAmSis, *iconFilePath,
            EMbmAppmngr2sisxQgn_menu_am_sis,
            EMbmAppmngr2sisxQgn_menu_am_sis_mask );
    CleanupStack::PushL( icon );
    aIconArray.AppendL( icon );
    CleanupStack::Pop( icon );

    // Icon 1: DRM expired rights icon, EQgnPropDrmExpLarge
    icon = AknsUtils::CreateGulIconL( skinInstance,
            KAknsIIDQgnPropDrmRightsExpLarge, *iconFilePath,
            EMbmAppmngr2sisxQgn_prop_drm_exp_large,
            EMbmAppmngr2sisxQgn_prop_drm_exp_large_mask );
    CleanupStack::PushL( icon );
    aIconArray.AppendL( icon );
    CleanupStack::Pop( icon );                                                

    // Icon 2: untrusted SIS application, EQgnIndiAmInstNoAdd
    // Indicator icons are color-skinned as normal icons are
    // graphically-skinned. CreateColorIconL() must be used to
    // create color-skinned icon.
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    AknsUtils::CreateColorIconLC( skinInstance,
            KAknsIIDQgnIndiAmInstNoAdd,
            KAknsIIDQsnIconColors, EAknsCIQsnIconColorsCG13,
            bitmap, mask, *iconFilePath,
            EMbmAppmngr2sisxQgn_indi_am_inst_no_add,
            EMbmAppmngr2sisxQgn_indi_am_inst_no_add_mask,
            KRgbBlack );
    icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );
    CleanupStack::Pop( 2 );   // bitmap and mask, order may vary
    CleanupStack::PushL( icon );
    aIconArray.AppendL( icon );
    CleanupStack::Pop( icon );

    CleanupStack::PopAndDestroy( iconFilePath );
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxRuntime::GetSupportedDataTypesL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxRuntime::GetSupportedDataTypesL(
        CDataTypeArray& aDataTypeArray )
    {
    TDataType sisxType( SwiUI::KSisxMimeType );
    aDataTypeArray.AppendL( sisxType );
    TDataType sisType( SwiUI::KSisMimeType );
    aDataTypeArray.AppendL( sisType );
    TDataType pipType( SwiUI::KPipMimeType );
    aDataTypeArray.AppendL( pipType );
    TDataType installedUidType( KSisInstallerUidType );
    aDataTypeArray.AppendL( installedUidType );
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxRuntime::GetAdditionalDirsToScanL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxRuntime::GetAdditionalDirsToScanL( RFs& aFsSession,
        RPointerArray<HBufC>& aDirs )
    {
    // Return KAppMngr2DaemonPrivateFolder directories that may contain
    // untrusted applications. Untrusted application is partially installed
    // and user needs to install it's SISX from KAppMngr2DaemonPrivateFolder
    // to complete it's installation. For example, when memory card is inserted
    // in mobile phone, installer processes applications installed in the memory
    // card. If some memory card application requires user-granted capabilities,
    // installer creates SISX in KAppMngr2DaemonPrivateFolder. The SISX must be
    // installed to grant the capabilities to the application.
    TDriveList driveList;
    TInt driveCount = 0;
    User::LeaveIfError( DriveInfo::GetUserVisibleDrives( aFsSession, driveList, driveCount ) );
    
    TInt driveListLength = driveList.Length();
    for( TInt driveNumber = 0; driveNumber < driveListLength; driveNumber++ )
        {
        if( driveList[ driveNumber ] )
            {
            // Internal and remote drives are not listed. Internal drives cannot
            // be removed/replaced, so there are no untrusted applications either.
            // Remote drives cannot be used to install applications at all.
            TUint driveStatus = 0;
            TInt err = DriveInfo::GetDriveStatus( aFsSession, driveNumber, driveStatus );
            if( err == KErrNone && !( driveStatus & DriveInfo::EDriveRemote ) &&
                    !( driveStatus & DriveInfo::EDriveInternal ) )
                {
                TChar driveLetter;
                if( RFs::DriveToChar( driveNumber, driveLetter ) == KErrNone )
                    {
                    const TInt dirLength = KDriveSpec().Length() +
                            KAppMngr2DaemonPrivateFolder().Length(); 
                    HBufC* dir = HBufC::NewLC( dirLength );
                    TPtr dirPtr = dir->Des();
                    dirPtr.Format( KDriveSpec, static_cast<TUint>( driveLetter ) );
                    dirPtr.Append( KAppMngr2DaemonPrivateFolder );
                    aDirs.AppendL( dir );
                    FLOG( "CAppMngr2SisxRuntime::GetAdditionalDirsToScanL: adding %S", dir );
                    CleanupStack::Pop( dir );
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxRuntime::GetInstallationFilesL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxRuntime::GetInstallationFilesL(
        RPointerArray<CAppMngr2PackageInfo>& aPackageInfos,
        const RPointerArray<CAppMngr2RecognizedFile>& aFileList,
        RFs& aFsSession, TRequestStatus& aStatus )
    {
    TInt fileCount = aFileList.Count();
    FLOG( "CAppMngr2SisxRuntime::GetInstallationFilesL: fileCount = %d", fileCount );
    if( fileCount )
        {
        // Check if this is the memory card installer daemon private folder.
        // All files are from the same directory, 
        CAppMngr2RecognizedFile* firstFile = aFileList[ 0 ];
        if( firstFile->FileName().Find( KAppMngr2DaemonPrivateFolder ) == KErrNotFound )
            {
            // No, it isn't. Process all files in aFileList and create package info objects.
            FLOG( "CAppMngr2SisxRuntime::GetInstallationFilesL: normal folder" );
            for( TInt index = 0; index < fileCount; index++ )
                {
                CAppMngr2RecognizedFile* file = aFileList[ index ];
                
                TPtrC fileName = file->FileName();
                FLOG( "CAppMngr2SisxRuntime::GetInstallationFilesL: file %S", &fileName );
                CreateNewPackageL( fileName, aPackageInfos, aFsSession );
                }
            }
        else
            {
            // It is. Process only those files that are not installed.
            FLOG( "CAppMngr2SisxRuntime::GetInstallationFilesL: swidaemon private folder" );
            RArray<TUid> uids;
            CleanupClosePushL( uids );
            iSisRegistrySession.InstalledUidsL( uids );
            
            for( TInt index = 0; index < fileCount; index++ )
                {
                CAppMngr2RecognizedFile* recFile = aFileList[ index ];
                
                TPtrC fileName = recFile->FileName();
                FLOG( "CAppMngr2SisxRuntime::GetInstallationFilesL: file %S", &fileName );
                
                // extract UID name from full path name
                TParsePtrC parse( fileName );
                TPtrC uidName = parse.Name().Left( KUidLength );
                FLOG( "CAppMngr2SisxRuntime::GetInstallationFilesL: uidName %S", &uidName );
                
                // convert UID name to numerical form
                TLex lex( uidName );
                TUint32 uidValue;
                TInt lexError = lex.Val( uidValue, EHex );
                if( lexError == KErrNone )
                    {
                    // It's an UID name, try to display package name instead
                    FLOG( "CAppMngr2SisxRuntime::GetInstallationFilesL: value %08x", uidValue );
                    
                    // check if this UID is already installed
                    TUid fileUid;
                    fileUid.iUid = uidValue;
                    if( uids.Find( fileUid ) == KErrNotFound )
                        {
                        FLOG( "CAppMngr2SisxRuntime::GetInstallationFilesL: not installed" );
                        // Not installed, must be displayed. 
                        CreateNewPackageL( fileName, aPackageInfos, aFsSession );
                        }
                    else
                        {
                        FLOG( "CAppMngr2SisxRuntime::GetInstallationFilesL: is installed" );
                        // Installed, two possible cases here. The package is in the
                        // SWI daemon private folder in memory card because:
                        // 1) memory card application is installed in use normally, or
                        // 2) the same application is already in ROM/internal drive.
                        // In case 1) this item MAY NOT be displayed here as it is already
                        // displayed in "Installed apps" side. In case of 2) it MUST be
                        // displayed, so that user can remove it from memory card. Cases
                        // 1) and 2) can be identified by checking the memory card's
                        // "unknown" list. If the sis package is unknown, it is case 2),
                        // as applications that are installed in use, are always known.
                        TChar driveLetter = parse.Drive()[ 0 ];
                        TInt driveNumber = 0;
                        User::LeaveIfError( RFs::CharToDrive( driveLetter, driveNumber ) );
                        CAppMngr2SisxUnknownList* unknownSisx = CAppMngr2SisxUnknownList::NewLC(
                                driveNumber );
                        TInt unknownIndex = unknownSisx->FindPkgWithUID( fileUid );
                        FLOG( "CAppMngr2SisxRuntime::GetInstallationFilesL: unknownIndex %d",
                                unknownIndex );
                        if( unknownIndex >= 0 && unknownIndex < unknownSisx->PkgCount() )
                            {
                            // It is unknown package after all, add it to the list.
                            CreateNewPackageL( fileName, aPackageInfos, aFsSession );
                            }
                        CleanupStack::PopAndDestroy( unknownSisx );
                        }
                    }
                else
                    {
                    // Not an UID name, must be displayed.
                    FLOG( "CAppMngr2SisxRuntime::GetInstallationFilesL: not UID name (lexErr %d)",
                            lexError );
                    CreateNewPackageL( fileName, aPackageInfos, aFsSession );
                    }
                }

            CleanupStack::PopAndDestroy( &uids );
            }
        }
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNone );
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxRuntime::CancelGetInstallationFiles()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxRuntime::CancelGetInstallationFiles()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxRuntime::GetInstalledAppsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxRuntime::GetInstalledAppsL(
        RPointerArray<CAppMngr2AppInfo>& aApps,
        RFs& aFsSession, TRequestStatus& aStatus )
    {
    FLOG( "CAppMngr2SisxRuntime::GetInstalledAppsL" );

    RPointerArray<Swi::CSisRegistryPackage> removablePackages;
    CleanupResetAndDestroyPushL( removablePackages );
    iSisRegistrySession.RemovablePackagesL( removablePackages );

    TInt appCount = removablePackages.Count();
    FLOG( "CAppMngr2SisxRuntime::GetInstalledAppsL, appCount %d", appCount );
    for( TInt index = 0; index < appCount; index++ )
        {
        Swi::RSisRegistryEntry entry;
        CleanupClosePushL( entry );
        FLOG( "CAppMngr2SisxRuntime::GetInstalledAppsL, uid 0x%08x, name %S",
                removablePackages[ index ]->Uid().Uid,
                &( removablePackages[ index ]->Name() ) );
        entry.OpenL( iSisRegistrySession, *( removablePackages[ index ] ) );

        FLOG( "CAppMngr2SisxRuntime::GetInstalledAppsL, IsPresentL = %d",
                entry.IsPresentL() );        
        // Check that sis entry is present. We should not show applications 
        // installed to memory card if card is not found.        
        if ( entry.IsPresentL() )
            {
            FLOG( "CAppMngr2SisxRuntime::GetInstalledAppsL, Sis is present." );
            	
            CAppMngr2SisxAppInfo* appObj = NULL;
            TRAPD( err, appObj = CAppMngr2SisxAppInfo::NewL( *this, entry, aFsSession ) );
            FLOG( "CAppMngr2SisxRuntime::GetInstalledAppsL, err %d", err );
            if( err == KErrNone )
                {
                CleanupStack::PushL( appObj );
                aApps.AppendL( appObj );
                CleanupStack::Pop( appObj );
                }
            }
            
        CleanupStack::PopAndDestroy( &entry );
        }

    CleanupStack::PopAndDestroy( &removablePackages );

    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, KErrNone );
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxRuntime::CancelGetInstalledApps()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxRuntime::CancelGetInstalledApps()
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxRuntime::CAppMngr2SisxRuntime()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxRuntime::CAppMngr2SisxRuntime( MAppMngr2RuntimeObserver &aObserver )
        : CAppMngr2Runtime( aObserver ) 
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxRuntime::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxRuntime::ConstructL( MAppMngr2RuntimeObserver& aObserver )
    {
    User::LeaveIfError( iSisRegistrySession.Connect() );
    iSwiMonitor = CAppMngr2SisxSwiMonitor::NewL( aObserver );
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxRuntime::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxRuntime::CreateNewPackageL( const TDesC& aFileName,
        RPointerArray<CAppMngr2PackageInfo>& aPackageInfos, RFs& aFs )
    {
    CAppMngr2SisxPackageInfo* packageInfo = NULL;
    TRAPD( err, packageInfo = CAppMngr2SisxPackageInfo::NewL( *this, aFileName, aFs ) );
    FLOG( "CAppMngr2SisxRuntime::CreateNewPackageL: file %S, err %d", &aFileName, err );
    if( err == KErrNone )
        {
        CleanupStack::PushL( packageInfo );
        aPackageInfos.AppendL( packageInfo );
        CleanupStack::Pop( packageInfo );
        }
    }

