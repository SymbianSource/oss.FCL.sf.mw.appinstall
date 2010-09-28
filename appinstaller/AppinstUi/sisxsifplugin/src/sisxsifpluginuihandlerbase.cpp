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
* Description:  Base class for SISX SIF plugin UI handlers.
*
*/

#include "sisxsifpluginuihandlerbase.h"     // CSisxSifPluginUiHandlerBase
#include "sisxsifplugininstallparams.h"     // CSisxSifPluginInstallParams
#include "sisxsifpluginerrorhandler.h"      // CSisxSifPluginErrorHandler
#include "sisxsifcleanuputils.h"            // CleanupResetAndDestroyPushL
#include <centralrepository.h>              // CRepository
#include <SWInstallerInternalCRKeys.h>      // KCRUidSWInstallerSettings
#include <hb/hbwidgets/hbdevicemessageboxsymbian.h> // CHbDeviceMessageBoxSymbian
// TODO: restore
//#include <hb/hbcore/hbtextresolversymbian.h> // HbTextResolverSymbian
#include <usif/scr/screntries.h>            // CComponentEntry

using namespace Usif;

_LIT( KTextResolverPath, "z:/resource/qt/translations/" );
_LIT( KSifUiTranslationFile, "sifuidevicedialogplugin_" );
_LIT( KCommonButtonContinue, "txt_common_button_continue" );

_LIT( KMemoryFull, "txt_error_info_memory_full" );
_LIT( KUnexpectedError, "txt_error_info_an_unexpected_error_occurred" );
_LIT( KNotEnoughSpace, "txt_error_info_there_is_not_enough_space_currently" );
_LIT( KNetworkUnavailable, "txt_error_info_network_is_unavailable_currently" );
_LIT( KInstallerBusy , "txt_error_info_installer_is_busy_currently" );
_LIT( KCorruptedPackage , "txt_error_info_installation_package_is_corrupted" );
_LIT( KNotCompatible , "txt_error_info_application_is_not_compatible_with" );
_LIT( KSecurityIssue , "txt_error_info_there_is_a_security_issue_with_this" );
_LIT( KNotInstalled , "txt_error_info_application_not_installed" );
_LIT( KNotDeleted , "txt_error_info_application_not_deleted" );
_LIT( KCannotBeDeleted , "txt_error_info_application_cannot_be_deleted" );

