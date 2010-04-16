/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "testnonnativeplugin.h"
#include "usiflog.h"
#include <usif/sif/sif.h>
#include <usif/sif/sifcommon.h>
#include <usif/usiferror.h>
#include <usif/sif/sifplugin.h>
#include <usif/scr/screntries.h>
#include <usif/scr/scr.h>
#include <usif/sts/sts.h>
#include <usif/sif/sifutils.h>

using namespace Usif;

namespace
	{
	const TInt KUserGrantableCapsBitMask = 49152; // ECapabilityReadUserData, ECapabilityWriteUserData
	const TInt KMaxInstalledSize = 1234;
	const TBool KHasExe = EFalse;
	const TBool KDriveSelectionRequired = EFalse;	
	const TAuthenticity KAuthenticity = EAuthenticated;
	_LIT_SECURE_ID(KSifTestSuiteSecureId, 0x10285BCC);
	_LIT_SECURE_ID(KSifRefInstallerServerSecureId, 0x1028634E);
	_LIT_SECURE_ID(KSifLauncherSecureId, 0x10285BD0);
	_LIT(KSifTestComponentVersion, "1.0.0");
	_LIT(KSifTestGlobalComponentId, "test.nonnativepackage.txt");
#ifndef TESTNONNATIVEPLUGIN2
	const TInt KTestNonNativePluginImpId = 0x10285BD2;
	const TComponentId KSifTestComponentId = 12345678;
	_LIT(KSifTestSoftwareType, "plain");
	_LIT(KSifTestComponentName, "Test Component");
	_LIT(KSifTestComponentVendor, "Symbian");
	_LIT(KSifTestInstallPath, "c:\\tusif\\tsif\\nonnativeplugin\\%S\\file%d");
	_LIT(KSifTestOutIntParam, "NonNativePlugin");
#else
	const TInt KTestNonNativePluginImpId = 0x102866EF;
	const TComponentId KSifTestComponentId = 87654321;
	_LIT(KSifTestSoftwareType, "testSoftwareType");
	_LIT(KSifTestComponentName, "Test Layered Execution Environment");
	_LIT(KSifTestComponentVendor, "Test Vendor");
	_LIT(KSifTestInstallPath, "c:\\tusif\\tsif\\nonnativeplugin2\\%S\\file%d");
	_LIT(KSifTestOutIntParam, "NonNativePlugin2");
#endif

	const TImplementationProxy ImplementationTable[] = 
		{
		IMPLEMENTATION_PROXY_ENTRY(KTestNonNativePluginImpId, CTestNonNativePlugin::NewL)
		};

	void UnpackCapabilitySet(TInt aPackedCapSet, TCapabilitySet& aCapSet)
		{
		for (TInt c=0; c<ECapability_Limit; ++c)
			{
			const TInt cap = 1<<c;
			if (aPackedCapSet&cap)
				{
				aCapSet.AddCapability(TCapability(c));
				}
			}
		}

	TComponentId AddComponentToScrL(RSoftwareComponentRegistry& aScr, const TDesC& aComponentName, const TDesC& aVendor)
		{
		// Check if the component is already installed
		RSoftwareComponentRegistryView scrView;
		CComponentFilter* filter = CComponentFilter::NewLC();
		filter->SetNameL(aComponentName);
		filter->SetVendorL(aVendor);

		scrView.OpenViewL(aScr, filter);
		CleanupClosePushL(scrView);

		CComponentEntry* component = scrView.NextComponentL();
		CleanupStack::PopAndDestroy(2, filter);
		if (component != NULL)
			{
			delete component;
			DEBUG_PRINTF2(_L("Non-native component: %S already installed"), &aComponentName);
			User::Leave(KErrSifSameVersionAlreadyInstalled);
			}

		TComponentId componentId = aScr.AddComponentL(aComponentName, aVendor, KSifTestSoftwareType);
		DEBUG_PRINTF2(_L("Added a non-native component with id %d"), componentId);

		return componentId;
		}

	void CreateAndRegisterFilesL(RSoftwareComponentRegistry& aScr, TComponentId aComponentId, const TDesC& aComponentName)
		{
		RStsSession sts;
		sts.CreateTransactionL();
		CleanupClosePushL(sts);

		const TInt numComponentFiles = 3;
		const TInt maxIntDigits = 10;
		
		TInt componentSize = 0;
		
		for (TInt i=0; i<numComponentFiles; ++i)
			{
			// Generate the full path of the i-th file
			const TInt maxPathLen = KSifTestInstallPath.iTypeLength+aComponentName.Length()+maxIntDigits;
			HBufC* filePath = HBufC::NewLC(maxPathLen);
			TPtr filePathPtr = filePath->Des();
			filePathPtr.Format(KSifTestInstallPath, &aComponentName, i);

			// Create the file
			RFile file;
			sts.CreateNewL(*filePath, file, TFileMode(EFileShareExclusive |EFileWrite));
			_LIT8(KNonNativeFootprint, "This file belongs to the Non-Native Test component.");
			User::LeaveIfError(file.Write(KNonNativeFootprint));
			componentSize += KNonNativeFootprint.iTypeLength;
			file.Close();

			// Register the file in the SCR
			aScr.RegisterComponentFileL(aComponentId, *filePath);

			CleanupStack::PopAndDestroy(filePath);
			}

		aScr.SetComponentSizeL(aComponentId, componentSize);

		sts.CommitL();
		CleanupStack::PopAndDestroy(&sts);
		}

	// The literals below are hardcoded because they are used only here so there
	// is no risk of duplication or a typo. Of course, they are also used in the
	// sif.ini file but we cannot reuse them there in any case.

	void ProcessOpaqueParamsL(const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams)
		{
		aOutputParams.AddIntL(KSifTestOutIntParam, ETrue);
		TInt silent = ETrue;
		TBool silentFound = aInputParams.GetIntByNameL(_L("DoSilent"), silent);
		if (silentFound)
			{
			aOutputParams.AddIntL(_L("DoneSilently"), silent);
			}
		}

	void InstallL(const TDesC& aComponentName, COpaqueNamedParams& aOutputParams)
		{
		RSoftwareComponentRegistry scr;
		User::LeaveIfError(scr.Connect());
		CleanupClosePushL(scr);
		scr.CreateTransactionL();

		TComponentId componentId = AddComponentToScrL(scr, aComponentName, KSifTestComponentVendor);
		aOutputParams.AddIntL(KSifOutParam_ComponentId, componentId);

		CreateAndRegisterFilesL(scr, componentId, aComponentName);

		scr.CommitTransactionL();
		CleanupStack::PopAndDestroy(&scr);
		}

	void InstallImplL(const TDesC& aFileName, const TSecurityContext& /*aSecurityContext*/,
					const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams)
		{
		// The "DummyInstallation" param instructs the installer not to execute any real operation. This dummy installation
		// is needed in order to test the ExcludeOperation mode supported by the SIF APIs. Running multiple concurrent
		// SIF operations may lead to concurrent SCR transactions which are not supported. Hence, this dummy installation
		// is used as a 'safe' installation that may be launched concurrently with a regular native installation.
		TInt dummyInstallation = EFalse;
		if (aInputParams.GetIntByNameL(_L("DummyInstallation"), dummyInstallation) && dummyInstallation)
			{
			return;
			}

		ProcessOpaqueParamsL(aInputParams, aOutputParams);

		TPtrC name;
		const TInt pos = aFileName.LocateReverse('\\');
		if (pos != KErrNotFound)
			{
			name.Set(aFileName.Mid(pos+1));
			}
		else
			{
			name.Set(aFileName);
			}
		
		InstallL(name, aOutputParams);
		}

	void InstallImplL(RFile& aFileHandle, const TSecurityContext& /*aSecurityContext*/,
					const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams)
		{
		ProcessOpaqueParamsL(aInputParams, aOutputParams);
		HBufC* name = HBufC::NewLC(KMaxFullName);
		TPtr namePtr = name->Des();
		User::LeaveIfError(aFileHandle.Name(namePtr));
		InstallL(*name, aOutputParams);
		CleanupStack::PopAndDestroy(name);
		}

	void UninstallImplL(TComponentId aComponentId, const TSecurityContext& /*aSecurityContext*/,
						const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams)
		{
		ProcessOpaqueParamsL(aInputParams, aOutputParams);

		// Uninstall the component using the SIF Utils Library
		UninstallL(aComponentId);
		}

	void CheckSecurityContextL(const TSecurityContext& aSecurityContext)
		{
		// There are three executables that may issue software management request for TestNonNative packages.
		// These are:
		// (1) TEF test server (KSifTestSuiteSecureId) - all USIF tests
		// (2) the Reference Installer (KSifRefInstallerServerSecureId) - cross installer tests (for embedded packages)
		// (3) the SIF Launcher - for testing integration with AppArc
		// For the first two we also check ReadUserData to ensure coverage in integration tests
		if (aSecurityContext.SecureId() == KSifLauncherSecureId)
			return;
		aSecurityContext.HasCapabilityL(ECapabilityReadUserData);
		if (aSecurityContext.SecureId() != KSifTestSuiteSecureId &&
			aSecurityContext.SecureId() != KSifRefInstallerServerSecureId )
			{
			User::Leave(KErrPermissionDenied);
			}
		}

	TBool CheckSecurityContext(const TSecurityContext& aSecurityContext)
		{
		if (aSecurityContext.SecureId() == KSifLauncherSecureId)
			return ETrue;
		return aSecurityContext.HasCapability(ECapabilityReadUserData) &&
			   (aSecurityContext.SecureId() == KSifTestSuiteSecureId ||
			    aSecurityContext.SecureId() == KSifRefInstallerServerSecureId);
		}

	void GetComponentInfoImplL(CComponentInfo& aComponentInfo, TBool aTestOverflow = EFalse)
		{
		// Prepare capabilities
		TCapabilitySet capSet;
		RPointerArray<Usif::CComponentInfo::CApplicationInfo>* applications = NULL;
		capSet.SetEmpty();
		UnpackCapabilitySet(KUserGrantableCapsBitMask, capSet);
		
		// Create the root node of the CComponentInfo object
		CComponentInfo::CNode* rootNode = CComponentInfo::CNode::NewLC(KSifTestSoftwareType, KSifTestComponentName,
						KSifTestComponentVersion, KSifTestComponentVendor, EDeactivated, ENewComponent,
						KSifTestComponentId, KSifTestGlobalComponentId, KAuthenticity, capSet, KMaxInstalledSize, KHasExe, KDriveSelectionRequired, applications);

		// Create a long string used for overflow tests
		HBufC* longStr = NULL;

		if (aTestOverflow)
			{
			// The CComponentInfo class defines KMaxComponentInfoStreamedSize as:
			// KMaxComponentInfoStreamedSize = 2*KMaxHeapBufLength+6*sizeof(TInt)+sizeof(TInt8), where KMaxHeapBufLength = 2048
			// So, we need a tree of CNode objects in order to cause a KErrOverflow error
			const TInt longStrLen = 256;
			longStr = HBufC::NewLC(longStrLen);
			TPtr longStrPtr(longStr->Des());
			longStrPtr.Fill(TChar(0xadcb), longStrLen);

			const TInt numChildren = 3;
			for (TInt i=0; i<numChildren; ++i)
				{
				CComponentInfo::CNode* childNode = CComponentInfo::CNode::NewLC(KSifTestSoftwareType, *longStr,
						KSifTestComponentVersion, *longStr, EDeactivated, ENewComponent,
						KSifTestComponentId, *longStr, KAuthenticity, capSet, KMaxInstalledSize, KHasExe, KDriveSelectionRequired, applications);
				
				rootNode->AddChildL(childNode);
				CleanupStack::Pop(childNode);
				}
			
			CleanupStack::PopAndDestroy(longStr);
			}

		aComponentInfo.SetRootNodeL(rootNode);
		CleanupStack::Pop(rootNode);

		}
	}

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

