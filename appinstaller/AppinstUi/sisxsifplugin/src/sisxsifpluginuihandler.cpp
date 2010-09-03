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
* Description: Native SISX SIF (Software Install Framework) plugin.
*
*/

#include "sisxsifpluginuihandler.h"     // CSisxSifPluginUiHandler
#include "sisxsifpluginerrorhandler.h"  // CSisxSifPluginErrorHandler
#include "sisxsifplugin.pan"            // Panic codes
#include "sisxsifcleanuputils.h"        // CleanupResetAndDestroyPushL
#include "sisxsifuiselectioncache.h"    // CSisxUISelectionCache
#include <sifui.h>                      // CSifUi
#include <sifuiappinfo.h>               // CSifUiAppInfo
#include <sifuicertificateinfo.h>       // CSifUiCertificateInfo
#include <sifuierrorinfo.h>             // CSifUiErrorInfo
#include <bautils.h>                    // BaflUtils
#include <driveinfo.h>                  // DriveInfo
#include <featmgr.h>                    // FeatureManager

using namespace Usif;

const TInt KFreeSpaceTreshold = 128*1024;   // bytes

// TODO: replace with proper tracing support
#ifdef _DEBUG
#define FLOG(x)         RDebug::Print(x);
#define FLOG_1(x,y)     RDebug::Print(x, y);
#define FLOG_2(x,y,z)   RDebug::Print(x, y, z);
#else
#define FLOG(x)
#define FLOG_1(x,y)
#define FLOG_2(x,y,z)
#endif


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::NewL()
// ---------------------------------------------------------------------------
//
CSisxSifPluginUiHandler* CSisxSifPluginUiHandler::NewL( RFs& aFs,
        CSisxSifPluginErrorHandler& aErrorHandler )
    {
    FLOG( _L("CSisxSifPluginUiHandler::NewL") );
    CSisxSifPluginUiHandler *self = new( ELeave ) CSisxSifPluginUiHandler( aFs, aErrorHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::~CSisxSifPluginUiHandler()
// ---------------------------------------------------------------------------
//
CSisxSifPluginUiHandler::~CSisxSifPluginUiHandler()
    {
    FLOG( _L("CSisxSifPluginUiHandler::~CSisxSifPluginUiHandler") );
    delete iSelectionCache;
    delete iSifUi;
    iSelectableDrives.Close();
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayTextL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayTextL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TFileTextOption aOption, const TDesC& aText )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayTextL") );
    TBool okToContinue = EFalse;

    switch( aOption )
        {
        case Swi::EInstFileTextOptionContinue:
            ShowQuestionWithContinueL( aText );
            okToContinue = ETrue;
            break;
        case Swi::EInstFileTextOptionSkipOneIfNo:
            okToContinue = ShowQuestionL( aText );
            break;
        case Swi::EInstFileTextOptionAbortIfNo:
        case Swi::EInstFileTextOptionExitIfNo:
            okToContinue = ShowQuestionL( aText );
            break;
        case Swi::EInstFileTextOptionForceAbort:
            ShowQuestionWithContinueL( aText );
            break;
        default:
            break;
        }

    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayErrorL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::DisplayErrorL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TErrorDialog aType, const TDesC& aParam )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayErrorL") );
    SetErrorSwiErrorL( aType, aParam );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayDependencyBreakL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayDependencyBreakL( const Swi::CAppInfo& /*aAppInfo*/,
        const RPointerArray<TDesC>& /*aComponents*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayDependencyBreakL") );
    TBool okToContinue = EFalse;

    // TODO: localized UI string needed
    _LIT( KText, "Removal may stop other applications working. Continue?" );
    okToContinue = ShowQuestionL( KText );

    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayApplicationsInUseL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayApplicationsInUseL( const Swi::CAppInfo& /*aAppInfo*/,
        const RPointerArray<TDesC>& /*aAppNames*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayApplicationsInUseL") );
    return ETrue;   // silently accepted
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayQuestionL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayQuestionL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TQuestionDialog aQuestion, const TDesC& /*aDes*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayQuestionL") );
    TBool okToContinue = EFalse;

    switch( aQuestion )
        {
        case Swi::EQuestionIncompatible:
            if( !iQuestionIncompatibleDisplayed )
                {
                _LIT( KText, "Application not compatible with phone. Continue anyway?" );
                okToContinue = ShowQuestionL( KText );
                iQuestionIncompatibleDisplayed = ETrue;
                }
            else
                {
                okToContinue = ETrue;   // already accepted once
                }
            break;

        case Swi::EQuestionOverwriteFile:
        default:
            okToContinue = ETrue;   // silently accepted
            break;
        }

    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayInstallL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayInstallL( const Swi::CAppInfo& /*aAppInfo*/,
        const CApaMaskedBitmap* /*aLogo*/,
        const RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayInstallL") );
    ASSERT( iOperationPhase == EInstalling );
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayGrantCapabilitiesL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayGrantCapabilitiesL( const Swi::CAppInfo& /*aAppInfo*/,
        const TCapabilitySet& aCapabilitySet )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayGrantCapabilitiesL") );
    TBool okToContinue = iSifUi->ShowGrantCapabilitiesL( aCapabilitySet );
    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayLanguageL()
// ---------------------------------------------------------------------------
//
TInt CSisxSifPluginUiHandler::DisplayLanguageL( const Swi::CAppInfo& /*aAppInfo*/,
        const RArray<TLanguage>& aLanguages )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayLanguageL") );
    TInt langIndex = iSifUi->ShowSelectLanguageL( aLanguages );
    return langIndex;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayDriveL()
// ---------------------------------------------------------------------------
//
TInt CSisxSifPluginUiHandler::DisplayDriveL( const Swi::CAppInfo& /*aAppInfo*/,
        TInt64 /*aSize*/, const RArray<TChar>& aDriveLetters,
        const RArray<TInt64>& /*aDriveSpaces*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayDriveL") );

    TInt driveNumber = EDriveC;
    TInt err = iSifUi->SelectedDrive( driveNumber );
    if( err )
        {
        FLOG_1( _L("CSisxSifPluginUiHandler::DisplayDriveL; SelectedDrive err=%d"), err );
        }

    TChar driveLetter = 'C';
    err = RFs::DriveToChar( driveNumber, driveLetter );
    if( err )
        {
        FLOG_1( _L("CSisxSifPluginUiHandler::DisplayDriveL; DriveToChar err=%d"), err );
        }

    TInt index = aDriveLetters.Find( driveLetter );
    if( index >= 0 && index < aDriveLetters.Count() )
        {
        return index;
        }
    return 0;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayUpgradeL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayUpgradeL( const Swi::CAppInfo& /*aAppInfo*/,
        const Swi::CAppInfo& /*aExistingAppInfo*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayUpgradeL") );
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayOptionsL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayOptionsL( const Swi::CAppInfo& /*aAppInfo*/,
        const RPointerArray<TDesC>& aOptions, RArray<TBool>& aSelections )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayOptionsL") );
    TInt optionCount = aOptions.Count();
    CPtrCArray* selectableItems = new( ELeave ) CPtrC16Array( optionCount );
    CleanupStack::PushL( selectableItems );
    for( TInt index = 0; index < optionCount; index++ )
        {
        selectableItems->AppendL( *aOptions[ index ] );
        }

    RArray<TInt> selectedIndexes;
    TBool isSelected = iSifUi->ShowSelectOptionsL( *selectableItems, selectedIndexes );
    if( isSelected && selectedIndexes.Count() )
        {
        TInt selectionsCount = aSelections.Count();
        __ASSERT_DEBUG( selectionsCount == optionCount, User::Invariant() );
        for( TInt index = 0; index < selectionsCount; index++ )
            {
            aSelections[ index ] = ( selectedIndexes.Find( index ) != KErrNotFound );
            }
        }
    return isSelected;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::HandleInstallEventL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::HandleInstallEventL( const Swi::CAppInfo& aAppInfo,
        Swi::TInstallEvent aEvent, TInt aValue, const TDesC& /*aDes*/ )
    {
    FLOG_2( _L("CSisxSifPluginUiHandler::HandleInstallEventL: aEvent %d, aValue %d"), aEvent, aValue );
    TBool okToContinue = EFalse;

    if( !iSifUi->IsCancelled() )
        {
        okToContinue = ETrue;

        switch( aEvent )
            {
            case Swi::EEventSetProgressBarFinalValue:
                iProgressBarFinalValue = aValue;
                if( iOperationPhase == EInstalling )
                    {
                    ShowProgressL( aAppInfo, iProgressBarFinalValue, CSifUi::EInstalling );
                    }
                break;

            case Swi::EEventUpdateProgressBar:
                if( iOperationPhase == EInstalling )
                    {
                    iSifUi->IncreaseProgressBarValueL( aValue );
                    }
                iProgressBarCurrentValue += aValue;
                PublishProgressL( EFileOperation );
                break;

            case Swi::EEventDevCert:
                // TODO: show "developer certificate" warning note
                break;

            case Swi::EEventOcspCheckEnd:
            case Swi::EEventAbortedInstall:
            case Swi::EEventAbortedUnInstall:
            case Swi::EEventCompletedInstall:
            case Swi::EEventCompletedUnInstall:
            case Swi::EEventCompletedNoMessage:
            case Swi::EEventLeave:
            default:
                break;
            }
        }

    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::HandleCancellableInstallEventL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::HandleCancellableInstallEventL( const Swi::CAppInfo& aAppInfo,
        Swi::TInstallCancellableEvent aEvent, Swi::MCancelHandler& aCancelHandler,
        TInt aValue, const TDesC& /*aDes*/ )
    {
    FLOG_2( _L("CSisxSifPluginUiHandler::HandleCancellableInstallEventL: aEvent %d, aValue %d"), aEvent, aValue );

    if( iSifUi->IsCancelled() )
        {
        aCancelHandler.HandleCancel();
        }
    else
        {
        switch( aEvent )
            {
            case Swi::EEventOcspCheckStart:
                ShowProgressL( aAppInfo, aValue, CSifUi::ECheckingCerts );
                iProgressBarCurrentValue += aValue;
                PublishProgressL( EOCSPCheck );
                break;

            case Swi::EEventRemovingFiles:
            case Swi::EEventCopyingFiles:
            case Swi::EEventShuttingDownApps:
            default:
                break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplaySecurityWarningL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplaySecurityWarningL( const Swi::CAppInfo& aAppInfo,
        Swi::TSignatureValidationResult aSigValidationResult,
        RPointerArray<CPKIXValidationResultBase>& aPkixResults,
        RPointerArray<Swi::CCertificateInfo>& aCertificates,
        TBool aInstallAnyway )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplaySecurityWarningL") );
    TBool result = EFalse;

    if( iIsDriveSelectionRequired )
        {
        AddMemorySelectionL();
        }
    AddCertificatesL( aCertificates, aPkixResults );

    switch( aSigValidationResult )
        {
        case Swi::EValidationSucceeded:
            result = ShowConfirmationL( aAppInfo );
            break;

        case Swi::ESignatureSelfSigned:
        case Swi::ENoCertificate:
        case Swi::ECertificateValidationError:
        case Swi::ESignatureNotPresent:
        case Swi::ESignatureCouldNotBeValidated:
        case Swi::ENoCodeSigningExtension:
        case Swi::ENoSupportedPolicyExtension:
        case Swi::EMandatorySignatureMissing:
            if( aInstallAnyway )
                {
                result = ShowConfirmationL( aAppInfo );
                }
            break;

        default:
            break;
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayOcspResultL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayOcspResultL( const Swi::CAppInfo& /*aAppInfo*/,
        Swi::TRevocationDialogMessage aMessage, RPointerArray<TOCSPOutcome>& /*aOutcomes*/,
        RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/, TBool aWarningOnly )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayOcspResultL") );
    TBool okToContinue = EFalse;

    if( aWarningOnly && !IsOcspMandatoryL() )
        {
        // TODO: localised UI string needed, see R_SISXUI_OCSP_SECURITY_WARNING
        _LIT( KText, "Installation security warning. Unable to verify supplier. Continue anyway?" );
        if( ShowQuestionL( KText ) )
            {
            okToContinue = ETrue;
            }
        }

    if( !okToContinue )
        {
        SetOcspErrorL( aMessage );
        }

    return okToContinue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayCannotOverwriteFileL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::DisplayCannotOverwriteFileL( const Swi::CAppInfo& /*aAppInfo*/,
        const Swi::CAppInfo& /*aInstalledAppInfo*/, const TDesC& /*aFileName*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayCannotOverwriteFileL") );
    // TODO: implement
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayMissingDependencyL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayMissingDependencyL( const Swi::CAppInfo& /*aAppInfo*/,
        const TDesC& /*aDependencyName*/, TVersion /*aWantedVersionFrom*/,
        TVersion /*aWantedVersionTo*/, TVersion /*aInstalledVersion*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayMissingDependencyL") );
    // TODO: implement
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayUninstallL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::DisplayUninstallL( const Swi::CAppInfo& /*aAppInfo*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayUninstallL") );
    ASSERT( iOperationPhase == EUninstalling );
    return ETrue;       // uninstall is always silent
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayPreparingInstallL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::DisplayPreparingInstallL( const TDesC& /*aFileName*/ )
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayPreparingInstallL") );
    // TODO: display preparing install
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayCompleteL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::DisplayCompleteL()
    {
    FLOG( _L("CSisxSifPluginUiHandler::DisplayCompleteL") );
    iSifUi->ShowCompleteL();
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::DisplayFailedL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::DisplayFailedL( const CSisxSifPluginErrorHandler& aError )
    {
    FLOG_1( _L("CSisxSifPluginUiHandler::DisplayFailedL, error code %d"), aError.ErrorCode() );
    CSifUiErrorInfo* errorInfo = CSifUiErrorInfo::NewLC( aError.ErrorCategory(),
        aError.ErrorCode(), aError.ExtendedErrorCode(), aError.ErrorMessage(),
        aError.ErrorMessageDetails() );
    iSifUi->ShowFailedL( *errorInfo );
    CleanupStack::PopAndDestroy( errorInfo );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::CancelDialogs()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::CancelDialogs()
    {
    iSifUi->CancelDialogs();
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::CSisxSifPluginUiHandler()
// ---------------------------------------------------------------------------
//
CSisxSifPluginUiHandler::CSisxSifPluginUiHandler( RFs& aFs,
        CSisxSifPluginErrorHandler& aErrorHandler ) :
        CSisxSifPluginUiHandlerBase( aFs, aErrorHandler )
    {
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::ConstructL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::ConstructL()
    {
    iSifUi = CSifUi::NewL();
    iSelectionCache = CSisxSifUiSelectionCache::NewL();
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::AddMemorySelectionL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::AddMemorySelectionL()
    {
    TInt64 adjustedSize = iMaxInstalledSize + KFreeSpaceTreshold;

    TDriveList driveList;
    TInt driveCount = 0;
    TInt err = DriveInfo::GetUserVisibleDrives( iFs, driveList, driveCount );
    User::LeaveIfError( err );

    iSelectableDrives.Reset();
    TInt driveListLength = driveList.Length();
    for( TInt driveNumber = 0; driveNumber < driveListLength; driveNumber++ )
        {
        if( driveList[ driveNumber ] )
            {
            TUint driveStatus = 0;
            err = DriveInfo::GetDriveStatus( iFs, driveNumber, driveStatus );
            if( !err && !( driveStatus & DriveInfo::EDriveRemote ) )
                {
                TVolumeInfo volumeInfo;
                err = iFs.Volume( volumeInfo, driveNumber );
                if( !err && volumeInfo.iFree > adjustedSize )
                    {
                    iSelectableDrives.Append( driveNumber );
                    }
                }
            }
        }

    iSifUi->SetMemorySelectionL( iSelectableDrives );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::AddCertificatesL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::AddCertificatesL(
        RPointerArray<Swi::CCertificateInfo>& aCertificates,
        RPointerArray<CPKIXValidationResultBase>& aPkixResults )
    {
    ASSERT( aCertificates.Count() == aPkixResults.Count() );
    RPointerArray<CSifUiCertificateInfo> certificates;
    CleanupResetAndDestroyPushL( certificates );

    for( TInt index = 0; index < aCertificates.Count(); ++index )
        {
        TValidationStatus status = aPkixResults[ index ]->Error();
        if( status.iReason == EValidatedOK )
            {
            CSifUiCertificateInfo* cert = CSifUiCertificateInfo::NewLC( *aCertificates[ index ] );
            certificates.AppendL( cert );
            CleanupStack::Pop( cert );
            }
        }
    iSifUi->SetCertificateInfoL( certificates );

    CleanupStack::PopAndDestroy( &certificates );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::GetAppInfoLC()
// ---------------------------------------------------------------------------
//
CSifUiAppInfo* CSisxSifPluginUiHandler::GetAppInfoLC( const Swi::CAppInfo& aAppInfo )
    {
    CSifUiAppInfo *appInfo = CSifUiAppInfo::NewLC( aAppInfo.AppName(), aAppInfo.AppVendor(),
            aAppInfo.AppVersion(), iMaxInstalledSize, iLogo );
    return appInfo;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::ShowProgressL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandler::ShowProgressL( const Swi::CAppInfo& aAppInfo,
        TInt aProgressBarFinalValue, CSifUi::TInstallingPhase aPhase )
    {
    CSifUiAppInfo *appInfo = GetAppInfoLC( aAppInfo );
    iSifUi->ShowProgressL( *appInfo, aProgressBarFinalValue, aPhase );
    CleanupStack::PopAndDestroy( appInfo );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandler::ShowConfirmationL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandler::ShowConfirmationL( const Swi::CAppInfo& aAppInfo )
    {
    CSifUiAppInfo *appInfo = GetAppInfoLC( aAppInfo );
    TBool result = iSifUi->ShowConfirmationL( *appInfo );
    CleanupStack::PopAndDestroy( appInfo );
    return result;
    }