const TInt KDefaultTimeout = 6000;  // milliseconds


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::CSisxSifPluginUiHandlerBase()
// ---------------------------------------------------------------------------
//
CSisxSifPluginUiHandlerBase::CSisxSifPluginUiHandlerBase( RFs& aFs,
        CSisxSifPluginErrorHandler& aErrorHandler ) : iFs( aFs ),
        iErrorHandler( aErrorHandler )
    {
    // TODO: restore
    //HbTextResolverSymbian::Init( KSifUiTranslationFile, KTextResolverPath );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::~CSisxSifPluginUiHandlerBase()
// ---------------------------------------------------------------------------
//
CSisxSifPluginUiHandlerBase::~CSisxSifPluginUiHandlerBase()
    {
    delete iInstallParams;
    delete iPublishSifOperationInfo;
    delete iGlobalComponentId;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::SetInstallParamsL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::SetInstallParamsL(
        const CSisxSifPluginInstallParams& aInstallParams )
    {
    if( iInstallParams )
        {
        delete iInstallParams;
        iInstallParams = NULL;
        }
    iInstallParams = CSisxSifPluginInstallParams::NewL( aInstallParams );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::SetMaxInstalledSize()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::SetMaxInstalledSize( TInt aSize )
    {
    iMaxInstalledSize = aSize;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::SetDriveSelectionRequired()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::SetDriveSelectionRequired( TBool aIsRequired )
    {
    iIsDriveSelectionRequired = aIsRequired;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::IsOcspMandatoryL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerBase::IsOcspMandatoryL() const
    {
    CRepository* cenRep = CRepository::NewLC( KCRUidSWInstallerSettings );
    TInt ocspProcedure = ESWInstallerOcspProcedureOff;
    User::LeaveIfError( cenRep->Get( KSWInstallerOcspProcedure, ocspProcedure ) );
    CleanupStack::PopAndDestroy( cenRep );
    return ( ocspProcedure == ESWInstallerOcspProcedureMust );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::PublishStartL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::PublishStartL( const CComponentInfo::CNode& aRootNode,
        TSifOperationPhase aPhase )
    {
    iOperationPhase = aPhase;

    RPointerArray<HBufC> appNames;
    CleanupResetAndDestroyPushL( appNames );
    RPointerArray<HBufC> appIcons;
    CleanupResetAndDestroyPushL( appIcons );

    const RPointerArray<CComponentInfo::CApplicationInfo>& apps = aRootNode.Applications();
    for( TInt index = 0; index < apps.Count(); ++index )
        {
        HBufC* name = apps[ index ]->Name().AllocLC();
        appNames.AppendL( name );
        CleanupStack::Pop( name );
        HBufC* icon = apps[ index ]->IconFileName().AllocLC();
        appIcons.AppendL( icon );
        CleanupStack::Pop( icon );
        }

    if( iGlobalComponentId )
        {
        delete iGlobalComponentId;
        iGlobalComponentId = NULL;
        }
    iGlobalComponentId = aRootNode.GlobalComponentId().AllocL();

    CSifOperationStartData* data = CSifOperationStartData::NewLC( *iGlobalComponentId,
            aRootNode.ComponentName(), appNames, appIcons, aRootNode.MaxInstalledSize(),
            KNullDesC, KNullDesC, aRootNode.SoftwareTypeName(), iOperationPhase );

    if( !iPublishSifOperationInfo )
        {
        iPublishSifOperationInfo = CPublishSifOperationInfo::NewL();
        }
    iPublishSifOperationInfo->PublishStartL( *data );

    CleanupStack::PopAndDestroy( 3, &appNames );    // data, appIcons, appNames
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::PublishStartL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::PublishStartL( const CComponentEntry& aEntry,
        TSifOperationPhase aPhase )
    {
    iOperationPhase = aPhase;

    RPointerArray<HBufC> appNames;
    CleanupResetAndDestroyPushL( appNames );
    RPointerArray<HBufC> appIcons;
    CleanupResetAndDestroyPushL( appIcons );

    if( iGlobalComponentId )
        {
        delete iGlobalComponentId;
        iGlobalComponentId = NULL;
        }
    iGlobalComponentId = aEntry.GlobalId().AllocL();

    CSifOperationStartData* data = CSifOperationStartData::NewLC( *iGlobalComponentId,
            aEntry.Name(), appNames, appIcons, aEntry.ComponentSize(),
            KNullDesC, KNullDesC, aEntry.SoftwareType(), iOperationPhase );

    if( !iPublishSifOperationInfo )
        {
        iPublishSifOperationInfo = CPublishSifOperationInfo::NewL();
        }
    iPublishSifOperationInfo->PublishStartL( *data );

    CleanupStack::PopAndDestroy( 3, &appNames );    // data, appIcons, appNames
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::PublishProgressL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::PublishProgressL( TSifOperationSubPhase aSubPhase )
    {
    User::LeaveIfNull( iPublishSifOperationInfo );
    CSifOperationProgressData* data = CSifOperationProgressData::NewLC( *iGlobalComponentId,
            iOperationPhase, aSubPhase, iProgressBarCurrentValue, iProgressBarFinalValue );
    iPublishSifOperationInfo->PublishProgressL( *data );
    CleanupStack::PopAndDestroy( data );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::PublishCompletionL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::PublishCompletionL()
    {
    User::LeaveIfNull( iPublishSifOperationInfo );
    CSifOperationEndData* data = CSifOperationEndData::NewLC( *iGlobalComponentId,
            iErrorHandler.ErrorCategory(), iErrorHandler.ErrorCode(),
            iErrorHandler.ErrorMessage(), iErrorHandler.ErrorMessageDetails() );
    iPublishSifOperationInfo->PublishCompletionL( *data );
    CleanupStack::PopAndDestroy( data );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::SetErrorL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::SetErrorL( TInt aErrorCode, TInt aExtErrorCode )
    {
    iErrorHandler.SetErrorCode( aErrorCode );
    iErrorHandler.SetExtendedErrorCode( aExtErrorCode );

    switch( iErrorHandler.ErrorCategory() )
        {
        case ELowMemory:
            SetLocalisedErrorMessageText( KMemoryFull );
            break;
        case ELowDiskSpace:
            SetLocalisedErrorMessageText( KNotEnoughSpace );
            break;
        case ENetworkUnavailable:
            SetLocalisedErrorMessageText( KNetworkUnavailable );
            break;
        case EInstallerBusy:
            SetLocalisedErrorMessageText( KInstallerBusy );
            break;
        case ECorruptedPackage:
            SetLocalisedErrorMessageText( KCorruptedPackage );
            break;
        case EApplicationNotCompatible:
            SetLocalisedErrorMessageText( KNotCompatible );
            break;
        case ESecurityError:
            SetLocalisedErrorMessageText( KSecurityIssue );
            break;
        case EUnexpectedError:
        case EUnknown:
            SetLocalisedErrorMessageText( KUnexpectedError );
            break;
        case EUserCancelled:
            if( iOperationPhase == EInstalling )
                {
                SetLocalisedErrorMessageText( KNotInstalled );
                }
            else
                {
                SetLocalisedErrorMessageText( KNotDeleted );
                }
            break;
        case EUninstallationBlocked:
            SetLocalisedErrorMessageText( KCannotBeDeleted );
            break;
        case ENone:
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::SetErrorL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::SetErrorL( TInt aErrorCode, TInt aExtErrorCode,
    const TDesC& aErrMsgDetails )
    {
    SetErrorL( aErrorCode, aExtErrorCode );
    iErrorHandler.SetErrorMessageDetails( aErrMsgDetails );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::SetErrorSwiErrorL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::SetErrorSwiErrorL( Swi::TErrorDialog aType,
        const TDesC& /*aParam*/ )
    {
    // TODO: localised detailed error messages
    // TODO: append aParam when message supports parameters
    TBuf<512> details;
    switch( aType )
        {
        case Swi::EUiAlreadyInRom:
            details.Copy(_L("EUiAlreadyInRom"));
            break;
        case Swi::EUiMissingDependency:
            details.Copy(_L("EUiMissingDependency"));
            break;
        case Swi::EUiRequireVer:
            details.Copy(_L("EUiRequireVer"));
            break;
        case Swi::EUiRequireVerOrGreater:
            details.Copy(_L("EUiRequireVerOrGreater"));
            break;
        case Swi::EUiFileCorrupt:
            details.Copy(_L("EUiFileCorrupt"));
            break;
        case Swi::EUiDiskNotPresent:
            details.Copy(_L("EUiDiskNotPresent"));
            break;
        case Swi::EUiCannotRead:
            details.Copy(_L("EUiCannotRead"));
            break;
        case Swi::EUiCannotDelete:
            details.Copy(_L("EUiCannotDelete"));
            break;
        case Swi::EUiInvalidFileName:
            details.Copy(_L("EUiInvalidFileName"));
            break;
        case Swi::EUiFileNotFound:
            details.Copy(_L("EUiFileNotFound"));
            break;
        case Swi::EUiInsufficientSpaceOnDrive:
            details.Copy(_L("EUiInsufficientSpaceOnDrive"));
            break;
        case Swi::EUiCapabilitiesCannotBeGranted:
            // aParam contains list of capability names
            details.Copy(_L("Cannot grant capabilities requested by the application."));
            break;
        case Swi::EUiUnknownFile:
            details.Copy(_L("EUiUnknownFile"));
            break;
        case Swi::EUiMissingBasePackage:
            details.Copy(_L("EUiMissingBasePackage"));
            break;
        case Swi::EUiConstraintsExceeded:
            details.Copy(_L("EUiConstraintsExceeded"));
            break;
        case Swi::EUiSIDViolation:
            details.Copy(_L("EUiSIDViolation"));
            break;
        case Swi::EUiVIDViolation:
            details.Copy(_L("EUiVIDViolation"));
            break;
        case Swi::EUiNoMemoryInDrive:
            details.Copy(_L("EUiNoMemoryInDrive"));
            break;
        case Swi::EUiUIDPackageViolation:
            details.Copy(_L("EUiUIDPackageViolation"));
            break;
        case Swi::EUiOSExeViolation:
            details.Copy(_L("EUiOSExeViolation"));
            break;
        case Swi::EUiSIDMismatch:
            details.Copy(_L("EUiSIDMismatch"));
            break;
        case Swi::EUiBlockingEclipsingFile:
            details.Copy(_L("EUiBlockingEclipsingFile"));
            break;
        default:
            break;
        }

    SetErrorL( KErrGeneral, aType, details );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::SetOcspErrorL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::SetOcspErrorL( Swi::TRevocationDialogMessage aMessage )
    {
    // TODO: localised error strings needed
    iErrorHandler.SetErrorMessage( _L("Unable to check certificate validity online." ) );
    iErrorHandler.SetExtendedErrorCode( aMessage );
    switch( aMessage )
        {
        case Swi::EInvalidRevocationServerUrl:
            iErrorHandler.SetErrorCode( KErrGeneral );
            iErrorHandler.SetErrorMessageDetails( _L("Invalid server URL. Check settings.") );
            break;
        case Swi::EUnableToObtainCertificateStatus:
            iErrorHandler.SetErrorCode( KErrGeneral );
            iErrorHandler.SetErrorMessageDetails( _L("Unable to obtain certificate status. Try again later.") );
            break;
        case Swi::EResponseSignatureValidationFailure:
            iErrorHandler.SetErrorCode( KErrGeneral );
            iErrorHandler.SetErrorMessageDetails( _L("Response signature validation failure. Check settings.") );
            break;
        case Swi::EInvalidRevocationServerResponse:
            iErrorHandler.SetErrorCode( KErrGeneral );
            iErrorHandler.SetErrorMessageDetails( _L("The OCSP server reply is invalid. Check settings.") );
            break;
        case Swi::EInvalidCertificateStatusInformation:
        case Swi::ECertificateStatusIsUnknownSelfSigned:
            iErrorHandler.SetErrorCode( KErrGeneral );
            iErrorHandler.SetErrorMessageDetails( _L("Invalid certificate status information. Try again later.") );
            break;
        case Swi::ECertificateStatusIsUnknown:
            iErrorHandler.SetErrorCode( KErrGeneral );
            iErrorHandler.SetErrorMessageDetails( _L("Unknown certificate. Try again later.") );
            break;
        case Swi::ECertificateStatusIsRevoked:
            iErrorHandler.SetErrorCode( KErrAccessDenied );
            iErrorHandler.SetErrorMessageDetails( _L("The certificate has been revoked.") );
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::ShowQuestionL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerBase::ShowQuestionL( const TDesC& aText ) const
    {
    TBool questionAccepted = EFalse;

    CHbDeviceMessageBoxSymbian *note = NULL;
    note = CHbDeviceMessageBoxSymbian::NewL( CHbDeviceMessageBoxSymbian::EQuestion );
    CleanupStack::PushL( note );

    note->SetTextL( aText );
    note->SetTimeout( KDefaultTimeout );
    if( note->ExecL() == CHbDeviceMessageBoxSymbian::EAcceptButton )
        {
        questionAccepted = ETrue;
        }

    CleanupStack::PopAndDestroy( note );
    return questionAccepted;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::ShowQuestionWithContinueL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::ShowQuestionWithContinueL( const TDesC& aText ) const
    {
    CHbDeviceMessageBoxSymbian *note = NULL;
    note = CHbDeviceMessageBoxSymbian::NewL( CHbDeviceMessageBoxSymbian::EQuestion );
    CleanupStack::PushL( note );

    note->SetTextL( aText );
    note->SetTimeout( KDefaultTimeout );
    note->SetButton( CHbDeviceMessageBoxSymbian::EAcceptButton, EFalse );
    note->SetButton( CHbDeviceMessageBoxSymbian::ERejectButton, ETrue );
    // TODO: restore
    //HBufC* contButton = HbTextResolverSymbian::LoadLC( KCommonButtonContinue );
    //note->SetButtonTextL( CHbDeviceMessageBoxSymbian::ERejectButton, contButton->Des() );
    note->SetButtonTextL( CHbDeviceMessageBoxSymbian::ERejectButton, _L("Continue") );
    (void)note->ExecL();
    //CleanupStack::PopAndDestroy( contButton );

    CleanupStack::PopAndDestroy( note );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::CompareDriveLetters()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginUiHandlerBase::CompareDriveLetters( const TChar& aDriveFirst,
        const TChar& aDriveSecond )
    {
    return( aDriveFirst.GetLowerCase() == aDriveSecond.GetLowerCase() );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::SetLocalisedErrorMessageText()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::SetLocalisedErrorMessageText( const TDesC& aLogicalName )
    {
    // TODO: restore
    //HBufC* errorText = HbTextResolverSymbian::LoadLC( aLogicalName );
    //iErrorHandler.SetErrorMessage( errorText->Des() );
    iErrorHandler.SetErrorMessage( aLogicalName );
    //CleanupStack::PopAndDestroy( errorText );
    }

