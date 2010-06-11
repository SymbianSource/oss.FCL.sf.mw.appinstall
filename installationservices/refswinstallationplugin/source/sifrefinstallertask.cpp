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
* This  file implements Reference Install tasks for getting ComponentInfo, installation, uninstallation and activation/deactivation.
*
*/


#include "sifrefinstallertask.h"
#include "sifrefbinpkgextractor.h"
#include "usiflog.h"
#include <usif/usiferror.h>

using namespace Usif;

_LIT(KRefInstPrivateDir, "c:\\private\\1028634e\\");

/**
A set of helper functions for Reference Installer tasks.
*/
namespace InstallHelper
	{
	_LIT(KSifReferenceSoftwareType, "reference");
	_LIT(KUiConfirmationQuestion, "Are you sure you want to %S component: %S, vendor: %S");
	_LIT(KParamNameErrDesc, "Error Description");
	_LIT(KErrFileAlreadyExists, "File already exists: ");
	_LIT(KUiConfirmationTypeInstall, "install");
	_LIT(KUiConfirmationTypeUpgrade, "upgrade");
	_LIT(KUiConfirmationTypeUninstall, "uninstall");
	_LIT(KUiParserErrorDesc, "Installer encountered a problem when parsing a package file.");
	const TInt KMaxConfirmationTypeLengh = KUiConfirmationTypeUninstall.iTypeLength;

	// This Reference Installer uses Component Name and Vendor Name in order to
	// identify a component in the SCR database. This identification scheme doesn't provide
	// unique ids and its results depend on the current language. Hence, a real installer
	// must use the Unique Id mechanism provided by the SCR.
	void FindComponentL(RSoftwareComponentRegistry& aScr, TComponentSearchData& aCompSearchData, const CSifRefPkgParser& aParser)
		{
		DEBUG_PRINTF(_L8("InstallHelper::FindComponentL()"));
		
		// Get the index of the current language
		const TLanguage curLang = User::Language();
		TInt langIndex = aParser.GetLanguageIndex(curLang);
		if (langIndex == KErrNotFound)
			{
			ASSERT(aParser.Languages().Count() > 0); // The parser should have already rejected packages without languages
			langIndex = 0;
			}

		// Data for CComponentInfo
		aCompSearchData.iName = aParser.ComponentNames()[langIndex];
		aCompSearchData.iVersion = aParser.Version().Name();
		aCompSearchData.iVendor = aParser.VendorNames()[langIndex];
		aCompSearchData.iScomoState = EDeactivated;
		aCompSearchData.iInstallStatus = ENewComponent;
		aCompSearchData.iComponentId = 0;

		RSoftwareComponentRegistryView scrView;
		CComponentFilter* filter = CComponentFilter::NewLC();
		filter->SetNameL(*aCompSearchData.iName);
		filter->SetVendorL(*aCompSearchData.iVendor);
		filter->SetSoftwareTypeL(KSifReferenceSoftwareType);

		scrView.OpenViewL(aScr, filter);
		CleanupClosePushL(scrView);

		// Iterate over matching components in order to determine the status of the package being processed.
		CComponentEntry* component = NULL;
		while ((component = scrView.NextComponentL()) != NULL)
			{
			const TDesC& installedVersion = component->Version();
			const TInt cmp = RSoftwareComponentRegistry::CompareVersionsL(installedVersion, aCompSearchData.iVersion);
			if (cmp > 0)
				{
				aCompSearchData.iInstallStatus = ENewerVersionAlreadyInstalled;
				aCompSearchData.iScomoState = component->ScomoState();
				aCompSearchData.iComponentId = component->ComponentId();
				delete component;
				break;
				}
			else if (cmp == 0 && aCompSearchData.iInstallStatus < EAlreadyInstalled)
				{
				aCompSearchData.iInstallStatus = EAlreadyInstalled;
				aCompSearchData.iScomoState = component->ScomoState();
				aCompSearchData.iComponentId = component->ComponentId();
				}
			else if (cmp < 0 && aCompSearchData.iInstallStatus < EUpgrade)
				{
				aCompSearchData.iInstallStatus = EUpgrade;
				aCompSearchData.iScomoState = component->ScomoState();
				aCompSearchData.iComponentId = component->ComponentId();
				}
			delete component;
			}
		CleanupStack::PopAndDestroy(2, filter);
		}

	TBool UnregisterAndDeleteFileL(RSoftwareComponentRegistry& aScr, RSoftwareComponentRegistryFilesList& aFileList, RStsSession& aSts, TComponentId aComponentId)
		{
		DEBUG_PRINTF(_L8("InstallHelper::UnregisterAndDeleteFileL()"));
		
		// Get next file
		HBufC* file = aFileList.NextFileL();
		if (file != NULL)
			{
			// and remove it from the file system
			CleanupStack::PushL(file);
			aSts.RemoveL(*file);
			CleanupStack::PopAndDestroy(file);
			}
		else
			{
			// Remove the component from the SCR if there are no files left
			aFileList.Close();
			aScr.DeleteComponentL(aComponentId);
			
			// Copying complete
			return ETrue;
			}

		// Copying in progress
		return EFalse;
		}

	MInstallerUIHandler* CreateUiHandlerL(const COpaqueNamedParams* aCustomArguments, TInstallerUIHandlerFactory aUiHandlerFactory)
		{
		DEBUG_PRINTF(_L8("InstallHelper::CreateUiHandlerL()"));
		
		// Instantiate a UI handler for non-silent requests
		if (aCustomArguments != NULL)
			{
			TInt silent = EFalse;
			aCustomArguments->GetIntByNameL(_L("Silent"), silent);
			if (silent)
				{
				return NULL;
				}
			}

		return aUiHandlerFactory();
		}

	enum TConfirmationType
		{
		EConfirmationInstall,
		EConfirmationUpgrade,
		EConfirmationUninstall
		};

	void userConfirmationL(TConfirmationType aType, MInstallerUIHandler& uiHandler, const TDesC& aComponent, const TDesC& aVendor)
		{
		DEBUG_PRINTF(_L8("InstallHelper::userConfirmationL()"));
		
		const TInt maxLen = KUiConfirmationQuestion.iTypeLength + KMaxConfirmationTypeLengh + aComponent.Length() + aVendor.Length();
		HBufC* info = HBufC::NewLC(maxLen);
		TPtr bufInfo(info->Des());
		
		switch (aType)
			{
			case EConfirmationInstall:
				bufInfo.Format(KUiConfirmationQuestion, &KUiConfirmationTypeInstall, &aComponent, &aVendor);
				break;

			case EConfirmationUpgrade:
				bufInfo.Format(KUiConfirmationQuestion, &KUiConfirmationTypeUpgrade, &aComponent, &aVendor);
				break;

			case EConfirmationUninstall:
				bufInfo.Format(KUiConfirmationQuestion, &KUiConfirmationTypeUninstall, &aComponent, &aVendor);
				break;

			default:
				User::Leave(KErrArgument);
			}

		if (!uiHandler.ConfirmationUIHandler(*info))
			{
			User::Leave(KErrCancel);
			}

		CleanupStack::PopAndDestroy(info);
		}
	}