CTestNonNativePlugin* CTestNonNativePlugin::NewL()
	{
	DEBUG_PRINTF(_L8("Constructing CTestNonNativePlugin"));
	CTestNonNativePlugin *self = new (ELeave) CTestNonNativePlugin();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CTestNonNativePlugin::ConstructL()
	{
	}

CTestNonNativePlugin::~CTestNonNativePlugin()
	{
	}

void CTestNonNativePlugin::CancelOperation()
	{
	DEBUG_PRINTF(_L8("Test non-native plugin - Cancel"));
	}

void CTestNonNativePlugin::GetComponentInfo(const TDesC& aFileName, const TSecurityContext& aSecurityContext,
										CComponentInfo& aComponentInfo, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Test non-native plugin - GetComponentInfo by filename"));
	aStatus = KRequestPending;

	TInt err = KErrNone;
	// We test a non-leaving version of TSecurityContext::HasCapability() here
	if (CheckSecurityContext(aSecurityContext))
		{
		_LIT(KTxtTestOverflow, "z:\\tusif\\tsif\\data\\test.overflow.txt");
		TRAP(err, GetComponentInfoImplL(aComponentInfo, aFileName == KTxtTestOverflow));
		}
	else
		{
		err = KErrPermissionDenied;
		}

	TRequestStatus *statusPtr(&aStatus);
	User::RequestComplete(statusPtr, err);
	}

void CTestNonNativePlugin::GetComponentInfo(RFile& /*aFileHandle*/, const TSecurityContext& aSecurityContext,
											CComponentInfo& aComponentInfo, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Test non-native plugin - GetComponentInfo by file handle"));
	aStatus = KRequestPending;

	TRAPD(err, CheckSecurityContextL(aSecurityContext);
			   GetComponentInfoImplL(aComponentInfo));

	TRequestStatus *statusPtr(&aStatus);
	User::RequestComplete(statusPtr, err);
	}

void CTestNonNativePlugin::Install(const TDesC& aFileName, const TSecurityContext& aSecurityContext,
									const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
									TRequestStatus& aStatus)
	{
	DEBUG_PRINTF2(_L("Test non-native plugin - install for %S"), &aFileName);
	aStatus = KRequestPending;

	TRAPD(err, CheckSecurityContextL(aSecurityContext);
				InstallImplL(aFileName, aSecurityContext, aInputParams, aOutputParams));

	TRequestStatus *statusPtr(&aStatus);
	User::RequestComplete(statusPtr, err);
	}

void CTestNonNativePlugin::Install(RFile& aFileHandle, const TSecurityContext& aSecurityContext,
									const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams,
									TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L("Test non-native plugin - install by file handle"));
	aStatus = KRequestPending;
	
	TRAPD(err, CheckSecurityContextL(aSecurityContext);
			   InstallImplL(aFileHandle, aSecurityContext, aInputParams, aOutputParams));

	TRequestStatus *statusPtr(&aStatus);
	User::RequestComplete(statusPtr, err);
	}

