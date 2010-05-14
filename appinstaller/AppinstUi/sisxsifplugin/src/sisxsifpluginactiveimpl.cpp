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
#include <featmgr.h>                    // FeatureManager

// TODO: replace with proper logging
#ifdef _DEBUG
#define FLOG(x)         RDebug::Print(x);
#define FLOG_1(x,y)     RDebug::Print(x, y);
#define FLOG_2(x,y,z)   RDebug::Print(x, y, z);
#define FLOG_3(x,y,z,v) RDebug::Print(x, y, z, v);
#else
#define FLOG(x)
#define FLOG_1(x,y)
#define FLOG_2(x,y,z)
#define FLOG_3(x,y,z,v)
#endif

using namespace Usif;

_LIT( KCompUid, "CompUid" );
_LIT( KStartupListUpdaterExecutable, "z:\\sys\\bin\\startuplistupdater.exe" );
_LIT( KStartupListUpdaterName, "StartupListUpdater" );

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
    FeatureManager::UnInitializeLib();
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
    FLOG_3( _L("CSisxSifPluginActiveImpl::RunL(), operation %d, phase %d, result %d"),
            iOperation, iPhase, result );
    User::LeaveIfError( result );

    switch( iOperation )
        {
        case EInstall:
            switch( iPhase )
                {
                case EPreprocessing:
                    if( iUseSilentMode )
                        {
                        StartSilentInstallingL();
                        }
                    else
                        {
                        StartInstallingL();
                        }
                    iPhase = ERunningOperation;
                    SetActive();
                    break;

                case ERunningOperation:
                    FinalizeInstallationL();
                    CompleteClientRequest( KErrNone );
                    iPhase = ENotActive;
                    break;

                default:
                    Panic( ESisxSifInternalError );
                    break;
                }
            break;

        case EUninstall:
            __ASSERT_DEBUG( iPhase == ERunningOperation, Panic( ESisxSifInternalError ) );
            UpdateStartupListL();
            CompleteClientRequest( KErrNone );
            iPhase = ENotActive;
            break;

        case EActivate:
        case EDeactivate:
            __ASSERT_DEBUG( iPhase == ERunningOperation, Panic( ESisxSifInternalError ) );
            CompleteClientRequest( KErrNone );
            iPhase = ENotActive;
            break;

        default:
            Panic( ESisxSifInternalError );
            break;
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::RunError()
// ---------------------------------------------------------------------------
//
TInt CSisxSifPluginActiveImpl::RunError( TInt aError )
    {
    FLOG_1( _L("CSisxSifPluginActiveImpl::RunError(), aError %d"), aError );
    TRAP_IGNORE( DoHandleErrorL( aError ) );
    CompleteClientRequest( aError );
    iPhase = ENotActive;
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
    FLOG_1( _L("CSisxSifPluginActiveImpl::GetComponentInfo: %S"), &aFileName );
    if( !IsActive() )
        {
        iOperation = EGetComponentInfo;
        aStatus = KRequestPending;
        iClientStatus = &aStatus;
        iPhase = ERunningOperation;

        TRAPD( err, iAsyncLauncher->GetComponentInfoL( UiHandlerL(), aFileName,
                *iInstallPrefs, aComponentInfo, iStatus ) );
        if( err )
            {
            FLOG_1( _L("CSisxSifPluginActiveImpl::GetComponentInfo ERROR %d"), err );
            CompleteClientRequest( err );
            return;
            }

        SetActive();
        }
    else
        {
        FLOG( _L("CSisxSifPluginActiveImpl::GetComponentInfo KErrInUse") );
        CompleteClientRequest( KErrInUse );
        }
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
    FLOG( _L("CSisxSifPluginActiveImpl::GetComponentInfo(RFile)") );
    if( !IsActive() )
        {
        iOperation = EGetComponentInfo;
        aStatus = KRequestPending;
        iClientStatus = &aStatus;
        iPhase = ERunningOperation;

        TRAPD( err, iAsyncLauncher->GetComponentInfoL( UiHandlerL(), aFileHandle,
                *iInstallPrefs, aComponentInfo, iStatus ) );
        if( err )
            {
            FLOG_1( _L("CSisxSifPluginActiveImpl::GetComponentInfo ERROR %d"), err );
            CompleteClientRequest( err );
            return;
            }

        SetActive();
        }
    else
        {
        FLOG( _L("CSisxSifPluginActiveImpl::GetComponentInfo KErrInUse") );
        CompleteClientRequest( KErrInUse );
        }
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
    FLOG_1( _L("CSisxSifPluginActiveImpl::Install: %S"), &aFileName );
    if( !IsActive() )
        {
        CommonRequestPreamble( aInputParams, aOutputParams, aStatus );
        FLOG_1( _L("CSisxSifPluginActiveImpl::Install, iUseSilentMode=%d"), iUseSilentMode );

        if( iUseSilentMode )
            {
            if( !aSecurityContext.HasCapability( ECapabilityTrustedUI ) )
                {
                FLOG( _L("CSisxSifPluginActiveImpl::Install, missing ECapabilityTrustedUI") );
                CompleteClientRequest( KErrPermissionDenied );
                return;
                }
            }
        else
            {
            iUiHandler->DisplayPreparingInstallL( aFileName );
            }
        SetInstallFileL( aFileName );

        TRAPD( err, iAsyncLauncher->GetComponentInfoL( UiHandlerL( iUseSilentMode ),
                aFileName, *iInstallPrefs, *iComponentInfo, iStatus ) );
        if( err )
            {
            FLOG_1( _L("CSisxSifPluginActiveImpl::Install, GetComponentInfoL ERROR %d"), err );
            CompleteClientRequest( err );
            return;
            }

        iOperation = EInstall;
        iPhase = EPreprocessing;
        SetActive();
        }
    else
        {
        FLOG( _L("CSisxSifPluginActiveImpl::GetComponentInfo, KErrInUse") );
        CompleteClientRequest( KErrInUse );
        }
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
    TFileName fileName;
    aFileHandle.Name( fileName );
    FLOG_1( _L("CSisxSifPluginActiveImpl::Install(RFile): %S"), &fileName );
    if( !IsActive() )
        {
        CommonRequestPreamble( aInputParams, aOutputParams, aStatus );
        FLOG_1( _L("CSisxSifPluginActiveImpl::Install, iUseSilentMode=%d"), iUseSilentMode );

        if( iUseSilentMode )
            {
            if( !aSecurityContext.HasCapability( ECapabilityTrustedUI ) )
                {
                FLOG( _L("CSisxSifPluginActiveImpl::Install, missing ECapabilityTrustedUI") );
                CompleteClientRequest( KErrPermissionDenied );
                return;
                }
            }
        else
            {
            iUiHandler->DisplayPreparingInstallL( fileName );
            }
        SetInstallFile( aFileHandle );

        TRAPD( err, iAsyncLauncher->GetComponentInfoL( UiHandlerL( iUseSilentMode ),
                aFileHandle, *iInstallPrefs, *iComponentInfo, iStatus ) );
        if( err )
            {
            FLOG_1( _L("CSisxSifPluginActiveImpl::Install, GetComponentInfoL ERROR %d"), err );
            CompleteClientRequest( err );
            return;
            }

        iOperation = EInstall;
        iPhase = EPreprocessing;
        SetActive();
        }
    else
        {
        FLOG( _L("CSisxSifPluginActiveImpl::Install KErrInUse") );
        CompleteClientRequest( KErrInUse );
        }
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
    FLOG_1( _L("CSisxSifPluginActiveImpl::Uninstall, aComponentId %d"), aComponentId );
    if( !IsActive() )
        {
        CommonRequestPreamble( aInputParams, aOutputParams, aStatus );

        // Uninstall is always silent. TrustedUI capability is required.
        if( !aSecurityContext.HasCapability( ECapabilityTrustedUI ) )
            {
            FLOG( _L( "CSisxSifPluginActiveImpl::Uninstall, missing ECapabilityTrustedUI") );
            CompleteClientRequest( KErrPermissionDenied );
            return;
            }
        iUseSilentMode = ETrue;     // no complete/error notes launched in RunL

        TRAPD( err, DoUninstallL( aComponentId ) );
        if( err )
            {
            FLOG_1( _L("CSisxSifPluginActiveImpl::Uninstall, DoUninstallL ERROR %d"), err );
            CompleteClientRequest( err );
            return;
            }

        iOperation = EUninstall;
        iPhase = ERunningOperation;
        SetActive();
        }
    else
        {
        FLOG( _L("CSisxSifPluginActiveImpl::Uninstall KErrInUse") );
        CompleteClientRequest( KErrInUse );
        }
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
    FLOG_1( _L("CSisxSifPluginActiveImpl::Activate, aComponentId %d"), aComponentId );
    if( !IsActive() )
        {
        aStatus = KRequestPending;
        iClientStatus = &aStatus;

        TRAPD( err, DoActivateL( aComponentId ) );
        if( err )
            {
            FLOG_1( _L("CSisxSifPluginActiveImpl::Activate, DoActivateL ERROR %d"), err );
            CompleteClientRequest( err );
            return;
            }

        iStatus = KRequestPending;
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );

        iOperation = EActivate;
        iPhase = ERunningOperation;
        SetActive();
        }
    else
        {
        FLOG( _L("CSisxSifPluginActiveImpl::Activate KErrInUse") );
        CompleteClientRequest( KErrInUse );
        }
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
    FLOG_1( _L("CSisxSifPluginActiveImpl::Deactivate, aComponentId %d"), aComponentId );
    if( !IsActive() )
        {
        aStatus = KRequestPending;
        iClientStatus = &aStatus;

        TRAPD( err, DoDeactivateL( aComponentId ) );
        if( err )
            {
            FLOG_1( _L("CSisxSifPluginActiveImpl::Deactivate, DoDeactivateL ERROR %d"), err );
            CompleteClientRequest( err );
            return;
            }

        iStatus = KRequestPending;
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );

        iOperation = EDeactivate;
        iPhase = ERunningOperation;
        SetActive();
        }
    else
        {
        FLOG( _L("CSisxSifPluginActiveImpl::Deactivate KErrInUse") );
        CompleteClientRequest( KErrInUse );
        }
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
    FeatureManager::InitializeLibL();

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
    CleanupClosePushL( sisRegSession );
    sisRegSession.ActivateComponentL( aComponentId );
    CleanupStack::PopAndDestroy( &sisRegSession );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::DoDeactivateL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::DoDeactivateL( TComponentId aComponentId )
    {
    Swi::RSisRegistryWritableSession sisRegSession;
    User::LeaveIfError( sisRegSession.Connect() );
    CleanupClosePushL( sisRegSession );
    sisRegSession.DeactivateComponentL( aComponentId );
    CleanupStack::PopAndDestroy( &sisRegSession );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::DoHandleErrorL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::DoHandleErrorL( TInt aError )
    {
    if( iOutputParams )
        {
        iOutputParams->AddIntL( KSifOutParam_ExtendedErrCode, aError );
        }
    TInt errorCode = ConvertToSifErrorCode( aError );
    if( !iUseSilentMode && ( errorCode != KErrCancel ) )
        {
        iUiHandler->DisplayFailedL( errorCode );
        }
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
// CSisxSifPluginActiveImpl::SetInstallFileL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::SetInstallFileL( const TDesC& aFileName )
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
// CSisxSifPluginActiveImpl::SetInstallFile()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::SetInstallFile( RFile& aFileHandle )
    {
    if( iFileName )
        {
        delete iFileName;
        iFileName = NULL;
        }
    iFileHandle = &aFileHandle;
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
TBool CSisxSifPluginActiveImpl::RequiresUserCapabilityL(
        const CComponentInfo::CNode& aRootNode )
    {
    // Silent install is not allowed when the package requires additional capabilities
    // than what it is signed for (package may request for some capability that is not
    // granted by the certificate used to sign it).
    const TCapabilitySet& componentUserCaps( aRootNode.UserGrantableCaps() );
    for( TInt cap = 0; cap < ECapability_Limit; cap++ )
        {
        if( componentUserCaps.HasCapability( TCapability( cap ) ) )
            {
            FLOG_1( _L("CSisxSifPluginActiveImpl::RequiresUserCapabilityL - %d"), cap );
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::StartInstallingL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::StartInstallingL()
    {
    Swi::MUiHandler& uiHandler = UiHandlerL( iUseSilentMode );

    if( !iUseSilentMode && iUiHandler )
        {
        const CComponentInfo::CNode& rootNode( iComponentInfo->RootNodeL() );
        TBool driveSelection = rootNode.DriveSeletionRequired();
        iUiHandler->SetDriveSelectionRequired( driveSelection );
        TInt maxInstalledSize = rootNode.MaxInstalledSize();
        iUiHandler->SetMaxInstalledSize( maxInstalledSize );
        }

    if( iFileHandle )
        {
        iAsyncLauncher->InstallL( uiHandler, *iFileHandle, *iInstallPrefs, iStatus );
        }
    else if( iFileName )
        {
        iAsyncLauncher->InstallL( uiHandler, *iFileName, *iInstallPrefs, iStatus );
        }
    else
        {
        Panic( ESisxSifInternalError );
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::StartSilentInstallingL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::StartSilentInstallingL()
    {
    const CComponentInfo::CNode& rootNode( iComponentInfo->RootNodeL() );
    TBool hasExecutable = rootNode.HasExecutable();
    TBool isAuthenticated = ( rootNode.Authenticity() == EAuthenticated );
    TBool requiresUserCapability = RequiresUserCapabilityL( rootNode );
    if( hasExecutable && !isAuthenticated )
        {
        FLOG( _L("Silent install is not allowed on unsigned packages containing executables") );
        CompleteClientRequest( KErrPermissionDenied );
        }
    else if( requiresUserCapability )
        {
        FLOG( _L("Silent install is not allowed when user capabilities are required") );
        CompleteClientRequest( KErrPermissionDenied );
        }
    else
        {
        StartInstallingL();
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::FinalizeInstallationL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::FinalizeInstallationL()
    {
    UpdateStartupListL();

    if( iOutputParams )
        {
        TComponentId componentId = GetLastInstalledComponentIdL();
        iOutputParams->AddIntL( KSifOutParam_ComponentId, componentId );
        }
    if( !iUseSilentMode )
        {
        iUiHandler->DisplayCompleteL();
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::UpdateStartupListL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::UpdateStartupListL()
    {
    if( FeatureManager::FeatureSupported( KFeatureIdExtendedStartup ) )
        {
        TFullName name( KStartupListUpdaterName );
        name.Append( '*' );
        TFindProcess findProcess( name );
        if( findProcess.Next( name ) == KErrNone )
            {
            // already running, no need to do anything
            return;
            }

        RProcess process;
        CleanupClosePushL( process );

        TInt result = process.Create( KStartupListUpdaterExecutable, KNullDesC );
        if( result == KErrNone )
            {
            TRequestStatus rendezvousStatus;
            process.Rendezvous( rendezvousStatus );

            // start process and wait until it is started
            process.Resume();
            User::WaitForRequest( rendezvousStatus );

            // ignore possible errors
            result = rendezvousStatus.Int();
            }

        CleanupStack::PopAndDestroy( &process );
        }
    }

