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
    if( iClientStatus )
        {
        if( iAsyncLauncher )
            {
            iAsyncLauncher->CancelOperation();
            delete iAsyncLauncher;
            iAsyncLauncher = NULL;
            }

        User::RequestComplete( iClientStatus, KErrCancel );
        iClientStatus = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::RunL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::RunL()
    {
    TInt result = iStatus.Int();
    FLOG_1( _L("CSisxSifPluginActiveImpl::RunL(), result %d"), result );

    if( iSilentInstall )
        {
        FLOG( _L("CSisxSifPluginActiveImpl::RunL, silent install") );
        ProcessSilentInstallL();
        }
    else
        {
        if( iOutputParams )
            {
            iOutputParams->AddIntL( KSifOutParam_ExtendedErrCode, result );

            if( iInstallRequest && result == KErrNone )
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
        if( !iSilentInstall )
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
    aStatus = KRequestPending;
    iClientStatus = &aStatus;

    TRAPD( err, iAsyncLauncher->GetComponentInfoL( *iUiHandler, aFileName,
            *iInstallPrefs, aComponentInfo, iStatus ) );
    FLOG_1( _L("CSisxSifPluginActiveImpl::GetComponentInfo, err = %d"), err );
    if( err != KErrNone )
        {
        CompleteRequest( aStatus, err );
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
    aStatus = KRequestPending;
    iClientStatus = &aStatus;

    TRAPD( err, iAsyncLauncher->GetComponentInfoL( *iUiHandler, aFileHandle,
            *iInstallPrefs, aComponentInfo, iStatus ) );
    FLOG_1( _L("CSisxSifPluginActiveImpl::GetComponentInfo, err = %d"), err );
    if( err != KErrNone )
        {
        CompleteRequest( aStatus, err );
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
    CommonRequestPreamble( aSecurityContext, aInputParams, aOutputParams, aStatus );

    FLOG_2( _L("CSisxSifPluginActiveImpl::Install: %S, iSilentInstall=%d"),
            &aFileName, iSilentInstall );

    TRAPD( err, DoInstallL( aFileName ) );
    FLOG_2( _L("CSisxSifPluginActiveImpl::Install, iInstallRequest=%d, err=%d"),
            iInstallRequest, err );
    if( err != KErrNone )
        {
        CompleteRequest( aStatus, err );
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
    CommonRequestPreamble( aSecurityContext, aInputParams, aOutputParams, aStatus );

    FLOG_1( _L("CSisxSifPluginActiveImpl::Install, iSilentInstall=%d"), iSilentInstall );

    TInt err;
    if( iSilentInstall )
        {
        // Silent install does a few addtional checks on the package to see if is
        // signed and had the required capabilities. So we need to the get the
        // package component information without installing it. Real silent install
        // operation is started in RunL() after this GetComponentInfoL() completes.
        SetSilentInstallFile( aFileHandle );
        TRAP( err, iAsyncLauncher->GetComponentInfoL( *iUiHandler, aFileHandle, *iInstallPrefs,
                *iComponentInfo, iStatus ) );
        }
    else
        {
        // Proceed with the normal installation.
        TRAP( err, iAsyncLauncher->InstallL( *iUiHandler, aFileHandle, *iInstallPrefs, iStatus ) );
        iInstallRequest = ETrue;
        }

    FLOG_2( _L("CSisxSifPluginActiveImpl::Install, iInstallRequest=%d, err=%d"),
            iInstallRequest, err );
    if( err != KErrNone )
        {
        CompleteRequest( aStatus, err );
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
    CommonRequestPreamble( aSecurityContext, aInputParams, aOutputParams, aStatus );

    TRAPD( err, DoUninstallL( aComponentId, aStatus ) );
    FLOG_1( _L("CSisxSifPluginActiveImpl::Uninstall, err=%d"), err );
    if( err != KErrNone )
        {
        CompleteRequest( aStatus, err );
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
    aStatus = KRequestPending;
    iClientStatus = &aStatus;

    TRAPD( err, DoActivateL( aComponentId ) );
    FLOG_2( _L("CSisxSifPluginActiveImpl::Activate, component %d, err=%d"), aComponentId, err );
    if( err != KErrNone )
        {
        CompleteRequest( aStatus, err );
        return;
        }

    iStatus = KRequestPending;
    CompleteRequest( iStatus, KErrNone );
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
    aStatus = KRequestPending;
    iClientStatus = &aStatus;

    Swi::RSisRegistryWritableSession sisRegSession;
    TRAPD( err, DoDeactivateL( aComponentId ) );
    FLOG_2( _L("CSisxSifPluginActiveImpl::Deactivate, component %d, err=%d"), aComponentId, err );
    if( err != KErrNone )
        {
        CompleteRequest( aStatus, err );
        return;
        }

    iStatus = KRequestPending;
    CompleteRequest( iStatus, KErrNone );
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

    iUiHandler = CSisxSifPluginUiHandler::NewL( iFs );
    iAsyncLauncher = Swi::CAsyncLauncher::NewL();
    iInstallPrefs = Swi::CInstallPrefs::NewL();
    iComponentInfo = CComponentInfo::NewL();
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::Complete()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::CompleteRequest( TRequestStatus& aStatus, TInt aResult )
    {
    TRequestStatus* statusPtr = &aStatus;
    User::RequestComplete( statusPtr, aResult );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::CommonRequestPreamble()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::CommonRequestPreamble(
        const TSecurityContext& aSecurityContext,
        const COpaqueNamedParams& aInputParams,
        COpaqueNamedParams& aOutputParams,
        TRequestStatus& aStatus )
    {
    aStatus = KRequestPending;
    iClientStatus = &aStatus;

    iInputParams = &aInputParams;
    iOutputParams = &aOutputParams;

    // Check InstallSilently opaque input argument
    TInt silentInstall = 0;
    TRAP_IGNORE( aInputParams.GetIntByNameL( KSifInParam_InstallSilently, silentInstall ) );
    if( silentInstall )
        {
        iSilentInstall = ETrue;
        if( !aSecurityContext.HasCapability( ECapabilityTrustedUI ) )
            {
            FLOG( _L("CSisxSifPluginActiveImpl: missing ECapabilityTrustedUI ERROR") );
            CompleteRequest( aStatus, KErrPermissionDenied );
            iClientStatus = NULL;
            }
        }

    // TODO: KSifInParam_InstallInactive
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::DoInstallL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::DoInstallL( const TDesC& aFileName )
    {
    if( iSilentInstall )
        {
        // Silent install does a few addtional checks on the package to see if is
        // signed and had the required capabilities. So we need to the get the
        // package component information without installing it. Real silent install
        // operation is started in RunL() after this GetComponentInfoL() completes.
        SetSilentInstallFileL( aFileName );
        iAsyncLauncher->GetComponentInfoL( *iUiHandler, aFileName, *iInstallPrefs,
                *iComponentInfo, iStatus );
        }
    else
        {
        // Proceed with the normal installation.
        iAsyncLauncher->InstallL( *iUiHandler, aFileName, *iInstallPrefs, iStatus );
        iInstallRequest = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::DoUninstallL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::DoUninstallL( TComponentId aComponentId, TRequestStatus& /*aStatus*/ )
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

    iAsyncLauncher->UninstallL( *iUiHandler, objectId, iStatus );
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
    ASSERT( iInstallRequest );

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
// CSisxSifPluginActiveImpl::NeedUserCapabilityL()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginActiveImpl::NeedUserCapabilityL()
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
    // We need to do this only once per installation request
    iSilentInstall = EFalse;
    iInstallRequest = ETrue;

    // TODO: should self-signed packages that do not contain executables be allowed?
    //TBool hasExecutable = iComponentInfo->RootNodeL().HasExecutable();

    TBool isNotAuthenticated = ( iComponentInfo->RootNodeL().Authenticity() == ENotAuthenticated );
    TBool reqUserCap = NeedUserCapabilityL();
    if( isNotAuthenticated || reqUserCap )
        {
        if( isNotAuthenticated )
            {
            FLOG( _L("Silent Install is not allowed on unsigned or self-signed packages") );
            }
        if( reqUserCap )
            {
            FLOG( _L("Silent Install is not allowed when user capabilities are required") );
            }
        User::RequestComplete( iClientStatus, KErrPermissionDenied );
        iClientStatus = NULL;
        }
    else
        {
        TInt err = KErrNone;
        if( iFileHandle )
            {
            TRAP( err, iAsyncLauncher->InstallL( *iUiHandler, *iFileHandle, *iInstallPrefs, iStatus ) );
            }
        else if( iFileName )
            {
            TRAP( err, iAsyncLauncher->InstallL( *iUiHandler, *iFileName, *iInstallPrefs, iStatus ) );
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
            User::RequestComplete( iClientStatus, err );
            iClientStatus = NULL;
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

