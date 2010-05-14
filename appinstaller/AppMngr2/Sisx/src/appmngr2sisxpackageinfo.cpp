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
* Description:   SisxPackageInfo implementation
*
*/


#include "appmngr2sisxpackageinfo.h"    // CAppMngr2SisxPackageInfo
#include "appmngr2sisxruntime.h"        // KAppMngr2SisxUid
#include "appmngr2sisxinfoiterator.h"   // CAppMngr2SisxInfoIterator
#include "appmngr2sisxunknownlist.h"    // CAppMngr2SisxUnknownList
#include "appmngr2sisx.hrh"             // Icon IDs
#include <appmngr2driveutils.h>         // TAppMngr2DriveUtils
#include <appmngr2debugutils.h>         // FLOG macros
#include <appmngr2common.hrh>           // Command IDs
#include <appmngr2.rsg>                 // Resource IDs
#include <eikenv.h>                     // CEikonEnv


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2SisxPackageInfo::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxPackageInfo* CAppMngr2SisxPackageInfo::NewL(
        CAppMngr2Runtime& aRuntime, const TDesC& aFileName, RFs& aFs )
    {
    CAppMngr2SisxPackageInfo* self = new (ELeave) CAppMngr2SisxPackageInfo(
            aRuntime, aFs );
    CleanupStack::PushL( self );
    self->ConstructL( aFileName );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxPackageInfo::~CAppMngr2SisxPackageInfo()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxPackageInfo::~CAppMngr2SisxPackageInfo()
    {
    CancelCommand();
    delete iName;
    delete iDetails;
    delete iFileName;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxPackageInfo::IconIndex()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2SisxPackageInfo::IconIndex() const
    {
    return EQgnMenuAmSis;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxPackageInfo::Name()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2SisxPackageInfo::Name() const
    {
    if( iName )
        {
        return *iName;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxPackageInfo::Details()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2SisxPackageInfo::Details() const
    {
    if( iDetails )
        {
        return *iDetails;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxPackageInfo::IndicatorIconIndex()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2SisxPackageInfo::IndicatorIconIndex() const
    {
    if( IsShowOnTop() )
        {
        return EQgnIndiAmInstNoAdd;
        }
    return CAppMngr2InfoBase::IndicatorIconIndex();
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxPackageInfo::SupportsGenericCommand()
// ---------------------------------------------------------------------------
//
TBool CAppMngr2SisxPackageInfo::SupportsGenericCommand( TInt /*aCmdId*/ )
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxPackageInfo::HandleCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxPackageInfo::HandleCommandL( TInt aCommandId, TRequestStatus& aStatus )
    {
    FLOG( "CAppMngr2SisxPackageInfo::HandleCommandL( %d )", aCommandId );
    switch( aCommandId )
        {
        case EAppMngr2CmdViewDetails:
            ShowDetailsL();
            break;
            
        case EAppMngr2CmdInstall:
            if( !iSWInstLauncher )
                {
                SwiUI::RSWInstLauncher* swInstLauncher = new (ELeave) SwiUI::RSWInstLauncher;
                CleanupStack::PushL( swInstLauncher );
                User::LeaveIfError( swInstLauncher->Connect() );
                CleanupStack::Pop( swInstLauncher );
                iSWInstLauncher = swInstLauncher;
                }
            iSWInstLauncher->Install( aStatus, *iFileName );
            return;     // async operation started
            
        case EAppMngr2CmdRemove:
            if( ShowDeleteConfirmationQueryL() )
                {
                HandleDeleteL();
                }
            else
                {
                User::Leave( KErrCancel );
                }
            break;

        default:
            break;
        }

    // sync operation done, complete aStatus
    TRequestStatus* statusPtr = &aStatus;
    User::RequestComplete( statusPtr, KErrNone );
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxPackageInfo::HandleCommandResultL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxPackageInfo::HandleCommandResultL( TInt aStatus )
    {
    FLOG( "CAppMngr2SisxPackageInfo::HandleCommandResultL( %d )", aStatus );
    if( iSWInstLauncher )
        {
        iSWInstLauncher->Close();
        delete iSWInstLauncher;
        iSWInstLauncher = NULL;
        }
    if( aStatus != SwiUI::KSWInstErrUserCancel && aStatus != KErrCancel )
        {
        User::LeaveIfError( aStatus );
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxPackageInfo::CancelCommand()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxPackageInfo::CancelCommand()
    {
    FLOG( "CAppMngr2SisxPackageInfo::CancelCommand()" );
    if( iSWInstLauncher )
        {
        iSWInstLauncher->CancelAsyncRequest( SwiUI::ERequestUninstall );
        iSWInstLauncher->Close();
        delete iSWInstLauncher;
        iSWInstLauncher = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxPackageInfo::CAppMngr2SisxPackageInfo()
// ---------------------------------------------------------------------------
//
CAppMngr2SisxPackageInfo::CAppMngr2SisxPackageInfo( CAppMngr2Runtime& aRuntime,
        RFs& aFs ) : CAppMngr2PackageInfo( aRuntime, aFs )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxPackageInfo::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxPackageInfo::ConstructL( const TDesC& aFileName )
    {
    FLOG( "CAppMngr2SisxPackageInfo::ConstructL( %S )", &aFileName );
    CAppMngr2PackageInfo::ConstructL();    // base class

    TEntry fileEntry;
    User::LeaveIfError( iFs.Entry( aFileName, fileEntry ) );
    
    if( aFileName.Find( KAppMngr2DaemonPrivateFolder ) != KErrNotFound )
        {
        // untrusted application, display on top of others
        iShowOnTop = ETrue;
        
        // Installer generates untrusted app SISX files using UID names
        // (e.g. "20001234.SIS"). Get the real package name and use it
        // as the name of the untrusted application SISX item in UI.
        TInt drive;
        User::LeaveIfError( RFs::CharToDrive( aFileName[ 0 ], drive ) );
        CAppMngr2SisxUnknownList* unknownList = CAppMngr2SisxUnknownList::NewLC( drive );

        TInt index = unknownList->FindPkgWithUIDName( fileEntry.iName );
        if( index > 0 && index < unknownList->PkgCount() )
            {
            // display package name instead of UID name
            Swi::CUninstalledPackageEntry& pkgEntry = unknownList->PkgEntry( index );
            iName = pkgEntry.Name().AllocL();
            }
        else
            {
            // package name not found, display file name anyway
            iName = fileEntry.iName.AllocL();
            }
        CleanupStack::PopAndDestroy( unknownList );
        
        iDetails = Runtime().EikonEnv().AllocReadResourceL( R_SWINS_UNIT_UNTRUSTED );
        }
    else
        {
        // normal SIS file, use file name
        iName = fileEntry.iName.AllocL();
        iDetails = SizeStringWithUnitsL( fileEntry.iSize );
        }
    FLOG( "CAppMngr2SisxPackageInfo::ConstructL, iName = %S", iName );
    FLOG( "CAppMngr2SisxPackageInfo::ConstructL, iDetails = %S", iDetails );
    
    iFileName = aFileName.AllocL();
    iLocation = TAppMngr2DriveUtils::LocationFromFileNameL( aFileName, iFs );
    iLocationDrive = TDriveUnit( aFileName );
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxPackageInfo::HandleDeleteL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxPackageInfo::HandleDeleteL()
    {
    FLOG( "CAppMngr2SisxPackageInfo::HandleDeleteL()" );
    // CAppMngr2SisxUnknownList must be used to delete pre-installed SISX files,
    // which are located in SWI daemon's private folder and that are listed in
    // the CAppMngr2SisxUnknownList (i.e. in the Swi::UninstalledSisPackages).
    // These SISX files are applications that have been installed in memory
    // card (using some other device), and that were not installed in this device
    // when the memory card was inserted (e.g. because of security reasons).
    if( iFileName )
        {
        if( iFileName->Find( KAppMngr2PreInstalledPath ) != KErrNotFound )
            {
            TParsePtrC parser( *iFileName );
            TInt drive;
            User::LeaveIfError( RFs::CharToDrive( parser.Drive()[ 0 ], drive ) );
            CAppMngr2SisxUnknownList* unknownList = CAppMngr2SisxUnknownList::NewLC( drive );
            
            TInt index = unknownList->FindPkgWithUIDName( parser.NameAndExt() );
            if( index >= 0 && index < unknownList->PkgCount() )
                {
                // found in "unknown" list, remove using Swi::UninstalledSisPackages
                unknownList->RemovePkgL( index );
                }
            else 
                {
                // some other sisx package, try to delete using RFs
                User::LeaveIfError( iFs.Delete( *iFileName ) );
                }
            
            CleanupStack::PopAndDestroy( unknownList );
            }
        else
            {
            // normal sisx package, delete using RFs
            User::LeaveIfError( iFs.Delete( *iFileName ) );
            }
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2SisxPackageInfo::ShowDetailsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2SisxPackageInfo::ShowDetailsL()
    {
    FLOG( "CAppMngr2SisxPackageInfo::ShowDetailsL()" );
    CAppMngr2SisxInfoIterator* iterator = CAppMngr2SisxInfoIterator::NewL(
            *this, EAppMngr2StatusNotInstalled );
    CleanupStack::PushL( iterator );
    SwiUI::CommonUI::CCUIDetailsDialog* details = SwiUI::CommonUI::CCUIDetailsDialog::NewL();
    details->ExecuteLD( *iterator );
    CleanupStack::PopAndDestroy( iterator );
    }