// =============================================================================================================

CSifRefGetComponentInfoTask* CSifRefGetComponentInfoTask::NewL(TTransportTaskParams& aParams)
	{
	DEBUG_PRINTF(_L8("CSifRefGetComponentInfoTask::NewL()"));
	
	// Validate the arguments first.
	if ((aParams.iFileName == NULL && aParams.iFileHandle == NULL) || aParams.iComponentInfo == NULL)
		{
		User::Leave(KErrArgument);
		}
		
	CSifRefGetComponentInfoTask* self = new (ELeave) CSifRefGetComponentInfoTask(aParams);
	return self;
	}

CSifRefGetComponentInfoTask::CSifRefGetComponentInfoTask(TTransportTaskParams& aParams)
: CSifTransportTask(aParams, EFalse), iStep(EExtractEmbeddedPkgs)
	{
	}

CSifRefGetComponentInfoTask::~CSifRefGetComponentInfoTask()
	{
	DEBUG_PRINTF(_L8("CSifRefGetComponentInfoTask::~CSifRefGetComponentInfoTask()"));
	
	iFile.Close();
	iFs.Close();
	iScr.Close();
	iSts.Close();
	iEmbeddedComponents.Close();
	if (iSifRequestInProgress)
		{
		iSif.CancelOperation();
		}
	iSif.Close();
	delete iParser;
	delete iComponentInfo;
	}

