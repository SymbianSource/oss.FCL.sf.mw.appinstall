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
* Description:  Implementation of RSifUiCli class.
*
*/

#include "sifui.h"                              // CSifUi
#include "sifuidefs.h"                          // SIF UI device dialog parameters
#include <hb/hbcore/hbsymbiandevicedialog.h>    // CHbDeviceDialog
#include <hb/hbcore/hbsymbianvariant.h>         // CHbSymbianVariantMap
#include <swi/msisuihandlers.h>                 // Swi::CAppInfo
#include <apgicnfl.h>                           // CApaMaskedBitmap


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSifUi::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CSifUi* CSifUi::NewLC()
    {
    CSifUi* self = new( ELeave ) CSifUi();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CSifUi::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CSifUi* CSifUi::NewL()
    {
    CSifUi* self = CSifUi::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSifUi::~CSifUi()
// ---------------------------------------------------------------------------
//
CSifUi::~CSifUi()
    {
    Cancel();
    delete iWait;
    delete iDeviceDialog;
    delete iVariantMap;
    delete iBitmap;
    }

// ---------------------------------------------------------------------------
// CSifUi::SetMode()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUi::SetMode( TMode aMode )
    {
    iMode = aMode;
    }

// ---------------------------------------------------------------------------
// CSifUi::Mode()
// ---------------------------------------------------------------------------
//
EXPORT_C CSifUi::TMode CSifUi::Mode()
    {
    return iMode;
    }

// ---------------------------------------------------------------------------
// CSifUi::ShowConfirmationL()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSifUi::ShowConfirmationL( const Swi::CAppInfo& aAppInfo,
        TInt aAppSize, const CApaMaskedBitmap* aAppIcon,
        const RPointerArray<Swi::CCertificateInfo>& aCertificates )
    {
    ChangeNoteTypeL( ESifUiConfirmationQuery );

    AddParamsAppInfoAndSizeL( aAppInfo, aAppSize );
    if( aAppIcon )
        {
        AddParamsIconL( aAppIcon );
        }
    if( aCertificates.Count() )
        {
        AddParamsCertificatesL( aCertificates );
        }

    // TODO: send drive list for drive selection, get selected drive
    AddParamL( KSifUiMemorySelection, ETrue );

    DisplayDeviceDialogL();
    User::LeaveIfError( WaitForResponse() );
    return( iReturnValue == KErrNone );
    }

// ---------------------------------------------------------------------------
// CSifUi::ShowProgressL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUi::ShowProgressL( const Swi::CAppInfo& aAppInfo,
        TInt aAppSize, TInt aProgressBarFinalValue )
    {
    ChangeNoteTypeL( ESifUiProgressNote );

    AddParamsAppInfoAndSizeL( aAppInfo, aAppSize );
    AddParamL( KSifUiProgressNoteFinalValue, aProgressBarFinalValue );

    DisplayDeviceDialogL();
    }

// ---------------------------------------------------------------------------
// CSifUi::IncreaseProgressBarValueL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUi::IncreaseProgressBarValueL( TInt aNewValue )
    {
    ChangeNoteTypeL( ESifUiProgressNote );

    AddParamL( KSifUiProgressNoteValue, aNewValue );

    DisplayDeviceDialogL();
    }

// ---------------------------------------------------------------------------
// CSifUi::ShowCompleteL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUi::ShowCompleteL()
    {
    ChangeNoteTypeL( ESifUiCompleteNote );
    DisplayDeviceDialogL();
    User::LeaveIfError( WaitForResponse() );
    }

// ---------------------------------------------------------------------------
// CSifUi::ShowFailedL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUi::ShowFailedL( TInt aErrorCode )
    {
    ChangeNoteTypeL( ESifUiErrorNote );

    AddParamL( KSifUiErrorCode, aErrorCode );

    DisplayDeviceDialogL();
    User::LeaveIfError( WaitForResponse() );
    }

