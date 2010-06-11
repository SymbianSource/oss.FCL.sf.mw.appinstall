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

#include "sisxsifpluginactiveimpl.h"        // CSisxSifPluginActiveImpl
#include "sisxsifpluginuihandlerbase.h"     // CSisxSifPluginUiHandlerBase
#include "sisxsifpluginuihandler.h"         // CSisxSifPluginUiHandler
#include "sisxsifpluginuihandlersilent.h"   // CSisxSifPluginUiHandlerSilent
#include "sisxsifplugininstallparams.h"     // CSisxSifPluginInstallParams
#include "sisxsifcleanuputils.h"            // CleanupResetAndDestroyPushL
#include "sisxsifplugin.pan"                // Panic codes
#include <usif/sif/sifcommon.h>             // Usif::CComponentInfo
#include <usif/scr/scr.h>                   // RSoftwareComponentRegistry
#include <usif/usiferror.h>                 // SIF error codes
#include <swi/sisinstallerrors.h>           // SWI error codes
#include <swi/asynclauncher.h>              // Swi::CAsyncLauncher
#include <swi/sisregistrysession.h>         // RSisRegistrySession
#include "sisregistrywritablesession.h"     // RSisRegistryWritableSession
#include <e32property.h>                    // RProperty
#include <sacls.h>                          // KUidSwiLatestInstallation
#include <featmgr.h>                        // FeatureManager

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
    iInputParams = NULL;    // not owned
    iOutputParams = NULL;   // not owned
    delete iInstallParams;
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
    FLOG_3( _L("CSisxSifPluginActiveImpl::RunL(), operation %d, phase %d, result %d"),
            iOperation, iPhase, iStatus.Int() );

    User::LeaveIfError( iStatus.Int() );
    switch( iOperation )
        {
        case EGetComponentInfo:
            __ASSERT_DEBUG( iPhase == ERunningOperation, Panic( ESisxSifInternalError ) );
            CompleteClientRequest( KErrNone );
            break;

        case EInstall:
            switch( iPhase )
                {
                case EPreprocessing:
                    if( IsSilentMode() )
                        {
                        StartSilentInstallingL();
                        }
                    else
                        {
                        StartInstallingL();
                        }
                    break;

                case ERunningOperation:
                    FinalizeInstallationL();
                    CompleteClientRequest( KErrNone );
                    break;

				// TODO: KSifInParam_InstallInactive

                default:
                    Panic( ESisxSifInternalError );
                    break;
                }
            break;

        case EUninstall:
            __ASSERT_DEBUG( iPhase == ERunningOperation, Panic( ESisxSifInternalError ) );
            UpdateStartupListL();
            CompleteClientRequest( KErrNone );
            break;

        case EActivate:
        case EDeactivate:
            __ASSERT_DEBUG( iPhase == ERunningOperation, Panic( ESisxSifInternalError ) );
            CompleteClientRequest( KErrNone );
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

    if( IsActive() )
        {
        FLOG( _L("CSisxSifPluginActiveImpl::GetComponentInfo, KErrInUse") );
        CompleteClientRequest( KErrInUse );
        return;
        }

    TRAPD( error, SetFileL( aFileName ) );
    if( error )
        {
        FLOG_1( _L("CSisxSifPluginActiveImpl::GetComponentInfo, set file error %d"), error );
        CompleteClientRequest( error );
        return;
        }

    TRAP( error, DoGetComponentInfoL( aComponentInfo, aStatus ) );
	if( error )
		{
		FLOG_1( _L("CSisxSifPluginActiveImpl::GetComponentInfo ERROR %d"), error );
		CompleteClientRequest( error );
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

    if( IsActive() )
        {
        FLOG( _L("CSisxSifPluginActiveImpl::GetComponentInfo, KErrInUse") );
        CompleteClientRequest( KErrInUse );
        return;
        }

    SetFile( aFileHandle );

	TRAPD( error, DoGetComponentInfoL( aComponentInfo, aStatus ) );
	if( error )
		{
		FLOG_1( _L("CSisxSifPluginActiveImpl::GetComponentInfo ERROR %d"), error );
		CompleteClientRequest( error );
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

    if( IsActive() )
        {
		FLOG( _L("CSisxSifPluginActiveImpl::Install, KErrInUse") );
		CompleteClientRequest( KErrInUse );
		return;
		}

    iHasAllFilesCapability = aSecurityContext.HasCapability( ECapabilityAllFiles );

	TRAPD( error, SetFileL( aFileName ) );
	if( error )
		{
		FLOG_1( _L("CSisxSifPluginActiveImpl::Install, set file error %d"), error );
		CompleteClientRequest( error );
		return;
		}

	TRAP( error, DoInstallL( aSecurityContext, aInputParams, aOutputParams, aStatus ) );
	if( error )
		{
		FLOG_1( _L("CSisxSifPluginActiveImpl::Install, DoInstallL error %d"), error );
		CompleteClientRequest( error );
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
    FLOG( _L("CSisxSifPluginActiveImpl::Install(RFile)") );

    if( IsActive() )
        {
		FLOG( _L("CSisxSifPluginActiveImpl::Install KErrInUse") );
		CompleteClientRequest( KErrInUse );
		return;
		}

    iHasAllFilesCapability = aSecurityContext.HasCapability( ECapabilityAllFiles );

    SetFile( aFileHandle );

    TRAPD( error, DoInstallL( aSecurityContext, aInputParams, aOutputParams, aStatus ) );
    if( error )
        {
        FLOG_1( _L("CSisxSifPluginActiveImpl::Install, DoInstallL error %d"), error );
        CompleteClientRequest( error );
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

    if( IsActive() )
        {
        FLOG( _L("CSisxSifPluginActiveImpl::Uninstall KErrInUse") );
        CompleteClientRequest( KErrInUse );
        return;
        }

    iHasAllFilesCapability = aSecurityContext.HasCapability( ECapabilityAllFiles );

    // Uninstall is always silent. TrustedUI capability is required.
	if( !aSecurityContext.HasCapability( ECapabilityTrustedUI ) )
		{
		FLOG( _L( "CSisxSifPluginActiveImpl::Uninstall, missing ECapabilityTrustedUI") );
		CompleteClientRequest( KErrPermissionDenied );
		return;
		}

	TRAPD( error, DoUninstallL( aComponentId, aInputParams, aOutputParams, aStatus ) );
	if( error )
		{
		FLOG_1( _L("CSisxSifPluginActiveImpl::Uninstall, DoUninstallL ERROR %d"), error );
		CompleteClientRequest( error );
		}
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::Activate()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::Activate(
        TComponentId aComponentId,
        const TSecurityContext& aSecurityContext,
        TRequestStatus& aStatus )
    {
    FLOG_1( _L("CSisxSifPluginActiveImpl::Activate, aComponentId %d"), aComponentId );

    if( IsActive() )
        {
        FLOG( _L("CSisxSifPluginActiveImpl::Activate KErrInUse") );
        CompleteClientRequest( KErrInUse );
        return;
        }

    iHasAllFilesCapability = aSecurityContext.HasCapability( ECapabilityAllFiles );

    TRAPD( error, DoActivateL( aComponentId, aStatus ) );
    if( error )
        {
        FLOG_1( _L("CSisxSifPluginActiveImpl::Activate, DoActivateL ERROR %d"), error );
        CompleteClientRequest( error );
        return;
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::Deactivate()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::Deactivate(
        TComponentId aComponentId,
        const TSecurityContext& aSecurityContext,
        TRequestStatus& aStatus )
    {
    FLOG_1( _L("CSisxSifPluginActiveImpl::Deactivate, aComponentId %d"), aComponentId );

    if( IsActive() )
        {
        FLOG( _L("CSisxSifPluginActiveImpl::Deactivate KErrInUse") );
        CompleteClientRequest( KErrInUse );
        return;
        }

    iHasAllFilesCapability = aSecurityContext.HasCapability( ECapabilityAllFiles );

    TRAPD( error, DoDeactivateL( aComponentId, aStatus ) );
    if( error )
        {
        FLOG_1( _L("CSisxSifPluginActiveImpl::Deactivate, DoDeactivateL ERROR %d"), error );
        CompleteClientRequest( error );
        return;
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
// CSisxSifPluginActiveImpl::CommonRequestPreambleL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::CommonRequestPreambleL( TRequestStatus& aStatus )
    {
    aStatus = KRequestPending;
    iClientStatus = &aStatus;

    if( iInstallParams )
        {
        delete iInstallParams;
        iInstallParams = NULL;
        }
    CreateUiHandlerL();

    iInputParams = NULL;
    iOutputParams = NULL;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::CommonRequestPreambleL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::CommonRequestPreambleL(
        const COpaqueNamedParams& aInputParams,
        COpaqueNamedParams& aOutputParams,
        TRequestStatus& aStatus )
    {
    aStatus = KRequestPending;
    iClientStatus = &aStatus;

    if( iInstallParams )
        {
        delete iInstallParams;
        iInstallParams = NULL;
        }
    iInstallParams = CSisxSifPluginInstallParams::NewL( aInputParams );
    CreateUiHandlerL();

    iInputParams = &aInputParams;
    iOutputParams = &aOutputParams;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::CompleteSelf()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::CompleteSelf( TInt aResult )
    {
    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, aResult );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::CreateUiHandlerL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::CreateUiHandlerL()
    {
    if( iUiHandler )
    	{
		delete iUiHandler;
		iUiHandler = NULL;
    	}
	if( IsSilentMode() )
		{
		iUiHandler = CSisxSifPluginUiHandlerSilent::NewL( iFs );
		}
	else
		{
		iUiHandler = CSisxSifPluginUiHandler::NewL( iFs );
		}
	if( iInstallParams )
		{
		iUiHandler->SetInstallParamsL( *iInstallParams );
		}
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::IsSilentMode()
// ---------------------------------------------------------------------------
//
TBool CSisxSifPluginActiveImpl::IsSilentMode()
	{
	return( iInstallParams && iInstallParams->IsSilentMode() );
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
    __ASSERT_DEBUG( !IsActive(), Panic( ESisxSifInternalError ) );
    iOperation = ENoOperation;
    iPhase = ENotActive;
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::DoGetComponentInfoL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::DoGetComponentInfoL( CComponentInfo& aComponentInfo,
		TRequestStatus& aStatus )
	{
	CommonRequestPreambleL( aStatus );

	if( iFileName )
		{
		iAsyncLauncher->GetComponentInfoL( *iUiHandler, *iFileName, *iInstallPrefs,
				aComponentInfo, iStatus );
		}
	else if( iFileHandle )
		{
		iAsyncLauncher->GetComponentInfoL( *iUiHandler, *iFileHandle, *iInstallPrefs,
				aComponentInfo, iStatus );
		}
	else
		{
		User::Leave( KErrGeneral );
		}

    iOperation = EGetComponentInfo;
    iPhase = ERunningOperation;
    SetActive();
	}

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::DoInstallL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::DoInstallL( const TSecurityContext& aSecurityContext,
		const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
		TRequestStatus& aStatus )
	{
    CommonRequestPreambleL( aInputParams, aOutputParams, aStatus );
    FLOG_1( _L("CSisxSifPluginActiveImpl::DoInstall, IsSilentMode=%d"), IsSilentMode() );

    if( IsSilentMode() )
        {
        if( !aSecurityContext.HasCapability( ECapabilityTrustedUI ) )
            {
            FLOG( _L("CSisxSifPluginActiveImpl::Install, missing ECapabilityTrustedUI") );
            CompleteClientRequest( KErrPermissionDenied );
            return;
            }
        }

    if( iFileName )
        {
        iUiHandler->DisplayPreparingInstallL( *iFileName );
        iAsyncLauncher->GetComponentInfoL( *iUiHandler, *iFileName, *iInstallPrefs,
                *iComponentInfo, iStatus );
        }
    else if( iFileHandle )
        {
        TFileName fileName;
        iFileHandle->Name( fileName );
        iUiHandler->DisplayPreparingInstallL( fileName );
        iAsyncLauncher->GetComponentInfoL( *iUiHandler, *iFileHandle, *iInstallPrefs,
                *iComponentInfo, iStatus );
        }
    else
        {
        User::Leave( KErrGeneral );
        }

    iOperation = EInstall;
    iPhase = EPreprocessing;
    SetActive();
	}

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::DoUninstallL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::DoUninstallL( TComponentId aComponentId,
		const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
		TRequestStatus& aStatus )
    {
    CommonRequestPreambleL( aInputParams, aOutputParams, aStatus );

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

    iOperation = EUninstall;
    iPhase = ERunningOperation;
    SetActive();
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::DoActivateL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::DoActivateL( TComponentId aComponentId,
        TRequestStatus& aStatus )
    {
    CommonRequestPreambleL( aStatus );

    Swi::RSisRegistryWritableSession sisRegSession;
    User::LeaveIfError( sisRegSession.Connect() );
    CleanupClosePushL( sisRegSession );
    sisRegSession.ActivateComponentL( aComponentId );
    CleanupStack::PopAndDestroy( &sisRegSession );

    CompleteSelf( KErrNone );

    iOperation = EActivate;
    iPhase = ERunningOperation;
    SetActive();
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::DoDeactivateL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::DoDeactivateL( TComponentId aComponentId,
        TRequestStatus& aStatus )
    {
    CommonRequestPreambleL( aStatus );

    Swi::RSisRegistryWritableSession sisRegSession;
    User::LeaveIfError( sisRegSession.Connect() );
    CleanupClosePushL( sisRegSession );
    sisRegSession.DeactivateComponentL( aComponentId );
    CleanupStack::PopAndDestroy( &sisRegSession );

    CompleteSelf( KErrNone );

    iOperation = EDeactivate;
    iPhase = ERunningOperation;
    SetActive();
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::DoHandleErrorL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::DoHandleErrorL( TInt aError )
    {
    FLOG_1( _L("CSisxSifPluginActiveImpl::DoHandleErrorL(), aError=%d"), aError );

    TErrorCategory category = ErrorCategory( aError );
    if( iOutputParams )
        {
        iOutputParams->AddIntL( KSifOutParam_ErrCode, aError );
        iOutputParams->AddIntL( KSifOutParam_ExtendedErrCode, aError );
        iOutputParams->AddIntL( KSifOutParam_ErrCategory, category );
        // TODO: how to get error message and detailed error message?
        // iOutputParams->AddStringL( KSifOutParam_ErrMessage, TBD );
        // iOutputParams->AddStringL( KSifOutParam_ErrMessageDetails, TBD );
        }

    if( aError != KErrNone && aError != KErrCancel )
        {
        // TODO: proper error messages
        iUiHandler->DisplayFailedL( category, aError, KNullDesC, KNullDesC );
        }

    // TODO: proper error messages
    iUiHandler->PublishCompletionL( category, aError, KNullDesC, KNullDesC  );
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::ErrorCategory()
// ---------------------------------------------------------------------------
//
TErrorCategory CSisxSifPluginActiveImpl::ErrorCategory( TInt aErrorCode )
    {
    switch( aErrorCode )
        {
        // System-wide error codes
        case KErrNone:
            return ENone;
        case KErrNotFound:
        case KErrGeneral:
            return EUnexpectedError;
        case KErrCancel:
            return EUserCancelled;
        case KErrNoMemory:
            return ELowMemory;
        case KErrNotSupported:
        case KErrArgument:
        case KErrTotalLossOfPrecision:
        case KErrBadHandle:
        case KErrOverflow:
        case KErrUnderflow:
        case KErrAlreadyExists:
        case KErrPathNotFound:
        case KErrDied:
            return EUnexpectedError;
        case KErrInUse:
            return EInstallerBusy;
        case KErrServerTerminated:
        case KErrServerBusy:
        case KErrCompletion:
        case KErrNotReady:
        case KErrUnknown:
            return EUnexpectedError;
        case KErrCorrupt:
            return ECorruptedPackage;
        case KErrAccessDenied:
            return ESecurityError;
        case KErrLocked:
        case KErrWrite:
        case KErrDisMounted:
        case KErrEof:
            return EUnexpectedError;
        case KErrDiskFull:
            return ELowDiskSpace;
        case KErrBadDriver:
        case KErrBadName:
        case KErrCommsLineFail:
        case KErrCommsFrame:
        case KErrCommsOverrun:
        case KErrCommsParity:
        case KErrTimedOut:
        case KErrCouldNotConnect:
        case KErrCouldNotDisconnect:
        case KErrDisconnected:
        case KErrBadLibraryEntryPoint:
        case KErrBadDescriptor:
        case KErrAbort:
        case KErrTooBig:
        case KErrDivideByZero:
        case KErrBadPower:
        case KErrDirFull:
        case KErrHardwareNotAvailable:
        case KErrSessionClosed:
            return EUnexpectedError;
        case KErrPermissionDenied:
            return ESecurityError;
        case KErrExtensionNotSupported:
        case KErrCommsBreak:
        case KErrNoSecureTime:
            return EUnexpectedError;

        // Native SW Installer error codes
        case KErrSISFieldIdMissing:
        case KErrSISFieldLengthMissing:
        case KErrSISFieldLengthInvalid:
        case KErrSISStringInvalidLength:
        case KErrSISSignedControllerSISControllerMissing:
        case KErrSISControllerSISInfoMissing:
        case KErrSISInfoSISUidMissing:
        case KErrSISInfoSISNamesMissing:
            return ECorruptedPackage;
        case KErrSISFieldBufferTooShort:
            return EUnexpectedError;
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
        case KErrSISPrerequisitesMissingDependency:
        case KErrSISControllerMissingPrerequisites:
        case KErrSISUpgradeRangeMissingVersion:
        case KErrSISUnexpectedFieldType:
        case KErrSISExpressionUnknownOperator:
        case KErrSISArrayReadError:
        case KErrSISArrayTypeMismatch:
        case KErrSISInvalidStringLength:
        case KErrSISCompressionNotSupported:
        case KErrSISTooDeeplyEmbedded:
            return ECorruptedPackage;
        case KErrSISInvalidTargetFile:
        case KErrSISWouldOverWrite:
            return ESecurityError;
        case KErrSISInfoMissingRemoveDirectories:
            return ECorruptedPackage;
        case KErrSISNotEnoughSpaceToInstall:
            return ELowDiskSpace;
        case KErrInstallerLeave:
        case KErrPolicyFileCorrupt:
            return EUnexpectedError;
        case KErrSignatureSchemeNotSupported:
        case KErrDigestNotSupported:
            return EApplicationNotCompatible;
        case KErrBadHash:
            return ECorruptedPackage;
        case KErrSecurityError:
            return ESecurityError;
        case KErrBadUsage:
        case KErrInvalidType:
        case KErrInvalidExpression:
        case KErrExpressionToComplex:
            return EUnexpectedError;
        case KErrMissingBasePackage:
        case KErrInvalidUpgrade:
            return EApplicationNotCompatible;
        case KErrInvalidEclipsing:
            return ESecurityError;
        case KErrWrongHeaderFormat:
            return EUnexpectedError;
        case KErrCapabilitiesMismatch:
            return ESecurityError;
        case KErrLegacySisFile:
        case KErrInvalidSoftwareTypeRegistrationFile:
            return EApplicationNotCompatible;

        // Other error codes
        default:
            __ASSERT_DEBUG( EFalse, Panic( ESisxSifUnknownErrorCode ) );
            return EUnexpectedError;
        }
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::SetFileL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::SetFileL( const TDesC& aFileName )
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
// CSisxSifPluginActiveImpl::SetFile()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::SetFile( RFile& aFileHandle )
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
    __ASSERT_DEBUG( iOperation == EInstall, Panic( ESisxSifInternalError ) );

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
	const CComponentInfo::CNode& rootNode( iComponentInfo->RootNodeL() );
	TBool driveSelection = rootNode.DriveSeletionRequired();
	iUiHandler->SetDriveSelectionRequired( driveSelection );
	TInt maxInstalledSize = rootNode.MaxInstalledSize();
	iUiHandler->SetMaxInstalledSize( maxInstalledSize );

	iUiHandler->PublishStartL( rootNode );

    if( iFileHandle )
        {
        iAsyncLauncher->InstallL( *iUiHandler, *iFileHandle, *iInstallPrefs, iStatus );
        }
    else if( iFileName )
        {
        iAsyncLauncher->InstallL( *iUiHandler, *iFileName, *iInstallPrefs, iStatus );
        }
    else
        {
        Panic( ESisxSifInternalError );
        }

    iPhase = ERunningOperation;
    SetActive();
    }

// ---------------------------------------------------------------------------
// CSisxSifPluginActiveImpl::StartSilentInstallingL()
// ---------------------------------------------------------------------------
//
void CSisxSifPluginActiveImpl::StartSilentInstallingL()
    {
    const CComponentInfo::CNode& rootNode( iComponentInfo->RootNodeL() );
    TBool isAuthenticated = ( rootNode.Authenticity() == EAuthenticated );

    // AllowUntrusted option is needed to install untrusted packages.
    if( !isAuthenticated && !iInstallParams->AllowUntrusted() )
        {
        FLOG( _L("Attempt to install unsigned package silently without AllowUntrusted option") );
        CompleteClientRequest( KErrPermissionDenied );
        }
    // GrantCapabilities option is needed to install packages that require user capabilities
    else if( RequiresUserCapabilityL( rootNode ) && !iInstallParams->GrantCapabilities() )
        {
        FLOG( _L("Attempt to grant user capabilities silently without GrantCapabilities option") );
        CompleteClientRequest( KErrPermissionDenied );
        }
    // AllFiles capability is needed to install untrusted packages that contains exe/dll binaries
    else if( !isAuthenticated && rootNode.HasExecutable() && !iHasAllFilesCapability )
        {
        FLOG( _L("Attempt to install untrusted binaries silently without AllFiles capability") );
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

	iUiHandler->DisplayCompleteL();
    iUiHandler->PublishCompletionL( ENone, KErrNone, KNullDesC, KNullDesC );
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