TBool CSifRefGetComponentInfoTask::ExecuteImplL()
	{
	DEBUG_PRINTF2(_L8("Exiting from CSifRefGetComponentInfoTask::ExecuteImplL(), iStep = %d"), iStep);
	
	TBool done = EFalse;
	
	switch (iStep)
		{
		case EExtractEmbeddedPkgs:
			ExtractEmbeddedPkgsL();
			++iStep;
			break;

		case EParsePkgFile:
			iStep = ParsePkgFileL();
			break;

		case EFindComponent:
			InstallHelper::FindComponentL(iScr, iCompSearchData, *iParser);
			++iStep;
			break;

		case ECreateComponentInfoNode:
			iStep = CreateComponentInfoNodeL();
			break;

		case ESetComponentInfo:
			SetComponentInfoL();
			done = ETrue;
			break;

		default:
			User::Leave(KErrGeneral);
		}

	if (!iSifRequestInProgress)
		{
		TRequestStatus* status(RequestStatus());
		User::RequestComplete(status, KErrNone);
		}
	
	DEBUG_PRINTF3(_L8("Exiting from CSifRefGetComponentInfoTask::ExecuteImplL(), done = %d, iStep = %d"), done, iStep);
	
	return done;
	}

void CSifRefGetComponentInfoTask::ExtractEmbeddedPkgsL()
	{
	DEBUG_PRINTF(_L8("CSifRefGetComponentInfoTask::ExtractEmbeddedPkgsL()"));
	
	// Start an STS transaction. The extraction of a package requires creation of temporary
	// files that must be deleted when the GetComponentInfo requiest is complete. This is why
	// we need this STS transaction here.
	iSts.CreateTransactionL();

	if (FileName())
		{
		SifRefBinPkgExtractor::BuildPkgTreeL(iSts, *FileName(), KRefInstPrivateDir, iEmbeddedComponents);
		}
	else if (FileHandle())
		{
		SifRefBinPkgExtractor::BuildPkgTreeL(iSts, *FileHandle(), KRefInstPrivateDir, iEmbeddedComponents);
		}
	else
		{
		ASSERT(0);
		}
	
	// Connect to the SCR. The installer needs this session to check if a component being queried
	// is already installed and, if yes, obtain its detials.
	User::LeaveIfError(iScr.Connect());
	}

TInt CSifRefGetComponentInfoTask::ParsePkgFileL()
	{
	DEBUG_PRINTF(_L8("CSifRefGetComponentInfoTask::ParsePkgFileL()"));
	
	delete iParser;
	iParser = NULL;

	// Check if the next component is of the type our installer supports. If yes, add it to the list of
	// the components to be processed. If not, use the SIF API to obtain its details.
	const SifRefBinPkgExtractor::CAuxNode& node = *iEmbeddedComponents[iCurrentComponent];
	if (node.Foreign())
		{
		// Connect to the SIF server
		iComponentInfo = CComponentInfo::NewL();
		User::LeaveIfError(iSif.Connect());

		// Our installer keeps temporary files under its private folder and therefore we have to
		// pass a file handle to the SIF API.
		User::LeaveIfError(iFs.Connect());
		iFs.ShareProtected();
		User::LeaveIfError(iFile.Open(iFs, node.FileNameL(), EFileShareReadersOnly));

		// Submit a SIF request 
		iSif.GetComponentInfo(iFile, *iComponentInfo, *RequestStatus());
		
		iSifRequestInProgress = ETrue;

		return ECreateComponentInfoNode;
		}
	else
		{
		iParser = CSifRefPkgParser::NewL(node.FileNameL());
		return EFindComponent;
		}
	}