// ---------------------------------------------------------------------------
// CSifUi::DoCancel()
// ---------------------------------------------------------------------------
//
void CSifUi::DoCancel()
    {
    if( iWait && iWait->IsStarted() && iWait->CanStopNow() )
        {
        iCompletionCode = KErrCancel;
        iWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CSifUi::RunL()
// ---------------------------------------------------------------------------
//
void CSifUi::RunL()
    {
    if( iWait )
        {
        iWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CSifUi::DataReceived()
// ---------------------------------------------------------------------------
//
void CSifUi::DataReceived( CHbSymbianVariantMap& aData )
    {
    const CHbSymbianVariant* acceptedVariant = aData.Get( KSifUiQueryAccepted );
    if( acceptedVariant )
        {
        TBool* acceptedValue = acceptedVariant->Value<TBool>();
        if( acceptedValue && *acceptedValue )
            {
            iReturnValue = KErrNone;
            }
        else
            {
            iReturnValue = KErrCancel;
            }
        ResponseReceived( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CSifUi::DeviceDialogClosed()
// ---------------------------------------------------------------------------
//
void CSifUi::DeviceDialogClosed( TInt aCompletionCode )
    {
    iIsDisplayingDialog = EFalse;
    ResponseReceived( aCompletionCode );
    }

// ---------------------------------------------------------------------------
// CSifUi::CSifUi()
// ---------------------------------------------------------------------------
//
CSifUi::CSifUi() : CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CSifUi::ConstructL()
// ---------------------------------------------------------------------------
//
void CSifUi::ConstructL()
    {
    iWait = new( ELeave ) CActiveSchedulerWait;
    // iDeviceDialog is allocated later, first call of DisplayDeviceDialogL()
    }

// ---------------------------------------------------------------------------
// CSifUi::ClearParamsL()
// ---------------------------------------------------------------------------
//
void CSifUi::ClearParamsL()
    {
    if( iVariantMap )
        {
        delete iVariantMap;
        iVariantMap = NULL;
        }
    iVariantMap = CHbSymbianVariantMap::NewL();
    }

// ---------------------------------------------------------------------------
// CSifUi::ChangeNoteTypeL()
// ---------------------------------------------------------------------------
//
void CSifUi::ChangeNoteTypeL( TInt aType )
    {
    ClearParamsL();
    AddParamL( KSifUiDialogType, aType );
    AddParamL( KSifUiDialogMode, iMode );
    }

// ---------------------------------------------------------------------------
// CSifUi::AddParamL()
// ---------------------------------------------------------------------------
//
void CSifUi::AddParamL( const TDesC& aKey, TInt aValue )
    {
    CHbSymbianVariant* variant = NULL;
    variant = CHbSymbianVariant::NewL( &aValue, CHbSymbianVariant::EInt );
    iVariantMap->Add( aKey, variant );
    }

// ---------------------------------------------------------------------------
// CSifUi::AddParamL()
// ---------------------------------------------------------------------------
//
void CSifUi::AddParamL( const TDesC& aKey, const TDesC& aValue )
    {
    CHbSymbianVariant* variant = NULL;
    variant = CHbSymbianVariant::NewL( &aValue, CHbSymbianVariant::EDes );
    iVariantMap->Add( aKey, variant );
    }

// ---------------------------------------------------------------------------
// CSifUi::AddParamsAppInfoAndSizeL()
// ---------------------------------------------------------------------------
//
void CSifUi::AddParamsAppInfoAndSizeL( const Swi::CAppInfo& aAppInfo, TInt aAppSize )
    {
    AddParamL( KSifUiApplicationName, aAppInfo.AppName() );
    const TVersion& version( aAppInfo.AppVersion() );
    if( version.iBuild || version.iMajor || version.iMinor )
        {
        AddParamL( KSifUiApplicationVersion, version.Name() );
        }
    if( aAppInfo.AppVendor().Length() )
        {
        AddParamL( KSifUiApplicationDetails, aAppInfo.AppVendor() );
        }
    if( aAppSize > 0 )
        {
        AddParamL( KSifUiApplicationSize, aAppSize );
        }
    }

// ---------------------------------------------------------------------------
// CSifUi::AddParamsIconL()
// ---------------------------------------------------------------------------
//
void CSifUi::AddParamsIconL( const CApaMaskedBitmap* aIcon )
    {
    if( aIcon )
        {
        if( iBitmap )
            {
            delete iBitmap;
            iBitmap = NULL;
            }
        iBitmap = CApaMaskedBitmap::NewL( aIcon );

        CHbSymbianVariant* variant = NULL;
        TInt bitmapHandle = iBitmap->Handle();
        variant = CHbSymbianVariant::NewL( &bitmapHandle, CHbSymbianVariant::EInt );
        iVariantMap->Add( KSifUiApplicationIconHandle, variant );
        TInt bitmapMaskHandle = iBitmap->Mask()->Handle();
        variant = CHbSymbianVariant::NewL( &bitmapMaskHandle, CHbSymbianVariant::EInt );
        iVariantMap->Add( KSifUiApplicationIconMaskHandle, variant );
        }
    }

// ---------------------------------------------------------------------------
// CSifUi::AddParamsCertificatesL()
// ---------------------------------------------------------------------------
//
void CSifUi::AddParamsCertificatesL( const RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/ )
    {
    // TODO: implement
    }

// ---------------------------------------------------------------------------
// CSifUi::DisplayDeviceDialogL()
// ---------------------------------------------------------------------------
//
void CSifUi::DisplayDeviceDialogL()
    {
    if( iDeviceDialog && iIsDisplayingDialog )
        {
        iDeviceDialog->Update( *iVariantMap );
        }
    else
        {
        if( !iDeviceDialog )
            {
            iDeviceDialog = CHbDeviceDialog::NewL();
            }
        iDeviceDialog->Show( KSifUiDeviceDialog, *iVariantMap, this );
        iIsDisplayingDialog = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CSifUi::WaitForResponse()
// ---------------------------------------------------------------------------
//
TInt CSifUi::WaitForResponse()
    {
    iCompletionCode = KErrInUse;
    iReturnValue = KErrUnknown;
    if( !IsActive() && iWait && !iWait->IsStarted() )
        {
        iStatus = KRequestPending;
        SetActive();
        iWait->Start();
        }
    return iCompletionCode;
    }

// ---------------------------------------------------------------------------
// CSifUi::ResponseReceived()
// ---------------------------------------------------------------------------
//
void CSifUi::ResponseReceived( TInt aCompletionCode )
    {
    if( IsActive() )
        {
        iCompletionCode = aCompletionCode;
        TRequestStatus* status( &iStatus );
        User::RequestComplete( status, KErrNone );
        }
    }

