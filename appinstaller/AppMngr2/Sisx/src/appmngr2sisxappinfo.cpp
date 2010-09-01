/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <DRMHelper.h>
#include <drmutility.h>


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
    
    if ( iRegSessionOpen )
        {
        iSisRegSession.Close();
        }
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
    FLOG( "CAppMngr2SisxAppInfo::ConstructL, aEntry.SizeL() = %Ld, iDetails = %S",
            aEntry.SizeL(), iDetails );

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
    
    iRegSessionOpen = EFalse;    
    // iProtectedFile is allocated only in ShowDetails function.
    iProtectedFile = NULL;                                                                         
         
    
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

    CAppMngr2SisxInfoIterator* iterator = 
            CAppMngr2SisxInfoIterator::NewL( *this,
                                             EAppMngr2StatusInstalled );
    CleanupStack::PushL( iterator );
    
    // Let's check if installed SIS package is DRM protected.
    // This check has been in constructor but it takes long time to check all 
    // files for all installed packages so overall delay in UI is too long. 
    // So we have move the DRM check into here.   
    CheckDRMContentL();
    

    FLOG( "CAppMngr2SisxAppInfo::ShowDetailsL, isDRM %d, noRightsObj %d, CertCount %d",
            iIsDRMProtected, iIsRightsObjectMissingOrExpired, iCertificates.Count() );

    RFile fileHandle;
    TInt fileOpenError = KErrNone;
    if( iIsDRMProtected && !iIsRightsObjectMissingOrExpired )
        {
        if ( iProtectedFile )
            {
            FLOG( "ShowDetailsL, iProtecteFile %S", iProtectedFile );
            fileOpenError = fileHandle.Open( iFs, 
                                            *iProtectedFile, 
                                            EFileShareReadersOnly | EFileRead );
            FLOG( "ShowDetailsL, fileOpenError %d", fileOpenError );
            if( !fileOpenError )
                {
                CleanupClosePushL( fileHandle );
                }
            }
        }

    SwiUI::CommonUI::CCUIDetailsDialog* details = SwiUI::CommonUI::CCUIDetailsDialog::NewL();
    if( iIsDRMProtected && !iIsRightsObjectMissingOrExpired && !fileOpenError )
        {
        if( iCertificates.Count() )
            {
            details->ExecuteLD( *iterator, iCertificates, fileHandle );
            }
        else
            {
            details->ExecuteLD( *iterator, fileHandle );
            }
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

    if( iIsDRMProtected && !iIsRightsObjectMissingOrExpired && !fileOpenError )
        {
        CleanupStack::PopAndDestroy( &fileHandle );
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

// ---------------------------------------------------------------------------
// CAppMngr2SisxAppInfo::CheckDRMContentL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxAppInfo::CheckDRMContentL()
    {
    FLOG( "CAppMngr2SisxAppInfo::CheckDRMContentL");
    TInt err = KErrNone;
        
    if ( !iRegSessionOpen )
        {
        err = iSisRegSession.Connect(); 
        FLOG( "CheckDRMContentL, iSisRegSession.Connect err %d", err );
        if ( err )
            {
            iRegSessionOpen = EFalse;
            }
        else
            {
            iRegSessionOpen = ETrue;
            }        
        }
    
    FLOG( "CheckDRMContentL, iRegSessionOpen = %d", err );
    
    if ( iRegSessionOpen )
        {               
        Swi::RSisRegistryEntry entry;                                     
        err = entry.Open( iSisRegSession, iAppUid );
        FLOG( "CheckDRMContentL, entry.Open err = %d", err );
        
        if ( !err )
            {
            CleanupClosePushL( entry );
            
            RPointerArray<HBufC> files;         
            TRAP( err, entry.FilesL( files ) );
            FLOG( "CheckDRMContentL, entry.FilesL err = %d", err );
                                     
            if( !err )
                {
                CleanupResetAndDestroyPushL( files );
                
                // Use DRMUtility for DRM check. Utility class is much faster
                // then IsDRMProtected function.        
                DRM::CDrmUtility* utility = DRM::CDrmUtility::NewLC();
                                             
                for ( TInt index = 0; index < files.Count(); index++ )
                    {   
                    RFile fileHandle;
                    TInt error = fileHandle.Open( iFs, 
                                                  *files[ index ], 
                                                  EFileRead );
                    FLOG( "CheckDRMContentL, File open error %d", error );
                               
                    if ( error == KErrNone )
                        {                
                        CleanupClosePushL( fileHandle );                
                        err = KErrNone;
                        // We need to tarp this function since it may leave with
                        // some files which do not have enough data. If ConstrucL 
                        // leaves package is not shown in UI.
                        TRAP( err, iIsDRMProtected = 
                                utility->IsProtectedL( fileHandle ) );
                   
                        if ( err )
                            {
                            // If we have leave let's handle this as not DRM 
                            // procteded.
                            iIsDRMProtected = EFalse;
                            FLOG("CheckDRMContentL, IsProtectedL err %d",err);
                            }
                        
                        CleanupStack::PopAndDestroy( &fileHandle ); 
                        
                        if ( iIsDRMProtected )
                            { 
                            FLOG("CheckDRMContentL: File is DRM protected");
                        
                            HBufC* fileName = files[ index ];                              
                            // Let's alloc this only when DRM package is found.
                            // KMaxFileName (256) sould be enoug for all 
                            // file names.
                            if ( iProtectedFile == NULL )
                                {
                                iProtectedFile = HBufC::NewL( KMaxFileName );    
                                }
                            // Let's copy filename to member because we dont know
                            // for sure that pointer to sis registry entry is valid
                            // after entry delete.
                            if ( iProtectedFile )    
                                {
                                TPtr ptr = iProtectedFile->Des();
                                ptr.Copy( *fileName );                                
                                }  
                            FLOG( "CheckDRMContentL: iProtecteFile %S", 
                                    iProtectedFile );
                            
                            files.Remove( index );                    
                            
                            CDRMHelper* helper = CDRMHelper::NewLC();                    
                            CDRMHelperRightsConstraints* playconst = NULL;
                            CDRMHelperRightsConstraints* dispconst = NULL;
                            CDRMHelperRightsConstraints* execconst = NULL;
                            CDRMHelperRightsConstraints* printconst = NULL;            
                            TBool sendingallowed = EFalse;
                                            
                            FLOG( "CheckDRMContentL: GetRightsDetailsL" );
                            error = KErrNone;
                            TRAP( error, helper->GetRightsDetailsL( 
                                               *fileName, 
                                               ContentAccess::EView, 
                                               iIsRightsObjectMissingOrExpired, 
                                               sendingallowed, 
                                               playconst, 
                                               dispconst, 
                                               execconst, 
                                               printconst ) );                     
                            FLOG( "GetRightsDetailsL TRAP err = %d", error );
                            FLOG( "iIsRightsObjectMissingOrExpired = %d", 
                                    iIsRightsObjectMissingOrExpired );                            
                            delete playconst;
                            delete dispconst;
                            delete execconst;
                            delete printconst;                            
                            CleanupStack::PopAndDestroy( helper );                                                                                      
                            }
                        }
                    }                
                CleanupStack::PopAndDestroy( utility );
                CleanupStack::PopAndDestroy( &files );
                }
            CleanupStack::PopAndDestroy( &entry );
            }                
        }           
    }

// EOF