TInt CSifRefGetComponentInfoTask::CreateComponentInfoNodeL()
	{
	DEBUG_PRINTF(_L8("CSifRefGetComponentInfoTask::CreateComponentInfoNodeL()"));
	
	SifRefBinPkgExtractor::CAuxNode& auxNode = *iEmbeddedComponents[iCurrentComponent];

	if (iSifRequestInProgress)
		{
		iSif.Close();
		iFile.Close();
		iFs.Close();
		
		iSifRequestInProgress = EFalse;
		
		// Add the root node of iComponentInfo to the tree
		User::LeaveIfError(RequestStatus()->Int());
		auxNode.SetCompInfoL(iComponentInfo);
		iComponentInfo = NULL;
		}
	else
		{
		// This reference installer uses the KExampleFileSize const value to calculate the maximum size of the installed
		// component on a phone. This is because the reference package file carries only the list of the files to be installed
		// without the files themselves. A real installer should use the size of the files to be installed.
		const TInt KExampleFileSize = 1024;
		const TInt maxInstalledSize = iParser->Files().Count() * KExampleFileSize;
		const TBool hasExe = EFalse;
		const TBool driveSelectionRequired = EFalse;
		RPointerArray<Usif::CComponentInfo::CApplicationInfo>* applications = NULL;
		// The example capabilities below are hardcoded due to the same reason. The reference package file doesn't contain
		// user grantable capabilities but a real package file should provide them.
		TCapabilitySet userGrantableCaps(ECapabilityReadUserData, ECapabilityWriteUserData);
	
		// Create a ComponentInfo node and set it as a root node.
		CComponentInfo::CNode* compInfoNode = CComponentInfo::CNode::NewLC(InstallHelper::KSifReferenceSoftwareType,
							*iCompSearchData.iName, iCompSearchData.iVersion, *iCompSearchData.iVendor,
							iCompSearchData.iScomoState, iCompSearchData.iInstallStatus, iCompSearchData.iComponentId,
							*iCompSearchData.iName, ENotAuthenticated, userGrantableCaps, maxInstalledSize, hasExe, driveSelectionRequired, applications);
		
		auxNode.SetNodeL(compInfoNode);
		CleanupStack::Pop(compInfoNode);
		}
	
	return (++iCurrentComponent < iEmbeddedComponents.Count()) ? EParsePkgFile : ESetComponentInfo;
	}

void CSifRefGetComponentInfoTask::SetComponentInfoL()
	{
	DEBUG_PRINTF(_L8("CSifRefGetComponentInfoTask::SetComponentInfoL()"));
	
	// At least one node must exist, otherwise we can't reach this point.
	ASSERT (iEmbeddedComponents.Count() > 0);
	
	// Build a real tree of the nodes from iEmbeddedComponents which is a flat list
	for (TInt i=iEmbeddedComponents.Count()-1; i>=1; --i)
		{
		iEmbeddedComponents[i]->RegisterChildToParentL();
		}
	
	// Set the tree built above as the root node of ComponentInfo().
	iEmbeddedComponents[0]->SetAsRootNodeL(*ComponentInfo());
	}

// =============================================================================================================

CSifRefInstallTask* CSifRefInstallTask::NewL(TTransportTaskParams& aParams, TInstallerUIHandlerFactory aUiHandlerFactory)
	{
	DEBUG_PRINTF(_L8("CSifRefInstallTask::NewL()"));
	
	CSifRefInstallTask* self = new (ELeave) CSifRefInstallTask(aParams);
	CleanupStack::PushL(self);
	self->ConstructL(aUiHandlerFactory);
	CleanupStack::Pop(self);
	return self;
	}

CSifRefInstallTask::CSifRefInstallTask(TTransportTaskParams& aParams)
: CSifTransportTask(aParams, EFalse), iStep(EExtractEmbeddedPkgs)
	{
	}

CSifRefInstallTask::~CSifRefInstallTask()
	{
	DEBUG_PRINTF(_L8("CSifRefInstallTask::~CSifRefInstallTask()"));
	
	iFileList.Close();
	iFile.Close();
	iFs.Close();
	iScr.Close();
	iSts.Close();
	iEmbeddedComponents.Close();
	if (iSifRequestInProgress)
		{
		iSif.CancelOperation();
		}
	iSif.Close();
	delete iParser;
	delete iUiHandler;
	delete iOpaqueArguments;
	delete iOpaqueResults;
	}

void CSifRefInstallTask::ConstructL(TInstallerUIHandlerFactory aUiHandlerFactory)
	{
	if ((FileName() == NULL && FileHandle() == NULL) ||
		CustomArguments() == NULL ||
		CustomResults() == NULL)
		{
		User::Leave(KErrArgument);
		}

	iUiHandler = InstallHelper::CreateUiHandlerL(CustomArguments(), aUiHandlerFactory);
	}

