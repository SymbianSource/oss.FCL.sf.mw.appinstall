/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   SisxAppInfo implementation
*
*/


#include "appmngr2sisxappinfo.h"        // CAppMngr2SisxAppInfo
#include "appmngr2sisxruntime.h"        // KAppMngr2SisxUid
#include "appmngr2sisxinfoiterator.h"   // CAppMngr2SisxInfoIterator
#include "appmngr2sisx.hrh"             // SISX command IDs
#include <eikmenup.h>                   // CEikMenuPaneItem
#include <appmngr2runtimeobserver.h>    // RefreshInstalledApps
#include <appmngr2driveutils.h>         // TAppMngr2DriveUtils
#include <appmngr2drmutils.h>           // TAppMngr2DRMUtils
#include <appmngr2cleanuputils.h>       // CleanupResetAndDestroyPushL
#include <appmngr2debugutils.h>         // FLOG macros
#include <x509cert.h>                   // CX509Certificate
#include <swi/sisregistrypackage.h>     // CSisRegistryPackage


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxAppInfo* CAppMngr2SisxAppInfo::NewL( CAppMngr2Runtime& aRuntime,
        Swi::RSisRegistryEntry& aEntry, RFs& aFsSession )
    {
    CAppMngr2SisxAppInfo* self = new (ELeave) CAppMngr2SisxAppInfo( aRuntime,
            aFsSession );
    CleanupStack::PushL( self );
    self->ConstructL( aEntry );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::~CAppMngr2SisxAppInfo()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxAppInfo::~CAppMngr2SisxAppInfo()
    {
    CancelCommand();
    delete iProtectedFile;
    delete iSWInstLauncherCustomUninstallParams;
    delete iName;
    delete iDetails;
    delete iVendor;
    iCertificates.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::IconIndex()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2SisxAppInfo::IconIndex() const
    {
    if( iIsDRMProtected && iIsRightsObjectMissingOrExpired )
        {
        return EQgnPropDrmExpLarge;
        }
    return EQgnMenuAmSis;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::Name()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2SisxAppInfo::Name() const
    {
    if( iName )
        {
        return *iName;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::Details()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2SisxAppInfo::Details() const
    {
    if( iDetails )
        {
        return *iDetails;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::SupportsGenericCommand()
// ---------------------------------------------------------------------------
//
TBool CAppMngr2SisxAppInfo::SupportsGenericCommand( TInt /*aCmdId*/ )
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::HandleCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxAppInfo::HandleCommandL( TInt aCommandId, TRequestStatus& aStatus )
    {
    FLOG( "CAppMngr2SisxAppInfo::HandleCommandL( %d )", aCommandId );
    iCommandId = aCommandId;
    switch( aCommandId )
        {
        case EAppMngr2CmdViewDetails:
            ShowDetailsL();
            break;

        case EAppMngr2CmdUninstall:
            HandleUninstallL( aStatus );
            return;     // async operation started

        default:
            break;
        }

    // sync operation done, complete aStatus
    TRequestStatus* statusPtr = &aStatus;
    User::RequestComplete( statusPtr, KErrNone );
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::HandleCommandResultL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxAppInfo::HandleCommandResultL( TInt aStatus )
    {
    FLOG( "CAppMngr2SisxAppInfo::HandleCommandResultL( %d ), cmd = %d", aStatus, iCommandId );
    if( iSWInstLauncher )
        {
        iSWInstLauncher->Close();
        delete iSWInstLauncher;
        iSWInstLauncher = NULL;
        delete iSWInstLauncherCustomUninstallParams;
        iSWInstLauncherCustomUninstallParams = NULL;
        }
    if( aStatus != SwiUI::KSWInstErrUserCancel && aStatus != KErrCancel )
        {
        User::LeaveIfError( aStatus );
        }
    if( iIsAugmentation && iCommandId == EAppMngr2CmdUninstall )
        {
        Runtime().Observer().RefreshInstalledApps();
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::CancelCommand()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxAppInfo::CancelCommand()
    {
    FLOG( "CAppMngr2SisxAppInfo::CancelCommand(), cmd = %d", iCommandId );
    if( iSWInstLauncher )
        {
        if( iIsAugmentation )
            {
            iSWInstLauncher->CancelAsyncRequest( SwiUI::ERequestCustomUninstall );
            }
        else
            {
            iSWInstLauncher->CancelAsyncRequest( SwiUI::ERequestUninstall );
            }
        iSWInstLauncher->Close();
        delete iSWInstLauncher;
        iSWInstLauncher = NULL;
        delete iSWInstLauncherCustomUninstallParams;
        iSWInstLauncherCustomUninstallParams = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::Version()
// ---------------------------------------------------------------------------
//
TVersion CAppMngr2SisxAppInfo::Version() const
    {
    return iVersion;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::Vendor()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2SisxAppInfo::Vendor() const
    {
    if( iVendor )
        {
        return *iVendor;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::IsTrusted()
// ---------------------------------------------------------------------------
//
TBool CAppMngr2SisxAppInfo::IsTrusted() const
    {
    return iIsTrusted;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::CAppMngr2SisxAppInfo()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxAppInfo::CAppMngr2SisxAppInfo( CAppMngr2Runtime& aRuntime,
        RFs& aFsSession ) : CAppMngr2AppInfo( aRuntime, aFsSession )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxAppInfo::ConstructL( Swi::RSisRegistryEntry& aEntry )
    {
    FLOG( "CAppMngr2SisxAppInfo::ConstructL()" );
    CAppMngr2AppInfo::ConstructL();     // base construct
    
    iAppUid = aEntry.UidL();
    FLOG( "CAppMngr2SisxAppInfo::ConstructL, iAppUid = 0x%08x", iAppUid.iUid );
    iName = aEntry.PackageNameL();
    FLOG( "CAppMngr2SisxAppInfo::ConstructL, iName = %S", iName );
    iDetails = SizeStringWithUnitsL( aEntry.SizeL() );
    FLOG( "CAppMngr2SisxAppInfo::ConstructL, iDetails = %S", iDetails );

    TUint drivesMask = aEntry.InstalledDrivesL();
    if( drivesMask )
        {
        // Select the highest drive as location drive
        TInt drive = EDriveA;
        while( drivesMask >>= 1 )
            {
            drive++;
            }
        iLocationDrive = drive;
        }
    else
        {
        // No installed files, select C: as location drive
        iLocationDrive = EDriveC;
        }
    iLocation = TAppMngr2DriveUtils::LocationFromDriveL( iLocationDrive, iFs );

    iVersion = aEntry.VersionL();
    iVendor = aEntry.LocalizedVendorNameL();
    
    iIsAugmentation = aEntry.IsAugmentationL();
    if( iIsAugmentation )
        {
        Swi::CSisRegistryPackage* pkg = aEntry.PackageL();
        iAugmentationIndex = pkg->Index();
        delete pkg;
        }

    Swi::TSisPackageTrust trustLevel = aEntry.TrustL();  
    if( trustLevel >= Swi::ESisPackageCertificateChainValidatedToTrustAnchor )
        {
        iIsTrusted = ETrue;
        }

    // If installed SIS package is DRM protected, find the protected file and
    // save it's full name in iProtectedFile member variable for later use.
    // Full name of the protected file is needed in ShowDetailsL() function
    // to show the licence information.
    RPointerArray<HBufC> files;
    TRAPD( err, aEntry.FilesL( files ) );
    if( err == KErrNone )
        {
        CleanupResetAndDestroyPushL( files );
        for( TInt fileIndex = 0; fileIndex < files.Count() && !iIsDRMProtected; fileIndex++ )
            {
            HBufC* fileName = files[ fileIndex ];
            iIsDRMProtected = TAppMngr2DRMUtils::IsDRMProtected( *fileName );
            if( iIsDRMProtected )
                {
                FLOG( "CAppMngr2SisxAppInfo::ConstructL, protected file %S", fileName );
                iProtectedFile = fileName;  // takes ownership
                files.Remove( fileIndex );
                iIsRightsObjectMissingOrExpired = 
                    TAppMngr2DRMUtils::IsDRMRightsObjectExpiredOrMissingL( *fileName ); 
                }
            }
        CleanupStack::PopAndDestroy( &files );
        }

    FLOG( "CAppMngr2SisxAppInfo::ConstructL, loc=%d, aug=%d, trust=%d, drm=%d, RO=%d",
            iLocation, iIsAugmentation, iIsTrusted, iIsDRMProtected,
            iIsRightsObjectMissingOrExpired );
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::ShowDetailsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxAppInfo::ShowDetailsL()
    {
    FLOG( "CAppMngr2SisxAppInfo::ShowDetailsL()" );
    TRAP_IGNORE( ReadCertificatesL() );
    
    CAppMngr2SisxInfoIterator* iterator = CAppMngr2SisxInfoIterator::NewL( *this,
            EAppMngr2StatusInstalled );
    CleanupStack::PushL( iterator );
    
    SwiUI::CommonUI::CCUIDetailsDialog* details = SwiUI::CommonUI::CCUIDetailsDialog::NewL();
    FLOG( "CAppMngr2SisxAppInfo::ShowDetailsL, isDRM %d, noRightsObj %d, CertCount %d",
            iIsDRMProtected, iIsRightsObjectMissingOrExpired, iCertificates.Count() );
    
    if( iIsDRMProtected && !iIsRightsObjectMissingOrExpired )
        {
        RFile fileHandle;
        TInt err = fileHandle.Open( iFs, *iProtectedFile, EFileShareReadersOnly | EFileRead );
        CleanupClosePushL( fileHandle );
        
        if( iCertificates.Count() )
            {
            details->ExecuteLD( *iterator, iCertificates, fileHandle );
            }
        else
            {
            details->ExecuteLD( *iterator, fileHandle );
            }

        CleanupStack::PopAndDestroy( &fileHandle );
        }
    else
        {
        if( iCertificates.Count() )
            {
            details->ExecuteLD( *iterator, iCertificates );
            }
        else
            {
            details->ExecuteLD( *iterator );
            }
        }
    
    CleanupStack::PopAndDestroy( iterator );
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::ReadCertificatesL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxAppInfo::ReadCertificatesL()
    {
    if( !iCertsRead )
        {
        FLOG_PERF_STATIC_BEGIN( SisxAppInfo_ReadCerts );
        
        Swi::RSisRegistrySession regSession;
        CleanupClosePushL( regSession );
        User::LeaveIfError( regSession.Connect() );

        Swi::RSisRegistryEntry entry;
        CleanupClosePushL( entry );
        TInt err = entry.Open( regSession, iAppUid );
        if( err != KErrNotFound )
            {
            User::LeaveIfError( err );
            }
        RPointerArray<HBufC8> certificateChain;
        CleanupResetAndDestroyPushL( certificateChain );
        if( err == KErrNone )
            {
            entry.CertificateChainsL( certificateChain );
            }

        TInt certCount = certificateChain.Count();
        for( TInt index = 0; index < certCount; index++ )
            {
            CX509Certificate* cert = CX509Certificate::NewL( *certificateChain[ index ] );
            CleanupStack::PushL( cert );
            SwiUI::CommonUI::CCUICertificateInfo* certInfo =
                SwiUI::CommonUI::CCUICertificateInfo::NewL( *cert );
            CleanupStack::PopAndDestroy( cert );
            iCertificates.AppendL( certInfo );
            }

        CleanupStack::PopAndDestroy( &certificateChain );
        CleanupStack::PopAndDestroy( &entry );
        CleanupStack::PopAndDestroy( &regSession );
        iCertsRead = ETrue;
        
        FLOG_PERF_STATIC_END( SisxAppInfo_ReadCerts )
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::HandleUninstallL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxAppInfo::HandleUninstallL( TRequestStatus& aStatus )
    {
    FLOG( "CAppMngr2SisxAppInfo::HandleUninstallL()" );
    if( !iSWInstLauncher )
        {
        SwiUI::RSWInstLauncher* swInstLauncher = new (ELeave) SwiUI::RSWInstLauncher;
        CleanupStack::PushL( swInstLauncher );
        User::LeaveIfError( swInstLauncher->Connect() );
        CleanupStack::Pop( swInstLauncher );
        iSWInstLauncher = swInstLauncher;
        }
    else
        {
        User::Leave( KErrInUse );
        }
    
    if( iIsAugmentation )
        {
        FLOG( "CAppMngr2SisxAppInfo::HandleUninstallL, is augmentation" );
        SwiUI::TOpUninstallIndexParam params;
        params.iUid = iAppUid;
        params.iIndex = iAugmentationIndex;
        
        SwiUI::TOpUninstallIndexParamPckg pckg( params );
        if( iSWInstLauncherCustomUninstallParams )
            {
            delete iSWInstLauncherCustomUninstallParams;
            iSWInstLauncherCustomUninstallParams = NULL;
            }
        TInt length = pckg.Length();
        iSWInstLauncherCustomUninstallParams = HBufC8::NewL( length );
        *iSWInstLauncherCustomUninstallParams = pckg;

        FLOG( "CAppMngr2SisxAppInfo::HandleUninstallL, calling CustomUninstall" );
        iSWInstLauncher->CustomUninstall( aStatus, SwiUI::EOperationUninstallIndex,
                *iSWInstLauncherCustomUninstallParams, SwiUI::KSisxMimeType() );
        }
    else
        {
        FLOG( "CAppMngr2SisxAppInfo::HandleUninstallL, calling Uninstall" );
        iSWInstLauncher->Uninstall( aStatus, iAppUid, SwiUI::KSisxMimeType() );
        }
    }

