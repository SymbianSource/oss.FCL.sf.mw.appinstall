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
* Description:   WidgetAppInfo implementation
*
*/


#include "appmngr2widgetappinfo.h"      // CAppMngr2WidgetAppInfo
#include "appmngr2widgetruntime.h"      // KAppMngr2WidgetUid
#include "appmngr2widgetinfoiterator.h" // CAppMngr2WidgetInfoIterator
#include "appmngr2widget.hrh"           // Widget command IDs
#include <WidgetRegistryData.h>         // CWidgetInfo
#include <appmngr2driveutils.h>         // TAppMngr2DriveUtils


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAppMngr2WidgetAppInfo::NewL()
// ---------------------------------------------------------------------------
//
CAppMngr2WidgetAppInfo* CAppMngr2WidgetAppInfo::NewL(
        CAppMngr2Runtime& aRuntime, const CWidgetInfo& aWidget, RFs& aFsSession )
    {
    CAppMngr2WidgetAppInfo* self = new (ELeave) CAppMngr2WidgetAppInfo( aRuntime, aFsSession );
    CleanupStack::PushL( self );
    self->ConstructL( aWidget );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetAppInfo::~CAppMngr2WidgetAppInfo()
// ---------------------------------------------------------------------------
//
CAppMngr2WidgetAppInfo::~CAppMngr2WidgetAppInfo()
    {
    CancelCommand();
    delete iName;
    delete iDetails;
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetAppInfo::IconIndex()
// ---------------------------------------------------------------------------
//
TInt CAppMngr2WidgetAppInfo::IconIndex() const
    {
    return 0;
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetAppInfo::Name()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2WidgetAppInfo::Name() const
    {
    if( iName )
        {
        return *iName;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetAppInfo::Details()
// ---------------------------------------------------------------------------
//
const TDesC& CAppMngr2WidgetAppInfo::Details() const
    {
    if( iDetails )
        {
        return *iDetails;
        }
    return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetAppInfo::SupportsGenericCommand()
// ---------------------------------------------------------------------------
//
TBool CAppMngr2WidgetAppInfo::SupportsGenericCommand( TInt /*aCmdId*/ )
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetAppInfo::HandleCommandL()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetAppInfo::HandleCommandL( TInt aCommandId, TRequestStatus& aStatus )
    {
    switch( aCommandId )
        {
        case EAppMngr2CmdViewDetails:
            ShowDetailsL();
            break;

        case EAppMngr2CmdUninstall:
            if( !iSWInstLauncher )
                {
                SwiUI::RSWInstLauncher* swInstLauncher = new (ELeave) SwiUI::RSWInstLauncher;
                CleanupStack::PushL( swInstLauncher );
                User::LeaveIfError( swInstLauncher->Connect() );
                CleanupStack::Pop( swInstLauncher );
                iSWInstLauncher = swInstLauncher;
                }
            iSWInstLauncher->Uninstall( aStatus, iWidgetUid, KDataTypeWidget );
            return;     // async operation started

        default:
            break;
        }

    // sync operation done, complete aStatus
    TRequestStatus* statusPtr = &aStatus;
    User::RequestComplete( statusPtr, KErrNone );
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetAppInfo::HandleCommandResultL()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetAppInfo::HandleCommandResultL( TInt aStatus )
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
// CAppMngr2WidgetAppInfo::CancelCommand()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetAppInfo::CancelCommand()
    {
    if( iSWInstLauncher )
        {
        iSWInstLauncher->CancelAsyncRequest( SwiUI::ERequestUninstall );
        iSWInstLauncher->Close();
        delete iSWInstLauncher;
        iSWInstLauncher = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetAppInfo::CAppMngr2WidgetAppInfo()
// ---------------------------------------------------------------------------
//
CAppMngr2WidgetAppInfo::CAppMngr2WidgetAppInfo( CAppMngr2Runtime& aRuntime,
        RFs& aFsSession ) : CAppMngr2AppInfo( aRuntime, aFsSession )
    {
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetAppInfo::ConstructL()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetAppInfo::ConstructL( const CWidgetInfo& aWidget )
    {
    CAppMngr2AppInfo::ConstructL();
    
    iWidgetUid = aWidget.iUid;
    iName = aWidget.iBundleName->AllocL();
    iDetails = SizeStringWithUnitsL( aWidget.iFileSize );

    iLocationDrive = TDriveUnit( *aWidget.iDriveName );
    iLocation = TAppMngr2DriveUtils::LocationFromDriveL( iLocationDrive, iFs );
    }

// ---------------------------------------------------------------------------
// CAppMngr2WidgetAppInfo::ShowDetailsL()
// ---------------------------------------------------------------------------
//
void CAppMngr2WidgetAppInfo::ShowDetailsL()
    {
    CAppMngr2WidgetInfoIterator* iterator = CAppMngr2WidgetInfoIterator::NewL(
            *this, EAppMngr2StatusInstalled );
    CleanupStack::PushL( iterator );
    SwiUI::CommonUI::CCUIDetailsDialog* details = SwiUI::CommonUI::CCUIDetailsDialog::NewL();
    details->ExecuteLD( *iterator );
    CleanupStack::PopAndDestroy( iterator );
    }

