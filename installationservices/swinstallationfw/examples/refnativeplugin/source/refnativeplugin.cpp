/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "refnativeplugin.h"
#include "usiflog.h"
#include <usif/sif/sif.h>
#include <usif/sif/sifcommon.h>
#include <usif/usiferror.h>
#include <usif/sif/sifplugin.h>
#include <usif/scr/screntries.h>
#include <usif/scr/scr.h>
#include <swi/msisuihandlers.h>
#include <swi/asynclauncher.h>
#include <swi/sisinstallerrors.h>
#include <e32property.h>
#include <sacls.h>
#include "sisregistrywritablesession.h"
#include "sisregistrysession.h"

using namespace Usif;

static const TInt KRefNativePluginImpId = 0x10285BC4;

static const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KRefNativePluginImpId, CRefNativePlugin::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

CRefNativePlugin* CRefNativePlugin::NewL()
	{
	DEBUG_PRINTF(_L8("Constructing CRefNativePlugin"));
	CRefNativePlugin *self = new (ELeave) CRefNativePlugin();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CRefNativePlugin::ConstructL()
	{
	iImpl = CRefNativePluginActiveImpl::NewL();
	}

CRefNativePlugin::~CRefNativePlugin()
	{
	delete iImpl;
	}

void CRefNativePlugin::CancelOperation()
	{
	DEBUG_PRINTF(_L8("Reference native plugin - Cancel"));
	iImpl->Cancel();
	}

void CRefNativePlugin::GetComponentInfo(const TDesC& aFileName, const TSecurityContext& /*aSecurityContext*/,
										 CComponentInfo& aComponentInfo, TRequestStatus& aStatus)
	{
	iImpl->GetComponentInfo(aFileName, aComponentInfo, aStatus);
	}

void CRefNativePlugin::GetComponentInfo(RFile& aFileHandle, const TSecurityContext& /*aSecurityContext*/,
						 				CComponentInfo& aComponentInfo, TRequestStatus& aStatus)
	{
	iImpl->GetComponentInfo(aFileHandle, aComponentInfo, aStatus);
	}

void CRefNativePlugin::Install(const TDesC& aFileName, const TSecurityContext& aSecurityContext,
								const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
								TRequestStatus& aStatus)
	{
	iImpl->Install(aFileName, aSecurityContext, aInputParams, aOutputParams, aStatus);
	}

void CRefNativePlugin::Install(RFile& aFileHandle, const TSecurityContext& aSecurityContext,
								const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
								TRequestStatus& aStatus)
	{
	iImpl->Install(aFileHandle, aSecurityContext, aInputParams, aOutputParams, aStatus);
	}

void CRefNativePlugin::Uninstall(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
								const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams, TRequestStatus& aStatus)
	{
	iImpl->Uninstall(aComponentId, aSecurityContext, aInputParams, aOutputParams, aStatus);
	}

void CRefNativePlugin::Activate(TComponentId aComponentId, const TSecurityContext& aSecurityContext, TRequestStatus& aStatus)

	{
	DEBUG_PRINTF(_L8("Reference native plugin - Activate"));
	iImpl->Activate(aComponentId, aSecurityContext, aStatus);
	}

void CRefNativePlugin::Deactivate(TComponentId aComponentId, const TSecurityContext& aSecurityContext, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - Deactivate"));
	iImpl->Deactivate(aComponentId, aSecurityContext, aStatus);
	}

//------------------CRefNativePluginActiveImpl---------------------

CRefNativePluginActiveImpl* CRefNativePluginActiveImpl::NewL()
	{
	DEBUG_PRINTF(_L8("Reference native plugin - Constructing CRefNativePluginActiveImpl"));
	CRefNativePluginActiveImpl *self = new (ELeave) CRefNativePluginActiveImpl();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(1, self);
	return self;
	}

void CRefNativePluginActiveImpl::ConstructL()
	{
	iInstallPrefs = Swi::CInstallPrefs::NewL();
	iAsyncLauncher = Swi::CAsyncLauncher::NewL();
	iComponentInfo = CComponentInfo::NewL();
	CActiveScheduler::Add(this);
	}

CRefNativePluginActiveImpl::~CRefNativePluginActiveImpl()
	{
	delete iAsyncLauncher;
	delete iInstallPrefs;
	delete iComponentInfo;
	}

const TInt KSystemWideErrorsBoundary = -100;

TInt ConvertToSifErrorCode(TInt aSwiErrorCode)
	{
	if (aSwiErrorCode > KSystemWideErrorsBoundary)
		return aSwiErrorCode; 
	switch (aSwiErrorCode)
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

TBool CRefNativePluginActiveImpl::NeedUserCapabilityL()
	{
	// Silent install is not allowed when the package requires additional capabilities
	// than what it is signed for (Pakage may request for some capability that is not 
	// granted by the certificate used to sign it).
	for(TInt cap=0; cap<ECapability_Limit; ++cap)
		{
		if (iComponentInfo->RootNodeL().UserGrantableCaps().HasCapability(TCapability(cap)))
			{
			DEBUG_PRINTF2(_L("Package requires additional capability - %d"), cap);
			return ETrue;
			}
		}
			
	return EFalse;
	}

void CRefNativePluginActiveImpl::RunL()
	{
	if (iSilentInstall)
		{
		DEBUG_PRINTF(_L("Silent install  - CRefNativePluginActiveImpl::RunL"));
		ProcessSilentInstallL();
		}
	else
		{
		TInt res = iStatus.Int();
		DEBUG_PRINTF2(_L8("Reference native plugin - Operation finished with result %d"), res);
	
		// Output options
		if (iOutputParams != NULL)
			{
			iOutputParams->AddIntL(KSifOutParam_ExtendedErrCode, res);
			
			if (iInstallRequest && res == KErrNone)
				{
				TComponentId resultComponentId = 0;
				TRAPD(getLastIdErr, resultComponentId = GetLastInstalledComponentIdL());
				if (getLastIdErr == KErrNone)
					iOutputParams->AddIntL(KSifOutParam_ComponentId, resultComponentId);
				}
			}
	
		User::RequestComplete(iClientStatus, ConvertToSifErrorCode(res));
		iClientStatus = NULL;
		}
	}

void CRefNativePluginActiveImpl::ProcessSilentInstallL()
	{
	// We need to do this only once per installation request
	iSilentInstall = EFalse;
	iInstallRequest = ETrue;
	
	TBool isNotAuthenticated = (ENotAuthenticated == iComponentInfo->RootNodeL().Authenticity());
	TBool reqUserCap = NeedUserCapabilityL();
	if (isNotAuthenticated || reqUserCap)
		{
		if (isNotAuthenticated)
			{
				DEBUG_PRINTF(_L("Silent Install is not allowed on unsigned or self-signed packages"));
			}
		
		if (reqUserCap)
			{
				DEBUG_PRINTF(_L("Silent Install is not allowed when user capabilities are required"));
			}
		
		User::RequestComplete(iClientStatus, KErrNotSupported);
		iClientStatus = NULL;
		}		
	else
		{
		TInt err;
		if (iFileHandle)
			{
			TRAP(err, iAsyncLauncher->InstallL(*this, *iFileHandle, *iInstallPrefs, iStatus));
			}
		else
			{
			//DEBUG_PRINTF2(_L("!!!Silent install for %S"), iFileName);
			TRAP(err, iAsyncLauncher->InstallL(*this, iFileName, *iInstallPrefs, iStatus));
			}
		
		if (err != KErrNone)
			{
			User::RequestComplete(iClientStatus, err);
			iClientStatus = NULL;			
			}
		
		SetActive();
		}
	}

void CRefNativePluginActiveImpl::DoCancel()
	{
	if (iClientStatus)
		{
		iAsyncLauncher->CancelOperation();
		delete iAsyncLauncher;
		iAsyncLauncher = NULL;
		
		User::RequestComplete(iClientStatus, KErrCancel);
		iClientStatus = NULL;
		}
	}

void CRefNativePluginActiveImpl::CommonRequestPreamble(const TSecurityContext& aSecurityContext, const COpaqueNamedParams& aInputParams, 
				COpaqueNamedParams& aOutputParams, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iClientStatus = &aStatus;

	iInputParams = &aInputParams;
	iOutputParams = &aOutputParams;

	TInt declineOperation = 0;
	TRAPD(err, aInputParams.GetIntByNameL(KDeclineOperationOptionName, declineOperation));
	if(err == KErrNone && declineOperation)
		{
		iDeclineOperation = ETrue;
		}
	
	// Check to see if we have the opaque input argument - InstallSilently 
	TInt silentInstall = 0;
	TRAP_IGNORE(aInputParams.GetIntByNameL(KSifInParam_InstallSilently, silentInstall));
	if (silentInstall)
		{
		iSilentInstall = ETrue;
		if (!aSecurityContext.HasCapability(ECapabilityTrustedUI))
			{
			User::RequestComplete(iClientStatus, KErrPermissionDenied);
			iClientStatus = NULL;			
			}
		}	
	}

TComponentId CRefNativePluginActiveImpl::GetLastInstalledComponentIdL()
  	{
  	ASSERT(iInstallRequest);
  
  	// Find the id of the last installed component and return it
  	TInt uid;
  	User::LeaveIfError(RProperty::Get(KUidSystemCategory, KUidSwiLatestInstallation, uid));

	Swi::RSisRegistrySession sisRegistrySession;
	User::LeaveIfError(sisRegistrySession.Connect());
	CleanupClosePushL(sisRegistrySession);
  
	TUid tuid(TUid::Uid(uid));
	TComponentId componentId = sisRegistrySession.GetComponentIdForUidL(tuid);
	CleanupStack::PopAndDestroy(&sisRegistrySession);
	
	return componentId;
  	}

void CRefNativePluginActiveImpl::GetComponentInfo(const TDesC& aFileName, CComponentInfo& aComponentInfo, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF2(_L("Reference native plugin - retrieving component info for %S"), &aFileName);

	aStatus = KRequestPending;
	iClientStatus = &aStatus;

	TRAPD(err, iAsyncLauncher->GetComponentInfoL(*this, aFileName, *iInstallPrefs, aComponentInfo, iStatus));
	if (err != KErrNone)
		{
		TRequestStatus *statusPtr(&aStatus);
		User::RequestComplete(statusPtr, err);
		return;
		}

	SetActive();
	}

void CRefNativePluginActiveImpl::GetComponentInfo(RFile& aFileHandle, CComponentInfo& aComponentInfo, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - getting component info by file handle"));
	
	aStatus = KRequestPending;
	iClientStatus = &aStatus;	

	TRAPD(err, iAsyncLauncher->GetComponentInfoL(*this, aFileHandle, *iInstallPrefs, aComponentInfo, iStatus));
	if (err != KErrNone)
		{
		TRequestStatus *statusPtr(&aStatus);
		User::RequestComplete(statusPtr, err);
		return;
		}

	SetActive();
	}	

void CRefNativePluginActiveImpl::Install(const TDesC& aFileName, const TSecurityContext& aSecurityContext,
											const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
											TRequestStatus& aStatus)
	{
	DEBUG_PRINTF2(_L("Reference native plugin - install for %S"), &aFileName);
	
	CommonRequestPreamble(aSecurityContext, aInputParams, aOutputParams, aStatus);
	
	TInt err;
	if (iSilentInstall)
		{
		// Silent install does a few addtional checks on the package to see if is 
		// signed and had the required capabilities. So we need to the get the 
		// package component information with out installing it.
		DEBUG_PRINTF2(_L("Silent install  - Get the ComponentInfo for %S"), &aFileName);
		iFileName = aFileName;
		TRAP(err, iAsyncLauncher->GetComponentInfoL(*this, aFileName, *iInstallPrefs, *iComponentInfo, iStatus));
		}
	else
		{
		// Proceed with the normal installation.
		TRAP(err, iAsyncLauncher->InstallL(*this, aFileName, *iInstallPrefs, iStatus));
		iInstallRequest = ETrue;
		}
	
	if (err != KErrNone)
		{
		TRequestStatus *statusPtr(&aStatus);
		User::RequestComplete(statusPtr, err);
		return;
		}
	
	SetActive();
	}

void CRefNativePluginActiveImpl::Install(RFile& aFileHandle, const TSecurityContext& aSecurityContext,
											const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
											TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - install by file handle"));

	CommonRequestPreamble(aSecurityContext, aInputParams, aOutputParams, aStatus);

	TInt err;
	if (iSilentInstall)
		{
		// Silent install does a few addtional checks on the package to see if is 
		// signed and had the required capabilities. So we need to the get the 
		// package component information with out installing it.
		iFileHandle = &aFileHandle;
		TRAP(err, iAsyncLauncher->GetComponentInfoL(*this, aFileHandle, *iInstallPrefs, *iComponentInfo, iStatus));
		}
	else
		{
		// Proceed with the normal installation.
		TRAP(err, iAsyncLauncher->InstallL(*this, aFileHandle, *iInstallPrefs, iStatus));
		iInstallRequest = ETrue;
		}
	
	if (err != KErrNone)
		{
		TRequestStatus *statusPtr(&aStatus);
		User::RequestComplete(statusPtr, err);
		return;
		}

	SetActive();
	}

void CRefNativePluginActiveImpl::Uninstall(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
		  const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams, TRequestStatus& aStatus)
	{
	TRAPD(err, UninstallL(aComponentId, aSecurityContext, aInputParams, aOutputParams, aStatus));
	if (err != KErrNone)
		{
		TRequestStatus *statusPtr(&aStatus);
		User::RequestComplete(statusPtr, err);
		return;
		}
	SetActive();
	}

void CRefNativePluginActiveImpl::UninstallL(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
		  const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - uninstall"));
	CommonRequestPreamble(aSecurityContext, aInputParams, aOutputParams, aStatus);
    // Get UID for given component id
	RSoftwareComponentRegistry scrSession;
	User::LeaveIfError(scrSession.Connect());
	CleanupClosePushL(scrSession);
	
	CPropertyEntry* propertyEntry = scrSession.GetComponentPropertyL(aComponentId, _L("CompUid"));
	CleanupStack::PushL(propertyEntry);

	CIntPropertyEntry* intPropertyEntry = dynamic_cast<CIntPropertyEntry *>(propertyEntry); 
	
	TRequestStatus *statusPtr(&aStatus);
	if (intPropertyEntry == NULL)
		{
		DEBUG_PRINTF2(_L8("UID property for component %d was not found"), aComponentId);
		User::RequestComplete(statusPtr, KErrNotFound);
		return;
		}

	TUid objectId = TUid::Uid(intPropertyEntry->IntValue());
	CleanupStack::PopAndDestroy(2, &scrSession);

	iAsyncLauncher->UninstallL(*this, objectId, iStatus);
	}

void CRefNativePluginActiveImpl::Activate(TComponentId aComponentId, const TSecurityContext& /*aSecurityContext*/, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - activate"));
	iStatus = KRequestPending;
	aStatus = KRequestPending;
	iClientStatus = &aStatus;

	TRequestStatus* rs(&iStatus);
	
	Swi::RSisRegistryWritableSession sisRegSession;
	TRAPD(err, 
			User::LeaveIfError(sisRegSession.Connect());
			sisRegSession.ActivateComponentL(aComponentId);
			)
	sisRegSession.Close();

	User::RequestComplete(rs, err);
	SetActive();
	}

void CRefNativePluginActiveImpl::Deactivate(TComponentId aComponentId, const TSecurityContext& /*aSecurityContext*/, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - deactivate"));
	iStatus = KRequestPending;
	aStatus = KRequestPending;
	iClientStatus = &aStatus;

	TRequestStatus* rs(&iStatus);

	Swi::RSisRegistryWritableSession sisRegSession;
	TRAPD(err, 
			User::LeaveIfError(sisRegSession.Connect());
			sisRegSession.DeactivateComponentL(aComponentId);
			)
	sisRegSession.Close();

	User::RequestComplete(rs, err);
	SetActive();
	}

// SWI::MUiHandler implementation
TInt CRefNativePluginActiveImpl::DisplayLanguageL(const Swi::CAppInfo& /*aAppInfo*/,const RArray<TLanguage>& /*aLanguages*/)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - DisplayLanguageL"));
	return 0;
	}

TInt CRefNativePluginActiveImpl::DisplayDriveL(const Swi::CAppInfo& /*aAppInfo*/,TInt64 /*aSize*/, const RArray<TChar>& /*aDriveLetters*/, const RArray<TInt64>& /*aDriveSpaces*/)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - DisplayDriveL"));
	return 0;
	}

