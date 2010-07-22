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
#include "sifuicertificateinfo.h"               // CSifUiCertificateInfo
#include "sifuiappinfo.h"                       // CSifUiAppInfo
#include <hb/hbcore/hbsymbianvariant.h>         // CHbSymbianVariantMap
#include <hb/hbwidgets/hbdevicenotificationdialogsymbian.h> // CHbDeviceNotificationDialogSymbian
#include <apgicnfl.h>                           // CApaMaskedBitmap
#include <s32mem.h>                             // RDesReadStream
#include <e32property.h>                        // RProperty

const TInt KDriveLettersLen = 32;
const TInt KCertificateBufferGranularity = 1024;
const TInt KProgFull = 100;                     // 100%

const TUid KInstallIndicatorCategory = { 0x20022FC5 };
const TUint KInstallIndicatorStatus = 0x2002E690;

_LIT( KSifUiDefaultApplicationIcon, "qtg_large_application" );


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
    delete iIndicator;
    delete iAppInfo;
    delete iSelectableDrives;
    delete iCertificateBuffer;
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
    AddParamsCertificatesL();

    UpdateDialogAndWaitForResponseL();
    return( iDialogReturnValue == KErrNone );
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
    if( iCertificateBuffer )
        {
        delete iCertificateBuffer;
        iCertificateBuffer = NULL;
        }
    if( aCertificates.Count() )
        {
        iCertificateBuffer = CBufFlat::NewL( KCertificateBufferGranularity );
        RBufWriteStream writeStream( *iCertificateBuffer );
        CleanupClosePushL( writeStream );

        TInt32 count = aCertificates.Count();
        writeStream.WriteInt32L( count );
        for( TInt index = 0; index < count; ++index )
           {
           aCertificates[ index ]->ExternalizeL( writeStream );
           }

        CleanupStack::PopAndDestroy( &writeStream );
        }
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::ShowProgressL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::ShowProgressL( const CSifUiAppInfo& aAppInfo,
        TInt aProgressBarFinalValue, CSifUi::TInstallingPhase aPhase )
    {
    ChangeNoteTypeL( ESifUiProgressNote );

    AddParamsAppInfoL( aAppInfo );
    AddParamL( KSifUiProgressNoteFinalValue, aProgressBarFinalValue );
    iProgressBarFinalValue = aProgressBarFinalValue;
    AddParamL( KSifUiProgressNotePhase, aPhase );
    iInstallingPhase = aPhase;
    AddParamsHiddenButtonsL();

    UpdateDialogOrIndicatorWithoutWaitingL();
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::IncreaseProgressBarValueL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::IncreaseProgressBarValueL( TInt aNewValue )
    {
    ChangeNoteTypeL( ESifUiProgressNote );

    AddParamL( KSifUiProgressNoteValue, aNewValue );
    iProgressBarCurrentValue += aNewValue;
    AddParamsHiddenButtonsL();

    UpdateDialogOrIndicatorWithoutWaitingL();
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::IsCancelled()
// ---------------------------------------------------------------------------
//
TBool CSifUiPrivate::IsCancelled()
    {
    return( iDialogReturnValue == KErrCancel );
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::SetButtonVisible()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::SetButtonVisible( CSifUi::TOptionalButton aButton, TBool aIsVisible )
    {
    switch( aButton )
        {
        case CSifUi::EHideProgressButton:
            iNoHideProgressButton = !aIsVisible;
            break;
        case CSifUi::ECancelProgressButton:
            iNoCancelProgressButton = !aIsVisible;
            break;
        case CSifUi::EShowInAppLibButton:
            iNoShowInAppLibButton = !aIsVisible;
            break;
        case CSifUi::EErrorDetailsButton:
            iNoErrorDetailsButton = !aIsVisible;
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::ShowCompleteL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::ShowCompleteL()
    {
    ChangeNoteTypeL( ESifUiCompleteNote );

    AddParamsHiddenButtonsL();

    CompleteDialogOrIndicatorAndWaitForResponseL( KErrNone );
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

    CompleteDialogOrIndicatorAndWaitForResponseL( aErrorCode );
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::DoCancel()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::DoCancel()
    {
    if( iWait && iWait->IsStarted() && iWait->CanStopNow() )
        {
        iWaitCompletionCode = KErrCancel;
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

    const CHbSymbianVariant* variant = aData.Get( KSifUiQueryReturnValue );
    if( variant )
        {
        TInt* value = variant->Value<TInt>();
        if( value )
            {
            TSifUiDeviceDialogReturnValue returnValue =
                    static_cast<TSifUiDeviceDialogReturnValue>( *value );
            switch( returnValue )
                {
                case ESifUiContinue:
                    iDialogReturnValue = KErrNone;
                    WaitedResponseReceived( KErrNone );
                    break;
                case ESifUiCancel:
                    iDialogReturnValue = KErrCancel;
                    WaitedResponseReceived( KErrNone );
                    break;
                case ESifUiIndicator:
                    ActivateInstallIndicatorL();
                    break;
                default:
                    __ASSERT_DEBUG( EFalse, User::Invariant() );
                    break;
                }
            }
        else
            {
            __ASSERT_DEBUG( EFalse, User::Invariant() );
            }
        }
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::DeviceDialogClosed()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::DeviceDialogClosed( TInt aCompletionCode )
    {
    iIsDisplayingDialog = EFalse;
    WaitedResponseReceived( aCompletionCode );
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::IndicatorUserActivated()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::IndicatorUserActivated( const TDesC& aType,
        CHbSymbianVariantMap& /*aData*/ )
    {
    if( aType == KSifUiInstallIndicatorType )
        {
        CloseInstallIndicator();
        iIsFirstTimeToDisplay = ETrue;
        TRAP_IGNORE( DisplayDeviceDialogL() );
        }
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::CSifUiPrivate()
// ---------------------------------------------------------------------------
//
CSifUiPrivate::CSifUiPrivate() : CActive( CActive::EPriorityStandard ),
        iIsFirstTimeToDisplay( ETrue )
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
    // iDeviceDialog is created later, in the first call of DisplayDeviceDialogL()
    // iIndicator is also created later, in the first call of ActivateInstallIndicator()
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
    iDialogType = static_cast< TSifUiDeviceDialogType >( aType );
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
    // TODO: icons missing, could use binary transfer as in certificates

    if( iAppInfo != &aAppInfo )
        {
        if( iAppInfo )
            {
            delete iAppInfo;
            iAppInfo = NULL;
            }
        iAppInfo = CSifUiAppInfo::NewL( aAppInfo );
        }
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::AddParamsCertificatesL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::AddParamsCertificatesL()
    {
    if( iCertificateBuffer )
        {
        const TPtrC8 dataPtr( iCertificateBuffer->Ptr( 0 ).Ptr(),
                iCertificateBuffer->Size() );
        CHbSymbianVariant* certificates = CHbSymbianVariant::NewL( &dataPtr,
                CHbSymbianVariant::EBinary );
        CleanupStack::PushL( certificates );
        User::LeaveIfError( VariantMapL()->Add( KSifUiCertificates, certificates ) );
        CleanupStack::Pop( certificates );
        }
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::AddParamsHiddenButtonsL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::AddParamsHiddenButtonsL()
    {
    CHbSymbianVariant* variant = NULL;
    if( iNoHideProgressButton )
        {
        variant = CHbSymbianVariant::NewL( &iNoHideProgressButton, CHbSymbianVariant::EBool );
        TInt err = VariantMapL()->Add( KSifUiProgressNoteIsHideButtonHidden, variant );
        User::LeaveIfError( err );
        }
    if( iNoCancelProgressButton )
        {
        variant = CHbSymbianVariant::NewL( &iNoCancelProgressButton, CHbSymbianVariant::EBool );
        TInt err = VariantMapL()->Add( KSifUiProgressNoteIsCancelButtonHidden, variant );
        User::LeaveIfError( err );
        }
    if( iNoShowInAppLibButton )
        {
        variant = CHbSymbianVariant::NewL( &iNoShowInAppLibButton, CHbSymbianVariant::EBool );
        TInt err = VariantMapL()->Add( KSifUiCompleteNoteIsShowButtonHidden, variant );
        User::LeaveIfError( err );
        }
    if( iNoErrorDetailsButton )
        {
        variant = CHbSymbianVariant::NewL( &iNoErrorDetailsButton, CHbSymbianVariant::EBool );
        TInt err = VariantMapL()->Add( KSifUiErrorNoteIsDetailsButtonHidden, variant );
        User::LeaveIfError( err );
        }
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::ResendAllInstallationDetailsL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::ResendAllInstallationDetailsL()
    {
    AddParamL( KSifUiDialogType, iDialogType );
    if( iAppInfo )
        {
        AddParamsAppInfoL( *iAppInfo );
        }
    AddParamsCertificatesL();
    if( iProgressBarFinalValue )
        {
        AddParamL( KSifUiProgressNoteFinalValue, iProgressBarFinalValue );
        }
    if( iProgressBarCurrentValue )
        {
        AddParamL( KSifUiProgressNoteValue, iProgressBarCurrentValue );
        }
    AddParamL( KSifUiProgressNotePhase, iInstallingPhase );
    AddParamsHiddenButtonsL();
    // TODO: AddParamsIconL();
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::ActivateInstallIndicatorL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::ActivateInstallIndicatorL()
    {
    if( !iIndicator )
        {
        iIndicator = CHbIndicatorSymbian::NewL();
        iIndicator->SetObserver( this );
        }

    UpdateInstallIndicatorProgressL();
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::UpdateInstallIndicatorProgressL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::UpdateInstallIndicatorProgressL()
    {
    CHbSymbianVariantMap* parameters = CHbSymbianVariantMap::NewL();
    CleanupStack::PushL( parameters );

    CHbSymbianVariant* param = NULL;

    if( iAppInfo && iAppInfo->Name().Length() )
        {
        TPtrC appName = iAppInfo->Name();
        param = CHbSymbianVariant::NewL( &appName, CHbSymbianVariant::EDes );
        parameters->Add( KSifUiInstallIndicatorAppName, param );
        }
    if( iInstallingPhase )
        {
        param = CHbSymbianVariant::NewL( &iInstallingPhase, CHbSymbianVariant::EInt );
        parameters->Add( KSifUiInstallIndicatorPhase, param );
        }
    if( iProgressBarFinalValue )
        {
        TInt progressPercent = KProgFull * iProgressBarCurrentValue / iProgressBarFinalValue;
        param = CHbSymbianVariant::NewL( &progressPercent, CHbSymbianVariant::EInt );
        parameters->Add( KSifUiInstallIndicatorProgress, param );
        }

    // TODO: should both install indicator and device dialog use the same variant map?

    param = CHbSymbianVariant::NewL( parameters, CHbSymbianVariant::EVariantMap );
    CleanupStack::Pop( parameters );
    CleanupStack::PushL( param );
    iIndicator->Activate( KSifUiInstallIndicatorType, param );
    CleanupStack::PopAndDestroy( param );
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::CloseInstallIndicator()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::CloseInstallIndicator()
    {
    if( iIndicator )
        {
        iIndicator->Deactivate( KSifUiInstallIndicatorType );
        delete iIndicator;
        iIndicator = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::IsIndicatorActive()
// ---------------------------------------------------------------------------
//
TBool CSifUiPrivate::IsIndicatorActive()
    {
    TBool isActive = EFalse;

    if( iIndicator )
        {
        TInt value = 0;
        TInt err = RProperty::Get( KInstallIndicatorCategory, KInstallIndicatorStatus,
            value );
        if( !err )
            {
            isActive = ( value > 0 );
            }
        }

    return isActive;
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::ShowInstallIndicatorCompleteL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::ShowInstallIndicatorCompleteL( TInt aErrorCode )
    {
    CHbDeviceNotificationDialogSymbian* completeNote =
            CHbDeviceNotificationDialogSymbian::NewL();
    CleanupStack::PushL( completeNote );
    completeNote->SetIconNameL( KSifUiDefaultApplicationIcon );
    if( aErrorCode == KErrNone )
        {
        // TODO: use HbTextResolverSymbian to get localized text
        _LIT( KInstalled, "Installed" );
        completeNote->SetTitleL( KInstalled );
        }
    else
        {
        // TODO: use HbTextResolverSymbian to get localized text
        _LIT( KInstallationFailed, "Installation failed" );
        completeNote->SetTitleL( KInstallationFailed );
        }
    completeNote->ShowL();
    CleanupStack::PopAndDestroy( completeNote );
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::UpdateDialogAndWaitForResponseL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::UpdateDialogAndWaitForResponseL()
    {
    CloseInstallIndicator();
    DisplayDeviceDialogL();
    WaitForResponseL();
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::UpdateDialogOrIndicatorWithoutWaitingL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::UpdateDialogOrIndicatorWithoutWaitingL()
    {
    if( IsIndicatorActive() )
        {
        UpdateInstallIndicatorProgressL();
        }
    else
        {
        DisplayDeviceDialogL();
        }
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::CompleteDialogOrIndicatorAndWaitForResponseL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::CompleteDialogOrIndicatorAndWaitForResponseL( TInt aErrorCode )
    {
    if( !IsCancelled() )
        {
        if( IsIndicatorActive() )
            {
            UpdateInstallIndicatorProgressL();
            ShowInstallIndicatorCompleteL( aErrorCode );
            CloseInstallIndicator();
            }
        else
            {
            DisplayDeviceDialogL();
            WaitForResponseL();
            }
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
        if( !iIsFirstTimeToDisplay )
            {
            ResendAllInstallationDetailsL();
            }
        User::LeaveIfError( iDeviceDialog->Show( KSifUiDeviceDialog, *VariantMapL(), this ) );
        iIsDisplayingDialog = ETrue;
        }
    iIsFirstTimeToDisplay = EFalse;
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::WaitForResponseL()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::WaitForResponseL()
    {
    iWaitCompletionCode = KErrInUse;    // changed in WaitedResponseReceived() or in DoCancel()
    iDialogReturnValue = KErrUnknown;   // changed in DataReceived()
    if( !IsActive() && iWait && !iWait->IsStarted() )
        {
        iStatus = KRequestPending;
        SetActive();
        iWait->Start();
        }
    User::LeaveIfError( iWaitCompletionCode );
    }

// ---------------------------------------------------------------------------
// CSifUiPrivate::WaitedResponseReceived()
// ---------------------------------------------------------------------------
//
void CSifUiPrivate::WaitedResponseReceived( TInt aCompletionCode )
    {
    if( IsActive() )
        {
        iWaitCompletionCode = aCompletionCode;
        TRequestStatus* status( &iStatus );
        User::RequestComplete( status, KErrNone );
        }
    }