TBool CSifRefInstallTask::ExecuteImplL()
	{
	DEBUG_PRINTF2(_L8("Exiting from CSifRefInstallTask::ExecuteImplL(), iStep = %d"), iStep);
	
	TBool done = EFalse;
	
	switch (iStep)
		{
		case EExtractEmbeddedPkgs:
			ExtractEmbeddedPkgsL();
			++iStep;
			break;

		case EParsePkgFile:
			iStep = ParsePkgFileL();
			break;

		case ELaunchForeignInstall:
			LaunchForeignInstallL();
			++iStep;
			break;

		case EFinishForeignInstall:
			FinishForeignInstallL();
			iStep = EParsePkgFile;
			break;

		case EFindAndCheckComponent:
			// Next step differs for ENewComponent and EUpgrade
			iStep = FindAndCheckComponentL();
			break;

		case EGetInstalledFileList:
			GetInstalledFileListL();
			++iStep;
			break;

		case EUnregisterAndDeleteFile:
			if (UnregisterAndDeleteFileL())
				{
				++iStep;
				}
			break;

		case ERegisterComponent:
			RegisterComponentL();
			++iStep;
			break;

		case ECopyFile:
			if (CopyFileL())
				{
				++iStep;
				}
			break;

		case ESetScomoState:
			iStep = SetScomoStateL();
			break;

		case ECommit:
			CommitL();
			done = ETrue;
			break;

		default:
			User::Leave(KErrGeneral);
		}

	if (!iSifRequestInProgress)
		{
		TRequestStatus* status(RequestStatus());
		User::RequestComplete(status, KErrNone);
		}
	
	DEBUG_PRINTF3(_L8("Exiting from CSifRefInstallTask::ExecuteImplL(), done = %d, iStep = %d"), done, iStep);
	
	return done;
	}

namespace
	{
	TInt AuxNodeSorter(const SifRefBinPkgExtractor::CAuxNode& aLeft, const SifRefBinPkgExtractor::CAuxNode& aRight)
		{
		const TBool l = aLeft.Foreign();
		const TBool r = aRight.Foreign();
		if (l == r)
			{
			return 0;
			}
		else if (!l && r)
			{
			return 1;
			}
		else
			{
			return -1;
			}
		}
	}

void CSifRefInstallTask::ExtractEmbeddedPkgsL()
	{
	DEBUG_PRINTF(_L8("CSifRefInstallTask::ExtractEmbeddedPkgsL()"));
	
	// Start an STS transaction
	iSts.CreateTransactionL();

	if (FileName())
		{
		SifRefBinPkgExtractor::BuildPkgTreeL(iSts, *FileName(), KRefInstPrivateDir, iEmbeddedComponents);
		}
	else if (FileHandle())
		{
		SifRefBinPkgExtractor::BuildPkgTreeL(iSts, *FileHandle(), KRefInstPrivateDir, iEmbeddedComponents);
		}
	else
		{
		ASSERT(0);
		}
	
	// Sort the list of the embedded components in order to install foreign packages first
	 const TLinearOrder<SifRefBinPkgExtractor::CAuxNode> sortOrder(AuxNodeSorter);
	iEmbeddedComponents.Sort(sortOrder);
	}

TInt CSifRefInstallTask::ParsePkgFileL()
	{
	DEBUG_PRINTF(_L8("CSifRefInstallTask::ParsePkgFileL()"));
	
	TInt nextStep = -1;
	TRAPD(err, nextStep = ParsePkgFileImplL());
	if (err != KErrNone)
		{
		if (iUiHandler)
			{
			iUiHandler->ErrorDescriptionUIHandler(InstallHelper::KUiParserErrorDesc);
			}
		User::Leave(err);
		}
	
	return nextStep;
	}

TInt CSifRefInstallTask::ParsePkgFileImplL()
	{
	DEBUG_PRINTF(_L8("CSifRefInstallTask::ParsePkgFileImplL()"));
	
	delete iParser;
	iParser = NULL;

	// Check if the next component is of the type our installer supports. If yes, add it to the list of
	// the components to be processed. If not, use the SIF API to install it.
	const SifRefBinPkgExtractor::CAuxNode& node = *iEmbeddedComponents[iCurrentComponent];
	if (node.Foreign())
		{
		return ELaunchForeignInstall;
		}
	else
		{
		iParser = CSifRefPkgParser::NewL(node.FileNameL());
		return EFindAndCheckComponent;
		}
	}

