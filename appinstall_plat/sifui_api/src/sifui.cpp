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
* Description:  Implementation of CSifUi class.
*
*/

#include "sifui.h"                              // CSifUi
#include "sifuiprivate.h"                       // CSifUiPrivate

// TODO: remove
#include <swi/msisuihandlers.h>                 // Swi::CAppInfo
#include "sifuiappinfo.h"                       // CSifUiAppInfo


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
    delete iPrivate;
    }

// ---------------------------------------------------------------------------
// CSifUi::ShowConfirmationL()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSifUi::ShowConfirmationL( const CSifUiAppInfo& aAppInfo )
    {
    return iPrivate->ShowConfirmationL( aAppInfo );
    }

// ---------------------------------------------------------------------------
// CSifUi::SetMemorySelectionL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUi::SetMemorySelectionL( const RArray<TInt>& aDriveNumbers )
    {
    iPrivate->SetMemorySelectionL( aDriveNumbers );
    }

// ---------------------------------------------------------------------------
// CSifUi::SelectedDrive()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSifUi::SelectedDrive( TInt& aDriveNumber )
    {
    return iPrivate->SelectedDrive( aDriveNumber );
    }

// ---------------------------------------------------------------------------
// CSifUi::SetCertificateInfoL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUi::SetCertificateInfoL(
        const RPointerArray<CSifUiCertificateInfo>& aCertificates )
    {
    iPrivate->SetCertificateInfoL( aCertificates );
    }

// ---------------------------------------------------------------------------
// CSifUi::ShowProgressL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUi::ShowProgressL( const CSifUiAppInfo& aAppInfo,
        TInt aProgressBarFinalValue )
    {
    iPrivate->ShowProgressL( aAppInfo, aProgressBarFinalValue );
    }

// ---------------------------------------------------------------------------
// CSifUi::IncreaseProgressBarValueL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUi::IncreaseProgressBarValueL( TInt aNewValue )
    {
    iPrivate->IncreaseProgressBarValueL( aNewValue );
    }

// ---------------------------------------------------------------------------
// CSifUi::ShowCompleteL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUi::ShowCompleteL()
    {
    iPrivate->ShowCompleteL();
    }

// ---------------------------------------------------------------------------
// CSifUi::ShowFailedL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUi::ShowFailedL( TInt aErrorCode, const TDesC& aErrorMessage,
        const TDesC& aErrorDetails )
    {
    iPrivate->ShowFailedL( aErrorCode, aErrorMessage, aErrorDetails );
    }

// ---------------------------------------------------------------------------
// CSifUi::CSifUi()
// ---------------------------------------------------------------------------
//
CSifUi::CSifUi()
    {
    }

// ---------------------------------------------------------------------------
// CSifUi::ConstructL()
// ---------------------------------------------------------------------------
//
void CSifUi::ConstructL()
    {
    iPrivate = CSifUiPrivate::NewL();
    }




// DEPRECATED FUNCTIONS -- TO BE REMOVED

EXPORT_C void CSifUi::SetMode( TMode /*aMode*/ )
    {
    User::Invariant();
    }

EXPORT_C CSifUi::TMode CSifUi::Mode()
    {
    User::Invariant();
    return EUnspecified;
    }

EXPORT_C void CSifUi::ShowFailedL( TInt /*aErrorCode*/ )
    {
    User::Invariant();
    }

EXPORT_C void CSifUi::SetCertificateInfoL(
        const RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/ )
    {
    User::Invariant();
    }

EXPORT_C TBool CSifUi::ShowConfirmationL( const Swi::CAppInfo& aAppInfo,
    TInt aAppSize, const CApaMaskedBitmap* aAppIcon )
    {
    CSifUiAppInfo* appInfo = CSifUiAppInfo::NewLC( aAppInfo.AppName(),
            aAppInfo.AppVendor(), aAppInfo.AppVersion(), aAppSize, aAppIcon );
    TBool retVal = iPrivate->ShowConfirmationL( *appInfo );
    CleanupStack::PopAndDestroy( appInfo );
    return retVal;
    }

EXPORT_C void CSifUi::ShowProgressL( const Swi::CAppInfo& aAppInfo,
        TInt aAppSize, TInt aProgressBarFinalValue )
    {
    CSifUiAppInfo* appInfo = CSifUiAppInfo::NewLC( aAppInfo.AppName(),
            aAppInfo.AppVendor(), aAppInfo.AppVersion(), aAppSize, NULL );
    iPrivate->ShowProgressL( *appInfo, aProgressBarFinalValue );
    CleanupStack::PopAndDestroy( appInfo );
    }

