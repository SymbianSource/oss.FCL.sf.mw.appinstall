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

#include "sifuiprivate.h"                       // CSifUiPrivate
#include "sifuidefs.h"                          // SIF UI device dialog parameters
#include <hb/hbcore/hbsymbiandevicedialog.h>    // CHbDeviceDialog
#include <hb/hbcore/hbsymbianvariant.h>         // CHbSymbianVariantMap
#include <swi/msisuihandlers.h>                 // Swi::CAppInfo
#include <apgicnfl.h>                           // CApaMaskedBitmap

const TInt KDriveLettersLen = 32;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSifUiPrivate::NewL()
// ---------------------------------------------------------------------------
//
CSifUiPrivate* CSifUiPrivate::NewL()
    {
    CSifUiPrivate* self = new( ELeave ) CSifUiPrivate();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::~CSifUiPrivate()
// ---------------------------------------------------------------------------
//
CSifUiPrivate::~CSifUiPrivate()
    {
    Cancel();
    delete iWait;
    delete iDeviceDialog;
    delete iVariantMap;
    delete iBitmap;
    delete iSelectableDrives;
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::ShowConfirmationL()
// ---------------------------------------------------------------------------
//
TBool CSifUiPrivate::ShowConfirmationL( const Swi::CAppInfo& aAppInfo,
    TInt aAppSize, const CApaMaskedBitmap* aAppIcon )
	{
	ChangeNoteTypeL( ESifUiConfirmationQuery );

	AddParamsAppInfoAndSizeL( aAppInfo, aAppSize );
	if( aAppIcon )
		{
		AddParamsIconL( aAppIcon );
		}
	if( iSelectableDrives )
		{
		AddParamL( KSifUiMemorySelection, *iSelectableDrives );
		}

	DisplayDeviceDialogL();
	User::LeaveIfError( WaitForResponse() );
	return( iReturnValue == KErrNone );
	}

// ---------------------------------------------------------------------------
// CSifUiPrivate::SetMemorySelectionL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::SetMemorySelectionL( const RArray<TInt>& aDriveNumbers )
	{
	if( iSelectableDrives )
		{
		delete iSelectableDrives;
		iSelectableDrives = NULL;
		}

	TInt driveCount = aDriveNumbers.Count();
	if( driveCount > 0 )
		{
		const TChar KComma = ',';
		TBuf<KDriveLettersLen> driveList;
		for( TInt index = 0; index < driveCount; ++index )
			{
			TChar driveLetter;
			TInt err = RFs::DriveToChar( aDriveNumbers[ index ], driveLetter );
			if( !err )
				{
				driveList.Append( driveLetter );
				driveList.Append( KComma );
				}
			}
		iSelectableDrives = driveList.AllocL();
		}
	}

// ---------------------------------------------------------------------------
// CSifUiPrivate::SelectedDrive()
// ---------------------------------------------------------------------------
//
TInt CSifUiPrivate::SelectedDrive( TInt& aDriveNumber )
	{
    if( iSelectedDriveSet )
        {
        return RFs::CharToDrive( iSelectedDrive, aDriveNumber );
        }
    return KErrNotFound;
	}

// ---------------------------------------------------------------------------
// CSifUiPrivate::SetCertificateInfoL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::SetCertificateInfoL(
        const RPointerArray<Swi::CCertificateInfo>& aCertificates )
	{
   if( aCertificates.Count() )
		{
		AddParamsCertificatesL( aCertificates );
		}
	}

// ---------------------------------------------------------------------------
// CSifUiPrivate::ShowProgressL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::ShowProgressL( const Swi::CAppInfo& aAppInfo,
        TInt aAppSize, TInt aProgressBarFinalValue )
    {
    ChangeNoteTypeL( ESifUiProgressNote );

    AddParamsAppInfoAndSizeL( aAppInfo, aAppSize );
    AddParamL( KSifUiProgressNoteFinalValue, aProgressBarFinalValue );

    DisplayDeviceDialogL();
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::IncreaseProgressBarValueL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::IncreaseProgressBarValueL( TInt aNewValue )
    {
    ChangeNoteTypeL( ESifUiProgressNote );

    AddParamL( KSifUiProgressNoteValue, aNewValue );

    DisplayDeviceDialogL();
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::ShowCompleteL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::ShowCompleteL()
    {
    ChangeNoteTypeL( ESifUiCompleteNote );
    DisplayDeviceDialogL();
    User::LeaveIfError( WaitForResponse() );
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::ShowFailedL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::ShowFailedL( TInt aErrorCode, const TDesC& aErrorMessage,
        const TDesC& aErrorDetails )
    {
    ChangeNoteTypeL( ESifUiErrorNote );

    AddParamL( KSifUiErrorCode, aErrorCode );
    AddParamL( KSifUiErrorMessage, aErrorMessage );
    if( aErrorDetails != KNullDesC )
        {
        AddParamL( KSifUiErrorDetails, aErrorDetails );
        }

    DisplayDeviceDialogL();
    User::LeaveIfError( WaitForResponse() );
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::DoCancel()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::DoCancel()
    {
    if( iWait && iWait->IsStarted() && iWait->CanStopNow() )
        {
        iCompletionCode = KErrCancel;
        iWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::RunL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::RunL()
    {
    if( iWait )
        {
        iWait->AsyncStop();
        }
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::DataReceived()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::DataReceived( CHbSymbianVariantMap& aData )
    {
    const CHbSymbianVariant* selectedDriveVariant = aData.Get( KSifUiSelectedMemory );
    if( selectedDriveVariant )
    	{
		iSelectedDrive = *( selectedDriveVariant->Value<TChar>() );
		iSelectedDriveSet = ETrue;
    	}

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
// CSifUiPrivate::DeviceDialogClosed()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::DeviceDialogClosed( TInt aCompletionCode )
    {
    iIsDisplayingDialog = EFalse;
    ResponseReceived( aCompletionCode );
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::CSifUiPrivate()
// ---------------------------------------------------------------------------
//
CSifUiPrivate::CSifUiPrivate() : CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::ConstructL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::ConstructL()
    {
    iWait = new( ELeave ) CActiveSchedulerWait;
    // iDeviceDialog is allocated later, first call of DisplayDeviceDialogL()
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::ClearParamsL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::ClearParamsL()
    {
    if( iVariantMap )
        {
        delete iVariantMap;
        iVariantMap = NULL;
        }
    iVariantMap = CHbSymbianVariantMap::NewL();
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::ChangeNoteTypeL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::ChangeNoteTypeL( TInt aType )
    {
    ClearParamsL();
    AddParamL( KSifUiDialogType, aType );
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::AddParamL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::AddParamL( const TDesC& aKey, TInt aValue )
    {
    CHbSymbianVariant* variant = NULL;
    variant = CHbSymbianVariant::NewL( &aValue, CHbSymbianVariant::EInt );
    iVariantMap->Add( aKey, variant );
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::AddParamL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::AddParamL( const TDesC& aKey, const TDesC& aValue )
    {
    CHbSymbianVariant* variant = NULL;
    variant = CHbSymbianVariant::NewL( &aValue, CHbSymbianVariant::EDes );
    iVariantMap->Add( aKey, variant );
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::AddParamListL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::AddParamListL( const TDesC& aKey, const MDesCArray& aList )
    {
    CHbSymbianVariant* variant = NULL;
    variant = CHbSymbianVariant::NewL( &aList, CHbSymbianVariant::EDesArray );
    iVariantMap->Add( aKey, variant );
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::AddParamsAppInfoAndSizeL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::AddParamsAppInfoAndSizeL( const Swi::CAppInfo& aAppInfo, TInt aAppSize )
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
// CSifUiPrivate::AddParamsIconL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::AddParamsIconL( const CApaMaskedBitmap* aIcon )
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
// CSifUiPrivate::AddParamsCertificatesL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::AddParamsCertificatesL( const RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/ )
    {
    // TODO: implement
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::DisplayDeviceDialogL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::DisplayDeviceDialogL()
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
// CSifUiPrivate::WaitForResponse()
// ---------------------------------------------------------------------------
//
TInt CSifUiPrivate::WaitForResponse()
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
// CSifUiPrivate::ResponseReceived()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::ResponseReceived( TInt aCompletionCode )
    {
    if( IsActive() )
        {
        iCompletionCode = aCompletionCode;
        TRequestStatus* status( &iStatus );
        User::RequestComplete( status, KErrNone );
        }
    }

