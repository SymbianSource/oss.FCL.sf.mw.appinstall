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
* Description:   WidgetPackageInfo implementation
*
*/


#include "appmngr2widgetpackageinfo.h"  // CAppMngr2WidgetPackageInfo
#include "appmngr2widgetruntime.h"      // KAppMngr2WidgetUid
#include "appmngr2widgetinfoiterator.h" // CAppMngr2WidgetInfoIterator
#include "appmngr2widget.hrh"           // Widget command IDs
#include <appmngr2driveutils.h>         // TAppMngr2DriveUtils


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2WidgetPackageInfo::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2WidgetPackageInfo* CAppMngr2WidgetPackageInfo::NewL(
        CAppMngr2Runtime& aRuntime, const TDesC& aFileName, RFs& aFs )
    {
    CAppMngr2WidgetPackageInfo* self = new (ELeave) CAppMngr2WidgetPackageInfo( aRuntime, aFs );
    CleanupStack::PushL( self );
    self->ConstructL( aFileName );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetPackageInfo::~CAppMngr2WidgetPackageInfo()
// ---------------------------------------------------------------------------
//
CAppMngr2WidgetPackageInfo::~CAppMngr2WidgetPackageInfo()
    {
    CancelCommand();
    delete iName;
    delete iDetails;
    delete iFileName;
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetPackageInfo::IconIndex()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2WidgetPackageInfo::IconIndex() const
    {
    return 0;   // Icon 0: qgn_menu_am_widget
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetPackageInfo::Name()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2WidgetPackageInfo::Name() const
    {
    if( iName )
        {
        return *iName;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetPackageInfo::Details()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2WidgetPackageInfo::Details() const
    {
    if( iDetails )
        {
        return *iDetails;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetPackageInfo::SupportsGenericCommand()
// ---------------------------------------------------------------------------
//
TBool CAppMngr2WidgetPackageInfo::SupportsGenericCommand( TInt /*aCmdId*/ )
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetPackageInfo::HandleCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetPackageInfo::HandleCommandL( TInt aCommandId, TRequestStatus& aStatus )
    {
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
                User::LeaveIfError( iFs.Delete( *iFileName ) );
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
// CAppMngr2WidgetPackageInfo::HandleCommandResultL()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetPackageInfo::HandleCommandResultL( TInt aStatus )
    {
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
// CAppMngr2WidgetPackageInfo::CancelCommand()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetPackageInfo::CancelCommand()
    {
    if( iSWInstLauncher )
        {
        iSWInstLauncher->CancelAsyncRequest( SwiUI::ERequestInstall );
        iSWInstLauncher->Close();
        delete iSWInstLauncher;
        iSWInstLauncher = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetPackageInfo::CAppMngr2WidgetPackageInfo()
// ---------------------------------------------------------------------------
//
CAppMngr2WidgetPackageInfo::CAppMngr2WidgetPackageInfo( CAppMngr2Runtime& aRuntime,
        RFs& aFs ) : CAppMngr2PackageInfo( aRuntime, aFs )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetPackageInfo::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetPackageInfo::ConstructL( const TDesC& aFileName )
    {
    CAppMngr2PackageInfo::ConstructL();    // base class
    
    iFileName = aFileName.AllocL();
    iLocation = TAppMngr2DriveUtils::LocationFromFileNameL( aFileName, iFs );
    iLocationDrive = TDriveUnit( aFileName );
    
    TEntry fileEntry;
    User::LeaveIfError( iFs.Entry( aFileName, fileEntry ) );
    iName = fileEntry.iName.AllocL();
    iDetails = SizeStringWithUnitsL( fileEntry.iSize );
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetPackageInfo::ShowDetailsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetPackageInfo::ShowDetailsL()
    {
    CAppMngr2WidgetInfoIterator* iterator = CAppMngr2WidgetInfoIterator::NewL(
            *this, EAppMngr2StatusNotInstalled );
    CleanupStack::PushL( iterator );
    SwiUI::CommonUI::CCUIDetailsDialog* details = SwiUI::CommonUI::CCUIDetailsDialog::NewL();
    details->ExecuteLD( *iterator );
    CleanupStack::PopAndDestroy( iterator );
    }