TBool CRefNativePluginActiveImpl::DisplayUninstallL(const Swi::CAppInfo& /*aAppInfo*/)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - DisplayUninstallL"));
	if (iDeclineOperation)
		{
		DEBUG_PRINTF(_L8("Reference native plugin - Received an option to decline operation - stopping uninstall"));
		return EFalse;
		}
	return ETrue;
	}

TBool CRefNativePluginActiveImpl::DisplayTextL(const Swi::CAppInfo& /*aAppInfo*/, Swi::TFileTextOption /*aOption*/, const TDesC& /*aText*/)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - DisplayTextL"));
	return ETrue;
	}

TBool CRefNativePluginActiveImpl::DisplayDependencyBreakL(const Swi::CAppInfo& /*aAppInfo*/, const RPointerArray<TDesC>& /*aComponents*/)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - DisplayDependencyBreakL"));
	return ETrue;
	}

TBool CRefNativePluginActiveImpl::DisplayApplicationsInUseL(const Swi::CAppInfo& /*aAppInfo*/, const RPointerArray<TDesC>& /*aAppNames*/)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - DisplayApplicationsInUseL"));
	return ETrue;
	}

TBool CRefNativePluginActiveImpl::DisplayQuestionL(const Swi::CAppInfo& /*aAppInfo*/, Swi::TQuestionDialog /*aQuestion*/, const TDesC& /*aDes*/)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - DisplayQuestionL"));
	return ETrue;
	}