void CTestNonNativePlugin::Uninstall(TComponentId aComponentId, const TSecurityContext& aSecurityContext,
								const COpaqueNamedParams& aInputParams, COpaqueNamedParams& aOutputParams, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF2(_L8("Test non-native plugin - uninstall for ComponentId =  %d"), aComponentId);
	aStatus = KRequestPending;

	TRAPD(err, CheckSecurityContextL(aSecurityContext);
	 		   UninstallImplL(aComponentId, aSecurityContext, aInputParams, aOutputParams));

	TRequestStatus* statusPtr(&aStatus);
	User::RequestComplete(statusPtr, err);
	}

void CTestNonNativePlugin::Activate(TComponentId /*aComponentId*/, const TSecurityContext& aSecurityContext, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Test non-native plugin - activate"));
	aStatus = KRequestPending;

	TRAPD(err, CheckSecurityContextL(aSecurityContext));

	TRequestStatus* statusPtr(&aStatus);
	User::RequestComplete(statusPtr, err);
	}

void CTestNonNativePlugin::Deactivate(TComponentId /*aComponentId*/, const TSecurityContext& aSecurityContext, TRequestStatus& aStatus)
	{
	DEBUG_PRINTF(_L8("Test non-native plugin - deactivate"));
	aStatus = KRequestPending;

	TRAPD(err, CheckSecurityContextL(aSecurityContext));

	TRequestStatus* statusPtr(&aStatus);
	User::RequestComplete(statusPtr, err);
	}