void CSifRefInstallTask::LaunchForeignInstallL()
	{
	DEBUG_PRINTF(_L8("CSifRefInstallTask::LaunchForeignInstallL()"));
	
	const SifRefBinPkgExtractor::CAuxNode& node = *iEmbeddedComponents[iCurrentComponent];
	
	// Connect to the SIF server
	User::LeaveIfError(iSif.Connect());

	// Our installer keeps temporary files under its private folder and therefore we have to
	// pass a file handle to the SIF API.
	User::LeaveIfError(iFs.Connect());
	iFs.ShareProtected();
	User::LeaveIfError(iFile.Open(iFs, node.FileNameL(), EFileShareReadersOnly));

	// Submit a SIF request 
	iOpaqueArguments = COpaqueNamedParams::NewL();
	iOpaqueResults = COpaqueNamedParams::NewL();
	iSif.Install(iFile, *iOpaqueArguments, *iOpaqueResults, *RequestStatus(), EFalse);

	iSifRequestInProgress = ETrue;
	}

void CSifRefInstallTask::FinishForeignInstallL()
	{
	DEBUG_PRINTF(_L8("CSifRefInstallTask::FinishForeignInstallL()"));
	
	iSif.Close();
	iFile.Close();
	iFs.Close();
	delete iOpaqueArguments;
	iOpaqueArguments = NULL;
	delete iOpaqueResults;
	iOpaqueResults = NULL;
	
	// Check the result of the concurrent installation
	iSifRequestInProgress = EFalse;
	User::LeaveIfError(RequestStatus()->Int());
	
	// There must be at least one component left because we install foreign components first
	++iCurrentComponent;
	}

TInt CSifRefInstallTask::FindAndCheckComponentL()
	{
	DEBUG_PRINTF(_L8("CSifRefInstallTask::FindAndCheckComponentL()"));
	
	// Connect to the SCR and create a new transaction to continue the installation of our own type
	if (!iScrTransaction)
		{
		User::LeaveIfError(iScr.Connect());
		iScr.CreateTransactionL();
		iScrTransaction = ETrue;
		}
	
	// Exit code
	TInt nextStep = ERegisterComponent;
	
	// Check if already installed
	InstallHelper::FindComponentL(iScr, iCompSearchData, *iParser);
	switch (iCompSearchData.iInstallStatus)
		{
		case ENewComponent:
			// nextStep already set to ERegisterComponent
			break;

		case EUpgrade:
			// Uninstall the previous version
			nextStep = EGetInstalledFileList;
			break;

		case EAlreadyInstalled:
			User::Leave(KErrSifSameVersionAlreadyInstalled);
			break;

		case ENewerVersionAlreadyInstalled:
			User::Leave(KErrSifNewerVersionAlreadyInstalled);
			break;

		default:
			ASSERT(0);
		}

	// Ask the user for the confirmation
	if (iUiHandler != NULL)
		{
		userConfirmationL(iCompSearchData.iInstallStatus == ENewComponent ? InstallHelper::EConfirmationInstall : InstallHelper::EConfirmationUpgrade, *iUiHandler, *iCompSearchData.iName, *iCompSearchData.iVendor);
		}

	return nextStep;
	}

void CSifRefInstallTask::GetInstalledFileListL()
	{
	DEBUG_PRINTF(_L8("CSifRefInstallTask::GetInstalledFileListL()"));
	
	// Get a list of files to be deleted
	iFileList.OpenListL(iScr, iCompSearchData.iComponentId);
	}

TBool CSifRefInstallTask::UnregisterAndDeleteFileL()
	{
	DEBUG_PRINTF(_L8("CSifRefInstallTask::UnregisterAndDeleteFileL()"));
	
	return InstallHelper::UnregisterAndDeleteFileL(iScr, iFileList, iSts, iCompSearchData.iComponentId);
	}

void CSifRefInstallTask::RegisterComponentL()
	{
	DEBUG_PRINTF(_L8("CSifRefInstallTask::RegisterComponentL()"));
	
	// Register a new component in the SCR
	RCPointerArray<CLocalizableComponentInfo> componentInfoArray;
	CleanupClosePushL(componentInfoArray);

	// ...for each language
	const RLanguageArray& languages = iParser->Languages();
	const TInt langCount = languages.Count();
	for (TInt i=0; i<langCount; ++i)
		{
		const TDesC& locName = *iParser->ComponentNames()[i];
		const TDesC& locVendor = *iParser->VendorNames()[i];
		CLocalizableComponentInfo* componentInfo = CLocalizableComponentInfo::NewLC(locName, locVendor, languages[i]);
		componentInfoArray.AppendL(componentInfo);
		CleanupStack::Pop(componentInfo);
		}
	iCompSearchData.iComponentId = iScr.AddComponentL(componentInfoArray, InstallHelper::KSifReferenceSoftwareType);

	// Set the version of a new component
	iScr.SetComponentVersionL(iCompSearchData.iComponentId, iCompSearchData.iVersion);

	CleanupStack::PopAndDestroy(&componentInfoArray);

	iCopyFileIndex = 0;
	iComponentSize = 0;
	
	// Send the id if the installed component to the client. If this is a compound package we send the id of the root component only.
	if (iEmbeddedComponents[iCurrentComponent]->Root())
		{
		CustomResults()->AddIntL(KSifOutParam_ComponentId, iCompSearchData.iComponentId);
		}
	}