TBool CRefNativePluginActiveImpl::DisplayInstallL(const Swi::CAppInfo& /*aAppInfo*/, const CApaMaskedBitmap* /*aLogo*/, const RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - DisplayInstallL"));

	if (iDeclineOperation)
		{
		DEBUG_PRINTF(_L8("Reference native plugin - Received an option to decline operation - stopping install"));
		return EFalse;
		}
	return ETrue;
	}

TBool CRefNativePluginActiveImpl::DisplayGrantCapabilitiesL(const Swi::CAppInfo& /*aAppInfo*/, const TCapabilitySet& /*aCapabilitySet*/)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - DisplayGrantCapabilitiesL"));
	return ETrue;
	}

TBool CRefNativePluginActiveImpl::DisplayUpgradeL(const Swi::CAppInfo& /*aAppInfo*/, const Swi::CAppInfo& /*aExistingAppInfo*/)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - DisplayUpgradeL"));
	return ETrue;
	}

TBool CRefNativePluginActiveImpl::DisplayOptionsL(const Swi::CAppInfo& /*aAppInfo*/, const RPointerArray<TDesC>& /*aOptions*/, RArray<TBool>& /*aSelections*/)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - DisplayOptionsL"));
	return ETrue;
	}

TBool CRefNativePluginActiveImpl::DisplaySecurityWarningL(const Swi::CAppInfo& /*aAppInfo*/, Swi::TSignatureValidationResult /*aSigValidationResult*/,
						RPointerArray<CPKIXValidationResultBase>& /*aPkixResults*/, RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/, TBool /*aInstallAnyway*/)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - DisplaySecurityWarningL"));
	return ETrue;
	}

