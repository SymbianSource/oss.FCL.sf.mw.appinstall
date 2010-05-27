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
* Description:  Implementation of CSifUiPrivate class.
*
*/

#include "sifuiprivate.h"                       // CSifUiPrivate
#include "sifuidefs.h"                          // SIF UI device dialog parameters
#include "sifuicertificateinfo.h"               // CSifUiCertificateInfo
#include "sifuiappinfo.h"                       // CSifUiAppInfo
#include <hb/hbcore/hbsymbianvariant.h>         // CHbSymbianVariantMap
#include <apgicnfl.h>                           // CApaMaskedBitmap
#include <s32mem.h>                             // RDesReadStream

const TInt KDriveLettersLen = 32;
const TInt KCertificateBufferGranularity = 1024;


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
    delete iCertificateInfo;
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::ShowConfirmationL()
// ---------------------------------------------------------------------------
//
TBool CSifUiPrivate::ShowConfirmationL( const CSifUiAppInfo& aAppInfo )
    {
    ChangeNoteTypeL( ESifUiConfirmationQuery );

    AddParamsAppInfoL( aAppInfo );
    if( iSelectableDrives )
        {
        AddParamL( KSifUiMemorySelection, *iSelectableDrives );
        }
    if( iCertificateInfo )
        {
        User::LeaveIfError( VariantMapL()->Add( KSifUiCertificates, iCertificateInfo ) );
        iCertificateInfo = NULL;
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
        const RPointerArray<CSifUiCertificateInfo>& aCertificates )
    {
    if( iCertificateInfo )
        {
        delete iCertificateInfo;
        iCertificateInfo = NULL;
        }
    if( aCertificates.Count() )
        {
        CBufBase* buf = CBufFlat::NewL( KCertificateBufferGranularity );
        CleanupStack::PushL( buf );
        RBufWriteStream writeStream( *buf );
        CleanupClosePushL( writeStream );

        TInt32 count = aCertificates.Count();
        writeStream.WriteInt32L( count );
        for( TInt index = 0; index < count; ++index )
           {
           aCertificates[ index ]->ExternalizeL( writeStream );
           }

        const TPtrC8 dataPtr( buf->Ptr( 0 ).Ptr(), buf->Size() );
        iCertificateInfo = CHbSymbianVariant::NewL( &dataPtr, CHbSymbianVariant::EBinary );

        CleanupStack::PopAndDestroy( 2, buf );  // writeStream, buf
        }
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::ShowProgressL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::ShowProgressL( const CSifUiAppInfo& aAppInfo,
        TInt aProgressBarFinalValue )
    {
    ChangeNoteTypeL( ESifUiProgressNote );

    AddParamsAppInfoL( aAppInfo );
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
// CSifUiPrivate::ClearParams()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::ClearParams()
    {
    if( iVariantMap )
        {
        delete iVariantMap;
        iVariantMap = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::VariantMapL()
// ---------------------------------------------------------------------------
//
CHbSymbianVariantMap* CSifUiPrivate::VariantMapL()
    {
    if( !iVariantMap )
        {
        iVariantMap = CHbSymbianVariantMap::NewL();
        }
    return iVariantMap;
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::ChangeNoteTypeL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::ChangeNoteTypeL( TInt aType )
    {
    ClearParams();
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
    User::LeaveIfError( VariantMapL()->Add( aKey, variant ) );
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::AddParamL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::AddParamL( const TDesC& aKey, const TDesC& aValue )
    {
    CHbSymbianVariant* variant = NULL;
    variant = CHbSymbianVariant::NewL( &aValue, CHbSymbianVariant::EDes );
    User::LeaveIfError( VariantMapL()->Add( aKey, variant ) );
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::AddParamListL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::AddParamListL( const TDesC& aKey, const MDesCArray& aList )
    {
    CHbSymbianVariant* variant = NULL;
    variant = CHbSymbianVariant::NewL( &aList, CHbSymbianVariant::EDesArray );
    User::LeaveIfError( VariantMapL()->Add( aKey, variant ) );
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::AddParamsAppInfoL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::AddParamsAppInfoL( const CSifUiAppInfo& aAppInfo )
    {
    // TODO: icons missing, could use binary transfer as in certificates
    AddParamL( KSifUiApplicationName, aAppInfo.Name() );
    const TVersion& version( aAppInfo.Version() );
    if( version.iBuild || version.iMajor || version.iMinor )
        {
        AddParamL( KSifUiApplicationVersion, version.Name() );
        }
    if( aAppInfo.Vendor().Length() )
        {
        AddParamL( KSifUiApplicationDetails, aAppInfo.Vendor() );
        }
    if( aAppInfo.Size() > 0 )
        {
        AddParamL( KSifUiApplicationSize, aAppInfo.Size() );
        }
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::AddParamsIconL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::AddParamsIconL( const CApaMaskedBitmap* aIcon )
    {
    // TODO: remove this function
    if( aIcon )
        {
        if( iBitmap )
            {
            delete iBitmap;
            iBitmap = NULL;
            }
        iBitmap = CApaMaskedBitmap::NewL( aIcon );

        CHbSymbianVariantMap* map = VariantMapL();
        CHbSymbianVariant* variant = NULL;
        TInt bitmapHandle = iBitmap->Handle();
        variant = CHbSymbianVariant::NewL( &bitmapHandle, CHbSymbianVariant::EInt );
        User::LeaveIfError( map->Add( KSifUiApplicationIconHandle, variant ) );
        TInt bitmapMaskHandle = iBitmap->Mask()->Handle();
        variant = CHbSymbianVariant::NewL( &bitmapMaskHandle, CHbSymbianVariant::EInt );
        User::LeaveIfError( map->Add( KSifUiApplicationIconMaskHandle, variant ) );
        }
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::DisplayDeviceDialogL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::DisplayDeviceDialogL()
    {
    if( iDeviceDialog && iIsDisplayingDialog )
        {
        User::LeaveIfError( iDeviceDialog->Update( *VariantMapL() ) );
        }
    else
        {
        if( !iDeviceDialog )
            {
            iDeviceDialog = CHbDeviceDialogSymbian::NewL();
            }
        User::LeaveIfError( iDeviceDialog->Show( KSifUiDeviceDialog, *VariantMapL(), this ) );
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