namespace
	{
	TBool CheckPathExistenceL(const TDesC& filePath)
		{
		TBool exists = EFalse;

		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
		TEntry entry;
		TInt error = fs.Entry(filePath, entry);
		if (error == KErrNone)
			{
			exists = ETrue;
			}
		else if (error != KErrPathNotFound && error != KErrNotFound)
			{
			User::Leave(error);
			}
		
		CleanupStack::PopAndDestroy(&fs);
		
		return exists;
		}
	}

TBool CSifRefInstallTask::CopyFileL()
	{
	DEBUG_PRINTF(_L8("CSifRefInstallTask::CopyFileL()"));
	
	// List of files to be copied from a package file 
	const RCHBufCArray& files = iParser->Files();

	// Register and copy a file if any left
	if (iCopyFileIndex < files.Count())
		{
		// The name of the current file
		const TDesC& filePath = *files[iCopyFileIndex];

		// Check if filePath already exists
		if (CheckPathExistenceL(filePath))
			{
			// Add a custom result describing the error
			HBufC* desc = HBufC::NewLC(InstallHelper::KErrFileAlreadyExists.iTypeLength + filePath.Length());
			TPtr bufDesc = desc->Des();
			bufDesc.Copy(InstallHelper::KErrFileAlreadyExists);
			bufDesc.Copy(filePath);
			CustomResults()->AddStringL(InstallHelper::KParamNameErrDesc, *desc);
			CleanupStack::PopAndDestroy(desc);
			}

		// Register the file in the SCR
		iScr.RegisterComponentFileL(iCompSearchData.iComponentId, filePath);

		// Copy the current file
		RFile file;
		iSts.CreateNewL(filePath, file, TFileMode(EFileShareExclusive |EFileWrite));
		_LIT8(KReferenceFootprint, "This file belongs to the SIF reference component.\n");
		const TInt numLines = 100; // The operation must take a while to simulate real copying
		for (TInt i=0; i<numLines; ++i)
			{
			User::LeaveIfError(file.Write(KReferenceFootprint));
			}
		iComponentSize += KReferenceFootprint.iTypeLength*numLines;
		file.Close();

		++iCopyFileIndex;
		}
	else
		{
		// Set the size of the component in the SCR
		iScr.SetComponentSizeL(iCompSearchData.iComponentId, iComponentSize);

		// Step complete, all the files have been copied
		return ETrue;
		}

	// Step not complete, there are still files left
	
	DEBUG_PRINTF(_L8("Exiting from CSifRefInstallTask::CopyFileL()"));
	
	return EFalse;
	}

TInt CSifRefInstallTask::SetScomoStateL()
	{
	DEBUG_PRINTF(_L8("CSifRefInstallTask::SetScomoStateL()"));
	
	// Activate the newly added component
	TInt inactive = EFalse;
	if (!CustomArguments()->GetIntByNameL(_L("InstallInactive"), inactive) || !inactive)
		{
		iScr.SetScomoStateL(iCompSearchData.iComponentId, EActivated);
		}
	
	// Check if there are any components to be installed left
	return (++iCurrentComponent < iEmbeddedComponents.Count()) ? EParsePkgFile : ECommit;
	}

void CSifRefInstallTask::CommitL()
	{
	DEBUG_PRINTF(_L8("CSifRefInstallTask::CommitL()"));
	
	// Commit the STS & SCR transactions
	iSts.CommitL();
	iScr.CommitTransactionL();
	}

// =============================================================================================================

CSifRefUninstallTask* CSifRefUninstallTask::NewL(TTransportTaskParams& aParams, TInstallerUIHandlerFactory aUiHandlerFactory)
	{
	DEBUG_PRINTF(_L8("CSifRefUninstallTask::NewL()"));
	
	CSifRefUninstallTask* self = new (ELeave) CSifRefUninstallTask(aParams);
	CleanupStack::PushL(self);
	self->ConstructL(aUiHandlerFactory);
	CleanupStack::Pop(self);
	return self;
	}

