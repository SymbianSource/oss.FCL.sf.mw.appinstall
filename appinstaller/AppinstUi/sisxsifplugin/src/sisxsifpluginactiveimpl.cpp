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

#include "sisxsifpluginactiveimpl.h"    // CSisxSifPluginActiveImpl
#include "sisxsifpluginuihandler.h"     // CSisxSifPluginUiHandler
#include "sisxsifpluginuihandlersilent.h" // CSisxSifPluginUiHandlerSilent
#include "sisxsifcleanuputils.h"        // CleanupResetAndDestroyPushL
#include "sisxsifplugin.pan"            // Panic codes
#include <usif/sif/sifcommon.h>         // Usif::CComponentInfo
#include <usif/scr/scr.h>               // RSoftwareComponentRegistry
#include <usif/usiferror.h>             // SIF error codes
#include <swi/sisinstallerrors.h>       // SWI error codes
#include <swi/asynclauncher.h>          // Swi::CAsyncLauncher
#include <swi/sisregistrysession.h>     // RSisRegistrySession
#include "sisregistrywritablesession.h" // RSisRegistryWritableSession
#include <e32property.h>                // RProperty
#include <sacls.h>                      // KUidSwiLatestInstallation

// TODO: replace with proper logging
#ifdef _DEBUG
#define FLOG(x)         RDebug::Print(x);
#define FLOG_1(x,y)     RDebug::Print(x, y);
#define FLOG_2(x,y,z)   RDebug::Print(x, y, z);
#else
#define FLOG(x)
#define FLOG_1(x,y)
#define FLOG_2(x,y,z)
#endif

using namespace Usif;

_LIT( KCompUid, "CompUid" );

