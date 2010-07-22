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
#include <usif/scr/screntries.h>            // CComponentEntry

using namespace Usif;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSisxSifPluginUiHandlerBase::CSisxSifPluginUiHandlerBase()
// ---------------------------------------------------------------------------
//
CSisxSifPluginUiHandlerBase::CSisxSifPluginUiHandlerBase( RFs& aFs,
        CSisxSifPluginErrorHandler& aErrorHandler ) : iFs( aFs ),
        iErrorHandler( aErrorHandler )
    {
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
void CSisxSifPluginUiHandlerBase::PublishStartL( const CComponentInfo::CNode& aRootNode )
    {
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
            KNullDesC, KNullDesC, aRootNode.SoftwareTypeName() );

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
void CSisxSifPluginUiHandlerBase::PublishStartL( const CComponentEntry& aEntry )
    {
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
            KNullDesC, KNullDesC, aEntry.SoftwareType() );

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
// CSisxSifPluginUiHandlerBase::SetDisplayErrorL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginUiHandlerBase::SetDisplayErrorL( Swi::TErrorDialog aType, const TDesC& aParam )
    {
    iErrorHandler.SetExtendedErrorCode( aType );

    // TODO: localised detailed error messages
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
            details.Copy(_L("EUiCapabilitiesCannotBeGranted"));
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

    if( aParam.Length() )
        {
        details.Append( _L("\n") );
        details.Append( aParam );
        }
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
    note->SetTimeout( 0 );
    if( note->ExecL() == CHbDeviceMessageBoxSymbian::EAcceptButton )
        {
        questionAccepted = ETrue;
        }

    CleanupStack::PopAndDestroy( note );
    return questionAccepted;
    }