TBool CRefNativePluginActiveImpl::DisplayOcspResultL(const Swi::CAppInfo& /*aAppInfo*/, Swi::TRevocationDialogMessage /*aMessage*/, RPointerArray<TOCSPOutcome>& /*aOutcomes*/, 
						RPointerArray<Swi::CCertificateInfo>& /*aCertificates*/,TBool /*aWarningOnly*/)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - DisplayOcspResultL"));
	return ETrue;
	}

TBool CRefNativePluginActiveImpl::DisplayMissingDependencyL(const Swi::CAppInfo& /*aAppInfo*/, const TDesC& /*aDependencyName*/, TVersion /*aWantedVersionFrom*/,
						TVersion /*aWantedVersionTo*/, TVersion /*aInstalledVersion*/)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - DisplayMissingDependencyL"));
	return ETrue;
	}

TBool CRefNativePluginActiveImpl::HandleInstallEventL(const Swi::CAppInfo& /*aAppInfo*/, Swi::TInstallEvent /*aEvent*/, TInt /*aValue*/, const TDesC& /*aDes*/)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - HandleInstallEventL"));
	return ETrue;
	}

void CRefNativePluginActiveImpl::HandleCancellableInstallEventL(const Swi::CAppInfo& /*aAppInfo*/, Swi::TInstallCancellableEvent /*aEvent*/, Swi::MCancelHandler& /*aCancelHandler*/,
						TInt /*aValue*/,const TDesC& /*aDes*/)
	{	
	DEBUG_PRINTF(_L8("Reference native plugin - HandleCancellableInstallEventL"));
	}

void CRefNativePluginActiveImpl::DisplayCannotOverwriteFileL(const Swi::CAppInfo& /*aAppInfo*/, const Swi::CAppInfo& /*aInstalledAppInfo*/,const TDesC& /*aFileName*/)
	{	
	DEBUG_PRINTF(_L8("Reference native plugin - DisplayCannotOverwriteFileL"));
	}

void CRefNativePluginActiveImpl::DisplayErrorL(const Swi::CAppInfo& /*aAppInfo*/, Swi::TErrorDialog /*aType*/, const TDesC& /*aParam*/)
	{
	DEBUG_PRINTF(_L8("Reference native plugin - DisplayErrorL"));
	}