const TInt KSystemWideErrorsBoundary = -100;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::NewL()
// ---------------------------------------------------------------------------
//
CSisxSifPluginActiveImpl* CSisxSifPluginActiveImpl::NewL()
	{
	FLOG( _L("Constructing CSisxSifPluginActiveImpl") );
	CSisxSifPluginActiveImpl *self = new( ELeave ) CSisxSifPluginActiveImpl;
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::~CSisxSifPluginActiveImpl()
// ---------------------------------------------------------------------------
//
CSisxSifPluginActiveImpl::~CSisxSifPluginActiveImpl()
	{
	FLOG( _L("Destructing CSisxSifPluginActiveImpl") );
	Cancel();
    delete iAsyncLauncher;
    delete iUiHandler;
    delete iUiHandlerSilent;
    delete iInstallPrefs;
    delete iComponentInfo;
    delete iFileName;
    iFs.Close();
	}

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::DoCancel()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::DoCancel()
    {
    FLOG( _L("CSisxSifPluginActiveImpl::DoCancel") );

    if( iClientStatus )
        {
        if( iAsyncLauncher )
            {
            iAsyncLauncher->CancelOperation();
            delete iAsyncLauncher;
            iAsyncLauncher = NULL;
            }

        CompleteClientRequest( KErrCancel );
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::RunL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::RunL()
    {
    TInt result = iStatus.Int();
    FLOG_2( _L("CSisxSifPluginActiveImpl::RunL(), op %d, result %d"), iOperation, result );

    if( result == KErrNone && iOperation == EInstall &&
            iUseSilentMode && !iIsPackageCheckedForSilentInstall )
        {
        ProcessSilentInstallL();    // makes the real silent install request
        iIsPackageCheckedForSilentInstall = ETrue;
        }
    else
        {
        if( iOutputParams )
            {
            iOutputParams->AddIntL( KSifOutParam_ExtendedErrCode, result );

            if( iOperation == EInstall && result == KErrNone )
                {
                TComponentId resultComponentId = 0;
                TRAPD( getLastIdErr, resultComponentId = GetLastInstalledComponentIdL() );
                if( getLastIdErr == KErrNone )
                    {
                    iOutputParams->AddIntL( KSifOutParam_ComponentId, resultComponentId );
                    }
                }
            }

        TInt errorCode = ConvertToSifErrorCode( result );
        if( !iUseSilentMode )
            {
            if( errorCode == KErrNone )
                {
                iUiHandler->DisplayCompleteL();
                }
            else
                {
                if( errorCode != KErrCancel )
                    {
                    iUiHandler->DisplayFailedL( errorCode );
                    }
                }
            }
        User::RequestComplete( iClientStatus, errorCode );
        iClientStatus = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::RunError()
// ---------------------------------------------------------------------------
//
TInt CSisxSifPluginActiveImpl::RunError( TInt aError )
    {
    if( iClientStatus )
        {
        User::RequestComplete( iClientStatus, aError );
        iClientStatus = NULL;
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::GetComponentInfo()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::GetComponentInfo(
        const TDesC& aFileName,
        const TSecurityContext& /*aSecurityContext*/,
        CComponentInfo& aComponentInfo,
        TRequestStatus& aStatus )
	{
    iOperation = EGetComponentInfo;
    aStatus = KRequestPending;
    iClientStatus = &aStatus;

    TRAPD( err, iAsyncLauncher->GetComponentInfoL( UiHandlerL(), aFileName,
            *iInstallPrefs, aComponentInfo, iStatus ) );
    FLOG_1( _L("CSisxSifPluginActiveImpl::GetComponentInfo, err = %d"), err );
    if( err != KErrNone )
        {
        CompleteClientRequest( err );
        return;
        }

    SetActive();
	}

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::GetComponentInfo()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::GetComponentInfo(
        RFile& aFileHandle,
        const TSecurityContext& /*aSecurityContext*/,
        CComponentInfo& aComponentInfo,
        TRequestStatus& aStatus )
	{
    iOperation = EGetComponentInfo;
    aStatus = KRequestPending;
    iClientStatus = &aStatus;

    TRAPD( err, iAsyncLauncher->GetComponentInfoL( UiHandlerL(), aFileHandle,
            *iInstallPrefs, aComponentInfo, iStatus ) );
    FLOG_1( _L("CSisxSifPluginActiveImpl::GetComponentInfo, err = %d"), err );

    if( err != KErrNone )
        {
        CompleteClientRequest( err );
        return;
        }
    SetActive();
	}

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::Install()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::Install(
        const TDesC& aFileName,
        const TSecurityContext& aSecurityContext,
        const COpaqueNamedParams& aInputParams,
        COpaqueNamedParams& aOutputParams,
        TRequestStatus& aStatus )
	{
    iOperation = EInstall;
    CommonRequestPreamble( aInputParams, aOutputParams, aStatus );

    if( iUseSilentMode && !aSecurityContext.HasCapability( ECapabilityTrustedUI ) )
        {
        FLOG( _L("CSisxSifPluginActiveImpl: missing ECapabilityTrustedUI ERROR") );
        CompleteClientRequest( KErrPermissionDenied );
        return;
        }

    FLOG_2( _L("CSisxSifPluginActiveImpl::Install: %S, iUseSilentMode=%d"),
            &aFileName, iUseSilentMode );
    TRAPD( err, DoInstallL( aFileName ) );
    FLOG_1( _L("CSisxSifPluginActiveImpl::Install, err=%d"), err );

    if( err != KErrNone )
        {
        CompleteClientRequest( err );
        return;
        }
    SetActive();
	}

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::Install()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::Install(
        RFile& aFileHandle,
        const TSecurityContext& aSecurityContext,
        const COpaqueNamedParams& aInputParams,
        COpaqueNamedParams& aOutputParams,
        TRequestStatus& aStatus )
	{
    iOperation = EInstall;
    CommonRequestPreamble( aInputParams, aOutputParams, aStatus );

    if( iUseSilentMode && !aSecurityContext.HasCapability( ECapabilityTrustedUI ) )
        {
        FLOG( _L("CSisxSifPluginActiveImpl: missing ECapabilityTrustedUI ERROR") );
        CompleteClientRequest( KErrPermissionDenied );
        return;
        }

    FLOG_1( _L("CSisxSifPluginActiveImpl::Install, iUseSilentMode=%d"), iUseSilentMode );
    TInt err = KErrNone;
    if( iUseSilentMode )
        {
        // Silent install does a few addtional checks on the package to see if is
        // signed and had the required capabilities. So we need to the get the
        // package component information without installing it. Real silent install
        // operation is started in RunL() after this GetComponentInfoL() completes.
        SetSilentInstallFile( aFileHandle );
        TRAP( err, iAsyncLauncher->GetComponentInfoL( UiHandlerL( iUseSilentMode ),
                aFileHandle, *iInstallPrefs, *iComponentInfo, iStatus ) );
        FLOG_1( _L("CSisxSifPluginActiveImpl::GetComponentInfoL, err=%d"), err );
        }
    else
        {
        // Proceed with the normal installation.
        TRAP( err, iAsyncLauncher->InstallL( UiHandlerL(), aFileHandle,
                *iInstallPrefs, iStatus ) );
        FLOG_1( _L("CSisxSifPluginActiveImpl::Install, err=%d"), err );
        }

    if( err != KErrNone )
        {
        CompleteClientRequest( err );
        return;
        }
    SetActive();
	}

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::Uninstall()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::Uninstall(
        TComponentId aComponentId,
        const TSecurityContext& aSecurityContext,
        const COpaqueNamedParams& aInputParams,
        COpaqueNamedParams& aOutputParams,
        TRequestStatus& aStatus )
	{
    iOperation = EUninstall;
    CommonRequestPreamble( aInputParams, aOutputParams, aStatus );

    // Uninstall is always silent. TrustedUI capability is always required.
    if( !aSecurityContext.HasCapability( ECapabilityTrustedUI ) )
        {
        FLOG( _L( "CSisxSifPluginActiveImpl: missing ECapabilityTrustedUI ERROR") );
        CompleteClientRequest( KErrPermissionDenied );
        return;
        }
    iUseSilentMode = ETrue;     // no complete/error notes launched in RunL

    FLOG( _L("CSisxSifPluginActiveImpl::Uninstall") );
    TRAPD( err, DoUninstallL( aComponentId ) );
    FLOG_1( _L("CSisxSifPluginActiveImpl::Uninstall, err=%d"), err );

    if( err != KErrNone )
        {
        CompleteClientRequest( err );
        return;
        }
    SetActive();
	}

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::Activate()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::Activate(
        TComponentId aComponentId,
        const TSecurityContext& /*aSecurityContext*/,
        TRequestStatus& aStatus )
	{
    iOperation = EActivate;
    aStatus = KRequestPending;
    iClientStatus = &aStatus;

    TRAPD( err, DoActivateL( aComponentId ) );
    FLOG_2( _L("CSisxSifPluginActiveImpl::Activate, component %d, err=%d"), aComponentId, err );
    if( err != KErrNone )
        {
        CompleteClientRequest( err );
        return;
        }

    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
	}

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::Deactivate()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::Deactivate(
        TComponentId aComponentId,
        const TSecurityContext& /*aSecurityContext*/,
        TRequestStatus& aStatus )
	{
    iOperation = EDeactivate;
    aStatus = KRequestPending;
    iClientStatus = &aStatus;

    TRAPD( err, DoDeactivateL( aComponentId ) );
    FLOG_2( _L("CSisxSifPluginActiveImpl::Deactivate, component %d, err=%d"), aComponentId, err );
    if( err != KErrNone )
        {
        CompleteClientRequest( err );
        return;
        }

    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
	}

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::CSisxSifPluginActiveImpl()
// ---------------------------------------------------------------------------
//
CSisxSifPluginActiveImpl::CSisxSifPluginActiveImpl() : CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::ConstructL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );

    iAsyncLauncher = Swi::CAsyncLauncher::NewL();
    iInstallPrefs = Swi::CInstallPrefs::NewL();
    iComponentInfo = CComponentInfo::NewL();
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::UiHandlerL()
// ---------------------------------------------------------------------------
//
Swi::MUiHandler& CSisxSifPluginActiveImpl::UiHandlerL( TBool aUseSilentMode )
    {
    Swi::MUiHandler* handler = NULL;

    if( aUseSilentMode )
        {
        if( iUiHandler )
            {
            delete iUiHandler;
            iUiHandler = NULL;
            }
        if( !iUiHandlerSilent )
            {
            iUiHandlerSilent = CSisxSifPluginUiHandlerSilent::NewL( iFs );
            }
        handler = iUiHandlerSilent;
        }
    else
        {
        if( iUiHandlerSilent )
            {
            delete iUiHandlerSilent;
            iUiHandlerSilent = NULL;
            }
        if( !iUiHandler )
            {
            iUiHandler = CSisxSifPluginUiHandler::NewL( iFs );
            }
        handler = iUiHandler;
        }

    return *handler;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::CommonRequestPreamble()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::CommonRequestPreamble(
        const COpaqueNamedParams& aInputParams,
        COpaqueNamedParams& aOutputParams,
        TRequestStatus& aStatus )
    {
    aStatus = KRequestPending;
    iClientStatus = &aStatus;

    TInt silentInstall = 0;
    TRAPD( err, aInputParams.GetIntByNameL( KSifInParam_InstallSilently, silentInstall ) );
    iUseSilentMode = ( err == KErrNone && silentInstall != 0 );
    iIsPackageCheckedForSilentInstall = EFalse;

    iInputParams = &aInputParams;
    iOutputParams = &aOutputParams;

    // TODO: KSifInParam_InstallInactive
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::CompleteClientRequest()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::CompleteClientRequest( TInt aResult )
    {
    if( iClientStatus )
        {
        User::RequestComplete( iClientStatus, aResult );
        iClientStatus = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::DoInstallL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::DoInstallL( const TDesC& aFileName )
    {
    if( iUseSilentMode )
        {
        // Silent install does a few addtional checks on the package to see if is
        // signed and had the required capabilities. So we need to the get the
        // package component information without installing it. Real silent install
        // operation is started in RunL() after this GetComponentInfoL() completes.
        SetSilentInstallFileL( aFileName );
        iIsPackageCheckedForSilentInstall = EFalse;
        iAsyncLauncher->GetComponentInfoL( UiHandlerL( iUseSilentMode ), aFileName,
                *iInstallPrefs, *iComponentInfo, iStatus );
        }
    else
        {
        // Proceed with the normal installation.
        iAsyncLauncher->InstallL( UiHandlerL(), aFileName, *iInstallPrefs, iStatus );
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::DoUninstallL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::DoUninstallL( TComponentId aComponentId )
    {
    RSoftwareComponentRegistry scrSession;
    User::LeaveIfError( scrSession.Connect() );
    CleanupClosePushL( scrSession );

    CPropertyEntry* propertyEntry = scrSession.GetComponentPropertyL( aComponentId, KCompUid );
    CleanupStack::PushL( propertyEntry );
    CIntPropertyEntry* intPropertyEntry = dynamic_cast< CIntPropertyEntry* >( propertyEntry );
    FLOG_2( _L("CSisxSifPluginActiveImpl::DoUninstallL, component %d, property 0x%08x"),
            aComponentId, intPropertyEntry );
    if( !intPropertyEntry )
        {
        FLOG( _L("CSisxSifPluginActiveImpl: UID property not found ERROR") );
        User::Leave( KErrNotFound );
        }

    TUid objectId = TUid::Uid( intPropertyEntry->IntValue() );
    CleanupStack::PopAndDestroy( 2, &scrSession );      // propertyEntry, scrSession

    iAsyncLauncher->UninstallL( UiHandlerL( iUseSilentMode ), objectId, iStatus );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::DoActivateL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::DoActivateL( TComponentId aComponentId )
    {
    Swi::RSisRegistryWritableSession sisRegSession;
    User::LeaveIfError( sisRegSession.Connect() );
    sisRegSession.ActivateComponentL( aComponentId );
    sisRegSession.Close();
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::DoDeactivateL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::DoDeactivateL( TComponentId aComponentId )
    {
    Swi::RSisRegistryWritableSession sisRegSession;
    User::LeaveIfError( sisRegSession.Connect() );
    sisRegSession.DeactivateComponentL( aComponentId );
    sisRegSession.Close();
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::ConvertToSifErrorCode()
// ---------------------------------------------------------------------------
//
TInt CSisxSifPluginActiveImpl::ConvertToSifErrorCode( TInt aSwiErrorCode )
    {
    FLOG_1( _L("CSisxSifPluginActiveImpl::ConvertToSifErrorCode(), aSwiErrorCode=%d"),
            aSwiErrorCode );

    // TODO: need to show also SWI error code in UI somehow when necessary

    if( aSwiErrorCode > KSystemWideErrorsBoundary )
        {
        return aSwiErrorCode;
        }

    switch( aSwiErrorCode )
        {
        case KErrSISFieldIdMissing:
        case KErrSISFieldLengthMissing:
        case KErrSISFieldLengthInvalid:
        case KErrSISStringInvalidLength:
        case KErrSISSignedControllerSISControllerMissing:
        case KErrSISControllerSISInfoMissing:
        case KErrSISInfoSISUidMissing:
        case KErrSISInfoSISNamesMissing:
        case KErrSISFieldBufferTooShort:
        case KErrSISStringArrayInvalidElement:
        case KErrSISInfoSISVendorNamesMissing:
        case KErrSISInfoSISVersionMissing:
        case KErrSISControllerSISSupportedLanguagesMissing:
        case KErrSISSupportedLanguagesInvalidElement:
        case KErrSISLanguageInvalidLength:
        case KErrSISContentsSISSignedControllerMissing:
        case KErrSISContentsSISDataMissing:
        case KErrSISDataSISFileDataUnitMissing:
        case KErrSISFileDataUnitTargetMissing:
        case KErrSISFileOptionsMissing:
        case KErrSISFileDataUnitDescriptorMissing:
        case KErrSISFileDataDescriptionMissing:
        case KErrSISContentsMissing:
        case KErrSISEmbeddedControllersMissing:
        case KErrSISEmbeddedDataUnitsMissing:
        case KErrSISControllerOptionsMissing:
        case KErrSISExpressionMissing:
        case KErrSISExpressionStringValueMissing:
        case KErrSISOptionsStringMissing:
        case KErrSISFileOptionsExpressionMissing:
        case KErrSISExpressionHeadValueMissing:
        case KErrSISEmbeddedSISOptionsMissing:
        case KErrSISInfoSISUpgradeRangeMissing:
        case KErrSISDependencyMissingUid:
        case KErrSISDependencyMissingVersion:
        case KErrSISDependencyMissingNames:
        case KErrSISControllerMissingPrerequisites:
        case KErrSISUpgradeRangeMissingVersion:
        case KErrSISUnexpectedFieldType:
        case KErrSISExpressionUnknownOperator:
        case KErrSISArrayReadError:
        case KErrSISArrayTypeMismatch:
        case KErrSISInvalidStringLength:
        case KErrSISCompressionNotSupported:
        case KErrSISTooDeeplyEmbedded:
        case KErrWrongHeaderFormat:
        case KErrExpressionToComplex:
        case KErrInvalidExpression:
        case KErrInvalidType:
            return KErrSifCorruptedPackage;

        case KErrBadUsage:
        case KErrInstallerLeave:
            return KErrSifUnknown;

        case KErrSISPrerequisitesMissingDependency:
            return KErrSifMissingDependencies;

        case KErrMissingBasePackage:
            return KErrSifMissingBasePackage;

        case KErrCapabilitiesMismatch:
        case KErrInvalidEclipsing:
        case KErrSecurityError:
        case KErrBadHash:
        case KErrDigestNotSupported:
        case KErrSignatureSchemeNotSupported:
        case KErrSISWouldOverWrite:
        case KErrSISInvalidTargetFile:
            return KErrPermissionDenied;

        case KErrPolicyFileCorrupt:
            return KErrSifBadInstallerConfiguration;

        case KErrInvalidUpgrade:
        case KErrLegacySisFile:
            return KErrSifPackageCannotBeInstalledOnThisDevice;

        case KErrSISNotEnoughSpaceToInstall:
            return KErrSifNotEnoughSpace;

        default:
            return KErrSifUnknown;
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::GetLastInstalledComponentIdL()
// ---------------------------------------------------------------------------
//
TComponentId CSisxSifPluginActiveImpl::GetLastInstalledComponentIdL()
    {
    ASSERT( iOperation == EInstall );

    // Find the id of the last installed component and return it
    TInt uid;
    User::LeaveIfError( RProperty::Get( KUidSystemCategory, KUidSwiLatestInstallation, uid ) );

    Swi::RSisRegistrySession sisRegistrySession;
    User::LeaveIfError( sisRegistrySession.Connect() );
    CleanupClosePushL( sisRegistrySession );

    TUid tuid( TUid::Uid( uid ) );
    TComponentId componentId = sisRegistrySession.GetComponentIdForUidL( tuid );
    CleanupStack::PopAndDestroy( &sisRegistrySession );

    return componentId;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::RequiresUserCapabilityL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginActiveImpl::RequiresUserCapabilityL()
    {
    // Silent install is not allowed when the package requires additional capabilities
    // than what it is signed for (Package may request for some capability that is not
    // granted by the certificate used to sign it).
    const TCapabilitySet& componentUserCaps( iComponentInfo->RootNodeL().UserGrantableCaps() );
    for( TInt cap = 0; cap < ECapability_Limit; cap++ )
        {
        if( componentUserCaps.HasCapability( TCapability( cap ) ) )
            {
            FLOG_1( _L("Package requires additional capability - %d"), cap );
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::ProcessSilentInstallL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::ProcessSilentInstallL()
    {
    TBool isAuthenticated = ( iComponentInfo->RootNodeL().Authenticity() == EAuthenticated );
    TBool requiresUserCapability = RequiresUserCapabilityL();
    if( !isAuthenticated || requiresUserCapability )
        {
        if( !isAuthenticated )
            {
            FLOG( _L("Silent Install is not allowed on unsigned or self-signed packages") );
            }
        if( requiresUserCapability )
            {
            FLOG( _L("Silent Install is not allowed when user capabilities are required") );
            }
        CompleteClientRequest( KErrPermissionDenied );
        }
    else
        {
        TInt err = KErrNone;
        if( iFileHandle )
            {
            TRAP( err, iAsyncLauncher->InstallL( UiHandlerL( iUseSilentMode ), *iFileHandle,
                    *iInstallPrefs, iStatus ) );
            }
        else if( iFileName )
            {
            TRAP( err, iAsyncLauncher->InstallL( UiHandlerL( iUseSilentMode ), *iFileName,
                    *iInstallPrefs, iStatus ) );
            }
        else
            {
            Panic( ESisxSifInternalError );
            }

        if( !err )
            {
            SetActive();
            }
        else
            {
            CompleteClientRequest( err );
            }
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::SetSilentInstallFileL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::SetSilentInstallFileL( const TDesC& aFileName )
    {
    if( iFileName )
        {
        delete iFileName;
        iFileName = NULL;
        }
    iFileName = aFileName.AllocL();
    iFileHandle = NULL;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::SetSilentInstallFile()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::SetSilentInstallFile( RFile& aFileHandle )
    {
    if( iFileName )
        {
        delete iFileName;
        iFileName = NULL;
        }
    iFileHandle = &aFileHandle;
    }

