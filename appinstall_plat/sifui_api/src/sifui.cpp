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
#include "sifuierrorinfo.h"                     // CSifUiErrorInfo


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
// CSifUi::ShowPreparingL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUi::ShowPreparingL()
    {
    iPrivate->ShowPreparingL();
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
// CSifUi::SetCertificateInfoL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUi::SetCertificateInfoL(
        const RPointerArray<CSifUiCertificateInfo>& aCertificates )
    {
    iPrivate->SetCertificateInfoL( aCertificates );
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
// CSifUi::SelectedDrive()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSifUi::SelectedDrive( TInt& aDriveNumber )
    {
    return iPrivate->SelectedDrive( aDriveNumber );
    }

// ---------------------------------------------------------------------------
// CSifUi::ShowProgressL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUi::ShowProgressL( const CSifUiAppInfo& aAppInfo,
        TInt aProgressBarFinalValue, TInstallingPhase aPhase )
    {
    iPrivate->ShowProgressL( aAppInfo, aProgressBarFinalValue, aPhase );
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
// CSifUi::IsCancelled()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSifUi::IsCancelled()
    {
    return iPrivate->IsCancelled();
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
EXPORT_C void CSifUi::ShowFailedL( const CSifUiErrorInfo& aErrorInfo )
    {
    iPrivate->ShowFailedL( aErrorInfo );
    }

// ---------------------------------------------------------------------------
// CSifUi::SetButtonVisible()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUi::SetButtonVisible( TOptionalButton aButton, TBool aIsVisible )
    {
    iPrivate->SetButtonVisible( aButton, aIsVisible );
    }

// ---------------------------------------------------------------------------
// CSifUi::ShowGrantCapabilitiesL()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSifUi::ShowGrantCapabilitiesL( const TCapabilitySet& aCapabilities )
    {
    return iPrivate->ShowGrantCapabilitiesL( aCapabilities );
    }

// ---------------------------------------------------------------------------
// CSifUi::ShowSelectLanguageL()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSifUi::ShowSelectLanguageL( const RArray<TLanguage>& aLanguages )
    {
    return iPrivate->ShowSelectLanguageL( aLanguages );
    }

// ---------------------------------------------------------------------------
// CSifUi::ShowSelectOptionsL()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSifUi::ShowSelectOptionsL( const MDesCArray& aSelectableItems,
        RArray<TInt>& aSelectedIndexes )
    {
    return iPrivate->ShowSelectOptionsL( aSelectableItems, aSelectedIndexes );
    }

// ---------------------------------------------------------------------------
// CSifUi::CancelDialogs()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSifUi::CancelDialogs()
    {
    iPrivate->CancelDialogs();
    }


// ---------------------------------------------------------------------------
// DEPRECATED: CSifUi::ShowFailedL()
// ---------------------------------------------------------------------------
//
// TODO: This function is deprecated, remove it completely.
EXPORT_C void CSifUi::ShowFailedL( TInt /*aErrorCode*/, const TDesC& /*aErrorMessage*/,
        const TDesC& /*aErrorDetails*/ )
    {
    User::Invariant();
    }

// ---------------------------------------------------------------------------
// DEPRECATED: CSifUi::ShowSingleSelectionL()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSifUi::ShowSingleSelectionL( const TDesC& /*aTitle*/,
            const MDesCArray& /*aSelectableItems*/, TInt& /*aSelectedIndex*/ )
    {
    User::Invariant();
    return EFalse;  // for compiler
    }

// ---------------------------------------------------------------------------
// DEPRECATED: CSifUi::ShowMultiSelectionL()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSifUi::ShowMultiSelectionL( const TDesC& /*aTitle*/,
            const MDesCArray& /*aSelectableItems*/, RArray<TInt>& /*aSelectedIndexes*/ )
    {
    User::Invariant();
    return EFalse;  // for compiler
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