CSifRefUninstallTask::CSifRefUninstallTask(TTransportTaskParams& aParams)
: CSifTransportTask(aParams), iStep(EGetFileList)
	{
	}

void CSifRefUninstallTask::ConstructL(TInstallerUIHandlerFactory aUiHandlerFactory)
	{
	if (ComponentId() == NULL || CustomArguments() == NULL || CustomResults() == NULL)
		{
		User::Leave(KErrArgument);
		}

	iUiHandler = InstallHelper::CreateUiHandlerL(CustomArguments(), aUiHandlerFactory);
	}

CSifRefUninstallTask::~CSifRefUninstallTask()
	{
	DEBUG_PRINTF(_L8("CSifRefUninstallTask::~CSifRefUninstallTask()"));
	
	// Make sure to close sub-session before the session
	iFileList.Close();	
	iScr.Close();
	iSts.Close();
	delete iUiHandler;
	}

TBool CSifRefUninstallTask::ExecuteImplL()
	{
	DEBUG_PRINTF2(_L8("CSifRefInstallTask::ExecuteImplL(), iStep = %d"), iStep);
	
	switch (iStep)
		{
		case EGetFileList:
			GetFileListL();
			break;

		case EUnregisterAndDeleteFile:
			if (!UnregisterAndDeleteFileL())
				{
				return EFalse;
				}
			break;

		case ECommit:
			CommitL();
			return ETrue;

		default:
			User::Leave(KErrGeneral);
		}

	++iStep;

	DEBUG_PRINTF2(_L8("Exiting from CSifRefInstallTask::ExecuteImplL(), iStep = %d"), iStep);
	
	return EFalse;
	}

void CSifRefUninstallTask::GetFileListL()
	{
	DEBUG_PRINTF(_L8("CSifRefUninstallTask::GetFileListL()"));
	
	// Connect to the SCR and start a transaction
	User::LeaveIfError(iScr.Connect());
	iScr.CreateTransactionL();

	// Ask the user for confirmation
	if (iUiHandler != NULL)
		{
		CComponentEntry* entry = CComponentEntry::NewLC();
		if (!iScr.GetComponentL(ComponentId(), *entry))
			{
			User::Leave(KErrNotFound);
			}
		userConfirmationL(InstallHelper::EConfirmationUninstall, *iUiHandler, entry->Name(), entry->Vendor());
		CleanupStack::PopAndDestroy(entry);
		}

	// Get a list of files to be deleted
	iFileList.OpenListL(iScr, ComponentId());

	// Start an STS transaction
	iSts.CreateTransactionL();
	}

TBool CSifRefUninstallTask::UnregisterAndDeleteFileL()
	{
	DEBUG_PRINTF(_L8("CSifRefUninstallTask::UnregisterAndDeleteFileL()"));
	
	return InstallHelper::UnregisterAndDeleteFileL(iScr, iFileList, iSts, ComponentId());
	}

void CSifRefUninstallTask::CommitL()
	{
	DEBUG_PRINTF(_L8("CSifRefUninstallTask::CommitL()"));
	
	// Commit the STS & SCR transactions
	iSts.CommitL();
	iScr.CommitTransactionL();
	}

// =============================================================================================================

CSifRefActivateDeactivateTask::CSifRefActivateDeactivateTask(TTransportTaskParams& aParams, TScomoState aScomoState)
: CSifTransportTask(aParams), iScomoState(aScomoState)
	{
	DEBUG_PRINTF(_L8("CSifRefActivateDeactivateTask::CSifRefActivateDeactivateTask()"));
	}

CSifRefActivateDeactivateTask::~CSifRefActivateDeactivateTask()
	{
	DEBUG_PRINTF(_L8("CSifRefActivateDeactivateTask::~CSifRefActivateDeactivateTask()"));
	
	iScr.Close();
	}

TBool CSifRefActivateDeactivateTask::ExecuteImplL()
	{
	DEBUG_PRINTF(_L8("CSifRefActivateDeactivateTask::ExecuteImplL()"));
	
	if (ComponentId() == NULL)
		{
		User::Leave(KErrArgument);
		}

	// Connect to SCR and activate/deactivate component
	User::LeaveIfError(iScr.Connect());
	iScr.SetScomoStateL(ComponentId(), iScomoState);

	return ETrue;
	}
